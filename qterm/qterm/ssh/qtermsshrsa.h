#ifndef QTERMSSHRSA_H
#define QTERMSSHRSA_H

#include <openssl/bn.h>
#include <openssl/rsa.h>

class QTermSSHRSA
{
public:
	RSA * d_rsa;
	QTermSSHRSA();
	~QTermSSHRSA();
	void publicEncrypt(BIGNUM *, BIGNUM *);
//	int private_decrypt(BIGNUM *, BIGNUM *);
//	void generate_additional_parameters();
};
	
#endif		// QTERMSSHRSA_H
