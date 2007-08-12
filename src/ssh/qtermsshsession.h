#ifndef QTERMSSHSESSION_H
#define QTERMSSHSESSION_H

#include "types.h"
#include <qobject.h>

namespace QTerm
{
class SSHPacketReceiver;
class SSHPacketSender;

class SSHSession : public QObject
{
	Q_OBJECT
protected:
	bool d_closed;
	
	SSHPacketReceiver * d_incomingPacket;
	SSHPacketSender * d_outcomingPacket;
	
public:
	SSHSession()
	{
	}
	virtual ~SSHSession()
	{
	}
	virtual void initSession(SSHPacketReceiver * packet, SSHPacketSender * output) = 0;
	virtual void closeConnection(char * reason) = 0;
	// virtual void handleDisconntion(int reason);
	virtual void changeWindowSize(int col, int row) = 0;
	
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void sessionOK();
	void readyRead();
};

class SSH1Session : public SSHSession
{
	Q_OBJECT
private:
	enum SSH1SessionState {
		BEGIN_SERVICE,
		REQPTY_SENT,
		//REQCMD_SENT,
		SERVICE_OK
	} d_serviceState;
	
public:
	SSH1Session();
	void initSession(SSHPacketReceiver * packet, SSHPacketSender * output);
	void closeConnection(char * reason);
	void changeWindowSize(int col, int row);
public slots:
	void handlePacket(int type);
};

} // namespace QTerm

#endif		//QTERMSSHSESSION_H
