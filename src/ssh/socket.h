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

#include "qtermsocket.h"
#include <QtCore/QObject>

namespace QTerm
{
class HostInfo;
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
    virtual void requestWindowSize(int column, int row) = 0;
signals:
    void readyRead();
    void error(const QString & message);
    void socketReady();
    void closeConnection();
};

class SSH2SocketPriv : public SSHSocketPriv
{
    Q_OBJECT
public:
    SSH2SocketPriv(SocketPrivate * plainSocket, QByteArray & banner, QObject * parent = 0);
    ~SSH2SocketPriv();
    QByteArray readData(unsigned long size);
    void writeData(const QByteArray & data);
    unsigned long bytesAvailable();
    void requestWindowSize(int column, int row);
signals:
    void allChannelsClosed();
private slots:
    void slotKexFinished(const QByteArray & sessionID);
    void slotAuthFinished();
    void slotNewChannel(int id);
    void slotChannelData(int id);
    void slotChannelClosed(int id);
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
    QByteArray m_sessionID;
    QList<uint> m_channelList;
    HostInfo * m_hostInfo;
};

class SSH1SocketPriv : public SSHSocketPriv
{
    Q_OBJECT
public:
    SSH1SocketPriv(SocketPrivate * plainSocket, QByteArray & banner, QObject * parent = 0);
    ~SSH1SocketPriv();
    QByteArray readData(unsigned long size);
    void writeData(const QByteArray & data);
    unsigned long bytesAvailable();
    void requestWindowSize(int column, int row);
private slots:
    void slotKexFinished();
    void slotAuthFinished();
private:
    SSH1InBuffer * m_inPacket;
    SSH1OutBuffer * m_outPacket;
    SSH1Kex * m_kex;
    SSH1Auth * m_auth;
    SSH1Channel * m_channel;
    HostInfo * m_hostInfo;
};

class SSHSocket : public Socket
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

    void connectToHost(HostInfo * hostInfo);

    QByteArray readBlock(unsigned long size);
    long writeBlock(const QByteArray & data);
    unsigned long bytesAvailable();
    void flush();
    void requestWindowSize(int column, int row);
public slots:
    void close();
signals:
    void readyRead();
private slots:
    void readData();
    void onError(const QString & message);
private:
    void checkVersion(const QByteArray & banner);
    enum SSHVersion
    {
        SSHV1, SSHV2, SSHUnknown
    };
    SSHVersion m_version;
    SocketPrivate * m_socket;
    SSHSocketPriv * m_priv;
    QByteArray m_data;
};


}

#endif // SSH_SOCKET_H
