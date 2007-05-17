#ifndef QTERMSOCKET_H
#define QTERMSOCKET_H

// _OS_X_ not defined if i dont include it
#include <qglobal.h>
// different 
#if defined(Q_OS_WIN32) || defined(_OS_WIN32_)
	#include <winsock2.h>
#elif defined(Q_OS_BSD4) || defined(_OS_FREEBSD_) \
	|| defined(Q_OS_MACX) || defined(Q_OS_DARWIN)
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#else
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
#endif

#include <qobject.h>

class QTcpSocket;

class QTermSocketPrivate : public QObject
{
	Q_OBJECT
public:
	QTermSocketPrivate();
	~QTermSocketPrivate();

	void flush();
	void setProxy(int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd);
	void connectToHost(const QString & hostname, quint16 portnumber);
	void close();
	QByteArray readBlock(unsigned long maxlen);
	long writeBlock(const QByteArray & data);
	unsigned long bytesAvailable();

signals:
	void connected();
	void hostFound();
	void connectionClosed();
	void delayedCloseFinished();
	void readyRead();
	void error(int);
	void SocketState(int);

protected slots:
	void socketConnected();
	void socketReadyRead();	
	
protected:
	// socks5 function
	void socks5_connect();
	void socks5_auth();
	void socks5_reply( const QByteArray&, int );

private:
	// proxy 
	int 		proxy_type;
	QString 	proxy_host;
	QString 	proxy_usr;
	quint16  	proxy_port;
	QString 	proxy_pwd;
	QString 	host;
	quint16 	port;
	int			proxy_state;
	bool		bauth;

	struct sockaddr_in  addr_host;

	QTcpSocket *m_socket;
};

// virtual base class for QTermTelnetSocket and QTermSSHSocket
class QTermSocket : public QObject
{
	Q_OBJECT
public:
	QTermSocket()
	{
	}
	virtual ~QTermSocket()
	{
	}
	virtual void flush() = 0;
	virtual void setProxy(int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd) = 0;
	virtual void connectToHost(const QString & host, quint16 port) = 0;
	virtual void close() = 0;
	virtual QByteArray readBlock(unsigned long maxlen) = 0;
	virtual long writeBlock(const QByteArray & data) = 0;
	virtual unsigned long bytesAvailable() = 0;
signals:
	void connected();
	void hostFound();
	void connectionClosed();
	void delayedCloseFinished();
	void readyRead();
	void error(int);
	void SocketState(int);
};

class QTermTelnetSocket : public QTermSocket
{
private:
	QTermSocketPrivate * d_socket;
public:
	QTermTelnetSocket();
	~QTermTelnetSocket();
	void flush();
	void setProxy(int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd);
	void connectToHost(const QString & host, quint16 port);
	void close();
	QByteArray readBlock(unsigned long maxlen);
	long writeBlock(const QByteArray & data);
	unsigned long bytesAvailable();
};

#endif		//QTERMSOCKET_H
