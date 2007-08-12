//qtermsshsocket.h
#ifndef QTERMSSHSOCKET_H
#define QTERMSSHSOCKET_H

#include "../qtermsocket.h"

namespace QTerm
{
class SocketPrivate;
class SSHPacketReceiver;
class SSHPacketSender;
class SSHBuffer;
class SSHKex;
class SSHAuth;
class SSHSession;

class SSHSocket : public Socket
{
	Q_OBJECT
private:
	enum SSHSocketState{
		BeforeSession,
		SockSession
	} d_state;
	SocketPrivate * d_socket;
	SSHBuffer * d_inBuffer;
	SSHBuffer * d_outBuffer;
	SSHBuffer * d_socketBuffer;
	SSHPacketReceiver * d_incomingPacket;
	SSHPacketSender * d_outcomingPacket;
	SSHKex * d_kex;
	SSHAuth * d_auth;
	SSHSession * d_session;

	int chooseVersion(const QString & ver);
	unsigned long socketWriteBlock(const char * data, unsigned long len);
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
	SSHSocket(const char * sshuser = NULL, const char * sshpasswd = NULL);
	~SSHSocket();
	void setProxy( int nProxyType,//0-no proxy; 1-wingate; 2-sock4; 3-socks5
		bool bAuth,	// if authentation needed
		const QString& strProxyHost,quint16 uProxyPort,
		const QString& strProxyUsr,const QString& strProxyPwd);	
	void connectToHost(const QString & host_name, quint16 port);
	QByteArray readBlock(unsigned long size);
	long writeBlock(const QByteArray & data);
	unsigned long bytesAvailable();
	void flush();
	void close();
};

} // namespace QTerm

#endif		//QTERMSSHSOCKET_H
