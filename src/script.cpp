#include "qterm.h"
#include "script.h"
#include "qtermwindow.h"
#include "qtermbuffer.h"
#include "qtermscreen.h"
#include "qtermframe.h"
#include "qtermbbs.h"
#include "qtermtextline.h"
#include "qtermzmodem.h"
#include <QtScript>

namespace QTerm
{
Script::Script(Window * parent, QScriptEngine * engine)
    :QObject(parent),m_accepted(false)
{
    m_window = parent;
    m_engine = engine;
}

Script::~Script()
{
}

bool Script::accepted() const
{
    return m_accepted;
}

void Script::setAccepted(bool accepted)
{
    m_accepted =  accepted;
}

int Script::caretX()
{
    return m_window->m_pBuffer->caret().x();
}

int Script::caretY()
{
    return m_window->m_pBuffer->caret().y();
}

int Script::char_x(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.x();
}

int Script::char_y(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.y() - m_window->m_pBBS->getScreenStart();
}

int Script::columns()
{
    return m_window->m_pBuffer->columns();
}

int Script::rows()
{
    return m_window->m_pBuffer->line();
}

QString Script::getUrl()
{
    return m_window->m_pBBS->getUrl();
}

QString Script::getIP()
{
    return m_window->m_pBBS->getIP();
}

bool Script::isConnected()
{
    return m_window->isConnected();
}

void Script::reconnect()
{
    m_window->reconnect();
}

void Script::disconnect()
{
    m_window->disconnect();
}

void Script::buzz()
{
    m_window->m_pFrame->buzz();
}

void Script::sendString(const QString & string)
{
    m_window->inputHandle(string);
}

void Script::sendParsedString(const QString & string)
{
    m_window->sendParsedString(string);
}

void Script::showMessage(const QString & message, int type, int duration)
{
    m_window->showMessage(message, type, duration);
}

void Script::cancelZmodem()
{
    m_window->zmodem()->zmodemCancel();
}

void Script::setZmodemFileList(const QStringList & fileList)
{
    m_window->zmodem()->setFileList(fileList);
}

QScriptValue Script::getLine(int line)
{
    TextLine * obj = m_window->m_pBuffer->screen(line);
    return m_engine->newQObject(obj);
}

} // namespace QTerm
#include <script.moc>
