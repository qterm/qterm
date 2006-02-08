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

QTermSSHSocket::QTermSSHSocket(const char * sshuser, const char * sshpasswd)
{
	d_socket = new QTermSocketPrivate();
	d_inBuffer = new QTermSSHBuffer(1024);
	d_outBuffer = new QTermSSHBuffer(1024);
	d_socketBuffer = new QTermSSHBuffer(1024);
	d_state = BeforeSession;
	d_incomingPacket = new QTermSSH1PacketReceiver;
	d_outcomingPacket = new QTermSSH1PacketSender;
	d_kex = new QTermSSH1Kex;
	d_auth = new QTermSSH1PasswdAuth(sshuser, sshpasswd);
	d_session = new QTermSSH1Session;

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

QTermSSHSocket::~QTermSSHSocket()
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

void QTermSSHSocket::kexOK()
{
	//qDebug("Key exchange completed!");
	d_auth->initAuth(d_incomingPacket, d_outcomingPacket);
}

void QTermSSHSocket::authOK()
{
	//qDebug("Auth completed!");
	d_session->initSession(d_incomingPacket, d_outcomingPacket);
}

void QTermSSHSocket::sessionOK()
{
	//qDebug("Session Started!");
}

void QTermSSHSocket::sessionReadyRead()
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

Q_ULONG QTermSSHSocket::socketWriteBlock(const char * data, Q_ULONG len)
{
	return d_socket->writeBlock(data, len);
}

void QTermSSHSocket::socketReadyRead()
{
	Q_ULONG size;

	switch (d_state) {
	case BeforeSession:
		char * str;
		int version;
		size = d_socket->bytesAvailable();
		str = new char[size];
		if (d_socket->readBlock(str, size) == -1){
			qDebug("Read error");
			return;
		}
		//qDebug("Server message: %s", str);
		version = chooseVersion(str);
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
		delete [] str;
		break;
	case SockSession:
		parsePacket();
	}
}

void QTermSSHSocket::parsePacket()
{
	Q_ULONG size;
	char * data;
	size = d_socket->bytesAvailable();
	data = new char[size];

	if (d_socket->readBlock(data, size) == -1) {
		qDebug("read error");
		delete [] data;
		return;
	}
	d_socketBuffer->putBuffer(data, size);
	d_incomingPacket->parseData(d_socketBuffer);

	delete [] data;
}

int QTermSSHSocket::chooseVersion(const QString & ver)
{
	if (ver.find("SSH-") != 0)
		return -1;
	if (ver.at(4) == '1')
		return 1;
	else if (ver.at(4) == '2')
		return 2;
	else
		return -1;
}
	
void QTermSSHSocket::connectToHost(const QString & host_name, Q_UINT16 port)
{
	d_state = BeforeSession;
	d_socket->connectToHost(host_name, port);
	d_kex->initKex(d_incomingPacket, d_outcomingPacket);
}

void QTermSSHSocket::writeData()
{
	socketWriteBlock((const char *)d_outcomingPacket->d_output->data(), d_outcomingPacket->d_output->len());
	d_socket->flush();
}

void QTermSSHSocket::handlePacket(int type)
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

Q_ULONG QTermSSHSocket::bytesAvailable()
{
	return d_inBuffer->len();
}

Q_LONG QTermSSHSocket::readBlock(char * data, Q_ULONG size)
{
	if (data == NULL)
		return -1;
	d_inBuffer->getBuffer(data, size);
	return size;
}

Q_LONG QTermSSHSocket::writeBlock(const char * data, Q_ULONG size)
{
	if (data == NULL)
		return -1;
	d_outBuffer->putBuffer(data, size);
	return size;
}

void QTermSSHSocket::flush()
{
	int size;
	size = d_outBuffer->len();
	d_outcomingPacket->startPacket(SSH1_CMSG_STDIN_DATA);
	d_outcomingPacket->putInt(size);
	d_outcomingPacket->putBuffer((const char *)d_outBuffer->data(), size);
	d_outcomingPacket->write();
	d_outBuffer->consume(size);
}

void QTermSSHSocket::close()
{
	d_socket->close();
}

void QTermSSHSocket::handleError(const char * reason)
{
	close();
	QMessageBox::critical(0, "QTerm SSH Error", QString("Connection closed because:\n")+reason);
	emit connectionClosed();
}

void QTermSSHSocket::setProxy( int nProxyType, bool bAuth,
			const QString& strProxyHost, Q_UINT16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
	d_socket->setProxy(nProxyType, bAuth,
			strProxyHost, uProxyPort,
			strProxyUsr, strProxyPwd);
}
#include <qtermsshsocket.moc>
