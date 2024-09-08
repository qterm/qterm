//
// C++ Implementation: sshtransport
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
#include "transport.h"
#include <QtCore/QString>

#ifdef SSH_DEBUG
#include <QtDebug>
extern void dumpData(const QByteArray & data);
#endif

#ifndef PRIdQSIZETYPE // Macro to be compatible with Qt6 to format qsizetype platform independently.
#define PRIdQSIZETYPE "d"
#endif

namespace QTerm
{

SSH2Transport::SSH2Transport(const QString & enc, const QString & mac, const QString & comp)
{
//  if ( enc == "3des-cbc" )
    m_enc = new SSH2Encryption(enc);
//  else
//   m_enc = NULL;

//  if ( mac == "hmac-sha1" )
    m_mac = new SSH2MAC(mac);
//  else
//   m_enc = NULL;

//   if ( comp != "none" )
//    qDebug() << "Compression is not yet supported";
//
//    m_comp = NULL;
}

SSH2Transport::~SSH2Transport()
{
    delete m_enc;
    delete m_mac;
//   delete m_comp;
}


/*!
    \fn QTerm::SSH2Transport::initEncryption(const QByteArray & secret, const QByteArray & iv)
 */
void SSH2Transport::initEncryption(const QByteArray & secret, const QByteArray & iv, SSH2Encryption::Method method)
{
    m_enc->init(secret, iv, method);
}



/*!
    \fn QTerm::SSH2Transport::initMAC(const QByteArray & key)
 */
void SSH2Transport::initMAC(const QByteArray & key)
{
    m_mac->setKey(key);
}
/*!
    \fn QTerm::SSH2Transport::crypt(const QByteArray & src)
 */
QByteArray SSH2Transport::crypt(const QByteArray & src)
{
    return m_enc->crypt(src);
}

SSH2Encryption::SSH2Encryption(const QString & algorithm)
{
    m_ctx = EVP_CIPHER_CTX_new();
    if (algorithm == "aes128-cbc") {
        m_ivLen = 16;
        m_blockSize = 16;
        m_secretLen = 16;
        m_evptype = EVP_aes_128_cbc();
    } else if (algorithm == "aes128-ctr") {
        m_ivLen = 16;
        m_blockSize = 16;
        m_secretLen = 16;
        m_evptype = EVP_aes_128_ctr();
    } else if (algorithm == "3des-cbc") {
        m_ivLen = 8;
        m_blockSize = 8;
        m_secretLen = 24;
        m_evptype = EVP_des_ede3_cbc();
    }

}

SSH2Encryption::~SSH2Encryption()
{
    EVP_CIPHER_CTX_free(m_ctx);
}

void SSH2Encryption::init(const QByteArray & key, const QByteArray & iv, Method method)
{
    if (EVP_CipherInit(m_ctx, m_evptype, (const uint8_t*) key.data(), (const uint8_t*) iv.data(), method == Encryption) == 0)
        qDebug("Cipher init failed");
}

QByteArray SSH2Encryption::crypt(const QByteArray & src)
{
    QByteArray dest;
    if (src.size() % m_blockSize) {
        qDebug("SSH2Encryption: bad plaintext length %" PRIdQSIZETYPE, src.size());
        return dest;
    }
    dest.resize(src.size());

    if (EVP_Cipher(m_ctx, (uint8_t*) dest.data(), (const uint8_t *) src.data(), src.size()) == 0)
        qDebug("SSH2Encryption: EVP_Cipher failed");

    return dest;
}

SSH2MAC::SSH2MAC(const QString & algorithm)
{
    m_ctx = HMAC_CTX_new();
    if (algorithm == "hmac-sha1") {
        m_keyLen = 20;
        m_macLen = 20;
        m_evptype = EVP_sha1();
    } else if (algorithm == "hmac-md5") {
        m_keyLen = 16;
        m_macLen = 16;
        m_evptype = EVP_md5();
    }
}

SSH2MAC::~SSH2MAC()
{
    HMAC_CTX_free(m_ctx);
}
QByteArray SSH2MAC::mac(const QByteArray & data)
{
    QByteArray hmac(m_macLen, 0);
    HMAC_Init_ex(m_ctx, (const uint8_t*) m_key.data(), m_keyLen, m_evptype, NULL);
    HMAC_Update(m_ctx, (const uint8_t *) data.data(), data.size());
    HMAC_Final(m_ctx, (uint8_t *) hmac.data(), NULL);
    return hmac;
}

SSH1Encryption::SSH1Encryption(Method method, const QByteArray & key)
        : m_method(method)
{
    DES_cblock desKey;
    uint8_t * data = (unsigned char *) key.data();
    memset(desKey, 0, sizeof(desKey));
    memcpy(desKey, data, sizeof(desKey));
    DES_set_key(&desKey, &m_key1);
    data += 8;
    memset(desKey, 0, sizeof(desKey));
    memcpy(desKey, data, sizeof(desKey));
    DES_set_key(&desKey, &m_key2);
    data += 8;
    memset(desKey, 0, sizeof(desKey));
    memcpy(desKey, data, sizeof(desKey));
    DES_set_key(&desKey, &m_key3);
    memset(m_IV1, 0, sizeof(m_IV1));
    memset(m_IV2, 0, sizeof(m_IV2));
    memset(m_IV3, 0, sizeof(m_IV3));
}

SSH1Encryption::~SSH1Encryption()
{}

QByteArray SSH1Encryption::crypt(const QByteArray & src)
{
    QByteArray dest;
    if (src.size() % 8)
        qDebug("============Bad data len: %" PRIdQSIZETYPE, src.size());
    dest.resize(src.size());
    if (m_method == Encryption) {
#ifdef SSH_DEBUG
        qDebug() << "Encryption";
#endif
        DES_ncbc_encrypt((uint8_t *)src.data(), (uint8_t *)dest.data(), dest.size(), &m_key1, &m_IV1, DES_ENCRYPT);
        DES_ncbc_encrypt((uint8_t *)dest.data(), (uint8_t *)dest.data(), dest.size(), &m_key2, &m_IV2, DES_DECRYPT);
        DES_ncbc_encrypt((uint8_t *)dest.data(), (uint8_t *)dest.data(), dest.size(), &m_key3, &m_IV3, DES_ENCRYPT);
    } else {
#ifdef SSH_DEBUG
        qDebug() << "Decryption";
#endif
        DES_ncbc_encrypt((uint8_t *)src.data(), (uint8_t *)dest.data(), dest.size(), &m_key3, &m_IV3, DES_DECRYPT);
        DES_ncbc_encrypt((uint8_t *)dest.data(), (uint8_t *)dest.data(), dest.size(), &m_key2, &m_IV2, DES_ENCRYPT);
        DES_ncbc_encrypt((uint8_t *)dest.data(), (uint8_t *)dest.data(), dest.size(), &m_key1, &m_IV1, DES_DECRYPT);
    }
    return dest;
}

}

