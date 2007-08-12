#ifndef QTERMSSHCIPHER_H
#define QTERMSSHCIPHER_H

#include "types.h"
#include <stdlib.h>
#include "qtermsshconst.h"

namespace QTerm
{
class QTermSSHCipher
{
protected:
	int d_blockSize;
	char * d_name;
	int d_code;
	int d_keyBits;
public:
	QTermSSHCipher() {
		d_blockSize = 0;
		d_name = NULL;
		d_code = 0;
		d_keyBits = 0;
	}

	virtual ~QTermSSHCipher()
	{
	}
	
	int code() const { return d_code; }
	int blockSize() const { return d_blockSize; }
	char * name() const { return d_name; }
	int keyBits() const { return d_keyBits; }

	virtual void setIV(const u_char * data) = 0;
	virtual void setKey(const u_char * data) = 0;
	virtual void encrypt(const u_char * source, u_char * dest, int len) = 0;
	virtual void decrypt(const u_char * source, u_char * dest, int len) = 0;
};

} // namespace QTerm

#endif		// QTERMSSHCIPHER_H
