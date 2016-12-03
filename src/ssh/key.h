//
// C++ Interface: sshkey
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2016
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SSH_KEY_H
#define SSH_KEY_H
extern "C" {
#include "libcrypto-compat.h"
}
#include <QByteArray>
namespace QTerm
{
class SSH2Key
{
public:
    SSH2Key() {}
    virtual ~SSH2Key() {}
    virtual QByteArray publicKey() = 0;
};

class SSH2RSAKey : public SSH2Key
{
public:
    SSH2RSAKey(RSA * keyData);
    virtual ~SSH2RSAKey();
    virtual QByteArray publicKey();
    RSA * privateKey() {return m_key;}
private:
    RSA * m_key;
};

class SSH2DSAKey : public SSH2Key
{
public:
    SSH2DSAKey(DSA * keyData);
    virtual ~SSH2DSAKey();
    virtual QByteArray publicKey();
    DSA * privateKey() {return m_key;}
private:
    DSA * m_key;
};

}

#endif // SSH_KEY_H

