//
// C++ Interface: sshkex
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_KEX_H
#define SSH_KEX_H

#include <QObject>
#include <QStringList>

#include <openssl/bn.h>


namespace QTerm
{
class SSH2InBuffer;
class SSH2OutBuffer;
class SSH1InBuffer;
class SSH1OutBuffer;
class SSH2Transport;
class SSH1Encryption;
/**
 @author hooey <hephooey@gmail.com>
*/
class SSH2Kex : public QObject
{
    Q_OBJECT
public:
    SSH2Kex(QByteArray * sessionID , SSH2InBuffer * in, SSH2OutBuffer * out, const QByteArray & server, const QByteArray & client, QObject * parent = 0);
    ~SSH2Kex();
    void sendKex();
    void startBinary();

signals:
    void kexFinished();
    void error(const QString & message);
public slots:
    void kexPacketReceived(int flag);
private:
    bool verifySignature(const QByteArray & hash, const QByteArray & hostKey, const QByteArray & signature);
    QByteArray deriveKey(const QByteArray & hash, const QByteArray * sessionID, char id, uint needed);
    void sendKexDH(const QString & dhtype);
    void readKexInit();
    void readKexReply();
    void DHGroup1();
    void DHGroup14();
    QString chooseAlgorithm(const QStringList & target, const QStringList & available);
    void initTransport(const QByteArray & hash);
    enum Status
    {
        Init, KexSent, NewKeysReceived, NewKeysSent
    };
    QStringList m_kexList;
    QStringList m_hostKeyList;
    QStringList m_encList;
    QStringList m_macList;
    QStringList m_compList;
    BN_CTX *ctx;
    BIGNUM *g;
    BIGNUM *p;
    BIGNUM *x; /* Random from client */
    BIGNUM *e; /* g^x mod p */
    BIGNUM *f; /* g^(Random from server) mod p */
    BIGNUM *K; /* The shared secret: f^x mod p */
    QByteArray V_S;
    QByteArray V_C;
    QByteArray I_S;
    QByteArray I_C;
    QByteArray K_S;
    Status m_status;
    QByteArray * m_sessionID;
    SSH2InBuffer * m_in;
    SSH2OutBuffer * m_out;
    SSH2Transport * m_inTrans;
    SSH2Transport * m_outTrans;
};

class SSH1Kex : public QObject
{
    Q_OBJECT
public:
    SSH1Kex(SSH1InBuffer * in, SSH1OutBuffer * out, QObject * parent = 0);
    ~SSH1Kex();
    void sendKex();
    void startBinary();
signals:
    void kexFinished();
public slots:
    void kexPacketReceived(int flag);
private:
    void readKex();
    void publicEncrypt(BIGNUM *out, BIGNUM *in, RSA *key);
    void initEncryption(const QByteArray & hash);
    enum Status
    {
        Init, KexSent, NewKeysReceived, NewKeysSent
    };
    Status m_status;
    QByteArray m_cookie;
    QByteArray m_sessionID;
    QByteArray m_sessionKey;
    SSH1InBuffer * m_in;
    SSH1OutBuffer * m_out;
    SSH1Encryption * m_inEncrypt;
    SSH1Encryption * m_outEncrypt;
};

}

#endif // SSH_KEX_H
