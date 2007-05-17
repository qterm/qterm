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
#include "transport.h"
#include <QtDebug>

extern void dumpData(const QByteArray & data);
namespace QTerm
{

SSH2Transport::SSH2Transport(const QByteArray & enc, const QByteArray & mac, const QByteArray & comp)
{
//  if ( enc == "3des-cbc" )
    m_enc = new SSH2Encryption;
//  else
//   m_enc = NULL;

//  if ( mac == "hmac-sha1" )
    m_mac = new SSH2MAC;
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

SSH2Encryption::SSH2Encryption()
{
    m_ctx = EVP_CIPHER_CTX_new();
    m_ivLen = 16;
    m_blockSize = 16;
    m_secretLen = 16;
}

SSH2Encryption::~SSH2Encryption()
{
    EVP_CIPHER_CTX_free(m_ctx);
}

void SSH2Encryption::init(const QByteArray & key, const QByteArray & iv, Method method)
{
    if (EVP_CipherInit(m_ctx, EVP_aes_128_cbc(), (const u_char*) key.data(), (const u_char*) iv.data(), method == Encryption) == 0)
        qDebug() << "Cipher init failed";
}

QByteArray SSH2Encryption::crypt(const QByteArray & src)
{
    QByteArray dest;
    if (src.size() % m_blockSize) {
        qDebug() << "SSH2Encryption: bad plaintext length " << src.size();
        return dest;
    }
    dest.resize(src.size());

    if (EVP_Cipher(m_ctx, (u_char*) dest.data(), (const u_char *) src.data(), src.size()) == 0)
        qDebug() << "SSH2Encryption: EVP_Cipher failed";

    return dest;
}

SSH2MAC::SSH2MAC()
{
    m_ctx = (HMAC_CTX*) malloc(sizeof(HMAC_CTX));
    HMAC_CTX_init(m_ctx);
    m_keyLen = 20;
    m_macLen = 20;
}

SSH2MAC::~SSH2MAC()
{
    HMAC_CTX_cleanup(m_ctx);
    free(m_ctx);
}
QByteArray SSH2MAC::mac(const QByteArray & data)
{
    QByteArray hmac(m_macLen, 0);
    HMAC_Init(m_ctx, (const u_char*) m_key.data(), m_keyLen, EVP_sha1());
    HMAC_Update(m_ctx, (const u_char *) data.data(), data.size());
    HMAC_Final(m_ctx, (u_char *) hmac.data(), NULL);
    return hmac;
}

SSH1Encryption::SSH1Encryption(Method method, const QByteArray & key)
        : m_method(method)
{
    DES_cblock desKey;
    u_char * data = (unsigned char *) key.data();
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
        qDebug() << "============Bad data len: " << src.size();
    dest.resize(src.size());
    if (m_method == Encryption) {
        qDebug() << "Encryption";
        DES_ncbc_encrypt((u_char *)src.data(), (u_char *)dest.data(), dest.size(), &m_key1, &m_IV1, DES_ENCRYPT);
        DES_ncbc_encrypt((u_char *)dest.data(), (u_char *)dest.data(), dest.size(), &m_key2, &m_IV2, DES_DECRYPT);
        DES_ncbc_encrypt((u_char *)dest.data(), (u_char *)dest.data(), dest.size(), &m_key3, &m_IV3, DES_ENCRYPT);
    } else {
        qDebug() << "Decryption";
        DES_ncbc_encrypt((u_char *)src.data(), (u_char *)dest.data(), dest.size(), &m_key3, &m_IV3, DES_DECRYPT);
        DES_ncbc_encrypt((u_char *)dest.data(), (u_char *)dest.data(), dest.size(), &m_key2, &m_IV2, DES_ENCRYPT);
        DES_ncbc_encrypt((u_char *)dest.data(), (u_char *)dest.data(), dest.size(), &m_key1, &m_IV1, DES_DECRYPT);
    }
    return dest;
}

}

