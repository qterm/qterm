#ifndef QTERMSOCKET_H
#define QTERMSOCKET_H

#include <qobject.h>

class QSocket;

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
	virtual void connectToHost(const QString & host, Q_UINT16 port) = 0;
	virtual void close() = 0;
	virtual Q_LONG readBlock(char * data, Q_ULONG maxlen) = 0;
	virtual Q_LONG writeBlock(const char * data, Q_ULONG len) = 0;
	virtual Q_ULONG bytesAvailable() = 0;
signals:
	void connected();
	void hostFound();
	void connectionClosed();
	void delayedCloseFinished();
	void readyRead();
	void error(int);
};

class QTermTelnetSocket : public QTermSocket
{
private:
	QSocket * d_socket;
public:
	QTermTelnetSocket();
	~QTermTelnetSocket();
	void flush();
	void connectToHost(const QString & host, Q_UINT16 port);
	void close();
	Q_LONG readBlock(char * data, Q_ULONG maxlen);
	Q_LONG writeBlock(const char * data, Q_ULONG len);
	Q_ULONG bytesAvailable();
};

#endif		//QTERMSOCKET_H
