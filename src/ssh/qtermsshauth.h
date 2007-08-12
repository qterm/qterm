#ifndef QTERMSSHAUTH_H
#define QTERMSSHAUTH_H

#include <qobject.h>

class QString;

namespace QTerm
{
class QTermSSHPacketReceiver;
class QTermSSHPacketSender;

class QTermSSHAuth : public QObject
{
	Q_OBJECT
protected:
	QString d_user;
	QTermSSHPacketReceiver * d_incomingPacket;
	QTermSSHPacketSender * d_outcomingPacket;
public:
	QTermSSHAuth(const char * sshuser = NULL)
		: d_user(sshuser)
	{
	}
	virtual void initAuth(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output) = 0;
public slots:
	virtual void handlePacket(int type) = 0;
signals:
	void authOK();
	void authError(const char * reason);
};

class QTermSSHPasswdAuth : public QTermSSHAuth
{
	Q_OBJECT
protected:
	QString d_passwd;
	bool d_isTried;
public:
	QTermSSHPasswdAuth(const char * sshuser, const char * sshpasswd)
		: QTermSSHAuth(sshuser), d_passwd(sshpasswd)
	{
	}
};

class QTermSSH1PasswdAuth : public QTermSSHPasswdAuth
{
	Q_OBJECT
private:
	enum QTermSSH1PasswdAuthState {
		BEFORE_AUTH,
		USER_SENT,
		PASS_SENT,
		AUTH_OK
	} d_state;
public:
	QTermSSH1PasswdAuth(const char * sshuser, const char * sshpasswd);
public slots:
	void handlePacket(int type);
	void initAuth(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output);
};

} // namespace QTerm

#endif		//QTERMSSHAUTH_H
