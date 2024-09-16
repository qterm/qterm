//
// C++ Interface: sshauth
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_AUTH_H
#define SSH_AUTH_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace QTerm
{
class HostInfo;
class SSHInfo;
class SSH2Key;
class SSH2InBuffer;
class SSH2OutBuffer;
class SSH1InBuffer;
class SSH1OutBuffer;

/**
 @author hooey <hephooey@gmail.com>
*/
class SSH2Auth : public QObject
{
    Q_OBJECT
public:
    SSH2Auth(QByteArray & sessionID, SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent = 0);

    ~SSH2Auth();
    void requestAuthService();
    void setHostInfo(HostInfo * hostInfo);
signals:
    void authFinished();
    void error(const QString & message);
private slots:
    void authPacketReceived(int flag);
private:
    enum AuthMethod
    {
        None,
        PublicKey,
        Keyboard,
        Password
    };
    enum KeyType
    {
        Unknown,
        SSH_OPENSSL
    };
    void noneAuth();
    void publicKeyAuth();
    void keyboardAuth();
    void passwordAuth();
    void requestInput();
    void generateSign();
    void failureHandler();
    KeyType checkPrivateKeyType(const QByteArray & data);
    QString m_username;
    QByteArray m_method;
    AuthMethod m_authMethod;
    AuthMethod m_lastTried;
    SSH2InBuffer * m_in;
    SSH2OutBuffer * m_out;
    QByteArray m_sessionID;
    KeyType m_keyType;
    SSHInfo * m_hostInfo;
    SSH2Key * m_key;
    bool m_publicKeyAuthAvailable;

    // Give keyboardAuth and passwordAuth 3 tries.
    int m_tries;
};

class SSH1Auth : public QObject
{
    Q_OBJECT
public:
    SSH1Auth(SSH1InBuffer * in, SSH1OutBuffer * out, QObject *parent = 0);
    ~SSH1Auth();
    void requestAuthService();
    void setHostInfo(HostInfo * hostInfo);
signals:
    void authFinished();
    void error(const QString & message);
private slots:
    void authPacketReceived(int flag);
private:
    enum Phase {UserName, PassWord};
    Phase m_phase;
    void passwordAuth();
//  void keyboardAuth();
    void requestInput();
    void generateSign();
    void failureHandler();
    SSH1InBuffer * m_in;
    SSH1OutBuffer * m_out;
    SSHInfo * m_hostInfo;

    int m_tries;
};

}

#endif // SSH_AUTH_H
