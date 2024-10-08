//
// C++ Implementation: sshauth
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <openssl/pem.h>
extern "C" {
#include "libcrypto-compat.h"
}
#include "auth.h"
#include "packet.h"
#include "ssh1.h"
#include "ssh2.h"
#include "hostinfo.h"
#include "key.h"
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>

#ifdef SSH_DEBUG
#include <QtDebug>
extern void dumpData(const QByteArray & data);
#endif

namespace QTerm
{

SSH2Auth::SSH2Auth(QByteArray & sessionID, SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent)
        : QObject(parent), m_username(), m_method(), m_authMethod(None), m_lastTried(None), m_sessionID(sessionID), m_keyType(Unknown), m_publicKeyAuthAvailable(false), m_tries(0)
{
    m_in = in;
    m_out = out;
    m_hostInfo = NULL;
    m_key = NULL;

    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH2Auth::~SSH2Auth()
{
    delete m_key;
}

void SSH2Auth::setHostInfo(HostInfo * hostInfo)
{
    if (hostInfo->type() == HostInfo::SSH) {
        m_hostInfo = static_cast<SSHInfo *>(hostInfo);
    }

    QString privateKeyFile = m_hostInfo->privateKeyFile();
    if (privateKeyFile.isEmpty())
        return;

    DSA *dsa = NULL;
    RSA *rsa = NULL;

    QString passphrase = m_hostInfo->passphrase();
    QFile file(privateKeyFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Cannot open the private key file");
        m_publicKeyAuthAvailable = false;
        return;
    }
    QByteArray privateKeyData = file.readAll();
    file.close();
    KeyType type = checkPrivateKeyType(privateKeyData);
    if (type == Unknown) {
        qDebug("Unknown private key type");
        m_publicKeyAuthAvailable = false;
        return;
    }

    BIO * mem = BIO_new_mem_buf((void*)privateKeyData.data(), -1);

    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(mem, NULL, m_hostInfo->passphraseCallback, passphrase.toUtf8().data());
    if (!pkey) {
        qDebug("Cannot read the private key file");
        BIO_free(mem);
        return;
    }
    int nid = EVP_PKEY_id(pkey);
    if ( nid == EVP_PKEY_DSA) {
        dsa = EVP_PKEY_get1_DSA(pkey);
        m_key = new SSH2DSAKey(dsa);
    } else if (nid == EVP_PKEY_RSA) {
        rsa = EVP_PKEY_get1_RSA(pkey);
        m_key = new SSH2RSAKey(rsa);
    } else {
        qDebug() << "Unknown private key type" << nid;
    }

    EVP_PKEY_free(pkey);
    BIO_free(mem);
    if (!rsa && !dsa) {
        m_publicKeyAuthAvailable = false;
        qDebug("Cannot read the private key file");
        return;
    }
    m_publicKeyAuthAvailable = true;
    m_keyType = type;

}

void SSH2Auth::authPacketReceived(int flag)
{
#ifdef SSH_DEBUG
    qDebug() << "flag " << flag;
#endif
    // TODO: BANNER
    switch (flag) {
    case SSH2_MSG_SERVICE_ACCEPT:
        noneAuth();
        break;
    case SSH2_MSG_USERAUTH_FAILURE:
        m_in->getUInt8();//flag
        m_method = m_in->getString();
#ifdef SSH_DEBUG
        qDebug() << "method: " << m_method << "flag " << m_in->getUInt8();
#endif
        failureHandler();
        break;
        // TODO: handle the conflict:
        // SSH2_MSG_USERAUTH_INFO_REQUEST
        // SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ
        // SSH2_MSG_USERAUTH_PK_OK
        // all three above share the same flag number.
    case 60:
        switch (m_authMethod) {
        case PublicKey:
            // TODO: discard the remaining data
#ifdef SSH_DEBUG
            qDebug() << "PublicKey: get pk ok message";
#endif
            generateSign();
            break;
        case Keyboard:
#ifdef SSH_DEBUG
            qDebug() << "Keyboard Interactive: request input";
#endif
            requestInput();
            break;
        case Password:
#ifdef SSH_DEBUG
            qDebug() << "Password: todo ;)";
#endif
            break;
        default:
#ifdef SSH_DEBUG
            qDebug() << "Unknown auth method";
#endif
            break;
        }
        break;
    case SSH2_MSG_USERAUTH_SUCCESS:
#ifdef SSH_DEBUG
        qDebug() << "====== success! ======";
#endif
        emit authFinished();
        break;
    default:
        qDebug() << "Unknown message: " << flag;
        break;
    }
}

void SSH2Auth::failureHandler()
{
    if (m_method.contains("publickey") && m_publicKeyAuthAvailable && m_lastTried == None) {
        m_lastTried = PublicKey;
        publicKeyAuth();
    } else if (m_method.contains("keyboard-interactive") && m_lastTried <= PublicKey) {
        if (m_tries > 1) {
            m_lastTried = Keyboard;
            m_tries = 0;
        }
        keyboardAuth();
        m_tries++;
    } else if (m_method.contains("password") && m_lastTried <= Keyboard) {
        if (m_tries > 1) {
            m_lastTried = Password;
            m_tries = 0;
        }
        passwordAuth();
        m_tries++;
    } else if (m_lastTried <= Password) {
        // We are out of luck.
        emit error("Every auth method has failed");
        return;
    } else {
        emit error("Server does not support keyboard interactive authtication method.");
        return;
    }
}

void SSH2Auth::requestAuthService()
{
    m_out->startPacket(SSH2_MSG_SERVICE_REQUEST);
    m_out->putString("ssh-userauth");
    m_out->sendPacket();
}

void SSH2Auth::noneAuth()
{
    bool ok;
    m_authMethod = None;
    m_username = m_hostInfo->userName(&ok);
    if (!ok) {
#ifdef SSH_DEBUG
        qDebug() << "User canceled!";
#endif
        emit error("User canceled");
        return;
    }
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("none");
    m_out->sendPacket();
}
void SSH2Auth::passwordAuth()
{
    bool ok;
    m_authMethod = Password;
    if (m_tries > 0) {
        m_hostInfo->reset();
    }
    QString password = m_hostInfo->password(&ok);
    if (!ok) {
#ifdef SSH_DEBUG
        qDebug() << "User canceled!";
#endif
        emit error("User canceled");
        return;
    }
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("password");
    m_out->putUInt8(0);
    m_out->putString(password.toUtf8());
    m_out->sendPacket();
}

void SSH2Auth::keyboardAuth()
{
    m_authMethod = Keyboard;
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("keyboard-interactive");
    m_out->putString("");
    m_out->putString("");
    m_out->sendPacket();
}

void SSH2Auth::publicKeyAuth()
{
#ifdef SSH_DEBUG
    qDebug() << "Start Public Key Auth";
#endif
    m_authMethod = PublicKey;
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("publickey");
    m_out->putUInt8(0);
    m_out->putString(m_key->type());
    m_out->putString(m_key->publicKey());
    m_out->sendPacket();
#ifdef SSH_DEBUG
    qDebug() << "Public Key Sent";
#endif
}

SSH2Auth::KeyType SSH2Auth::checkPrivateKeyType(const QByteArray & data)
{
    if (data.startsWith("-----BEGIN RSA PRIVATE KEY-----") ||
        data.startsWith("-----BEGIN DSA PRIVATE KEY-----") ||
        data.startsWith("-----BEGIN PRIVATE KEY-----") ||
        data.startsWith("-----BEGIN ENCRYPTED PRIVATE KEY-----")) {
        return SSH_OPENSSL;
    } else {
        return Unknown;
    }
}

void SSH2Auth::generateSign()
{
    // TODO: ugly
    SSH2OutBuffer tmp(0);
    tmp.startPacket();
    tmp.putString(m_sessionID);

    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("publickey");
    m_out->putUInt8(1);
    if (m_key->type() == "ssh-rsa") {
        RSA * rsa = static_cast<SSH2RSAKey *>(m_key)->privateKey();
        m_out->putString("ssh-rsa");
        m_out->putString(m_key->publicKey());
        QByteArray buf = QCryptographicHash::hash(tmp.buffer() + m_out->buffer(), QCryptographicHash::Sha1);
        QByteArray sigblob(RSA_size(rsa), 0);
        unsigned int siglen = 0;
        RSA_sign(NID_sha1, (uchar*) buf.data(), buf.size(), (unsigned char *) sigblob.data(), &siglen, rsa);
        m_out->putUInt32(4 + 7 + 4 + sigblob.size());
        m_out->putString("ssh-rsa");
        m_out->putString(sigblob);
    } else if (m_key->type() == "ssh-dsa") {
        DSA_SIG *sig = NULL;
        uint rlen, slen;
        DSA * dsa = static_cast<SSH2DSAKey *>(m_key)->privateKey();
        m_out->putString("ssh-dss");
        m_out->putString(m_key->publicKey());
        QByteArray buf = QCryptographicHash::hash(tmp.buffer() + m_out->buffer(), QCryptographicHash::Sha1);
        QByteArray sigblob(40, 0);
        sig = DSA_do_sign((uchar*) buf.data(), buf.size(), dsa);

        const BIGNUM * r;
        const BIGNUM * s;

        DSA_SIG_get0(sig, &r, &s);

        rlen = BN_num_bytes(r);
        slen = BN_num_bytes(s);

        //TODO: check rlen and slen: ssh-dss.c in openssh
        BN_bn2bin(r, (uchar *) sigblob.data() + 20 - rlen);
        BN_bn2bin(s, (uchar *) sigblob.data() + 40 - slen);
        DSA_SIG_free(sig);
        m_out->putUInt32(4 + 7 + 4 + sigblob.size());
        m_out->putString("ssh-dss");
        m_out->putString(sigblob);
    }

    m_out->sendPacket();

}
void SSH2Auth::requestInput()
{
    if (m_tries > 0) {
        m_hostInfo->reset();
    }
    m_in->getUInt8(); //flag
    QString name = QString::fromUtf8(m_in->getString());
#ifdef SSH_DEBUG
    qDebug() << "name: " << name;
#endif
    QString instruction = QString::fromUtf8(m_in->getString());
#ifdef SSH_DEBUG
    qDebug() << "instruction: " << instruction;
#endif
    QByteArray langTag = m_in->getString();
#ifdef SSH_DEBUG
    if (!langTag.isEmpty())
        qDebug() << "langTag is not empty, ignore it";
#endif
    int numPrompts = m_in->getUInt32();
#ifdef SSH_DEBUG
    qDebug() << "number of prompts: " << numPrompts;
#endif
    QList<QString> answerList;
    bool ok;
    for (int i = 0; i < numPrompts; i++) {
        QString prompt = QString::fromUtf8(m_in->getString());
        QString answer;
        if (m_in->getUInt8() == 1)
            answer = m_hostInfo->answer(prompt, SSHInfo::Normal, &ok);
        else
            answer = m_hostInfo->answer(prompt, SSHInfo::Password, &ok);
        if (!ok) {
#ifdef SSH_DEBUG
            qDebug() << "User canceled!";
#endif
            emit error("User canceled!");
            return;
        }
        answerList << answer;
    }

    m_out->startPacket(SSH2_MSG_USERAUTH_INFO_RESPONSE);
    m_out->putUInt32(answerList.size());
#ifdef SSH_DEBUG
    qDebug() << "number of answers: " << answerList.size();
#endif
    foreach(QString answer, answerList)
    m_out->putString(answer.toUtf8());
    m_out->sendPacket();
}

SSH1Auth::SSH1Auth(SSH1InBuffer * in, SSH1OutBuffer * out, QObject *parent)
        : QObject(parent), m_phase(UserName), m_tries(0)
{
    m_in = in;
    m_out = out;
    m_hostInfo = NULL;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH1Auth::~SSH1Auth()
{}

void SSH1Auth::setHostInfo(HostInfo * hostInfo)
{
    if (hostInfo->type() == HostInfo::SSH) {
        m_hostInfo = static_cast<SSHInfo *>(hostInfo);
    }
}

void SSH1Auth::requestAuthService()
{
    bool ok;
    QString username = m_hostInfo->userName(&ok);
    if (!ok) {
#ifdef SSH_DEBUG
        qDebug() << "User canceled!";
#endif
        emit error("User canceled");
        return;
    }
    m_out->startPacket(SSH_CMSG_USER);
    m_out->putString(username.toUtf8());
    m_out->sendPacket();
}

void SSH1Auth::authPacketReceived(int flag)
{
#ifdef SSH_DEBUG
    qDebug() << "flag " << flag;
#endif
    // TODO: BANNER
    switch (flag) {
    case SSH_SMSG_SUCCESS:
        if (m_phase == UserName)
            qDebug("No auth needed");
#ifdef SSH_DEBUG
        else
            qDebug("passwordAuth succeed");
#endif
        emit authFinished();
        break;
    case SSH_SMSG_FAILURE:
        if (m_phase == UserName) {
#ifdef SSH_DEBUG
            qDebug("Prepare to auth");
#endif
            passwordAuth();
            m_phase = PassWord;
        }
        else {
            qDebug("Wrong password?");
            m_tries ++;
            passwordAuth();
        }
        break;
    default:
        break;
    }
}
void SSH1Auth::passwordAuth()
{
    //QString password = QString::fromLatin1 ( getpass ( "Password: " ) );
    if (m_tries > 0) {
        if (m_tries > 3) {
            return;
        }
        m_hostInfo->reset();
    }
    bool ok;
    QString password = m_hostInfo->password(&ok);
    if (!ok) {
#ifdef SSH_DEBUG
        qDebug() << "User canceled!";
#endif
        emit error("User canceled");
        return;
    }
    m_out->startPacket(SSH_CMSG_AUTH_PASSWORD);
    m_out->putString(password.toUtf8());
    m_out->sendPacket();
}

}

#include "moc_auth.cpp"

