//qtermsshsocket.h
#ifndef QTERMSSHSOCKET_H
#define QTERMSSHSOCKET_H

#include "../qtermsocket.h"

namespace QTerm
{
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
	QTermSSHSocket(const char * sshuser = NULL, const char * sshpasswd = NULL);
	~QTermSSHSocket();
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
