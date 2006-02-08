#ifndef QTERMSSHKEX_H
#define QTERMSSHKEX_H

#include "types.h"
#include <openssl/bn.h>
#include <qobject.h>

#include "qtermsshrsa.h"
#include "qtermsshpacket.h"
#include "qtermsshconst.h"

class QTermSSHPacketReceiver;
class QTermSSHPacketSender;

class QTermSSHKex : public QObject
{
	Q_OBJECT
protected:
	QTermSSHPacketReceiver * d_incomingPacket;
	QTermSSHPacketSender * d_outcomingPacket;
public:
	QTermSSHKex()
	{
	}
	virtual void initKex(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output) = 0;
	
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void startEncryption(const u_char * sessionkey);
	void kexOK();
	void reKex();
	void kexError(const char * reason);
};

class QTermSSH1Kex : public QTermSSHKex
{
	Q_OBJECT
private:
	enum QTermSSH1KexState {
		BEFORE_PUBLICKEY,
		SESSIONKEY_SENT,
		KEYEX_OK
	} d_state;
	bool d_first_kex;
	QTermSSHRSA * d_hostKey;
	QTermSSHRSA * d_servKey;
	u_char d_cookie[8];
	int d_servFlag, d_sciphers, d_sauth;
	u_char d_sessionid[16];
	u_char d_sessionkey[32];
	
	void makeSessionId();
	void makeSessionKey();
public:
	QTermSSH1Kex();
	~QTermSSH1Kex();
	void initKex(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output);
public slots:
	void handlePacket(int type);
};

#endif		//QTERMSSHKEX_H
