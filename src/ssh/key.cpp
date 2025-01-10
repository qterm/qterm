//
// C++ Implementation: ssh2key
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2016
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "key.h"
#include "packet.h"

namespace QTerm
{

SSH2RSAKey::SSH2RSAKey(RSA * keyData)
    :SSH2Key("ssh-rsa")
{
    m_key = keyData;
}

SSH2RSAKey::~SSH2RSAKey()
{
    RSA_free(m_key);
}

QByteArray SSH2RSAKey::publicKey()
{
    const BIGNUM *e, *n;
    SSH2OutBuffer tmp(NULL);
    tmp.startPacket();
    tmp.putString(m_type);
    RSA_get0_key(m_key, &n, &e, NULL);
    tmp.putBN(e);
    tmp.putBN(n);
    return tmp.buffer();
}

SSH2DSAKey::SSH2DSAKey(DSA * keyData)
    :SSH2Key("ssh-dsa")
{
    m_key = keyData;
}

SSH2DSAKey::~SSH2DSAKey()
{
    DSA_free(m_key);
}

QByteArray SSH2DSAKey::publicKey()
{
    const BIGNUM *p, *q, *g, *n;
    SSH2OutBuffer tmp(NULL);
    tmp.startPacket();
    tmp.putString(m_type);
    DSA_get0_pqg(m_key, &p, &q, &g);
    DSA_get0_key(m_key, &n, NULL);
    tmp.putBN(p);
    tmp.putBN(q);
    tmp.putBN(g);
    tmp.putBN(n);
    return tmp.buffer();
}

}
