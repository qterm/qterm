//qtermsshpacket.h
#ifndef QTERMSSHPACKET_H
#define QTERMSSHPACKET_H

#include <openssl/bn.h>
#include "types.h"
#include "qtermsshbuffer.h"

#ifndef QOBJECT_H
#include <qobject.h>
#endif

//class SSHBuffer;
namespace QTerm
{
class SSHCipher;


class SSHPacketSender : public QObject
{
	Q_OBJECT
protected:
	bool d_isEncrypt;
	SSHCipher * d_cscipher;
	SSHBuffer * d_buffer;
	virtual void makePacket() = 0;
public:
	SSHBuffer * d_output;
	SSHPacketSender();
	virtual ~SSHPacketSender();

	void startPacket(int pkt_type);
	void putByte(int data);
	void putInt(u_int data);
	virtual void putBN(BIGNUM * bignum) = 0;
	void putString(const char * string);
	void putBuffer(const char * data, int length);
	void write();
public slots:
	virtual void startEncryption(const u_char * sessionkey) = 0;
	virtual void resetEncryption() = 0;
signals:
	void dataToWrite();
};

class SSH1PacketSender : public SSHPacketSender
{
	Q_OBJECT
protected:
	void makePacket();
public:
	SSH1PacketSender()
		:SSHPacketSender()
	{
	}
	void putBN(BIGNUM * bn);
public slots:
	void startEncryption(const u_char * sessionkey);
	void resetEncryption();
};

class SSHPacketReceiver : public QObject
{
	Q_OBJECT
protected:
	bool d_isDecrypt;
	int d_packetType;
	int d_nextPacket;
	u_int d_totalLen;
	u_int d_padding;
	SSHBuffer * d_buffer;
	SSHCipher * d_sccipher;
	int debug;
	
public:
	SSHPacketReceiver();
	virtual ~SSHPacketReceiver();
	int packetType() const { return d_packetType; }
	virtual int packetLen() = 0;
	u_char getByte();
	u_int  getInt();
	virtual void getBN(BIGNUM * bignum) = 0;
	void * getString(int * length = NULL);
	void getBuffer(char * data, int length);
	virtual void parseData(SSHBuffer * input) = 0;
public slots:
	virtual void startEncryption(const u_char * sessionkey) = 0;
	virtual void resetEncryption() = 0;
signals:
	void packetAvaliable(int type);
	void packetError(const char * reason);
};

class SSH1PacketReceiver : public SSHPacketReceiver
{
	Q_OBJECT
private:
	u_int d_realLen;
public:
	SSH1PacketReceiver()
		:SSHPacketReceiver()
	{
	}
	int packetLen();
	void getBN(BIGNUM * bignum);
	void parseData(SSHBuffer * input);
	void onPacket();
public slots:
	void startEncryption(const u_char * sessionkey);
	void resetEncryption();
};

} // namespace QTerm

#endif		//qtermsshpacket.h

