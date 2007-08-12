#ifndef QTERMSSHSESSION_H
#define QTERMSSHSESSION_H

#include "types.h"
#include <qobject.h>

namespace QTerm
{
class QTermSSHPacketReceiver;
class QTermSSHPacketSender;

class QTermSSHSession : public QObject
{
	Q_OBJECT
protected:
	bool d_closed;
	
	QTermSSHPacketReceiver * d_incomingPacket;
	QTermSSHPacketSender * d_outcomingPacket;
	
public:
	QTermSSHSession()
	{
	}
	virtual ~QTermSSHSession()
	{
	}
	virtual void initSession(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output) = 0;
	virtual void closeConnection(char * reason) = 0;
	// virtual void handleDisconntion(int reason);
	virtual void changeWindowSize(int col, int row) = 0;
	
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void sessionOK();
	void readyRead();
};

class QTermSSH1Session : public QTermSSHSession
{
	Q_OBJECT
private:
	enum QTermSSH1SessionState {
		BEGIN_SERVICE,
		REQPTY_SENT,
		//REQCMD_SENT,
		SERVICE_OK
	} d_serviceState;
	
public:
	QTermSSH1Session();
	void initSession(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output);
	void closeConnection(char * reason);
	void changeWindowSize(int col, int row);
public slots:
	void handlePacket(int type);
};

} // namespace QTerm

#endif		//QTERMSSHSESSION_H
