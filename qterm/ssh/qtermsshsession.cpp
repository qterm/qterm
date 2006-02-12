#include "qtermsshsession.h"
#include "qtermsshconst.h"
#include "qtermsshpacket.h"

//==============================================================================
//QTermSSH1Session
//==============================================================================

QTermSSH1Session::QTermSSH1Session()
	: QTermSSHSession()
{
	d_serviceState = QTermSSH1Session::BEGIN_SERVICE;
}

void QTermSSH1Session::initSession(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output)
{
	d_incomingPacket = packet;
	d_outcomingPacket = output;
	d_incomingPacket->disconnect(this);
	connect(d_incomingPacket, SIGNAL(packetAvaliable(int)), this, SLOT(handlePacket(int)));
	d_outcomingPacket->startPacket(SSH1_CMSG_REQUEST_PTY);
	// pty request is of no use in BBS, but we do this
	d_outcomingPacket->putString("vt100");
	d_outcomingPacket->putInt(24);
	d_outcomingPacket->putInt(80);
	d_outcomingPacket->putInt(0);
	d_outcomingPacket->putInt(0);
	d_outcomingPacket->putByte(0);
	d_outcomingPacket->write();
	d_serviceState = QTermSSH1Session::REQPTY_SENT;
}

void QTermSSH1Session::changeWindowSize(int col, int row)
{
	d_outcomingPacket->startPacket(SSH1_CMSG_WINDOW_SIZE);
	d_outcomingPacket->putInt(row);
	d_outcomingPacket->putInt(col);
	d_outcomingPacket->putInt(0);
	d_outcomingPacket->putInt(0);
	d_outcomingPacket->write();
}

void QTermSSH1Session::closeConnection(char * reason)
{
	d_outcomingPacket->startPacket(SSH1_MSG_DISCONNECT);
	d_outcomingPacket->putString(reason);
	d_outcomingPacket->write();
	d_closed = true;
}

void QTermSSH1Session::handlePacket(int type)
{
	int i;
	switch (d_serviceState) {
	case BEGIN_SERVICE:
		qDebug("Session: We should not be here");
		break;
	case REQPTY_SENT:
		if (type != SSH1_SMSG_SUCCESS)
			qFatal("Server refused pty allocation!");
		d_outcomingPacket->startPacket(SSH1_CMSG_EXEC_SHELL);
		d_outcomingPacket->write();
		emit sessionOK();
		d_serviceState = QTermSSH1Session::SERVICE_OK;
		break;
	case SERVICE_OK:
		switch (type) {
		case SSH1_SMSG_STDOUT_DATA:
		case SSH1_SMSG_STDERR_DATA:
			emit readyRead();
			break;
		case SSH1_SMSG_X11_OPEN:
		case SSH1_SMSG_AGENT_OPEN:
		case SSH1_MSG_PORT_OPEN:
			i = d_incomingPacket->getInt();
			d_outcomingPacket->startPacket(SSH1_MSG_CHANNEL_OPEN_FAILURE);
			d_outcomingPacket->putInt(i);
			d_outcomingPacket->write();
			break;
		case SSH1_SMSG_EXIT_STATUS:
			d_outcomingPacket->startPacket(SSH1_CMSG_EXIT_CONFIRMATION);
			d_outcomingPacket->write();
			closeConnection("end");
			d_closed = true;
			break;
		case SSH1_SMSG_SUCCESS:
		case SSH1_SMSG_FAILURE:
			break;
		default:
			qDebug("unimplemented message");
		}
	}
}

#include <qtermsshsession.moc>
