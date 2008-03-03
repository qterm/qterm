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
#include "auth.h"
#include "packet.h"
#include "ssh1.h"
#include "ssh2.h"
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QInputDialog>

#ifdef SSH_DEBUG
#include <QtDebug>
extern void dumpData(const QByteArray & data);
#endif

namespace QTerm
{

SSH2Auth::SSH2Auth(QByteArray & sessionID, SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent)
        : QObject(parent), m_username(), m_publicKey(), m_sessionID(sessionID), m_authMethod(None), m_lastTried(None), m_tries(0), m_method()
{
    m_in = in;
    m_out = out;

    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH2Auth::~SSH2Auth()
{}

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
    default:
        break;
    }
}

void SSH2Auth::failureHandler()
{
    if (m_method.contains("publickey") && m_lastTried == None) {
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
    m_username = QInputDialog::getText(0, "QTerm", "Username: ", QLineEdit::Normal, "", &ok);
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
    QString password = QInputDialog::getText(0, "Password", "Password: ", QLineEdit::Password, "", &ok);
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
    m_out->putString("ssh-dss");
    // TODO: Select other key files
    QFile file(QDir::homePath() + "/.ssh/id_dsa.pub");
    // TODO: Die
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Cannot open the public key file");
        failureHandler();
        return;
    }
    QList<QByteArray> pubKeyLine = file.readLine().split(' ');
#ifdef SSH_DEBUG
    if (pubKeyLine[0] == "ssh-dss") {
        qDebug() << "Get dss key";
    }
#endif
    m_publicKey = pubKeyLine[1];
#ifdef SSH_DEBUG
    qDebug() << "Public Key: " << pubKeyLine[1];
#endif
    m_out->putString(QByteArray::fromBase64(pubKeyLine[1]));
    m_out->sendPacket();
#ifdef SSH_DEBUG
    qDebug() << "Public Key Sent";
#endif
}

void SSH2Auth::generateSign()
{
    DSA *dsa;
    FILE *fp;
    DSA_SIG *sig;
    // TODO: use some #define?
    QByteArray sigblob(40, 0);
    uint rlen, slen;
    QString passphrase = "";
    QString privateKeyFile = QDir::homePath() + "/.ssh/id_dsa";
    if (!QFile::exists(privateKeyFile)) {
        qDebug("Cannot find the private key file");
        failureHandler();
        return;
    }
    fp = fopen(privateKeyFile.toUtf8().data(), "r");
    if (!fp) {
        qDebug("Cannot open the private key file");
        failureHandler();
        return;
    }
    // Copy from libssh
    if (!EVP_get_cipherbyname("des")) {
        OpenSSL_add_all_ciphers();
    }
    dsa = PEM_read_DSAPrivateKey(fp, NULL, NULL, passphrase.toUtf8().data());
    if (!dsa) {
        fclose(fp);
#ifdef SSH_DEBUG
        qDebug() << "Cannot read the private key file";
#endif
        failureHandler();
        return;
    }
    fclose(fp);
#ifdef SSH_DEBUG
    qDebug() << "Private key read ok";
#endif
    // TODO: ugly
    SSH2OutBuffer tmp(0);
    tmp.startPacket();
    tmp.putString(m_sessionID);

    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("publickey");
    m_out->putUInt8(1);
    m_out->putString("ssh-dss");
    m_out->putString(QByteArray::fromBase64(m_publicKey));
    QByteArray buf = QCryptographicHash::hash(tmp.buffer() + m_out->buffer(), QCryptographicHash::Sha1);
    sig = DSA_do_sign((uchar*) buf.data(), buf.size(), dsa);

    rlen = BN_num_bytes(sig->r);
    slen = BN_num_bytes(sig->s);

    // TODO: check rlen and slen: ssh-dss.c in openssh
    BN_bn2bin(sig->r, (uchar *) sigblob.data() + 20 - rlen);
    BN_bn2bin(sig->s, (uchar *) sigblob.data() + 40 - slen);
    DSA_SIG_free(sig);
    m_out->putUInt32(4 + 7 + 4 + sigblob.size());
    m_out->putString("ssh-dss");
    m_out->putString(sigblob);
    m_out->sendPacket();

}
void SSH2Auth::requestInput()
{
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
    uint numPrompts = m_in->getUInt32();
#ifdef SSH_DEBUG
    qDebug() << "number of prompts: " << numPrompts;
#endif
    QList<QString> answerList;
    bool ok;
    for (int i = 0; i < numPrompts; i++) {
        QString prompt = QString::fromUtf8(m_in->getString());
        QString answer;
        if (m_in->getUInt8() == 1)
            answer = QInputDialog::getText(0, name, prompt, QLineEdit::Normal, "", &ok);
        else
            answer = QInputDialog::getText(0, name, prompt, QLineEdit::Password, "", &ok);
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
        : QObject(parent), m_phase(UserName)
{
    m_in = in;
    m_out = out;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH1Auth::~SSH1Auth()
{}

void SSH1Auth::requestAuthService()
{
    QString username = QInputDialog::getText(0, "QTerm", "Username: ", QLineEdit::Normal);
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
        else
            qDebug("Wrong password?");
        break;
    default:
        break;
    }
}
void SSH1Auth::passwordAuth()
{
    //QString password = QString::fromLatin1 ( getpass ( "Password: " ) );
    QString password = QInputDialog::getText(0, "QTerm", "Password: ", QLineEdit::Password);
    m_out->startPacket(SSH_CMSG_AUTH_PASSWORD);
    m_out->putString(password.toUtf8());
    m_out->sendPacket();
}

}

#include "auth.moc"

