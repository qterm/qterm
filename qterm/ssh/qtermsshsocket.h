//qtermsshsocket.h
#ifndef QTERMSSHSOCKET_H
#define QTERMSSHSOCKET_H

#include "../qtermsocket.h"

class QTermSocketPrivate;
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
	QTermSocketPrivate * d_socket;
	QTermSSHBuffer * d_inBuffer;
	QTermSSHBuffer * d_outBuffer;
	QTermSSHBuffer * d_socketBuffer;
	QTermSSHPacketReceiver * d_incomingPacket;
	QTermSSHPacketSender * d_outcomingPacket;
	QTermSSHKex * d_kex;
	QTermSSHAuth * d_auth;
	QTermSSHSession * d_session;
	bool d_allowInput;

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
	void setProxy( int nProxyType,//0-no proxy; 1-wingate; 2-sock4; 3-socks5
		bool bAuth,	// if authentation needed
		const QString& strProxyHost,Q_UINT16 uProxyPort,
		const QString& strProxyUsr,const QString& strProxyPwd);	
	void connectToHost(const QString & host_name, Q_UINT16 port);
	Q_LONG readBlock(char * data, Q_ULONG size);
	Q_LONG writeBlock(const char * data, Q_ULONG size);
	Q_ULONG bytesAvailable();
	void flush();
	void close();
	bool allowInput();
};

#endif		//QTERMSSHSOCKET_H
