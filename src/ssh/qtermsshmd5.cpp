#include "qtermsshmd5.h"

namespace QTerm
{
//==============================================================================
//QTermSSHMD5
//==============================================================================
void QTermSSHMD5::final(u_char * digest)
{
	MD5_Final(digest, &d_md5);
}

void QTermSSHMD5::update(u_char * data, int len)
{
	MD5_Update(&d_md5, data, len);
}

} // namespace QTerm

