#ifndef QTERMSSHHASH_H
#define QTERMSSHHASH_H

#include "types.h"
namespace QTerm
{
class QTermSSHHash
{
protected:
	int d_digestLength;
	int digestLength() const { return d_digestLength; }
public:
	QTermSSHHash()	{
		d_digestLength = 0;
	}
	virtual void update(u_char * data, int len) = 0;
	virtual void final(u_char * data) = 0;
};

} // namespace QTerm

#endif		//QTERMSSHHASH_H
