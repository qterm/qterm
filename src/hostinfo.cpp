#include "hostinfo.h"
#include <QtGui/QInputDialog>
#include <QtDebug>

namespace QTerm
{

HostInfo::HostInfo(const QString & hostName, quint16 port)
    :m_hostName(hostName), m_port(port), m_type(Telnet)
{
}

HostInfo::HostInfo()
    :m_hostName(), m_port(0)
{
}

HostInfo::~HostInfo()
{
}

void HostInfo::setHostName(const QString & hostName)
{
    m_hostName = hostName;
}

void HostInfo::setPort(quint16 port)
{
    m_port = port;
}

const QString & HostInfo::hostName()
{
    return m_hostName;
}

quint16 HostInfo::port()
{
    return m_port;
}

HostInfo::Type HostInfo::type()
{
    return m_type;
}

void HostInfo::setType(HostInfo::Type type)
{
    m_type = type;
}

TelnetInfo::TelnetInfo(const QString & hostName, quint16 port)
    :HostInfo(hostName, port)
{
    setType(Telnet);
}

TelnetInfo::~TelnetInfo()
{
}

SSHInfo::SSHInfo(const QString & hostName, quint16 port)
    :HostInfo(hostName, port), m_userName(), m_password(), m_autoCompletion()
{
    setType(SSH);
}

SSHInfo::~SSHInfo()
{
}

void SSHInfo::setUserName(const QString & userName)
{
    m_userName = userName;
}

void SSHInfo::setPassword(const QString & password)
{
    m_password = password;
}

void SSHInfo::setAutoCompletion(const Completion & autoCompletion)
{
    m_autoCompletion = autoCompletion;
}

const QString & SSHInfo::userName(bool * ok)
{
    if (m_userName.isEmpty()) {
        QString username = QInputDialog::getText(0, "QTerm", "Username: ", QLineEdit::Normal, "", ok);
        m_userName = username;
    } else if (ok != 0) {
        *ok = true;
    }
    return m_userName;
}

const QString & SSHInfo::password(bool * ok)
{
    if (m_password.isEmpty()) {
        QString password = QInputDialog::getText(0, "QTerm", "Password: ", QLineEdit::Password, "", ok);
        m_password = password;
    } else if (ok != 0) {
        *ok = true;
    }
    return m_password;
}

const QString & SSHInfo::answer(const QString & prompt, QueryType type, bool * ok)
{
    if (!m_autoCompletion.contains(prompt)||m_autoCompletion[prompt].isEmpty()) {
        QString input;
        if (type == Normal) {
            input = QInputDialog::getText(0, "QTerm", prompt, QLineEdit::Normal, "", ok);
        } else {
            input = QInputDialog::getText(0, "QTerm", prompt, QLineEdit::Password, "", ok);
        }
        m_autoCompletion[prompt] = input;
    } else if (ok != 0) {
        *ok = true;
    }
    return m_autoCompletion[prompt];
}

void SSHInfo::reset()
{
    m_userName = "";
    m_password = "";
    m_autoCompletion.clear();
}

} // namespace QTerm
