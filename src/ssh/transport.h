//
// C++ Interface: sshtransport
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_TRANSPORT_H
#define SSH_TRANSPORT_H


#include <openssl/hmac.h>
#include <openssl/evp.h>
// TODO: change to EVP?
#include <openssl/des.h>
#include <stdint.h>
#include <QtCore/QByteArray>

namespace QTerm
{

/**
 @author hooey <hephooey@gmail.com>
*/
class SSH2Encryption
{
public:
    SSH2Encryption(const QString & algorithm);
    ~SSH2Encryption();
    enum Method
    {
        Encryption, Decryption
    };
    void init(const QByteArray & iv, const QByteArray & key, Method method);
    QByteArray crypt(const QByteArray & src);

    uint ivLen() const
    {
        return m_ivLen;
    }

    uint blockSize() const
    {
        return m_blockSize;
    }

    uint secretLen() const
    {
        return m_secretLen;
    }

private:
    EVP_CIPHER_CTX * m_ctx;
    uint m_secretLen;
    uint m_blockSize;
    uint m_ivLen;
    const EVP_CIPHER * m_evptype;
};

class SSH2MAC
{
public:
    SSH2MAC(const QString & algorithm);
    ~SSH2MAC();
    QByteArray mac(const QByteArray & data);

    uint keyLen() const
    {
        return m_keyLen;
    }

    uint macLen() const
    {
        return m_macLen;
    }

    void setKey(const QByteArray& theValue)
    {
        m_key = theValue;
    }



private:
    HMAC_CTX * m_ctx;
    QByteArray m_key;
    uint m_keyLen;
    uint m_macLen;
    const EVP_MD * m_evptype;
};
/*
 class SSHCompression
 {
  public:
   SSHCompression();

   ~SSHCompression();
 };
*/
class SSH2Transport
{
public:
    SSH2Transport(const QString & enc, const QString & mac, const QString & comp);
    ~SSH2Transport();

    /*!
        \fn QTerm::SSH2Transport::secretLen()
     */
    uint secretLen() const
    {
        return m_enc->secretLen();
    }

    /*!
        \fn QTerm::SSH2Transport::ivLen()
     */
    uint ivLen() const
    {
        return m_enc->ivLen();
    }

    /*!
        \fn QTerm::SSH2Transport::keyLen()
     */
    uint keyLen() const
    {
        return m_mac->keyLen();
    }

    /*!
        \fn QTerm::SSH2Transport::macLen()
     */
    uint macLen() const
    {
        return m_mac->macLen();
    }
    void initMAC(const QByteArray & key);
    void initEncryption(const QByteArray & secret, const QByteArray & iv, SSH2Encryption::Method method);

    /*!
        \fn QTerm::SSH2Transport::blockLen()
     */
    uint blockSize() const
    {
        return m_enc->blockSize();
    }
    QByteArray crypt(const QByteArray & src);

    /*!
        \fn QTerm::SSH2Transport::mac(QByteArray & src)
     */
    QByteArray mac(const QByteArray & src)
    {
        return m_mac->mac(src);
    }
private:
    SSH2Encryption * m_enc;
    SSH2MAC * m_mac;
//    SSHCompression * m_comp;

};

class SSH1Encryption
{
public:
    enum Method { Encryption, Decryption };
    SSH1Encryption(Method method, const QByteArray & key);
    ~SSH1Encryption();
    QByteArray crypt(const QByteArray & src);
//  void encrypt ( const uint8_t * source, uint8_t * dest, int len );
//  void decrypt ( const uint8_t * source, uint8_t * dest, int len );
private:
    DES_cblock m_IV1;
    DES_cblock m_IV2;
    DES_cblock m_IV3;

    DES_key_schedule m_key1;
    DES_key_schedule m_key2;
    DES_key_schedule m_key3;

    Method m_method;

};

}

#endif // SSH_TRANSPORT_H
