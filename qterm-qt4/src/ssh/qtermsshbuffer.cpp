#include <stdlib.h>
#include <stdio.h>

#include "qtermsshbuffer.h"
#include "getput.h"

//==============================================================================
// QTermSSHBuffer
//==============================================================================

QTermSSHBuffer::QTermSSHBuffer(int size)
{
	d_allocSize = size;
	d_bufferSize = 0;
	d_offset = 0;
	d_buffer = new u_char[d_allocSize];
}

QTermSSHBuffer::~QTermSSHBuffer()
{
	delete [] d_buffer;
	d_offset = 0;
	d_bufferSize = 0;
	d_allocSize = 0;
}

void QTermSSHBuffer::ensure(int len)
{
	if (len <= (d_allocSize - (d_offset + d_bufferSize)))
		return;
	else {
		d_allocSize = d_bufferSize + len;
		rebuffer();
	}
}

void QTermSSHBuffer::rebuffer()
{
	u_char * newBuffer;
	newBuffer = new u_char[d_allocSize];
	memset(newBuffer, 0, d_allocSize);
	memcpy(newBuffer, d_buffer+d_offset, d_bufferSize);
	delete [] d_buffer;
	d_buffer = newBuffer;
	d_offset = 0;
}

void QTermSSHBuffer::clear()
{
	memset(d_buffer, 0, d_allocSize);
	d_offset = 0;
	d_bufferSize = 0;
}

void QTermSSHBuffer::consume(int len)
{
	if (len > d_bufferSize)
		len = d_bufferSize;
	d_offset += len;
	d_bufferSize -= len;
}

void QTermSSHBuffer::putBuffer(const char * data, int len)
{
	if (len < 0){
		fprintf(stderr, "putBuffer: write data error: %d\n", len);
	}
	ensure(len);
	memcpy((d_buffer + d_offset + d_bufferSize), data, len);
	d_bufferSize += len;
}

void QTermSSHBuffer::getBuffer(char * data, int len)
{
	if (len <= d_bufferSize && len >= 0) {
		memcpy(data, d_buffer + d_offset, len);
		consume(len);
	}
	else
		fprintf(stderr, "getBuffer: read too many data %d\n", len);
}

//==============================================================================
// Store an BIGNUM in the buffer with a 2-byte msb first bit count, followed by
// (bits+7)/8 bytes of binary data, msb first.
//==============================================================================

void QTermSSHBuffer::putSSH1BN(BIGNUM * bignum)
{
	int bits = BN_num_bits(bignum);
	int bin_size = (bits + 7) / 8;
	u_char * buf = new u_char[bin_size];
	int oi;
	u_char msg[2];

	// Get the value of in binary
	oi = BN_bn2bin(bignum, buf);
	if (oi != bin_size)
		fprintf(stderr, "putSSH1BN: BN_bn2bin() failed: oi %d != bin_size %d\n", oi, bin_size);

	// Store the number of bits in the buffer in two bytes, msb first
	PUT_16BIT(msg, bits);
	putBuffer((char *)msg, 2);
	// Store the binary data.
	putBuffer((char *)buf, oi);
	
	memset(buf, 0, bin_size);
	delete [] buf;
}

//==============================================================================
// Retrieves a BIGNUM from the buffer.
//==============================================================================

void QTermSSHBuffer::getSSH1BN(BIGNUM * bignum)
{
	int bits, bytes;
	u_char buf[2];
	u_char * bin;

	// Get the number for bits.
	getBuffer((char *)buf, 2);
	bits = GET_16BIT(buf);
	// Compute the number of binary bytes that follow.
	bytes = (bits + 7) /8;
	if (bytes > 8 *1024) {
		fprintf(stderr, "getSSH1BN: cannot handle BN of size %d\n", bytes); 
		return;
	}
	if (len() < bytes) {
		fprintf(stderr, "getSSH1BN: input buffer too small\n");
		return;
	}
	bin = data();
	BN_bin2bn(bin, bytes, bignum);
	consume(bytes);
}

u_short QTermSSHBuffer::getWord()
{
	u_char buf[2];
	u_short data;
	
	getBuffer((char *)buf, 2);
	data = GET_16BIT(buf);
	return data;
}

void QTermSSHBuffer::putWord(u_short data)
{
	u_char buf[2];

	PUT_16BIT(buf, data);
	putBuffer((char *)buf, 2);
}

u_int QTermSSHBuffer::getInt()
{
	u_char buf[4];
	u_int data;
	getBuffer((char *)buf, 4);
	data = GET_32BIT(buf);
	return data;
}

void QTermSSHBuffer::putInt(u_int data)
{
	u_char buf[4];

	PUT_32BIT(buf, data);
	putBuffer((char *)buf, 4);
}

//==============================================================================
// Return a character from the buffer (0-255).
//==============================================================================

u_char QTermSSHBuffer::getByte()
{
	u_char ch;
	
	getBuffer((char *)&ch, 1);
	return ch;
}

//==============================================================================
// Stores a character in the buffer.
//==============================================================================

void QTermSSHBuffer::putByte(int data)
{
	u_char ch = data;
	
	putBuffer((char *)&ch, 1);
}

//==============================================================================
// Stores an arbitrary binary string in the buffer.
//==============================================================================

void QTermSSHBuffer::putString(const char * string)
{
	if (string == NULL){
		fprintf(stderr,"putString: s == NULL\n");
		return;
	}
	putInt(strlen(string));
	putBuffer(string, strlen(string));
}

//==============================================================================
// Return an arbitrary binary string from the buffer. The string cannot be 
// longer than 256k. The returned value points to memory allocated with new;
// It is the responsibility of the calling function to free the data. 
//==============================================================================

void * QTermSSHBuffer::getString(int * length)
{
	u_char * data;
	u_int len;

	// Get the length.
	len = getInt();
	if (len > d_bufferSize) {
		fprintf(stderr, "getString: bad string length %d\n", len);
		return 0;
	}
	// Allocate space for the string. Add one byte for a null character.
	data = new u_char[len + 1];
	// Get the string.
	getBuffer((char *)data, len);
	// Append a null character to make processing easier.
	data[len] = 0;
	if (length != NULL)
		* length = len + 1;
	// return the length of the string.
	return data;
}

// Dumps the contents of the buffer to stderr. */

void QTermSSHBuffer::dump()
{
	int i;
	u_char * ucp = data();

	for (i = 0; i < d_bufferSize; i++) {
		fprintf(stderr, "%02x", ucp[i]);
		//if ((i - d_offset)%16 == 15)
		if (i%16 == 15)
			fprintf(stderr, "\r\n");
		//else if ((i - d_offset)%2 == 1)
		else if (i%2 == 1)
			fprintf(stderr, " ");
	}
	fprintf(stderr, "\r\n");
}

