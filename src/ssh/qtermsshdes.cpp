#include "qtermsshconst.h"
#include "qtermsshdes.h"

#include <memory.h>
#include <openssl/des.h>

#define DES_ENCRYPT 1
#define DES_DECRYPT 0

namespace QTerm
{
//==============================================================================
//QTermSSH1DES3
//==============================================================================

QTermSSH1DES3::QTermSSH1DES3()
{
	d_name = "des3-cbc";
	d_blockSize = 8;
	d_keyBits = 16;
	d_code = SSH_CIPHER_3DES;

	memset(d_IV1, 0, sizeof(d_IV1));
	memset(d_IV2, 0, sizeof(d_IV2));
	memset(d_IV3, 0, sizeof(d_IV3));
}

void QTermSSH1DES3::decrypt(const u_char * source, u_char * dest, int len)
{
	DES_ncbc_encrypt(source, dest, len, &d_key3, &d_IV3, DES_DECRYPT);
	DES_ncbc_encrypt(dest, dest, len, &d_key2, &d_IV2, DES_ENCRYPT);
	DES_ncbc_encrypt(dest, dest, len, &d_key1, &d_IV1, DES_DECRYPT);
}

void QTermSSH1DES3::encrypt(const u_char * source, u_char * dest, int len)
{
	DES_ncbc_encrypt(source, dest, len, &d_key1, &d_IV1, DES_ENCRYPT);
	DES_ncbc_encrypt(dest, dest, len, &d_key2, &d_IV2, DES_DECRYPT);
	DES_ncbc_encrypt(dest, dest, len, &d_key3, &d_IV3, DES_ENCRYPT);
}

void QTermSSH1DES3::setIV(const u_char * data)
{
	memset(d_IV1, 0, sizeof(d_IV1));
	memset(d_IV2, 0, sizeof(d_IV2));
	memset(d_IV3, 0, sizeof(d_IV3));
}

void QTermSSH1DES3::setKey(const u_char * data)
{
	DES_cblock key;
	memset(key, 0, sizeof(key));
	memcpy(key, data, sizeof(key));
	DES_set_key(&key, &d_key1);
	data += 8;
	memset(key, 0, sizeof(key));
	memcpy(key, data, sizeof(key));
	DES_set_key(&key, &d_key2);
	data += 8;
	memset(key, 0, sizeof(key));
	memcpy(key, data, sizeof(key));
	DES_set_key(&key, &d_key3);
}

} // namespace QTerm

