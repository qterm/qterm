//qtermsshsocket.h
#ifndef QTERMSSHSOCKET_H
#define QTERMSSHSOCKET_H

#include "../qtermsocket.h"

class QSocket;
class QTermSSHPacketReceiver;
class QTermSSHPacketSender;
class QTermSSHBuffer;
class QTermSSHKex;
class QTermSSHAuth;
class QTermSSHSession;

class QTermSSHSocket : public QTermSocket
{
	Q_OBJECT
private:
	enum QTermSSHSocketState{
		BeforeSession,
		SockSession
	} d_state;
	QSocket * d_socket;
	QTermSSHBuffer * d_inBuffer;
	QTermSSHBuffer * d_outBuffer;
	QTermSSHPacketReceiver * d_incomingPacket;
	QTermSSHPacketSender * d_outcomingPacket;
	QTermSSHKex * d_kex;
	QTermSSHAuth * d_auth;
	QTermSSHSession * d_session;

	int chooseVersion(const QString & ver);
	Q_ULONG socketWriteBlock(const char * data, Q_ULONG len);
	void parsePacket();
		
private slots:
	void handlePacket(int type);
	void writeData();
	void kexOK();
	void authOK();
	void sessionOK();
	void sessionReadyRead();
	void socketReadyRead();
	void handleError(const char * reason);

public:
	QTermSSHSocket(const char * sshuser = NULL, const char * sshpasswd = NULL);
	~QTermSSHSocket();
	void connectToHost(const QString & host_name, Q_UINT16 port);
	Q_LONG readBlock(char * data, Q_ULONG size);
	Q_LONG writeBlock(const char * data, Q_ULONG size);
	Q_ULONG bytesAvailable();
	void flush();
	void close();
};

#endif		//QTERMSSHSOCKET_H
