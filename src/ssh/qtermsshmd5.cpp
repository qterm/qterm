#include "qtermsshmd5.h"

namespace QTerm
{
//==============================================================================
//SSHMD5
//==============================================================================
void SSHMD5::final(u_char * digest)
{
	MD5_Final(digest, &d_md5);
}

void SSHMD5::update(u_char * data, int len)
{
	MD5_Update(&d_md5, data, len);
}

} // namespace QTerm

