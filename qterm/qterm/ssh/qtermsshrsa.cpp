#include <sys/types.h>
#include <qglobal.h>

#include "qtermsshrsa.h"

QTermSSHRSA::QTermSSHRSA()
{
	d_rsa = RSA_new();
	d_rsa->n = BN_new();
	d_rsa->e = BN_new();
}

QTermSSHRSA::~QTermSSHRSA()
{
	if (d_rsa != NULL)
		RSA_free(d_rsa);
}

void QTermSSHRSA::publicEncrypt(BIGNUM * out, BIGNUM * in)
{
	u_char *inbuf, *outbuf;
	int len, ilen, olen;

	if (BN_num_bits(d_rsa->e) < 2 || !BN_is_odd(d_rsa->e))
		qFatal("public_encrypt() exponent too small or not odd\n");

	olen = BN_num_bytes(d_rsa->n);
	outbuf = new u_char[olen];
	
	ilen = BN_num_bytes(in);
	inbuf = new u_char[ilen];
	BN_bn2bin(in, inbuf);

	if ((len = RSA_public_encrypt(ilen, inbuf, outbuf, d_rsa,
	    RSA_PKCS1_PADDING)) <= 0) //RSA_PKCS1_PADDING = 1
		qFatal("rsa_public_encrypt() failed\n");

	BN_bin2bn(outbuf, len, out);

	delete [] outbuf;
	delete [] inbuf;
}
/*
int QTermSSHRSA::private_decrypt(BIGNUM * out, BIGNUM * in)
{
	u_char *inbuf, *outbuf;
	int len, ilen, olen;

	olen = BN_num_bytes(d_rsa->n);
	outbuf = new u_char[olen];

	ilen = BN_num_bytes(in);
	inbuf = new u_char[ilen];
	BN_bn2bin(in, inbuf);

	if ((len = RSA_private_decrypt(ilen, inbuf, outbuf, d_rsa,
	    RSA_PKCS1_PADDING)) <= 0) {
		qFatal("rsa_private_decrypt() failed");
	} else {
		BN_bin2bn(outbuf, len, out);
	}

	delete [] outbuf;
	delete [] inbuf;
	return len;
}

// calculate p-1 and q-1 

void
QTermSSHRSA::generate_additional_parameters()
{
	BIGNUM *aux;
	BN_CTX *ctx;

	if ((aux = BN_new()) == NULL)
		qFatal("rsa_generate_additional_parameters: BN_new failed");
	if ((ctx = BN_CTX_new()) == NULL)
		qFatal("rsa_generate_additional_parameters: BN_CTX_new failed");

	BN_sub(aux, d_rsa->q, BN_value_one());
	BN_mod(d_rsa->dmq1, d_rsa->d, aux, ctx);

	BN_sub(aux, d_rsa->p, BN_value_one());
	BN_mod(d_rsa->dmp1, d_rsa->d, aux, ctx);

	BN_clear_free(aux);
	BN_CTX_free(ctx);
}
*/
