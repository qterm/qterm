//
// C++ Implementation: sshsocket
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "socket.h"
#include "packet.h"
#include "kex.h"
#include "auth.h"
#include "channel.h"
#include "ssh2.h"
#include "qtermsocket.h"
#include <QtDebug>
#include <QDataStream>
#include <QBuffer>

#include <openssl/bn.h>
// FIXME: smth have some problem with \r\n
#define QTERM_SSHV1_BANNER "SSH-1.5-QTermSSH\n"
#define QTERM_SSHV2_BANNER "SSH-2.0-QTermSSH\r\n"

namespace QTerm
{
SSH2SocketPriv::SSH2SocketPriv(QTermSocketPrivate * plainSocket, QByteArray & banner, QObject * parent)
        : SSHSocketPriv(parent), m_banner(banner), m_status(Init), m_sessionID(), m_channelList()
{
    m_sessionID = NULL;
    m_auth = NULL;
    m_inPacket = new SSH2InBuffer(plainSocket, this);
    m_outPacket = new SSH2OutBuffer(plainSocket, this);
    m_kex = new SSH2Kex(m_sessionID, m_inPacket, m_outPacket, m_banner, QTERM_SSHV2_BANNER, this);
    m_kex->sendKex();
    connect(m_kex, SIGNAL(kexFinished()), this, SLOT(slotKexFinished()));
    // connect ( m_inPacket, SIGNAL ( packetReady ( int ) ), this, SLOT ( newPacket ( int ) ) );
}

SSH2SocketPriv::~SSH2SocketPriv()
{}

void SSH2SocketPriv::slotKexFinished()
{
    m_kex->deleteLater();
    qDebug() << "kex finished";
    m_auth = new SSH2Auth(m_inPacket, m_outPacket);
    connect(m_auth, SIGNAL(authFinished()), this, SLOT(slotAuthFinished()));
    m_auth->requestAuthService();
}

void SSH2SocketPriv::slotAuthFinished()
{
    m_auth->deleteLater();
    qDebug() << "authFinished";
    m_channel = new SSH2Channel(m_inPacket, m_outPacket);
    connect(m_channel, SIGNAL(newChannel(int)), this, SLOT(slotNewChannel(int)));
    connect(m_channel, SIGNAL(dataReady(int)), this, SLOT(slotChannelData(int)));
    m_channel->openChannel();
}

void SSH2SocketPriv::slotNewChannel(int id)
{
    // TODO: identify different channels
    m_channelList << id;
}

void SSH2SocketPriv::slotChannelData(int id)
{
    if (id == 0)
        emit readyRead();
}

QByteArray SSH2SocketPriv::readData(unsigned long size)
{
    return m_channel->readData(0, size);
}

void SSH2SocketPriv::writeData(const QByteArray & data)
{
    m_channel->writeData(0, data);
}

unsigned long SSH2SocketPriv::bytesAvailable()
{
    m_channel->bytesAvailable(0);
}

SSH1SocketPriv::SSH1SocketPriv(QTermSocketPrivate * plainSocket, QByteArray & banner, QObject * parent)
{
    qDebug() << "init ssh1 session";
    m_inPacket = new SSH1InBuffer(plainSocket, this);
    m_outPacket = new SSH1OutBuffer(plainSocket, this);
    m_kex = new SSH1Kex(m_inPacket, m_outPacket, this);
    connect(m_kex, SIGNAL(kexFinished()), this, SLOT(slotKexFinished()));
}

SSH1SocketPriv::~SSH1SocketPriv()
{}

void SSH1SocketPriv::slotKexFinished()
{
    m_kex->deleteLater();
    qDebug() << "kex finished";
    m_auth = new SSH1Auth(m_inPacket, m_outPacket, this);
    connect(m_auth, SIGNAL(authFinished()), this, SLOT(slotAuthFinished()));
    m_auth->requestAuthService();
}

void SSH1SocketPriv::slotAuthFinished()
{
    m_auth->deleteLater();
    qDebug() << "authFinished";
    m_channel = new SSH1Channel(m_inPacket, m_outPacket, this);
//  connect ( m_channel, SIGNAL ( newChannel ( int ) ), this, SLOT ( slotNewChannel ( int ) ) );
    connect(m_channel, SIGNAL(dataReady()), this, SIGNAL(readyRead()));
//  m_channel->openChannel();
}

QByteArray SSH1SocketPriv::readData(unsigned long size)
{
    return m_channel->readData(size);
}

void SSH1SocketPriv::writeData(const QByteArray & data)
{
    m_channel->writeData(data);
}

unsigned long SSH1SocketPriv::bytesAvailable()
{
    m_channel->bytesAvailable();
}

SSHSocket::SSHSocket(QObject * parent)
        : QTermSocket(parent), m_data(), m_buffer(&m_data)
{
    m_socket = new QTermSocketPrivate(this);
    m_version = SSHUnknown;
    connect(m_socket, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(connectionClosed()), this, SIGNAL(connectionClosed()));
    connect(m_socket, SIGNAL(delayedCloseFinished()), this, SIGNAL(delayedCloseFinished()));
    connect(m_socket, SIGNAL(SocketState(int)), this, SIGNAL(SocketState(int)));

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void SSHSocket::setProxy(int nProxyType, //0-no proxy; 1-wingate; 2-sock4; 3-socks5
                         bool bAuth, // if authentation needed
                         const QString& strProxyHost, quint16 uProxyPort,
                         const QString& strProxyUsr, const QString& strProxyPwd)
{
    // TODO: proxy?
}

void SSHSocket::flush()
{}

void SSHSocket::close()
{
    // TODO: clean up the rest of it
    m_socket->close();
}

void SSHSocket::connectToHost(const QString & hostName, quint16 port)
{
    qDebug() << "connect to: " << hostName << port;
    m_socket->connectToHost(hostName, port);
}


QByteArray SSHSocket::readBlock(unsigned long size)
{
    return m_priv->readData(size);
}

long SSHSocket::writeBlock(const QByteArray & data)
{
    m_priv->writeData(data);
    return data.size();
}

unsigned long SSHSocket::bytesAvailable()
{
    return m_priv->bytesAvailable();
}

void SSHSocket::checkVersion(const QByteArray & banner)
{
    qDebug() << banner;
    QString server(banner);
    if (!server.startsWith("SSH-"))
        return;
    m_version = SSHV1;
    switch (m_version) {
    case SSHV1:
        m_socket->writeBlock(QTERM_SSHV1_BANNER);
        qDebug() << "Send banner version 1";
        break;
    case SSHV2:
        m_socket->writeBlock(QTERM_SSHV2_BANNER);
        qDebug() << "Send banner version 2";
        break;
    case SSHUnknown:
        qDebug() << "Unknown ssh version";
        break;
    default:
        break;
    }
}

void SSHSocket::readData()
{
    qDebug() << "Read SSH data!";
    qint64 nbyte = m_socket->bytesAvailable();
    QByteArray to_socket;
    QByteArray from_socket = m_socket->readBlock(nbyte);
    checkVersion(from_socket);
    if (m_version == SSHV2) {
        m_priv = new SSH2SocketPriv(m_socket, from_socket, this);
        disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(m_priv, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
//   m_socket->write ( QTERM_SSHV2_BANNER );
    } else if (m_version == SSHV1) {
        m_priv = new SSH1SocketPriv(m_socket, from_socket, this);
        disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(m_priv, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
//   m_socket->write ( QTERM_SSHV1_BANNER );
    }
}

}

#include "socket.moc"

