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

#include <QObject>
#include <QString>
namespace QTerm
{
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
    SSH2Auth(SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent = 0);

    ~SSH2Auth();
    void requestAuthService();
signals:
    void authFinished();
    void error(const QString & message);
private slots:
    void authPacketReceived(int flag);
private:
    void noneAuth();
    void keyboardAuth();
    void requestInput();
    void failureHandler();
    QString m_username;
    SSH2InBuffer * m_in;
    SSH2OutBuffer * m_out;
};

class SSH1Auth : public QObject
{
    Q_OBJECT
public:
    SSH1Auth(SSH1InBuffer * in, SSH1OutBuffer * out, QObject *parent = 0);
    ~SSH1Auth();
    void requestAuthService();
signals:
    void authFinished();
private slots:
    void authPacketReceived(int flag);
private:
    enum Phase {UserName, PassWord};
    Phase m_phase;
    void passwordAuth();
//  void keyboardAuth();
    void requestInput();
    void failureHandler();
    SSH1InBuffer * m_in;
    SSH1OutBuffer * m_out;
};

}

#endif // SSH_AUTH_H
