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
ScriptHelper::ScriptHelper(Window * parent, QScriptEngine * engine)
    :QObject(parent),m_accepted(false)
{
    m_window = parent;
    m_engine = engine;
}

ScriptHelper::~ScriptHelper()
{
}

bool ScriptHelper::accepted() const
{
    return m_accepted;
}

void ScriptHelper::setAccepted(bool accepted)
{
    m_accepted =  accepted;
}

int ScriptHelper::caretX()
{
    return m_window->m_pBuffer->caret().x();
}

int ScriptHelper::caretY()
{
    return m_window->m_pBuffer->caret().y();
}

int ScriptHelper::char_x(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.x();
}

int ScriptHelper::char_y(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.y() - m_window->m_pBBS->getScreenStart();
}

int ScriptHelper::columns()
{
    return m_window->m_pBuffer->columns();
}

int ScriptHelper::rows()
{
    return m_window->m_pBuffer->line();
}

QString ScriptHelper::getUrl()
{
    return m_window->m_pBBS->getUrl();
}

QString ScriptHelper::getIP()
{
    return m_window->m_pBBS->getIP();
}

bool ScriptHelper::isConnected()
{
    return m_window->isConnected();
}

void ScriptHelper::reconnect()
{
    m_window->reconnect();
}

void ScriptHelper::disconnect()
{
    m_window->disconnect();
}

void ScriptHelper::buzz()
{
    m_window->m_pFrame->buzz();
}

void ScriptHelper::sendString(const QString & string)
{
    m_window->inputHandle(string);
}

void ScriptHelper::sendParsedString(const QString & string)
{
    m_window->sendParsedString(string);
}

void ScriptHelper::showMessage(const QString & message, int type, int duration)
{
    m_window->showMessage(message, type, duration);
}

void ScriptHelper::cancelZmodem()
{
    m_window->zmodem()->zmodemCancel();
}

void ScriptHelper::setZmodemFileList(const QStringList & fileList)
{
    m_window->zmodem()->setFileList(fileList);
}

QScriptValue ScriptHelper::getLine(int line)
{
    TextLine * obj = m_window->m_pBuffer->screen(line);
    return m_engine->newQObject(obj);
}

} // namespace QTerm
#include <script.moc>
