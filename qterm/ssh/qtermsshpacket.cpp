#include "qtermsshbuffer.h"
#include "qtermsshpacket.h"
#include "qtermsshdes.h"

#include "getput.h"
#include "crc32.h"

//==============================================================================
//QTermSSHPacketSender
//==============================================================================

QTermSSHPacketSender::QTermSSHPacketSender()
{
	d_buffer = new QTermSSHBuffer(1024);
	d_output = new QTermSSHBuffer(1024);
	d_isEncrypt = false;
}

QTermSSHPacketSender::~QTermSSHPacketSender()
{
	delete d_buffer;
	delete d_output;
	if (d_isEncrypt)
		delete d_cscipher;
}

void QTermSSHPacketSender::putBuffer(const char * data, int len)
{
	d_buffer->putBuffer(data, len);
}

void QTermSSHPacketSender::putByte(int data)
{
	d_buffer->putByte(data);
}

void QTermSSHPacketSender::putInt(u_int data)
{
	d_buffer->putInt(data);
}

void QTermSSHPacketSender::putString(const char * string)
{
	d_buffer->putString(string);
}

void QTermSSHPacketSender::startPacket(int pkt_type)
{
	d_buffer->clear();
	d_buffer->putByte(pkt_type);
}

void QTermSSHPacketSender::write()
{
	makePacket();
	emit dataToWrite();
}

//==============================================================================
//QTermSSH1PacketSender
//==============================================================================

void QTermSSH1PacketSender::putBN(BIGNUM * bn)
{
	d_buffer->putSSH1BN(bn);
}

void QTermSSH1PacketSender::makePacket()
{
	int len, padding, i;
	u_int32_t rand = 0;
	//QTermSSHBuffer * output;
	delete d_output;

	len = d_buffer->len() + 4; //CRC32
	padding = 8 - (len % 8);
	// qDebug("length: %d, padding: %d", len, padding);
	d_output = new QTermSSHBuffer(len + padding + 4); //pktlen and crc32
	d_output->putInt(len);
	
	for (i = 0; i < padding; i++) {
		if (i % 4 == 0)
			rand = random();
		d_output->putByte(rand & 0xff);
		rand >>= 8;
	}

	d_output->putBuffer((const char *)d_buffer->data(), d_buffer->len());
	d_output->putInt(ssh_crc32(d_output->data() + 4, d_output->len() - 4));
	
	// For debug:
	// qDebug("outcoming packet dump:");
	// d_output->dump();
	
	if (d_isEncrypt)
		d_cscipher->encrypt(d_output->data() + 4, d_output->data() + 4, d_output->len() - 4);

}

void QTermSSH1PacketSender::startEncryption(const u_char * sessionkey)
{
	d_cscipher = new QTermSSH1DES3;
	d_cscipher->setIV(NULL);
	d_cscipher->setKey(sessionkey);
	d_isEncrypt = true;
}

void QTermSSH1PacketSender::resetEncryption()
{
	if (d_isEncrypt) {
		delete d_cscipher;
		d_isEncrypt = false;
	}
}

//==============================================================================
//QTermSSHPacketReceiver
//==============================================================================

QTermSSHPacketReceiver::QTermSSHPacketReceiver()
{
	d_buffer = new QTermSSHBuffer(1024);
	d_isDecrypt = false;
	d_nextPacket = 0;
}

QTermSSHPacketReceiver::~QTermSSHPacketReceiver()
{
	delete d_buffer;
	if (d_isDecrypt)
		delete d_sccipher;
}

void QTermSSHPacketReceiver::getBuffer(char * data, int length)
{
	d_buffer->getBuffer(data, length);
}

u_char QTermSSHPacketReceiver::getByte()
{
	return d_buffer->getByte();
}

u_int QTermSSHPacketReceiver::getInt()
{
	return d_buffer->getInt();
}

void * QTermSSHPacketReceiver::getString(int * length)
{
	return d_buffer->getString(length);
}
//==============================================================================
//QTermSSH1PacketReceiver
//==============================================================================

void QTermSSH1PacketReceiver::getBN(BIGNUM * bignum)
{
	d_buffer->getSSH1BN(bignum);
}

//==============================================================================
//
//Packet Structure:
//  --------------------------------------------------------------------------
//  | length | padding  | type  |                data                | crc32 |
//  --------------------------------------------------------------------------
//  | uint32 | 1-7bytes | uchar |                                    | 4bytes|
//  --------------------------------------------------------------------------
//  encrypt = padding + type + data + crc32
//  length =  type + data + crc32
//
//==============================================================================

void QTermSSH1PacketReceiver::parseData(QTermSSHBuffer * input)
{
	u_int mycrc, gotcrc;
	u_char * buf;
	u_char * targetData;
	u_char * sourceData;

	// Get the length of the packet.
	// fprintf(stderr, "input packet len: %d\n", input->len());
	while (input->len() > 0) {
		// qDebug("incoming packet dump:");
		// input->dump();
		if (input->len() < 4) {
			qDebug("parseData_readlen: packet too small");
			return;
		}
		buf = input->data();
		d_realLen = GET_32BIT(buf);
		// d_realLen = input->getInt();
		// qDebug("incoming packet length: %d", (d_realLen + 8) & ~7);
		if (d_realLen > SSH_BUFFER_MAX) {
			emit packetError("parseData: packet too big");
			return;
		}
		d_totalLen = (d_realLen + 8) & ~7;
		d_padding = d_totalLen - d_realLen;
		// qDebug("parseData: padding %d\n", d_padding);
		d_realLen -= 5;
		d_buffer->clear();

		// Get the data of the packet.
		if (input->len() - 4 < d_totalLen) {
			qDebug("parseData_readdata: packet too small\n");
			return;
		}
		d_realLen = input->getInt() - 5;
		targetData = new u_char [d_totalLen];
		sourceData = new u_char [d_totalLen];
		memset(targetData, 0, d_totalLen);
		memset(sourceData, 0, d_totalLen);
		// qDebug("packet length remain: %d", input->len());
		input->getBuffer((char *) sourceData, d_totalLen);
		if (d_isDecrypt)
			d_sccipher->decrypt(sourceData, targetData, d_totalLen);
		else
			memcpy(targetData, sourceData, d_totalLen);
		d_buffer->putBuffer((char *)targetData, d_totalLen);

		// For debug:
		// qDebug("After decryption:");
		// d_buffer->dump();
	 
		// Check the crc32.
		buf = d_buffer->data() + d_totalLen - 4;
		mycrc = GET_32BIT(buf);
		gotcrc = ssh_crc32(d_buffer->data(), d_totalLen - 4);
		if (mycrc != gotcrc) {
			emit packetError("parseData: bad CRC32");
			break;
		}
		// Drop the padding.
		d_buffer->consume(d_padding);

		d_packetType = d_buffer->getByte();
	
		onPacket();
		delete [] sourceData;
		delete [] targetData;
	}
}

void QTermSSH1PacketReceiver::onPacket()
{
	emit packetAvaliable(d_packetType);
}

void QTermSSH1PacketReceiver::startEncryption(const u_char * sessionkey)
{
	qDebug("We start encrypt communication\n");
	d_sccipher = new QTermSSH1DES3;
	d_sccipher->setIV(NULL);
	d_sccipher->setKey(sessionkey);
	d_isDecrypt = true;
}

void QTermSSH1PacketReceiver::resetEncryption()
{
	if (d_isDecrypt) {
		delete d_sccipher;
		d_isDecrypt = false;
	}
}
int QTermSSH1PacketReceiver::packetLen()
{
	return d_realLen;
}
