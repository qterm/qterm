#include "qtermsocket.h"
#include <qsocket.h>

//==============================================================================
//QTermTelnetSocket
//==============================================================================

QTermTelnetSocket::QTermTelnetSocket()
	: QTermSocket()
{
	d_socket = new QSocket(this);
	connect(d_socket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(d_socket, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
	connect(d_socket, SIGNAL(connectionClosed()), this, SIGNAL(connectionClosed()));
	connect(d_socket, SIGNAL(delayedCloseFinished()), this, SIGNAL(delayedCloseFinished()));
	connect(d_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
	connect(d_socket, SIGNAL(error(int)), this, SIGNAL(error(int)));
}

QTermTelnetSocket::~QTermTelnetSocket()
{
	delete d_socket;
}

void QTermTelnetSocket::flush()
{
	d_socket->flush();
}

void QTermTelnetSocket::connectToHost(const QString & host, Q_UINT16 port)
{
	d_socket->connectToHost(host, port);
}

void QTermTelnetSocket::close()
{
	d_socket->close();
}

Q_LONG QTermTelnetSocket::readBlock(char * data, Q_ULONG maxlen)
{
	return d_socket->readBlock(data, maxlen);
}

Q_LONG QTermTelnetSocket::writeBlock(const char * data, Q_ULONG len)
{
	return d_socket->writeBlock(data, len);
}

Q_ULONG QTermTelnetSocket::bytesAvailable()
{
	return d_socket->bytesAvailable();
}
