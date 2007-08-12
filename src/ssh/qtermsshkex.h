#ifndef QTERMSSHKEX_H
#define QTERMSSHKEX_H

#include "types.h"
#include <openssl/bn.h>
#include <qobject.h>

#include "qtermsshrsa.h"
#include "qtermsshpacket.h"
#include "qtermsshconst.h"

namespace QTerm
{
class SSHPacketReceiver;
class SSHPacketSender;

class SSHKex : public QObject
{
	Q_OBJECT
protected:
	SSHPacketReceiver * d_incomingPacket;
	SSHPacketSender * d_outcomingPacket;
public:
	SSHKex()
	{
	}
	virtual void initKex(SSHPacketReceiver * packet, SSHPacketSender * output) = 0;
	
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void startEncryption(const u_char * sessionkey);
	void kexOK();
	void reKex();
	void kexError(const char * reason);
};

class SSH1Kex : public SSHKex
{
	Q_OBJECT
private:
	enum SSH1KexState {
		BEFORE_PUBLICKEY,
		SESSIONKEY_SENT,
		KEYEX_OK
	} d_state;
	bool d_first_kex;
	SSHRSA * d_hostKey;
	SSHRSA * d_servKey;
	u_char d_cookie[8];
	int d_servFlag, d_sciphers, d_sauth;
	u_char d_sessionid[16];
	u_char d_sessionkey[32];
	
	void makeSessionId();
	void makeSessionKey();
public:
	SSH1Kex();
	~SSH1Kex();
	void initKex(SSHPacketReceiver * packet, SSHPacketSender * output);
public slots:
	void handlePacket(int type);
};

} // namespace QTerm

#endif		//QTERMSSHKEX_H
