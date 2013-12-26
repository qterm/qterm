//
// C++ Implementation: Global
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "qtermglobal.h"
#include "qtermconfig.h"
#include "qtermparam.h"
#include "qtermconvert.h"
#ifdef KWALLET_ENABLED
#include "wallet.h"
#endif // KWALLET_ENABLED
#include "qterm.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTranslator>
#include <QtCore/QVariant>
#include <QtCore/QUrl>
#include <QtCore/QProcess>
#include <QtCore/QLibraryInfo>
#include <QtCore/QUuid>
#include <QtCore/QTextStream>
#include <QtGui/QDesktopServices>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#else
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#endif
#include <QtXml/QDomDocument>

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
#include <windows.h>
#include <shellapi.h>
#ifndef MAX_PATH
#define MAX_PATH 128
#endif
#endif

namespace QTerm
{

Global * Global::m_Instance = 0;

Global * Global::instance()
{
    static QMutex mutex;
    if (!m_Instance) {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new Global;

        mutex.unlock();
    }

    return m_Instance;
}

Global::Global()
        : m_fileCfg("./qterm.cfg"), m_addrCfg("./address.cfg"), m_addrXml("./address.xml"),
        m_pathLib("./"), m_pathCfg("./"), 
        m_windowState(), m_status(INIT_OK), m_style(), 
        m_fullScreen(false), m_language(Global::English)
{
#ifdef KWALLET_ENABLED
    if (Wallet::isWalletAvailable()) {
        qDebug() << "KWallet service found";
        m_wallet = new Wallet(this);
    } else {
        m_wallet = NULL;
    }
#endif // KWALLET_ENABLED
    if (!iniWorkingDir(qApp->arguments()[0])) {
        m_status = INIT_ERROR;
        return;
    }
    m_translatorQT    = new QTranslator(0);
    m_translatorQTerm = new QTranslator(0);
    
    m_config    = new Config(m_fileCfg);
    m_converter = new Convert();
    if (!iniSettings()) {
        m_status = INIT_ERROR;
        return;
    }
}

bool Global::isOK()
{
    if (m_status == INIT_OK) {
        return true;
    }
    return false;
}

Config * Global::fileCfg()
{
    return m_config;
}

QDomDocument Global::addrXml()
{
    QDomDocument doc;
    QFile file(m_addrXml);
    if (file.open(QIODevice::ReadOnly)) {
        doc.setContent(&file);
        file.close();
    }
    return doc;
}

const QString & Global::pathLib()
{
    return m_pathLib;
}

const QString & Global::pathCfg()
{
    return m_pathCfg;
}

void Global::clearDir(const QString & path)
{
    QDir dir(path);
    if (dir.exists()) {
        const QFileInfoList list = dir.entryInfoList();
        foreach(QFileInfo fi, list) {
            if (fi.isFile())
                dir.remove(fi.fileName());
        }
    }
}

QMap<QString,QString> Global::loadFavoriteList(QDomDocument doc)
{
    QMap<QString,QString> listSite;
    // import xml address book
    QDomNodeList nodeList = doc.elementsByTagName("site");
    for (int i=0; i<nodeList.count(); i++) {
        QDomElement node = nodeList.at(i).toElement();
        if (node.attribute("favor") == "1")
            listSite[node.attribute("uuid")] = node.attribute("name");
    }

    return listSite;
}

bool Global::loadAddress(QDomDocument doc, QString uuid, Param& param)
{
    if (uuid.isEmpty())
        uuid = QUuid().toString();
    QDomNodeList nodeList = doc.elementsByTagName("site");
    for (int i=0; i<nodeList.count(); i++) {
        QDomElement node = nodeList.at(i).toElement();
        if (uuid == node.attribute("uuid"))
            foreach (QString key, param.m_mapParam.keys())  {
                #ifdef KWALLET_ENABLED
                if (key == "password" && m_wallet != NULL) {
                    m_wallet->open();
                    param.m_mapParam["password"] = m_wallet->readPassword(
                        node.attribute("name"), node.attribute("user"));
                } else
                #endif // KWALLET_ENABLED
                param.m_mapParam[key] = node.attribute(key);
            }
    }
    return true;
}

bool Global::loadAddress(Config& addrCfg, int n, Param& param)
{
    QString strTmp, strSection;
    strTmp = addrCfg.getItemValue("bbs list", "num").toString();
    if ((n < 0 && strTmp.toInt() <= 0) || n < -1)
        strSection = "default";
    else {
        n = n < 0 ? 0 : n;
        strSection.sprintf("bbs %d", n);
    }

    // check if larger than existence
    strTmp = addrCfg.getItemValue("bbs list", "num").toString();
    if (n >= strTmp.toInt())
        return false;

    foreach(QString key,param.m_mapParam.keys()) {
        #ifdef KWALLET_ENABLED
        if (key == "password" && m_wallet != NULL) {
            m_wallet->open();
            param.m_mapParam["password"] = m_wallet->readPassword(
                param.m_mapParam["name"].toString(),
                param.m_mapParam["user"].toString());
        } else
        #endif // KWALLET_ENABLED
            param.m_mapParam[key] = addrCfg.getItemValue(strSection,key);
    }

    return true;
}

void Global::saveAddress(QDomDocument doc, QString uuid, const Param& param)
{
    bool result = false;
    // find and replace existing site
    QDomNodeList nodeList = doc.elementsByTagName("site");
    for (int i=0; i<nodeList.count(); i++) {
        QDomElement node = nodeList.at(i).toElement();
        if (uuid == node.attribute("uuid")) {
            foreach (QString key, param.m_mapParam.keys()) 
                node.setAttribute(key, 
                    param.m_mapParam[key].toString());
            result = true;
            break;
        }
    }
    // create new site otherwise
    if (!result) {
        QDomElement site = doc.createElement("site");
        site.setAttribute("uuid", uuid);
        foreach (QString key, param.m_mapParam.keys()) 
                site.setAttribute(key, 
                    param.m_mapParam[key].toString());
        doc.documentElement().appendChild(site);
        result = true;
    }

    if (!result) return;

}

void Global::removeAddress(QDomDocument doc, QString uuid)
{
    QDomNodeList nodeList;
    // remove the actual site
    nodeList = doc.elementsByTagName("site");
    for (int i=0; i<nodeList.count(); i++) {
        QDomElement node = nodeList.at(i).toElement();
        if (uuid == node.attribute("uuid")) {
            doc.removeChild(node);
        }
    }
}

bool Global::convertAddressBook2XML()
{
    QDir dir;
    if (dir.exists(m_addrXml))// do nothing if address.xml existed
        return true;
    else {
        if (!dir.exists(m_addrCfg)) // simply copy from system if even address.cfg not existed
            return createLocalFile(m_addrXml, m_pathLib + "address.xml");
    }
    // import system address.xml or create new one
    QDomDocument doc;
    QDomElement addresses;
    QFile file(m_pathLib + "address.xml");
    if (file.open(QIODevice::ReadOnly) && doc.setContent(&file)) {
        addresses = doc.documentElement();
    } else {
        QDomProcessingInstruction instr =
                doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instr);

        addresses = doc.createElement("addresses");
        doc.appendChild(addresses);
    }
    // Combine cfg address book
    Config addrCfg(m_addrCfg);
    int num = addrCfg.getItemValue("bbs list", "num").toInt();


    QDomElement imported = doc.createElement("folder");
    imported.setAttribute("name", tr("imported sites"));
        addresses.insertBefore(imported, QDomNode());

    for (int i = -1; i < num; i++) {
        Param param;
        loadAddress(addrCfg, i, param);
        QDomElement site = doc.createElement("site");
        if (i==-1)
            site.setAttribute("uuid", QUuid().toString());
        else 
        {
            QString uuid = QUuid::createUuid().toString();
            site.setAttribute("uuid", uuid);
            QDomElement addsite = doc.createElement("addsite");
            addsite.setAttribute("uuid", uuid);
            imported.appendChild(addsite);
        }
        foreach(QString key,param.m_mapParam.keys())
            site.setAttribute(key, param.m_mapParam[key].toString());

        site.setAttribute("opacity","100");
        site.setAttribute("blinkcursor","true");
        addresses.appendChild(site);
    }
    saveAddressXml(doc);
    return true;
}

void Global::saveAddressXml(const QDomDocument& doc)
{
    QFile ofile(m_addrXml);
    if (ofile.open(QIODevice::WriteOnly)) {
        QTextStream out(&ofile);
        out.setCodec("UTF-8");
        out << doc.toString();
        ofile.close();
    }
}

void Global::loadPrefence()
{
    QString strTmp;

    m_pref.XIM = (Global::Conversion)m_config->getItemValue("preference", "xim").toInt();
    m_pref.nWordWrap = m_config->getItemValue("preference", "wordwrap").toInt();
    m_pref.bWheel = m_config->getItemValue("preference", "wheel").toBool();
    m_pref.bUrl = m_config->getItemValue("preference", "url").toBool();
    m_pref.bBlinkTab = m_config->getItemValue("preference", "blinktab").toBool();
    m_pref.bWarn = m_config->getItemValue("preference", "warn").toBool();
    m_pref.nBeep = m_config->getItemValue("preference", "beep").toInt();
    m_pref.strWave = m_config->getItemValue("preference", "wavefile").toString();
    m_pref.strHttp = m_config->getItemValue("preference", "http").toString();
    m_pref.bAA = m_config->getItemValue("preference", "antialias").toBool();
    m_pref.bTray = m_config->getItemValue("preference", "tray").toBool();
    m_pref.strPlayer = m_config->getItemValue("preference", "externalplayer").toString();
    m_pref.strImageViewer = m_config->getItemValue("preference", "image").toString();
    m_pref.bClearPool = m_config->getItemValue("preference", "clearpool").toBool();

    strTmp = m_config->getItemValue("preference", "pool").toString();
    m_pref.strPoolPath = strTmp.isEmpty() ? Global::instance()->pathCfg() + "pool/" : strTmp;
    if (m_pref.strPoolPath.right(1) != "/")
        m_pref.strPoolPath.append('/');
    
    strTmp = m_config->getItemValue("preference", "zmodem").toString();
    m_pref.strZmPath = strTmp.isEmpty() ? Global::instance()->pathCfg() + "zmodem/" : strTmp;
    if (m_pref.strZmPath.right(1) != "/")
        m_pref.strZmPath.append('/');
    
    
}

QString Global::getOpenFileName(const QString & filter, QWidget * widget)
{
    QString path = m_config->getItemValue("global", "openfiledialog").toString();

    QString strOpen = QFileDialog::getOpenFileName(widget, "choose a file", path, filter);

    if (!strOpen.isEmpty()) {
        // save the path
        QFileInfo fi(strOpen);
        m_config->setItemValue("global", "openfiledialog", fi.absolutePath());
        m_config->save();
    }

    return strOpen;
}

// warning for overwrite getSaveFileName
QString Global::getSaveFileName(const QString& filename, QWidget* widget)
{
    // get the previous dir
    QString path = m_config->getItemValue("global", "savefiledialog").toString();

    QString strSave = QFileDialog::getSaveFileName(widget, tr("Choose a file to save under"), path + "/" + filename, "*");

    QFileInfo fi(strSave);

    while (fi.exists()) {
        int yn = QMessageBox::warning(widget, "QTerm",
                                      tr("File exists. Overwrite?"), tr("Yes"), tr("No"));
        if (yn == 0)
            break;
        strSave = QFileDialog::getSaveFileName(widget, tr("Choose a file to save under"), path + "/" + filename, "*");
        if (strSave.isEmpty())
            break;
    }

    if (!strSave.isEmpty()) {
        // save the path
        m_config->setItemValue("global", "savefiledialog", fi.absolutePath());
        m_config->save();
    }

    return strSave;
}

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
bool Global::iniWorkingDir(QString param)
{
    char ExeNamePath[MAX_PATH], _fileCfg[MAX_PATH], _addrCfg[MAX_PATH], _addrXml[MAX_PATH];
    size_t LastSlash = 0;

    if (0 == GetModuleFileNameA(NULL, ExeNamePath, MAX_PATH)) {
        const char *DefaultModuleName = "./qterm.exe";
        strcpy(ExeNamePath, DefaultModuleName);
    }
    for (size_t i = 0, ModuleNameLen = strlen(ExeNamePath) ;
            i < ModuleNameLen ; ++i) {
        if (ExeNamePath[i] == '\\') {
            ExeNamePath[i] = '/';
            LastSlash = i;
        }
    }
    ExeNamePath[LastSlash+1] = '\0';
    m_pathLib = QString::fromLocal8Bit(ExeNamePath);
    m_pathCfg = QString::fromLocal8Bit(ExeNamePath);
    strcpy(_fileCfg, ExeNamePath);
    strcat(_fileCfg, "qterm.cfg");
    m_fileCfg = QString::fromLocal8Bit(_fileCfg);
    strcpy(_addrCfg, ExeNamePath);
    strcat(_addrCfg, "address.cfg");
    m_addrCfg = QString::fromLocal8Bit(_addrCfg);
    strcpy(_addrXml, ExeNamePath);
    strcat(_addrXml, "address.xml");    
    m_addrXml = QString::fromLocal8Bit(_addrXml);
    if (!convertAddressBook2XML())
        return false;

    QString pathScheme = m_pathCfg + "scheme";
    if (!isPathExist(pathScheme))
        return false;
    return true;
}
#else
bool Global::iniWorkingDir(QString param)
{
    QDir dir;
    QFileInfo fi;
    QString prefix = QCoreApplication::applicationDirPath();
#ifdef Q_OS_MACX
    // $HOME/Library/QTerm/
    QString pathHome = QDir::homePath();
    m_pathCfg = pathHome + "/Library/QTerm/";
    if (!isPathExist(m_pathCfg))
        return false;

    // get executive file path
    m_pathLib = prefix + "/../Resources/";
#else

    QFileInfo conf(prefix+"/qterm.cfg");
    if (conf.exists()) {
        QString path= QCoreApplication::applicationDirPath()+"/";
        m_pathLib = path;
        m_pathCfg = path;
        return true;
    }

    prefix.chop(3); // "bin"
    m_pathCfg = QDir::homePath() + "/.qterm/";
    if (!isPathExist(m_pathCfg))
        return false;

    // pathLib --- where datedir "pic", "cursor", "po"
    if (param.indexOf('/') == -1)
        m_pathLib = prefix + "share/qterm/";
    else {
        QFileInfo fi(param);
        if (fi.isSymLink())
            param = fi.readLink();
        // get the pathname
        param.truncate(param.lastIndexOf('/'));
        QString oldPath = QDir::currentPath();
        QDir::setCurrent(param);
        dir.setPath(QCoreApplication::applicationDirPath());
        if (dir == QDir::current())
            m_pathLib = prefix + "share/qterm/";
        else
            m_pathLib = QDir::currentPath();
        QDir::setCurrent(oldPath);
        m_pathLib += '/';
    }
#endif

    QString pathScheme = m_pathCfg + "scheme";
    if (!isPathExist(pathScheme))
        return false;


    // copy configuration files
    m_fileCfg = m_pathCfg + "qterm.cfg";
    if (!createLocalFile(m_fileCfg, m_pathLib + "qterm.cfg"))
        return false;
    m_addrCfg = m_pathCfg + "address.cfg";
    //if (!createLocalFile(m_addrCfg, m_pathLib + "address.cfg"))
    //    return false;
    m_addrXml = m_pathCfg + "address.xml";
    //if (!createLocalFile(m_addrXml, m_pathLib + "address.xml"))
    //    return false;

    if (!convertAddressBook2XML())
        return false;
    return true;
}
#endif

bool Global::iniSettings()
{
    //install the translator
    QString lang = m_config->getItemValue("global", "language").toString();
    Global::Language language = Global::English;
    if (lang == "chs")
        language = Global::SimplifiedChinese;
    else if (lang == "cht")
        language = Global::TraditionalChinese;
    setLanguage(language);
    //set font
    QString family = m_config->getItemValue("global", "font").toString();
    QString pointsize = m_config->getItemValue("global", "pointsize").toString();
    if (!family.isEmpty()) {
        QFont font(family);
        if (pointsize.toInt() > 0)
            font.setPointSize(pointsize.toInt());
        QString bAA = m_config->getItemValue("global", "antialias").toString();
        if (bAA != "0")
            font.setStyleStrategy(QFont::PreferAntialias);
        qApp->setFont(font);
    }

    // zmodem and pool directory
    QString pathZmodem = m_config->getItemValue("preference", "zmodem").toString();
    if (pathZmodem.isEmpty())
        pathZmodem = m_pathCfg + "zmodem";
    if (!isPathExist(pathZmodem))
        return false;

    QString pathPool = m_config->getItemValue("preference", "pool").toString();
    if (pathPool.isEmpty())
        pathPool = m_pathCfg + "pool/";
    if (pathPool.right(1) != "/")
        pathPool.append('/');

    QString pathCache = pathPool + "shadow-cache/";

    if ((!isPathExist(pathPool)) || (!isPathExist(pathCache)))
        return false;

    return true;
}


bool Global::isPathExist(const QString& path)
{
    QDir dir(path);
    if (! dir.exists()) {
        if (!dir.mkdir(path)) {
            qDebug("Failed to create directory %s", (const char*)path.toLocal8Bit());
            return false;
        }
    }
    return true;
}

bool Global::createLocalFile(const QString& dst, const QString& src)
{
    QDir dir;
    if (dir.exists(dst))
        return true;

    if (!dir.exists(src)) {
        qDebug("QTerm failed to find %s.\n"
               "Please copy the %s from the source tarball to %s yourself.",
               (const char*)src.toLocal8Bit(), (const char*)src.toLocal8Bit(),
               (const char*) dst.toLocal8Bit());
        return false;
    }

    if (!QFile::copy(src, dst)) {
        qDebug("QTerm failed to copy %s to %s.\n"
               "Please copy the %s from the source tarball to %s yourself.",
               (const char*)src.toLocal8Bit(), (const char *)dst.toLocal8Bit(),
               (const char*)src.toLocal8Bit(), (const char *)dst.toLocal8Bit());
        return false;
    }
    return true;
}
bool Global::isBossColor() const
{
    return m_bossColor;
}

const QString & Global::escapeString() const
{
    return m_escape;
}

Global::Conversion Global::clipConversion() const
{
    return m_clipConversion;
}

Global::Language Global::language() const
{
    return m_language;
}

Global::Position Global::scrollPosition() const
{
    return m_scrollPos;
}

bool Global::isFullScreen() const
{
    return m_fullScreen;
}

bool Global::showSwitchBar() const
{
    return m_switchBar;
}

bool Global::showStatusBar() const
{
    return m_statusBar;
}

bool Global::showMenuBar() const
{
    return m_menuBar;
}

void Global::setClipConversion(Global::Conversion conversionId)
{
    m_clipConversion = conversionId;
}

void Global::setEscapeString(const QString & escapeString)
{
    m_escape = escapeString;
}

void Global::setScrollPosition(Global::Position position)
{
    m_scrollPos = position;
}

void Global::setStatusBar(bool isShow)
{
    m_statusBar = isShow;
}

void Global::setMenuBar(bool isShow)
{
    m_menuBar = isShow;
}

void Global::setBossColor(bool isBossColor)
{
    m_bossColor = isBossColor;
}

void Global::setFullScreen(bool isFullscreen)
{
    m_fullScreen = isFullscreen;
}

void Global::setSwitchBar(bool isShow)
{
    m_switchBar = isShow;
}

void Global::setLanguage(Global::Language language)
{
    m_language = language;
    // unload previous translation
    if (!m_translatorQT->isEmpty())
        qApp->removeTranslator(m_translatorQT);
    if (!m_translatorQTerm->isEmpty())
        qApp->removeTranslator(m_translatorQTerm);
    // check new translation files
    QString qt_qm, qterm_qm;
    switch(language)
    {
    case Global::SimplifiedChinese:
        qt_qm = QLibraryInfo::location(QLibraryInfo::TranslationsPath)+"/qt_zh_CN.qm";
        qterm_qm = m_pathCfg + "/po/qterm_chs.qm";
        if (!QFile::exists(qterm_qm))
            qterm_qm = m_pathLib + "po/qterm_chs.qm";

        break;
    case Global::TraditionalChinese:
        qt_qm = QLibraryInfo::location(QLibraryInfo::TranslationsPath)+"/qt_zh_TW.qm";
        qterm_qm = m_pathCfg + "/po/qterm_cht.qm";
        if (!QFile::exists(qterm_qm))
            qterm_qm = m_pathLib + "po/qterm_cht.qm";
        break;
    case Global::English:
        return;
    }
    // load qt library translation
    if (m_translatorQT->load(qt_qm))
        qApp->installTranslator(m_translatorQT);
    // load qterm translation
    if (m_translatorQTerm->load(qterm_qm))
        qApp->installTranslator(m_translatorQTerm);
}

const QString & Global::style() const
{
    return m_style;
}

void Global::setStyle(const QString & style)
{
    m_style = style;
}

void Global::loadConfig()
{
    setFullScreen(m_config->getItemValue("global", "fullscreen").toBool());
    setStyle(m_config->getItemValue("global", "theme").toString());
    setClipConversion((Conversion)m_config->getItemValue("global", "clipcodec").toInt());
    setScrollPosition((Position)m_config->getItemValue("global", "vscrollpos").toInt());
    setMenuBar(m_config->getItemValue("global", "menubar").toBool());
    setStatusBar(m_config->getItemValue("global", "statusbar").toBool());
    setSwitchBar( m_config->getItemValue("global", "switchbar").toBool());

    setBossColor(false);
    setEscapeString("");

    loadPrefence();
}

void Global::saveConfig()
{

    QString lang;
    //language
    switch (m_language)
    {
    case English:            lang = "eng"; break;
    case SimplifiedChinese:  lang = "chs"; break;
    case TraditionalChinese: lang = "cht"; break;
    }
    m_config->setItemValue("global", "language", lang);
    m_config->setItemValue("global", "font", qApp->font().family());
    m_config->setItemValue("global", "pointsize", QFontInfo(qApp->font()).pointSize());
    m_config->setItemValue("global", "fullscreen", isFullScreen());
    m_config->setItemValue("global", "theme", style());
    m_config->setItemValue("global", "clipcodec", clipConversion());
    m_config->setItemValue("global", "vscrollpos", scrollPosition());
    m_config->setItemValue("global", "menubar", showMenuBar());
    m_config->setItemValue("global", "statusbar", showStatusBar());
    m_config->setItemValue("global", "switchbar", showSwitchBar());
    m_config->save();

}

QByteArray Global::loadGeometry(const QString & window)
{
    return m_config->getItemValue("global",window+"_geometry").toByteArray();
}

QByteArray Global::loadState(const QString & window)
{
    return m_config->getItemValue("global",window+"_state").toByteArray();
}

void Global::saveGeometry(const QString & window, const QByteArray & geometry)
{
    m_config->setItemValue("global", window+"_geometry", geometry);
}

void Global::saveState(const QString & window, const QByteArray & state)
{
    m_config->setItemValue("global", window+"_state", state);
}

void Global::saveSession(const QList<QVariant>& sites)
{
    m_config->setItemValue("global", "sites", sites);
}

QList<QVariant> Global::loadSession()
{
    return m_config->getItemValue("global", "sites").toList();
}

void Global::cleanup()
{
#ifdef KWALLET_ENABLED
    if (m_wallet != NULL)
        m_wallet->close();
#endif // KWALLET_ENABLED
    if (m_pref.bClearPool) {
        clearDir(m_pref.strZmPath);
        clearDir(m_pref.strPoolPath);
        clearDir(m_pref.strPoolPath + "shadow-cache/");
    }
}

void Global::openUrl(const QString & urlStr)
{
    QString command = m_pref.strHttp;
    if (command.isEmpty()) {
        QUrl url(urlStr,QUrl::TolerantMode);
        if (!QDesktopServices::openUrl(url)) {
            qDebug("Failed to open the url with QDesktopServices");
        }
        return;
    }

    if(command.indexOf("%L")==-1) // no replace
        //QApplication::clipboard()->setText(strUrl);
        command += " \"" + urlStr +"\"";
    else
        command.replace("%L",  "\"" + urlStr + "\"");
        //cstrCmd.replace("%L",  strUrl.toLocal8Bit());

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    command += " &";
    system(command.toUtf8().data());
#else
    QProcess::startDetached(command);
#endif

}

QString Global::convert(const QString & source, Global::Conversion flag)
{
    switch (flag) {
    case No_Conversion:
        return source;
    case Simplified_To_Traditional:
        return m_converter->S2T(source);
    case Traditional_To_Simplified:
        return m_converter->T2S(source);
    default:
        return source;
    }
    // Just in case
    qDebug("Global::convert: we should not be here");
    return source;
}

} // namespace QTerm

#include <moc_qtermglobal.cpp>
