#include "qterm.h"
#include "scripthelper.h"
#include "qtermwindow.h"
#include "qtermbuffer.h"
#include "qtermscreen.h"
#include "qtermframe.h"
#include "qtermbbs.h"
#include "qtermtextline.h"
#include "qtermzmodem.h"
#include "qtermglobal.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtScript>

namespace QTerm
{
ScriptHelper::ScriptHelper(Window * parent, QScriptEngine * engine)
    :QObject(parent),m_accepted(false),m_scriptList()
{
    m_window = parent;
    m_scriptEngine = engine;
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

int ScriptHelper::charX(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.x();
}

int ScriptHelper::charY(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.y() - m_window->m_pBBS->getScreenStart();
}

int ScriptHelper::posX()
{
    return m_window->mousePos().x();
}

int ScriptHelper::posY()
{
    return m_window->mousePos().y();
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
    return m_scriptEngine->newQObject(obj);
}

QScriptValue ScriptHelper::window()
{
    return m_scriptEngine->newQObject(m_window);
}

bool ScriptHelper::addPopupMenu(QString id, QString menuTitle, QString icon)
{
    QAction * action = new QAction(menuTitle, this);
    action->setObjectName(id);
    QMenu * popupMenu = m_window->popupMenu();
    popupMenu->addAction(action);
    QScriptValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->globalObject().property( "QTerm" ).setProperty( id, newItem );
    return true;
}

bool ScriptHelper::addUrlMenu(QString id, QString menuTitle, QString icon)
{
    QAction * action = new QAction(menuTitle, this);
    action->setObjectName(id);
    QMenu * urlMenu = m_window->urlMenu();
    urlMenu->addAction(action);
    QScriptValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->globalObject().property( "QTerm" ).setProperty( id, newItem );
    return true;
}

void ScriptHelper::addImportedScript(const QString & filename)
{
    m_scriptList << filename;
}

bool ScriptHelper::isScriptLoaded(const QString & filename)
{
    return m_scriptList.contains(filename);
}

QString ScriptHelper::globalPath()
{
    return Global::instance()->pathLib();
}

QString ScriptHelper::localPath()
{
    return Global::instance()->pathCfg();
}

void ScriptHelper::import(const QString & filename)
{
    QFileInfo fileInfo(localPath() + "scripts/" + filename);
    if (!fileInfo.exists()) {
        fileInfo = QFileInfo(globalPath() + "scripts/" + filename);
    }
    if (!fileInfo.exists()) {
        return;
    }
    if (isScriptLoaded(fileInfo.absoluteFilePath())) {
        return;
    }
    m_window->loadScriptFile(fileInfo.absoluteFilePath());
    qDebug() << "load script file: " << filename;
    addImportedScript(fileInfo.absoluteFilePath());
}

} // namespace QTerm
#include <scripthelper.moc>
