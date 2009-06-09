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
    :QObject(parent),m_accepted(false),m_scriptList(),m_popupActionList(),m_urlActionList()
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
    if (m_popupActionList.contains(id)) {
        qDebug("action with the same name is already registered");
        return false;
    }
    QAction * action = new QAction(menuTitle, this);
    action->setObjectName(id);
    QMenu * popupMenu = m_window->popupMenu();
    popupMenu->addAction(action);
    QScriptValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->globalObject().property( "QTerm" ).setProperty( id, newItem );
    m_popupActionList << id;
    return true;
}

bool ScriptHelper::addUrlMenu(QString id, QString menuTitle, QString icon)
{
    if (m_urlActionList.contains(id)) {
        qDebug("action with the same name is already registered");
        return false;
    }
    QAction * action = new QAction(menuTitle, this);
    action->setObjectName(id);
    QMenu * urlMenu = m_window->urlMenu();
    urlMenu->addAction(action);
    QScriptValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->globalObject().property( "QTerm" ).setProperty( id, newItem );
    m_urlActionList << id;
    return true;
}

void ScriptHelper::addPopupSeparator()
{
    m_window->popupMenu()->addSeparator();
}

void ScriptHelper::addUrlSeparator()
{
    m_window->urlMenu()->addSeparator();
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
    QFileInfo fileInfo(filename);
    if (!fileInfo.isAbsolute()) {
        fileInfo = QFileInfo(localPath() + "scripts/" + filename);
        if (!fileInfo.exists()) {
            fileInfo = QFileInfo(globalPath() + "scripts/" + filename);
        }
    }
    if (!fileInfo.exists()) {
        qDebug() << "Script file " << filename << "not found";
        return;
    }
    if (isScriptLoaded(fileInfo.absoluteFilePath())) {
        qDebug() << "Script file " << fileInfo.absoluteFilePath() << "is already loaded";
        return;
    }
    loadScriptFile(fileInfo.absoluteFilePath());
    qDebug() << "load script file: " << filename;
    addImportedScript(fileInfo.absoluteFilePath());
}

void ScriptHelper::loadExtension(const QString & extension)
{
    QScriptValue ret = m_scriptEngine->importExtension(extension);
    if (ret.isError()) {
        showMessage("Fail to load extension: "+extension);
        qDebug() << "Fail to load extension: " << extension;
    }
}

void ScriptHelper::openUrl(const QString & url)
{
    Global::instance()->openUrl(url);
}

QString ScriptHelper::getSelectedText(bool rect, bool color, const QString & escape)
{
    return m_window->m_pBuffer->getSelectText(rect,color,escape);
}

QString ScriptHelper::version()
{
    return QTERM_VERSION;
}

void ScriptHelper::loadScriptFile(const QString & filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QString scripts = QString::fromUtf8(file.readAll());
    file.close();
    if (!m_scriptEngine->canEvaluate(scripts))
        qDebug() << "Cannot evaluate this script";
    m_scriptEngine->evaluate(scripts);
    if (m_scriptEngine->hasUncaughtException()) {
        qDebug() << "Exception: " << m_scriptEngine->uncaughtExceptionBacktrace();
    }
}


} // namespace QTerm
#include <scripthelper.moc>
