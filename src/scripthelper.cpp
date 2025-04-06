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
#include <QAction>
#include <QMenu>

namespace QTerm
{
ScriptHelper::ScriptHelper(Window * parent, QQmlEngine * engine)
    :QObject(parent),m_accepted(false),m_qtbindingsAvailable(true),m_scriptList(),m_popupActionList(),m_urlActionList()
{
    m_window = parent;
    m_scriptEngine = engine;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    m_scriptEngine->installExtensions(QJSEngine::TranslationExtension);
#endif
}

ScriptHelper::~ScriptHelper()
{
}

bool ScriptHelper::accepted() const
{
    return m_accepted;
}

bool ScriptHelper::qtbindingsAvailable() const
{
    return m_qtbindingsAvailable;
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
    QPoint pt = m_window->screen()->mapToChar(QPoint(x,y));
    return pt.x();
}

int ScriptHelper::charY(int x, int y)
{
    QPoint pt = m_window->screen()->mapToChar(QPoint(x,y));
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

void ScriptHelper::osdMessage(const QString & message, int type, int duration)
{
    m_window->osdMessage(message, type, duration);
}

void ScriptHelper::showMessage(const QString & title, const QString & message, int duration)
{
    m_window->showMessage(title, message, duration);
}

void ScriptHelper::cancelZmodem()
{
    m_window->zmodem()->zmodemCancel();
}

void ScriptHelper::setZmodemFileList(const QStringList & fileList)
{
    m_window->zmodem()->setFileList(fileList);
}

QJSValue ScriptHelper::getLine(int line)
{
    TextLine * obj = m_window->m_pBuffer->screen(line);
    QJSValue value = m_scriptEngine->newQObject(obj);
    m_scriptEngine->setObjectOwnership(obj, QJSEngine::CppOwnership);
    return value;
}

QJSValue ScriptHelper::window()
{
    QJSValue value = m_scriptEngine->newQObject(m_window);
    m_scriptEngine->setObjectOwnership(m_window, QJSEngine::CppOwnership);
    return value;
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
    QJSValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->setObjectOwnership(action, QJSEngine::CppOwnership);
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
    QJSValue newItem = m_scriptEngine->newQObject( action );
    m_scriptEngine->setObjectOwnership(action, QJSEngine::CppOwnership);
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

QString ScriptHelper::findFile(const QString & filename)
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
        return "";
    }
    return fileInfo.absoluteFilePath();
}

void ScriptHelper::loadScript(const QString & filename)
{
    QString scriptFile = findFile(filename);
    if (filename.isEmpty()) {
        return;
    }
    if (isScriptLoaded(scriptFile)) {
        qDebug() << "Script file " << scriptFile << "is already loaded";
        return;
    }
    loadScriptFile(scriptFile);
    qDebug() << "load script file: " << filename;
    addImportedScript(scriptFile);
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
    QJSValue result = m_scriptEngine->evaluate(scripts, filename);
    if (result.isError()) {
        qDebug() << "Exception at " << filename << ":" << result.property("lineNumber").toInt() << ":" << result.toString();
    }
}


} // namespace QTerm
#include <moc_scripthelper.cpp>
