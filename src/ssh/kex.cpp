//
// C++ Implementation: sshkex
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

extern "C" {
#include "libcrypto-compat.h"
}
#include "kex.h"
#include "packet.h"
#include "transport.h"
#include "ssh1.h"
#include "ssh2.h"
#include "hostinfo.h"
#include <openssl/rand.h>
#include <QtCore/QCryptographicHash>

#ifdef SSH_DEBUG
#include <QtDebug>
extern void dumpData(const QByteArray & data);
#endif

#define INTBLOB_LEN     20
#define SSH_CIPHER_3DES    3

namespace QTerm
{

SSH2Kex::SSH2Kex(SSH2InBuffer * in, SSH2OutBuffer * out, const QByteArray & server, const QByteArray & client, QObject * parent)
        : QObject(parent), m_kexList(), m_hostKeyList(), m_encList(), m_macList(), m_compList(), V_S(server), V_C(client), I_S(), I_C(), m_status(Init), m_sessionID()
{
    m_kexList << "diffie-hellman-group14-sha1" << "diffie-hellman-group1-sha1";
    m_hostKeyList << "ssh-rsa" << "ssh-dss";
    m_encList << "aes128-cbc" << "aes128-ctr" << "3des-cbc";
    m_macList << "hmac-sha1" << "hmac-md5";
    m_compList << "none";

    m_in = in;
    m_out = out;
    m_hostInfo = NULL;
    m_inTrans = NULL;
    m_outTrans = NULL;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(kexPacketReceived(int)));

    ctx = BN_CTX_new();
    g = BN_new();
    p = BN_new();
    x = BN_new(); /* Random from client */
    e = BN_new(); /* g^x mod p */
    f = BN_new(); /* g^(Random from server) mod p */
    K = BN_new(); /* The shared secret: f^x mod p */

}

SSH2Kex::~SSH2Kex()
{
    BN_clear_free(g);
    BN_clear_free(p);
    BN_clear_free(x);
    BN_clear_free(e);
    BN_clear_free(f);
    BN_clear_free(K);
    BN_CTX_free(ctx);
}

void SSH2Kex::setHostInfo(HostInfo * hostInfo)
{
    if (hostInfo->type() == HostInfo::SSH) {
        m_hostInfo = static_cast<SSHInfo *>(hostInfo);
    }
}

QString SSH2Kex::chooseAlgorithm(const QStringList & target, const QStringList & available)
{
    QString algorithm;
    foreach(algorithm, available) {
        if (target.contains(algorithm)) {
            return algorithm;
        }
    }
#ifdef SSH_DEBUG
    qDebug() << "no algorithm available!";
#endif
    emit error("No proper algorithm available!");
    return QString();
}

void SSH2Kex::sendKex()
{
//  I_S = in->buffer();
//  m_encCS = m_encSC = "3des-cbc";
//  m_macCS = m_macSC = "hmac-sha1";
//  m_compCS = m_compSC = "none";
    QByteArray cookie;

    m_out->startPacket(SSH2_MSG_KEXINIT);
    cookie.resize(16);

    RAND_bytes((unsigned char *) cookie.data(), 16);

    m_out->putData(cookie);
    m_out->putString(m_kexList.join(",").toUtf8());
    m_out->putString(m_hostKeyList.join(",").toUtf8());
    m_out->putString(m_encList.join(",").toUtf8());
    m_out->putString(m_encList.join(",").toUtf8());
    m_out->putString(m_macList.join(",").toUtf8());
    m_out->putString(m_macList.join(",").toUtf8());
    m_out->putString(m_compList.join(",").toUtf8());
    m_out->putString(m_compList.join(",").toUtf8());
    m_out->putString("");
    m_out->putString("");
    m_out->putUInt8(0);
    m_out->putUInt32(0);

    I_C = m_out->buffer();

    m_out->sendPacket();
    m_status = KexSent;

}

void SSH2Kex::sendKexDH(const QString & dhtype)
{
    if (dhtype == "diffie-hellman-group14-sha1") {
        DHGroup14();
    } else if (dhtype == "diffie-hellman-group1-sha1") {
        DHGroup1();
    }
    m_out->startPacket(SSH2_MSG_KEXDH_INIT);
    m_out->putBN(e);
    m_out->sendPacket();
}

void SSH2Kex::DHGroup1()
{
#ifdef SSH_DEBUG
    qDebug() << "diffie-hellman-group1-sha1";
#endif
    unsigned char p_value[128] = {
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                     0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
                                     0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
                                     0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
                                     0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
                                     0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
                                     0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
                                     0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
                                     0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
                                     0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
                                     0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
                                     0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
                                     0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
                                     0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
                                     0x49, 0x28, 0x66, 0x51, 0xEC, 0xE6, 0x53, 0x81,
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    BN_set_word(g, 2);
    BN_bin2bn(p_value, 128, p);
    BN_rand(x, 128, 0, 0);
    BN_mod_exp(e, g, x, p, ctx);
}

void SSH2Kex::DHGroup14()
{
#ifdef SSH_DEBUG
    qDebug() << "diffie-hellman-group14-sha1";
#endif
    unsigned char p_value[256] = {
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                     0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
                                     0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
                                     0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
                                     0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
                                     0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
                                     0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
                                     0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
                                     0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
                                     0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
                                     0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
                                     0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
                                     0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
                                     0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
                                     0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
                                     0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
                                     0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A,
                                     0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
                                     0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96,
                                     0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
                                     0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
                                     0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
                                     0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C,
                                     0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
                                     0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03,
                                     0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
                                     0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
                                     0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
                                     0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5,
                                     0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
                                     0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68,
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    BN_set_word(g, 2);
    BN_bin2bn(p_value, 256, p);
    BN_rand(x, 256, 0, 0);
    BN_mod_exp(e, g, x, p, ctx);

}
void SSH2Kex::readKexInit()
{
    m_in->getUInt8();
    m_in->getData(16);
    QStringList nameList;
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
    QString::SplitBehavior behavior = QString::SkipEmptyParts;
#else
    Qt::SplitBehavior behavior = Qt::SkipEmptyParts;
#endif
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString kexType = chooseAlgorithm(nameList, m_kexList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString hostKeyType = chooseAlgorithm(nameList, m_hostKeyList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString encTypeCS = chooseAlgorithm(nameList, m_encList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString encTypeSC = chooseAlgorithm(nameList, m_encList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString macTypeCS = chooseAlgorithm(nameList, m_macList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString macTypeSC = chooseAlgorithm(nameList, m_macList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString compTypeCS = chooseAlgorithm(nameList, m_compList);
    nameList = QString::fromUtf8(m_in->getString()).split(",", behavior);
    QString compTypeSC = chooseAlgorithm(nameList, m_compList);
    //TODO: language?

    if (encTypeCS.isEmpty() || macTypeCS.isEmpty() || compTypeCS.isEmpty()) {
        return;
    }
    if (encTypeSC.isEmpty() || macTypeSC.isEmpty() || compTypeSC.isEmpty()) {
        return;
    }
    m_inTrans = new SSH2Transport(encTypeSC, macTypeSC, compTypeSC);
    m_outTrans = new SSH2Transport(encTypeCS, macTypeCS, compTypeCS);

    I_S = m_in->buffer();
    if (m_status == Init)
        sendKex();
    sendKexDH(kexType);
}

void SSH2Kex::kexPacketReceived(int flag)
{
    switch (flag) {
    case SSH2_MSG_KEXINIT:
        readKexInit();
        break;
    case SSH2_MSG_KEXDH_REPLY:
        readKexReply();
        break;
    case SSH2_MSG_NEWKEYS:
        if (m_status == NewKeysSent) {
            m_in->setTransport(m_inTrans);
            m_out->setTransport(m_outTrans);
            emit kexFinished(m_sessionID);
        } else
            m_status = NewKeysReceived;
        break;
    default:
        break;
    }
}

void SSH2Kex::readKexReply()
{
    m_in->getUInt8();
    K_S = m_in->getString();

    if (!m_hostInfo->checkHostKey(K_S)) {
        qDebug("Reject the host key");
        emit error("hostkey does not match!");
        return;
    }

    m_in->getBN(f);
    QByteArray sign = m_in->getString();
    m_in->atEnd();

    BN_mod_exp(K, f, x, p, ctx);
    SSH2OutBuffer tmp(NULL);
    tmp.startPacket();
    tmp.putString(V_C.replace("\r","").replace("\n",""));
    tmp.putString(V_S.replace("\r","").replace("\n",""));
    tmp.putString(I_C);
    tmp.putString(I_S);
    tmp.putString(K_S);
    tmp.putBN(e);
    tmp.putBN(f);
    tmp.putBN(K);

    QByteArray key = QCryptographicHash::hash(tmp.buffer(), QCryptographicHash::Sha1);

    if (!verifySignature(key, K_S, sign)) {
        qDebug("Signature check error");
        emit error("Signature check error!");
        return;
    }
    m_out->startPacket(SSH2_MSG_NEWKEYS);
    m_out->sendPacket();

    if (m_sessionID.isEmpty())
        m_sessionID = key;
    initTransport(key);

    if (m_status == NewKeysReceived) {
        m_in->setTransport(m_inTrans);
        m_out->setTransport(m_outTrans);
        emit kexFinished(m_sessionID);
    } else
        m_status = NewKeysSent;
}

void SSH2Kex::initTransport(const QByteArray & hash)
{
    // client to server
    QByteArray iv = deriveKey(hash, m_sessionID, 'A', m_outTrans->ivLen());
    QByteArray secret = deriveKey(hash, m_sessionID, 'C', m_outTrans->secretLen());
    QByteArray key = deriveKey(hash, m_sessionID, 'E', m_outTrans->keyLen());
#ifdef SSH_DEBUG
    // dumpData ( key );
#endif
    m_outTrans->initEncryption(secret, iv, SSH2Encryption::Encryption);
    m_outTrans->initMAC(key);
    //server to client
    iv = deriveKey(hash, m_sessionID, 'B', m_inTrans->ivLen());
    secret = deriveKey(hash, m_sessionID, 'D', m_inTrans->secretLen());
    key = deriveKey(hash, m_sessionID, 'F', m_inTrans->keyLen());
#ifdef SSH_DEBUG
    // dumpData ( key );
#endif
    m_inTrans->initEncryption(secret, iv, SSH2Encryption::Decryption);
    m_inTrans->initMAC(key);
}

bool SSH2Kex::verifySignature(const QByteArray & hash, const QByteArray & hostKey, const QByteArray & signature)
{
    int ret = 0;
    SSH2InBuffer tmp(NULL);
    tmp.buffer().append(hostKey);
    QByteArray type = tmp.getString();
#ifdef SSH_DEBUG
    qDebug() << "key type: " << type;
#endif
    DSA *dsa = DSA_new();
    RSA *rsa = RSA_new();
    BIGNUM * e = BN_new();
    BIGNUM * n = BN_new();
    BIGNUM * p = BN_new();
    BIGNUM * q = BN_new();
    BIGNUM * g = BN_new();
    BIGNUM * pub_key = BN_new();

    if (type == "ssh-rsa") {
        tmp.getBN(e);
        tmp.getBN(n);
        tmp.atEnd();
        RSA_set0_key(rsa, n, e, NULL);
        qDebug() << "key size: " << RSA_size(rsa);
    }

    if (type == "ssh-dss") {
#ifdef SSH_DEBUG
        qDebug() << "generate DSA key";
#endif
        tmp.getBN(p);
        tmp.getBN(q);
        tmp.getBN(g);
        tmp.getBN(pub_key);
        tmp.atEnd();
        DSA_set0_pqg(dsa, p, q, g);
        DSA_set0_key(dsa, pub_key, NULL);
    }

    tmp.buffer().append(signature);
    type.resize(0);
    type = tmp.getString();
    QByteArray signBlob = tmp.getString();
#ifdef SSH_DEBUG
        //dumpData ( signBlob );
#endif
    if (type == "ssh-rsa") {
        if (signBlob.size() != RSA_size(rsa)) {
            qDebug() << "TODO: key size mismatch";
        }
        QByteArray digest = QCryptographicHash::hash(hash, QCryptographicHash::Sha1);
        ret = RSA_verify(NID_sha1, (const unsigned char *) digest.data(), digest.size(), (const unsigned char *)signBlob.data(), signBlob.size(), rsa);
        qDebug() << "Verify RSA: " << ret;
    }

    if (type == "ssh-dss") {
#ifdef SSH_DEBUG
        qDebug() << "generate DSA signature";
#endif
        DSA_SIG * sig;
        sig = DSA_SIG_new();
        BIGNUM * r = BN_new();
        BIGNUM * s = BN_new();

        BN_bin2bn((const unsigned char *) signBlob.data(), INTBLOB_LEN, r);

        BN_bin2bn((const unsigned char *) signBlob.data() + INTBLOB_LEN, INTBLOB_LEN, s);

        DSA_SIG_set0(sig, r, s);

        QByteArray digest = QCryptographicHash::hash(hash, QCryptographicHash::Sha1);

        ret = DSA_do_verify((const unsigned char *) digest.data(), digest.size(), sig, dsa);

        DSA_SIG_free(sig);

    }

    DSA_free(dsa);
    RSA_free(rsa);

    if (ret == 1)
        return true;
    else if (ret == 0)
        qDebug("incorrect");
    else
        qDebug("error: %d", ret);

    return false;
}

QByteArray SSH2Kex::deriveKey(const QByteArray & hash, const QByteArray & sessionID, char id, uint need)
{
    SSH2OutBuffer tmp(0);
    tmp.startPacket();
    tmp.putBN(K);
    QCryptographicHash sha1Hash(QCryptographicHash::Sha1);
    sha1Hash.addData(tmp.buffer());
    sha1Hash.addData(hash);
    sha1Hash.addData(QByteArray(1,id));
    sha1Hash.addData(sessionID);
    QByteArray digest = sha1Hash.result();
    for (uint have = 20; need > have; have += need) {
        sha1Hash.reset();
        sha1Hash.addData(tmp.buffer());
        sha1Hash.addData(hash);
        sha1Hash.addData(digest);
        digest += sha1Hash.result();
    }
    // digest.truncate ( need );
    return digest;
}
SSH1Kex::SSH1Kex(SSH1InBuffer * in, SSH1OutBuffer * out, QObject * parent)
        : QObject(parent), m_cookie(), m_sessionID(), m_sessionKey()
{
    m_in = in;
    m_out = out;
//  m_inTrans = NULL;
//  m_outTrans = NULL;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(kexPacketReceived(int)));
}

SSH1Kex::~SSH1Kex()
{}

void SSH1Kex::kexPacketReceived(int flag)
{
#ifdef SSH_DEBUG
    qDebug() << "flag: " << flag;
#endif
    switch (flag) {
    case SSH_SMSG_PUBLIC_KEY:
        readKex();
        break;
    case SSH_SMSG_SUCCESS:
//    m_out->startPacket(SSH_CMSG_USER);
//    m_out->putString("hooey");
//    m_out->sendPacket();
        emit kexFinished();
        break;
    default: {
        m_in->getUInt8();
        QString error = m_in->getString();
        qDebug("unknown packet");
        qDebug("error: %s",error.toLatin1().data());
        break;
    }
    }
}

void SSH1Kex::readKex()
{
    m_in->getUInt8();
    m_cookie = m_in->getData(8);
    int serverKeyLength = m_in->getUInt32();
#ifdef SSH_DEBUG
    qDebug() << "length" << serverKeyLength;
#endif

    RSA * serverKey = RSA_new();
    BIGNUM * s_e = BN_new();
    BIGNUM * s_n = BN_new();
    m_in->getBN(s_e);
    m_in->getBN(s_n);
    RSA_set0_key(serverKey, s_n, s_e, NULL);
    QByteArray server_n(BN_num_bytes(s_n), 0);
    BN_bn2bin(s_n, (unsigned char *) server_n.data());
    // TODO: rbits = BN_num_bits(d_servKey->d_rsa->n);

    int hostKeyLength = m_in->getUInt32();
#ifdef SSH_DEBUG
    qDebug() << "length" << hostKeyLength;
#endif
    RSA * hostKey = RSA_new();
    BIGNUM * h_e = BN_new();
    BIGNUM * h_n = BN_new();
    m_in->getBN(h_e);
    m_in->getBN(h_n);
    RSA_set0_key(hostKey, h_n, h_e, NULL);
    QByteArray host_n(BN_num_bytes(h_n), 0);
    BN_bn2bin(h_n, (unsigned char *) host_n.data());
    m_sessionID = QCryptographicHash::hash(host_n + server_n + m_cookie, QCryptographicHash::Md5);
#ifdef SSH_DEBUG
    dumpData(m_sessionID);
#endif

    m_sessionKey.resize(32);
    RAND_bytes((unsigned char *) m_sessionKey.data(), 32);
    initEncryption(m_sessionKey);
#ifdef SSH_DEBUG
    dumpData(m_sessionKey);
#endif

    BIGNUM * key = BN_new();
    BN_set_word(key, 0);
    for (int i = 0; i < 32; i++) {
        BN_lshift(key, key, 8);
        if (i < 16)
            BN_add_word(key, ((u_char)m_sessionKey[i]) ^((u_char)m_sessionID[i]));
        else
            BN_add_word(key, (u_char)m_sessionKey[i]);
    }
    if (BN_cmp(s_n, h_n) < 0) {
        publicEncrypt(key, key, serverKey);
        publicEncrypt(key, key, hostKey);
    } else {
        publicEncrypt(key, key, hostKey);
        publicEncrypt(key, key, serverKey);
    }

    uint serverFlag = m_in->getUInt32();
    uint serverCiphers = m_in->getUInt32();
    uint serverAuth = m_in->getUInt32();
    if ((serverCiphers & (1 << SSH_CIPHER_3DES)) != 0)
        qDebug("We can use 3DES");
//  qDebug() << serverFlag << serverCiphers << serverAuth;
    m_in->atEnd();
    m_out->startPacket(SSH_CMSG_SESSION_KEY);
    m_out->putUInt8(SSH_CIPHER_3DES);
    m_out->putData(m_cookie);
    m_out->putBN(key);
    m_out->putUInt32(1);
    m_out->sendPacket();
    m_in->setEncryption(m_inEncrypt);
    m_out->setEncryption(m_outEncrypt);
}

void SSH1Kex::publicEncrypt(BIGNUM *out, BIGNUM *in, RSA *key)
{
    QByteArray inbuf;
    QByteArray outbuf;
    int len, ilen, olen;

    const BIGNUM * e;
    const BIGNUM * n;

    RSA_get0_key(key, &n, &e, NULL);

    if (BN_num_bits(e) < 2 || !BN_is_odd(e))
        qDebug("rsa_public_encrypt() exponent too small or not odd");

    olen = BN_num_bytes(n);
    outbuf.resize(olen);

    ilen = BN_num_bytes(in);
    inbuf.resize(ilen);
    BN_bn2bin(in, (unsigned char *) inbuf.data());

    // FIXME: failure handler
    if ((len = RSA_public_encrypt(ilen, (unsigned char *) inbuf.data(), (unsigned char *) outbuf.data(), key,
                                  RSA_PKCS1_PADDING)) <= 0)
        qDebug("rsa_public_encrypt() failed");

    if (BN_bin2bn((unsigned char *) outbuf.data(), len, out) == NULL)
        qDebug("rsa_public_encrypt: BN_bin2bn failed");
}

void SSH1Kex::initEncryption(const QByteArray & key)
{
    m_inEncrypt = new SSH1Encryption(SSH1Encryption::Decryption, key);
    m_outEncrypt = new SSH1Encryption(SSH1Encryption::Encryption, key);
}

}

#include "moc_kex.cpp"

