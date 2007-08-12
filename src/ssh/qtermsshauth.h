#ifndef QTERMSSHAUTH_H
#define QTERMSSHAUTH_H

#include <qobject.h>

class QString;

namespace QTerm
{
class SSHPacketReceiver;
class SSHPacketSender;

class SSHAuth : public QObject
{
	Q_OBJECT
protected:
	QString d_user;
	SSHPacketReceiver * d_incomingPacket;
	SSHPacketSender * d_outcomingPacket;
public:
	SSHAuth(const char * sshuser = NULL)
		: d_user(sshuser)
	{
	}
	virtual void initAuth(SSHPacketReceiver * packet, SSHPacketSender * output) = 0;
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void authOK();
	void authError(const char * reason);
};

class SSHPasswdAuth : public SSHAuth
{
	Q_OBJECT
protected:
	QString d_passwd;
	bool d_isTried;
public:
	SSHPasswdAuth(const char * sshuser, const char * sshpasswd)
		: SSHAuth(sshuser), d_passwd(sshpasswd)
	{
	}
};

class SSH1PasswdAuth : public SSHPasswdAuth
{
	Q_OBJECT
private:
	enum SSH1PasswdAuthState {
		BEFORE_AUTH,
		USER_SENT,
		PASS_SENT,
		AUTH_OK
	} d_state;
public:
	SSH1PasswdAuth(const char * sshuser, const char * sshpasswd);
public slots:
	void handlePacket(int type);
	void initAuth(SSHPacketReceiver * packet, SSHPacketSender * output);
};

} // namespace QTerm

#endif		//QTERMSSHAUTH_H
