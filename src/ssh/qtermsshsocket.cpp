#include "qtermsshsocket.h"
#include "qtermsshpacket.h"
#include "qtermsshbuffer.h"
#include "qtermsshrsa.h"
#include "qtermsshkex.h"
#include "qtermsshconst.h"
#include "qtermsshauth.h"
#include "qtermsshsession.h"

//#include <q3socket.h>
#include <qmessagebox.h>

#define V1STR	"SSH-1.5-QTermSSH\n"
#define V2STR	"SSH-2.0-QTermSSH\n"

namespace QTerm
{
SSHSocket::SSHSocket(const char * sshuser, const char * sshpasswd)
{
	d_socket = new SocketPrivate();
	d_inBuffer = new SSHBuffer(1024);
	d_outBuffer = new SSHBuffer(1024);
	d_socketBuffer = new SSHBuffer(1024);
	d_state = BeforeSession;
	d_incomingPacket = new SSH1PacketReceiver;
	d_outcomingPacket = new SSH1PacketSender;
	d_kex = new SSH1Kex;
	d_auth = new SSH1PasswdAuth(sshuser, sshpasswd);
	d_session = new SSH1Session;

	connect(d_socket, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
	connect(d_socket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(d_socket, SIGNAL(connectionClosed()), this, SIGNAL(connectionClosed()));
	connect(d_socket, SIGNAL(delayedCloseFinished()), this, SIGNAL(delayedCloseFinished()));
	connect(d_socket, SIGNAL(error(int)), this, SIGNAL(error(int)));
	connect(d_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
	connect(d_socket, SIGNAL(SocketState(int)), this, SIGNAL(SocketState(int)));	
	connect(d_incomingPacket, SIGNAL(packetAvaliable(int)), this, SLOT(handlePacket(int)));
	connect(d_incomingPacket, SIGNAL(packetError(const char *)), this, SLOT(handleError(const char *)));
	connect(d_outcomingPacket, SIGNAL(dataToWrite()), this, SLOT(writeData()));
	connect(d_kex, SIGNAL(kexOK()), this, SLOT(kexOK()));
	connect(d_kex, SIGNAL(kexError(const char *)), this, SLOT(handleError(const char *)));
	connect(d_kex, SIGNAL(reKex()), d_incomingPacket, SLOT(resetEncryption()));
	connect(d_kex, SIGNAL(reKex()), d_outcomingPacket, SLOT(resetEncryption()));
	connect(d_kex, SIGNAL(startEncryption(const u_char *)), d_incomingPacket, SLOT(startEncryption(const u_char *)));
	connect(d_kex, SIGNAL(startEncryption(const u_char *)), d_outcomingPacket, SLOT(startEncryption(const u_char *)));
	connect(d_auth, SIGNAL(authOK()), this, SLOT(authOK()));
	connect(d_auth, SIGNAL(authError(const char *)), this, SLOT(handleError(const char *)));
	connect(d_session, SIGNAL(sessionOK()), this, SLOT(sessionOK()));
	connect(d_session, SIGNAL(readyRead()), this, SLOT(sessionReadyRead()));
}

SSHSocket::~SSHSocket()
{
	delete d_socket;
	delete d_inBuffer;
	delete d_outBuffer;
	delete d_socketBuffer;
	delete d_incomingPacket;
	delete d_outcomingPacket;
	delete d_kex;
	delete d_auth;
	delete d_session;
}

void SSHSocket::kexOK()
{
	//qDebug("Key exchange completed!");
	d_auth->initAuth(d_incomingPacket, d_outcomingPacket);
}

void SSHSocket::authOK()
{
	//qDebug("Auth completed!");
	d_session->initSession(d_incomingPacket, d_outcomingPacket);
}

void SSHSocket::sessionOK()
{
	//qDebug("Session Started!");
}

void SSHSocket::sessionReadyRead()
{
	u_char * data;
	int size;
	size = d_incomingPacket->packetLen();
	/*
	data = (u_char *)d_incomingPacket->getString(&size);
	qDebug("string size: %d", size);
	d_inBuffer->putBuffer((const char *) data, size);
	*/
	data = new u_char[size];
	d_incomingPacket->getBuffer((char *)data, size);
	d_inBuffer->putBuffer((const char *)data + 4, size - 4);
	//qDebug("dump inBuffer: ");
	//d_inBuffer->dump();
	delete [] data;
	emit readyRead();
}

unsigned long SSHSocket::socketWriteBlock(const char * data, unsigned long len)
{
	QByteArray to_write(data, len);
	return d_socket->writeBlock(to_write);
}

void SSHSocket::socketReadyRead()
{
	unsigned long size;

	switch (d_state) {
	case BeforeSession:
		{
		QByteArray str;
		int version;
		size = d_socket->bytesAvailable();
		str = d_socket->readBlock(size);
		//qDebug("Server message: %s", str);
		version = chooseVersion(str.data());
		//qDebug("SSH server version: %d", version);
		if (version == 1)
			socketWriteBlock(V1STR, 17);
		else if (version == 2) {
			handleError("It seems that your server can only use SSH2 which is  not yet supported.");
		}
		else {
			handleError("unknown SSH version, Check if you set the right server and port.");
			return;
		}
		d_state = SockSession;
		d_socket->flush();
		break;
		}
	case SockSession:
		parsePacket();
	}
}

void SSHSocket::parsePacket()
{
	unsigned long size;
	QByteArray data;
	size = d_socket->bytesAvailable();
	data = d_socket->readBlock(size);

	d_socketBuffer->putBuffer(data.data(), size);
	d_incomingPacket->parseData(d_socketBuffer);
}

int SSHSocket::chooseVersion(const QString & ver)
{
	if (ver.indexOf("SSH-") != 0)
		return -1;
	if (ver.at(4) == '1')
		return 1;
	else if (ver.at(4) == '2')
		return 2;
	else
		return -1;
}
	
void SSHSocket::connectToHost(const QString & host_name, quint16 port)
{
	d_state = BeforeSession;
	d_socket->connectToHost(host_name, port);
	d_kex->initKex(d_incomingPacket, d_outcomingPacket);
}

void SSHSocket::writeData()
{
	socketWriteBlock((const char *)d_outcomingPacket->d_output->data(), d_outcomingPacket->d_output->len());
	d_socket->flush();
}

void SSHSocket::handlePacket(int type)
{
	switch (type) {
	case SSH1_MSG_DISCONNECT:
		char * reason;
		reason = (char *)d_incomingPacket->getString();
		qDebug("Disconnect because: %s", reason);
		delete [] reason;
		break;
	default:
		return;
	}
}

unsigned long SSHSocket::bytesAvailable()
{
	return d_inBuffer->len();
}

QByteArray SSHSocket::readBlock(unsigned long size)
{
	qDebug("read Block");
	QByteArray data(size, 0);
	d_inBuffer->getBuffer(data.data(), size);
	return data;
}

long SSHSocket::writeBlock(const QByteArray & data)
{
	unsigned long size = data.size();
	d_outBuffer->putBuffer(data.data(), size);
	return size;
}

void SSHSocket::flush()
{
	int size;
	size = d_outBuffer->len();
	d_outcomingPacket->startPacket(SSH1_CMSG_STDIN_DATA);
	d_outcomingPacket->putInt(size);
	d_outcomingPacket->putBuffer((const char *)d_outBuffer->data(), size);
	d_outcomingPacket->write();
	d_outBuffer->consume(size);
}

void SSHSocket::close()
{
	d_socket->close();
}

void SSHSocket::handleError(const char * reason)
{
	close();
	QMessageBox::critical(0, "QTerm SSH Error", QString("Connection closed because:\n")+reason);
	emit connectionClosed();
}

void SSHSocket::setProxy( int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
	d_socket->setProxy(nProxyType, bAuth,
			strProxyHost, uProxyPort,
			strProxyUsr, strProxyPwd);
}

} // namespace QTerm

#include <qtermsshsocket.moc>

