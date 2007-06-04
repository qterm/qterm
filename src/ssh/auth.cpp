//
// C++ Implementation: sshauth
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "auth.h"
#include "packet.h"
#include "ssh2.h"
#include "ssh1.h"
#include <QInputDialog>
#include <QtDebug>
extern void dumpData(const QByteArray & data);
namespace QTerm
{

SSH2Auth::SSH2Auth(SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent)
        : QObject(parent), m_username()
{
    m_in = in;
    m_out = out;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH2Auth::~SSH2Auth()
{}

void SSH2Auth::authPacketReceived(int flag)
{
//  qDebug() << "flag " << flag;
    // TODO: BANNER
    switch (flag) {
    case SSH2_MSG_SERVICE_ACCEPT:
        noneAuth();
        break;
    case SSH2_MSG_USERAUTH_FAILURE:
        failureHandler();
        break;
    case SSH2_MSG_USERAUTH_INFO_REQUEST:
        requestInput();
        break;
    case SSH2_MSG_USERAUTH_SUCCESS:
        qDebug() << "====== success! ======";
        emit authFinished();
    default:
        break;
    }
}

void SSH2Auth::failureHandler()
{
    m_in->getUInt8();//flag
    QByteArray method = m_in->getString();
    int partialSuccess = m_in->getUInt8();
    qDebug() << "method: " << method << "flag " << partialSuccess;
    if (method.contains("keyboard-interactive")) {
        keyboardAuth();
    } else {
        emit error("Server does not support keyboard interactive authtication method.");
        return;
    }
}

void SSH2Auth::requestAuthService()
{
    m_out->startPacket(SSH2_MSG_SERVICE_REQUEST);
    m_out->putString("ssh-userauth");
    m_out->sendPacket();
}

void SSH2Auth::noneAuth()
{
    bool ok;
    m_username = QInputDialog::getText(0, "QTerm", "Username: ", QLineEdit::Normal, "", &ok);
    if (!ok) {
        qDebug() << "User canceled!";
        emit error("User canceled");
        return;
    }
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("none");
    m_out->sendPacket();
}

void SSH2Auth::keyboardAuth()
{
    m_out->startPacket(SSH2_MSG_USERAUTH_REQUEST);
    m_out->putString(m_username.toUtf8());
    m_out->putString("ssh-connection");
    m_out->putString("keyboard-interactive");
    m_out->putString("");
    m_out->putString("");
    m_out->sendPacket();
}

void SSH2Auth::requestInput()
{
    m_in->getUInt8(); //flag
    QString name = QString::fromUtf8(m_in->getString());
    qDebug() << "name: " << name;
    QString instruction = QString::fromUtf8(m_in->getString());
    qDebug() << "instruction: " << instruction;
    QByteArray langTag = m_in->getString();
    if (!langTag.isEmpty())
        qDebug() << "langTag is not empty, ignore it";
    uint numPrompts = m_in->getUInt32();
    qDebug() << "number of prompts: " << numPrompts;
    QList<QString> answerList;
    bool ok;
    for (int i = 0; i < numPrompts; i++) {
        QString prompt = QString::fromUtf8(m_in->getString());
        QString answer;
        if (m_in->getUInt8() == 1)
            answer = QInputDialog::getText(0, name, prompt, QLineEdit::Normal, "", &ok);
        else
            answer = QInputDialog::getText(0, name, prompt, QLineEdit::Password, "", &ok);
        if (!ok) {
            qDebug() << "User canceled!";
            emit error("User canceled!");
            return;
        }
        answerList << answer;
    }

    m_out->startPacket(SSH2_MSG_USERAUTH_INFO_RESPONSE);
    m_out->putUInt32(answerList.size());
    qDebug() << "number of answers: " << answerList.size();
    foreach(QString answer, answerList)
    m_out->putString(answer.toUtf8());
    m_out->sendPacket();
}

SSH1Auth::SSH1Auth(SSH1InBuffer * in, SSH1OutBuffer * out, QObject *parent)
        : QObject(parent), m_phase(UserName)
{
    m_in = in;
    m_out = out;
    connect(m_in, SIGNAL(packetReady(int)), this, SLOT(authPacketReceived(int)));
}


SSH1Auth::~SSH1Auth()
{}

void SSH1Auth::requestAuthService()
{
    QString username = QInputDialog::getText(0, "QTerm", "Username: ", QLineEdit::Normal);
    m_out->startPacket(SSH_CMSG_USER);
    m_out->putString(username.toUtf8());
    m_out->sendPacket();
}

void SSH1Auth::authPacketReceived(int flag)
{
    qDebug() << "flag " << flag;
    // TODO: BANNER
    switch (flag) {
    case SSH_SMSG_SUCCESS:
        if (m_phase == UserName)
            qDebug() << "No auth needed";
        else
            qDebug() << "passwordAuth succeed";
        emit authFinished();
        break;
    case SSH_SMSG_FAILURE:
        if (m_phase == UserName) {
            qDebug() << "Prepare to auth";
            passwordAuth();
            m_phase = PassWord;
        } else
            qDebug() << "Wrong password?";
        break;
    default:
        break;
    }
}
void SSH1Auth::passwordAuth()
{
    //QString password = QString::fromLatin1 ( getpass ( "Password: " ) );
    QString password = QInputDialog::getText(0, "QTerm", "Password: ", QLineEdit::Password);
    m_out->startPacket(SSH_CMSG_AUTH_PASSWORD);
    m_out->putString(password.toUtf8());
    m_out->sendPacket();
}

}

#include "auth.moc"

