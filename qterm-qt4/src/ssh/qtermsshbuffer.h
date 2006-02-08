//qtermsshbuffer.h
#ifndef QTERMSSHBUFFER_H
#define QTERMSSHBUFFER_H

#include <openssl/bn.h>
#include "types.h"
#include <string.h>

#define SSH_BUFFER_MAX 10000000

class QTermSSHBuffer
{
private:
	u_char	* d_buffer;
	int	d_offset;
	int	d_bufferSize;
	int	d_allocSize;
	void	ensure(int len);
	void	rebuffer();
public:
	QTermSSHBuffer(int size);
	~QTermSSHBuffer();
	
	u_char * data() { return d_buffer + d_offset; }
	int len() const { return d_bufferSize; }
	void	consume(int len);

	void clear();

	void putBuffer(const char * data, int len);
	void getBuffer(char * data, int len);

	void putSSH1BN(BIGNUM * bignum);
	void getSSH1BN(BIGNUM * bignum);

	void putInt(u_int data);
	u_int getInt();

	void putWord(u_short data);
	u_short getWord();

	void putByte(int data);
	u_char getByte();

	void putString(const char * string);
	void * getString(int * length = NULL);

	void dump();
};

#endif		//qtermsshbuffer.h
