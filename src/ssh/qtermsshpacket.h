//qtermsshpacket.h
#ifndef QTERMSSHPACKET_H
#define QTERMSSHPACKET_H

#include <openssl/bn.h>
#include "types.h"
#include "qtermsshbuffer.h"

#ifndef QOBJECT_H
#include <qobject.h>
#endif

//class QTermSSHBuffer;
class QTermSSHCipher;


class QTermSSHPacketSender : public QObject
{
	Q_OBJECT
protected:
	bool d_isEncrypt;
	QTermSSHCipher * d_cscipher;
	QTermSSHBuffer * d_buffer;
	virtual void makePacket() = 0;
public:
	QTermSSHBuffer * d_output;
	QTermSSHPacketSender();
	virtual ~QTermSSHPacketSender();

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

class QTermSSH1PacketSender : public QTermSSHPacketSender
{
	Q_OBJECT
protected:
	void makePacket();
public:
	QTermSSH1PacketSender()
		:QTermSSHPacketSender()
	{
	}
	void putBN(BIGNUM * bn);
public slots:
	void startEncryption(const u_char * sessionkey);
	void resetEncryption();
};

class QTermSSHPacketReceiver : public QObject
{
	Q_OBJECT
protected:
	bool d_isDecrypt;
	int d_packetType;
	int d_nextPacket;
	u_int d_totalLen;
	u_int d_padding;
	QTermSSHBuffer * d_buffer;
	QTermSSHCipher * d_sccipher;
	int debug;
	
public:
	QTermSSHPacketReceiver();
	virtual ~QTermSSHPacketReceiver();
	int packetType() const { return d_packetType; }
	virtual int packetLen() = 0;
	u_char getByte();
	u_int  getInt();
	virtual void getBN(BIGNUM * bignum) = 0;
	void * getString(int * length = NULL);
	void getBuffer(char * data, int length);
	virtual void parseData(QTermSSHBuffer * input) = 0;
public slots:
	virtual void startEncryption(const u_char * sessionkey) = 0;
	virtual void resetEncryption() = 0;
signals:
	void packetAvaliable(int type);
	void packetError(const char * reason);
};

class QTermSSH1PacketReceiver : public QTermSSHPacketReceiver
{
	Q_OBJECT
private:
	u_int d_realLen;
public:
	QTermSSH1PacketReceiver()
		:QTermSSHPacketReceiver()
	{
	}
	int packetLen();
	void getBN(BIGNUM * bignum);
	void parseData(QTermSSHBuffer * input);
	void onPacket();
public slots:
	void startEncryption(const u_char * sessionkey);
	void resetEncryption();
};

#endif		//qtermsshpacket.h
