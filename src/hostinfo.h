#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <QtCore/QMap>
#include <QtCore/QString>

namespace QTerm
{
typedef QMap<QString, QString> Completion;

class HostInfo
{
public:
    enum Type {
        Telnet,
        SSH
    };
    HostInfo(const QString & hostName, quint16 port);
    HostInfo();
    virtual ~HostInfo();
    void setHostName(const QString & hostName);
    void setPort(quint16 port);
    const QString & hostName();
    quint16 port();
    Type type();
    const QString & termType();
    void setTermType(const QString & termType);
protected:
    void setType(Type type);
private:
    QString m_hostName;
    quint16 m_port;
    QString m_termType;
    Type m_type;
};

class TelnetInfo : public HostInfo
{
public:
    TelnetInfo(const QString & hostName, quint16 port);
    ~TelnetInfo();
};
class SSHInfo : public HostInfo
{
public:
    enum QueryType {
        Normal,
        Password
    };
    SSHInfo(const QString & hostName, quint16 port);
    SSHInfo();
    virtual ~SSHInfo();
    void setUserName(const QString & userName);
    void setPassword(const QString & password);
    void setAutoCompletion(const Completion & autoCompletion);
    const QString & userName(bool * ok = 0);
    const QString & password(bool * ok = 0);
    const QString & answer(const QString & prompt, QueryType type = Normal, bool * ok = 0);
    void reset();

private:
    QString m_userName;
    QString m_password;
    Completion m_autoCompletion;
};

} // namespace QTerm
#endif // HOSTINFO_H
