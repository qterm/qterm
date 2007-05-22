//
// C++ Interface: sshsocket
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_SOCKET_H
#define SSH_SOCKET_H

#include <QObject>
#include "qtermsocket.h"

namespace QTerm
{
class SSH2InBuffer;
class SSH2OutBuffer;
class SSH1InBuffer;
class SSH1OutBuffer;
class SSH2Kex;
class SSH1Kex;
class SSH2SocketPriv;
class SSH2Auth;
class SSH1Auth;
class SSH2Channel;
class SSH1Channel;

/**
 @author hooey <hephooey@gmail.com>
*/
class SSHSocketPriv : public QObject
{
    Q_OBJECT
public:
    SSHSocketPriv(QObject * parent = 0)
            : QObject(parent)
    {}
    virtual ~SSHSocketPriv()
    {}
    virtual QByteArray readData(unsigned long size) = 0;
    virtual void writeData(const QByteArray & data) = 0;
    virtual unsigned long bytesAvailable() = 0;
signals:
    void readyRead();
};

class SSH2SocketPriv : public SSHSocketPriv
{
    Q_OBJECT
public:
    SSH2SocketPriv(QTermSocketPrivate * plainSocket, QByteArray & banner, QObject * parent = 0);
    ~SSH2SocketPriv();
    QByteArray readData(unsigned long size);
    void writeData(const QByteArray & data);
    unsigned long bytesAvailable();
private slots:
    void slotKexFinished();
    void slotAuthFinished();
    void slotNewChannel(int id);
    void slotChannelData(int id);
private:
    enum SSHStatus
    {
        Init, Kex, Unknown
    };
    SSH2InBuffer * m_inPacket;
    SSH2OutBuffer * m_outPacket;
    SSH2Kex * m_kex;
    SSH2Auth * m_auth;
    SSH2Channel * m_channel;
    QByteArray m_banner;
    SSHStatus m_status;
    QByteArray * m_sessionID;
    QList<uint> m_channelList;
};

class SSH1SocketPriv : public SSHSocketPriv
{
    Q_OBJECT
public:
    SSH1SocketPriv(QTermSocketPrivate * plainSocket, QByteArray & banner, QObject * parent = 0);
    ~SSH1SocketPriv();
    QByteArray readData(unsigned long size);
    void writeData(const QByteArray & data);
    unsigned long bytesAvailable();
private slots:
    void slotKexFinished();
    void slotAuthFinished();
private:
    SSH1InBuffer * m_inPacket;
    SSH1OutBuffer * m_outPacket;
    SSH1Kex * m_kex;
    SSH1Auth * m_auth;
    SSH1Channel * m_channel;
    QTermSocketPrivate * m_socket;
};

class SSHSocket : public QTermSocket
{
    Q_OBJECT
public:
    SSHSocket(QObject * parent = 0);
    ~SSHSocket()
    {}
    void setProxy(int nProxyType,  //0-no proxy; 1-wingate; 2-sock4; 3-socks5
                  bool bAuth, // if authentation needed
                  const QString& strProxyHost, quint16 uProxyPort,
                  const QString& strProxyUsr, const QString& strProxyPwd);

    void connectToHost(const QString & hostName, quint16 port);

    QByteArray readBlock(unsigned long size);
    long writeBlock(const QByteArray & data);
    unsigned long bytesAvailable();
    void flush();
    void close();
signals:
    void readyRead();
private slots:
    void readData();
private:
    void checkVersion(const QByteArray & banner);
    enum SSHVersion
    {
        SSHV1, SSHV2, SSHUnknown
    };
    SSHVersion m_version;
    QTermSocketPrivate * m_socket;
    SSHSocketPriv * m_priv;
    QByteArray m_data;
};


}

#endif // SSH_SOCKET_H
