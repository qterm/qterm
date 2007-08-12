#ifndef QTERMSSHMD5_H
#define QTERMSSHMD5_H

#include "types.h"
#include <openssl/md5.h>
#include "qtermsshhash.h"

namespace QTerm
{
class SSHMD5 : public SSHHash
{
protected:
	MD5_CTX d_md5;
public:
	SSHMD5()
		:SSHHash()
	{
		d_digestLength = 16;
		MD5_Init(&d_md5);
	}
	void update(u_char * data, int len);
	void final(u_char * digest);
};

} // namespace QTerm

#endif		//QTERMSSHMD5_H
