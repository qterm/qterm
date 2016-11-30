#include "hostinfo.h"
#include <QInputDialog>

#include <QFile>
#include <QMessageBox>

namespace QTerm
{

HostInfo::HostInfo(const QString & hostName, quint16 port, QObject * parent)
    :QObject(parent), m_hostName(hostName), m_port(port), m_termType(), m_column(80), m_row(24), m_type(Telnet)
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

const QString & HostInfo::termType()
{
    return m_termType;
}

void HostInfo::setTermInfo(const QString & termType, int column, int row)
{
    m_termType = termType;
    m_column = column;
    m_row = row;
}

TelnetInfo::TelnetInfo(const QString & hostName, quint16 port, QObject * parent)
    :HostInfo(hostName, port, parent)
{
    setType(Telnet);
}

TelnetInfo::~TelnetInfo()
{
}

SSHInfo::SSHInfo(const QString & hostName, quint16 port, QObject * parent)
    :HostInfo(hostName, port, parent), m_userName(), m_password(), m_publicKeyFile(), m_privateKeyFile(), m_passphrase(), m_hostKey(), m_autoCompletion()
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

void SSHInfo::setPassphrase(const QString & passphrase)
{
    m_passphrase = passphrase;
}

void SSHInfo::setPublicKeyFile(const QString & filename)
{
    m_publicKeyFile = filename;
}

void SSHInfo::setPrivateKeyFile(const QString & filename)
{
    m_privateKeyFile = filename;
}

const QString & SSHInfo::publicKeyFile()
{
    return m_publicKeyFile;
}

const QString & SSHInfo::privateKeyFile()
{
    return m_privateKeyFile;
}

bool SSHInfo::publicKeyAuthAvailable()
{
    if (m_publicKeyFile.isEmpty() || m_privateKeyFile.isEmpty()) {
        return false;
    }
    return true;
}

void SSHInfo::setHostKey(const QString & hostKey)
{
    m_hostKey= hostKey;
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

const QString & SSHInfo::passphrase()
{
    return m_passphrase;
}

int SSHInfo::passphraseCallback(char *buf, int size, int rwflag, void *u)
{
    if (buf == NULL) {
        return 0;
    }

    memset(buf, '\0', size);

    QString passphrase = QString::fromUtf8((const char *) u);

    if (passphrase.isEmpty()) {
        passphrase = QInputDialog::getText(0, "QTerm", "Passphrase for the private key: ", QLineEdit::Password, "", NULL);
    }
    int len = strlen(passphrase.toUtf8().data());
    if (len > size){
         len = size;
    }
    memcpy(buf, passphrase.toUtf8().data(), len);
    return len;
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

bool SSHInfo::checkHostKey(const QByteArray & hostKey)
{
    if (hostKey.toBase64() == m_hostKey) {
        return true;
    } else if (m_hostKey.isEmpty()) {
        QMessageBox::StandardButton rb = QMessageBox::question(static_cast<QWidget*>(parent()), tr("New Host Key"), tr("No host key is found for the server. Do you want to add the host key and continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes) {
            m_hostKey = hostKey.toBase64();
            emit(hostKeyChanged(m_hostKey));
            return true;
        }
    } else {
        QMessageBox::StandardButton rb = QMessageBox::critical(static_cast<QWidget*>(parent()), tr("Host Key Mismatch"), tr("HOST KEY DOES NOT MATCH! THIS COULD BE A MITM ATTACK! DO YOU REALLY WANT TO CONTINUE? IF YOU DO NOT KNOW WHAT YOU ARE DOING, CHOOSE NO!"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(rb == QMessageBox::Yes) {
            QMessageBox::StandardButton rb2 = QMessageBox::question(static_cast<QWidget*>(parent()), tr("Update Host Key"), tr("Are you sure you want to update the host key?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if(rb2 == QMessageBox::Yes) {
                m_hostKey = hostKey.toBase64();
                emit(hostKeyChanged(m_hostKey));
                return true;
            }
        }

    }
    return false;
}

void SSHInfo::reset()
{
    m_userName = "";
    m_password = "";
    m_autoCompletion.clear();
}

} // namespace QTerm

#include "moc_hostinfo.cpp"
