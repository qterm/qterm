#ifndef QTERMSSHRSA_H
#define QTERMSSHRSA_H

#include <openssl/bn.h>
#include <openssl/rsa.h>

namespace QTerm
{
class SSHRSA
{
public:
	RSA * d_rsa;
	SSHRSA();
	~SSHRSA();
	void publicEncrypt(BIGNUM *, BIGNUM *);
//	int private_decrypt(BIGNUM *, BIGNUM *);
//	void generate_additional_parameters();
};

} // namespace QTerm
	
#endif		// QTERMSSHRSA_H
