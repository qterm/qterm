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
#include "qterm.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
#ifndef MAX_PATH
#define MAX_PATH 128
#endif
#include <windows.h>
#include <shellapi.h>
#endif

namespace QTerm
{

Global * Global::m_Instance = 0;

Global * Global::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new Global;

        mutex.unlock();
    }

    return m_Instance;
}

Global::Global()
    : m_fileCfg("./qterm.cfg"),m_addrCfg("./address.cfg"),m_pathLib("./"),m_pathPic("./"),m_pathCfg("./"), m_status(OK)
{
    if (!iniWorkingDir(qApp->arguments()[0])) {
        m_status = ERROR;
        return;
    }
    if (!iniSettings()) {
        m_status = ERROR;
        return;
    }
    m_config = new Config(m_fileCfg);
    m_address = new Config(m_addrCfg);
}

bool Global::isOK()
{
    if (m_status == OK) {
        return true;
    }
    return false;
}

Config * Global::fileCfg()
{
    return m_config;
}

Config * Global::addrCfg()
{
    return m_address;
}

const QString & Global::pathLib()
{
    return m_pathLib;
}

const QString & Global::pathPic()
{
    return m_pathPic;
}

const QString & Global::pathCfg()
{
    return m_pathCfg;
}

void Global::clearDir(const QString & path)
{
    QDir dir(path);
    if( dir.exists() )
    {
        const QFileInfoList list = dir.entryInfoList();
        foreach (QFileInfo fi, list) 
        {
            if(fi.isFile())
                dir.remove(fi.fileName());
        }
    }
}

QStringList Global::loadNameList()
{
    QStringList listName;

    QString strTmp = m_address->getItemValue("bbs list", "num");

    QString strSection;

    for(int i=0; i<strTmp.toInt(); i++)
    {
        strSection.sprintf("bbs %d", i);
        listName.append(m_address->getItemValue(strSection, "name"));
    }

    return listName;
}

bool Global::loadAddress(int n, Param& param)
{
    QString strTmp, strSection;
    if(n<0)
        strSection="default";
    else
        strSection.sprintf("bbs %d", n);

    // check if larger than existence
    strTmp = m_address->getItemValue("bbs list","num");
    if(n>=strTmp.toInt())
        return false;
    param.m_strName = m_address->getItemValue(strSection, "name");
    param.m_strAddr = m_address->getItemValue(strSection, "addr");
    strTmp = m_address->getItemValue(strSection, "port");
    param.m_uPort = strTmp.toUShort();
    strTmp = m_address->getItemValue(strSection, "hosttype");
    param.m_nHostType = strTmp.toInt();
    strTmp = m_address->getItemValue(strSection, "autologin"); 
    param.m_bAutoLogin = (strTmp!="0");
    param.m_strPreLogin = m_address->getItemValue(strSection,"prelogin");
    param.m_strUser = m_address->getItemValue(strSection, "user");
    param.m_strPasswd = m_address->getItemValue(strSection, "password");
    param.m_strPostLogin = m_address->getItemValue(strSection, "postlogin");

    strTmp = m_address->getItemValue(strSection, "bbscode");
    param.m_nBBSCode = strTmp.toInt();
    strTmp = m_address->getItemValue(strSection, "displaycode");
    param.m_nDispCode = strTmp.toInt();
    strTmp = m_address->getItemValue(strSection, "autofont");
    param.m_bAutoFont = (strTmp!="0");
    strTmp = m_address->getItemValue(strSection, "alwayshighlight");
    param.m_bAlwaysHighlight = (strTmp!="0");
    strTmp = m_address->getItemValue(strSection, "ansicolor");
    param.m_bAnsiColor = (strTmp!="0");
    param.m_strFontName = m_address->getItemValue(strSection, "fontname");
    strTmp = m_address->getItemValue(strSection, "fontsize");
    param.m_nFontSize = strTmp.toInt();
    param.m_clrFg.setNamedColor(m_address->getItemValue(strSection, "fgcolor"));
    param.m_clrBg.setNamedColor(m_address->getItemValue(strSection, "bgcolor"));
    param.m_strSchemaFile = m_address->getItemValue(strSection, "schemafile");

    param.m_strTerm = m_address->getItemValue(strSection, "termtype");
    strTmp = m_address->getItemValue(strSection, "keytype");
    param.m_nKey = strTmp.toInt();
    strTmp =  m_address->getItemValue(strSection, "column");
    param.m_nCol = strTmp.toInt();
    strTmp =  m_address->getItemValue(strSection, "row" );
    param.m_nRow = strTmp.toInt();
    strTmp =  m_address->getItemValue(strSection, "scroll");
    param.m_nScrollLines = strTmp.toInt();
    strTmp =  m_address->getItemValue(strSection, "cursor");
    param.m_nCursorType = strTmp.toInt();
    param.m_strEscape = m_address->getItemValue(strSection, "escape");

    strTmp =  m_address->getItemValue(strSection, "proxytype");
    param.m_nProxyType = strTmp.toInt();
    strTmp = m_address->getItemValue(strSection, "proxyauth");
    param.m_bAuth = (strTmp!="0");
    param.m_strProxyHost = m_address->getItemValue(strSection, "proxyaddr");
    strTmp = m_address->getItemValue(strSection, "proxyport");
    param.m_uProxyPort = strTmp.toInt();
    param.m_strProxyUser = m_address->getItemValue(strSection, "proxyuser");
    param.m_strProxyPasswd = m_address->getItemValue(strSection, "proxypassword");
    strTmp = m_address->getItemValue(strSection,"protocol");
    param.m_nProtocolType = strTmp.toInt();

    strTmp = m_address->getItemValue(strSection, "maxidle");
    param.m_nMaxIdle = strTmp.toInt();
    param.m_strReplyKey = m_address->getItemValue(strSection, "replykey");
    if(param.m_strReplyKey.isNull())
        printf("loading null\n");

    param.m_strAntiString = m_address->getItemValue(strSection, "antiidlestring");
    param.m_strAutoReply = m_address->getItemValue(strSection, "autoreply");
    strTmp = m_address->getItemValue(strSection, "bautoreply");
    param.m_bAutoReply = (strTmp!="0");

    strTmp = m_address->getItemValue(strSection, "reconnect");
    param.m_bReconnect = (strTmp!="0");
    strTmp = m_address->getItemValue(strSection, "interval");
    param.m_nReconnectInterval = strTmp.toInt();
    strTmp = m_address->getItemValue(strSection, "retrytimes");
    param.m_nRetry = strTmp.toInt();

    strTmp = m_address->getItemValue(strSection, "loadscript");
    param.m_bLoadScript = (strTmp!="0");
    param.m_strScriptFile = m_address->getItemValue(strSection, "scriptfile");

    strTmp = m_address->getItemValue(strSection, "menutype");
    param.m_nMenuType = strTmp.toInt();
    param.m_clrMenu.setNamedColor(m_address->getItemValue(strSection, "menucolor"));

    return true;
}

void Global::saveAddress(int n, const Param& param)
{
    QString strTmp, strSection;
    if(n<0)
        strSection="default";
    else
        strSection.sprintf("bbs %d", n);

    m_address->setItemValue(strSection, "name", param.m_strName);
    m_address->setItemValue(strSection, "addr", param.m_strAddr);
    strTmp.setNum(param.m_uPort);
    m_address->setItemValue(strSection, "port", strTmp);
    strTmp.setNum( param.m_nHostType );
    m_address->setItemValue(strSection, "hosttype", strTmp);
    m_address->setItemValue(strSection, "autologin", param.m_bAutoLogin?"1":"0"); 
    m_address->setItemValue(strSection,"prelogin", param.m_strPreLogin);
    m_address->setItemValue(strSection, "user",param.m_strUser);
    m_address->setItemValue(strSection, "password", param.m_strPasswd);
    m_address->setItemValue(strSection, "postlogin",param.m_strPostLogin);

    strTmp.setNum( param.m_nBBSCode );
    m_address->setItemValue(strSection, "bbscode", strTmp);
    strTmp.setNum(param.m_nDispCode);
    m_address->setItemValue(strSection, "displaycode", strTmp);
    m_address->setItemValue(strSection, "autofont", param.m_bAutoFont?"1":"0");
    m_address->setItemValue(strSection, "alwayshighlight", param.m_bAlwaysHighlight?"1":"0");
    m_address->setItemValue(strSection, "ansicolor", param.m_bAnsiColor?"1":"0");
    m_address->setItemValue(strSection, "fontname", param.m_strFontName.toLocal8Bit());
    strTmp.setNum(param.m_nFontSize);
    m_address->setItemValue(strSection, "fontsize", strTmp);
    m_address->setItemValue(strSection, "fgcolor", param.m_clrFg.name());
    m_address->setItemValue(strSection, "bgcolor", param.m_clrBg.name());
    m_address->setItemValue(strSection, "schemafile", param.m_strSchemaFile);

    m_address->setItemValue(strSection, "termtype",param.m_strTerm);
    strTmp.setNum(param.m_nKey);
    m_address->setItemValue(strSection, "keytype",strTmp);
    strTmp.setNum(param.m_nCol);
    m_address->setItemValue(strSection, "column",strTmp);
    strTmp.setNum(param.m_nRow);
    m_address->setItemValue(strSection, "row",strTmp);
    strTmp.setNum(param.m_nScrollLines);
    m_address->setItemValue(strSection, "scroll",strTmp);
    strTmp.setNum(param.m_nCursorType);
    m_address->setItemValue(strSection, "cursor",strTmp);
    m_address->setItemValue(strSection, "escape",param.m_strEscape);

    strTmp.setNum(param.m_nProxyType);
    m_address->setItemValue(strSection, "proxytype",strTmp);
    m_address->setItemValue(strSection, "proxyauth", param.m_bAuth?"1":"0");
    m_address->setItemValue(strSection, "proxyaddr", param.m_strProxyHost);
    strTmp.setNum(param.m_uProxyPort);
    m_address->setItemValue(strSection, "proxyport",strTmp);
    m_address->setItemValue(strSection, "proxyuser",param.m_strProxyUser);
    m_address->setItemValue(strSection, "proxypassword",param.m_strProxyPasswd);
    strTmp.setNum(param.m_nProtocolType);
    m_address->setItemValue(strSection,"protocol",strTmp);

    strTmp.setNum(param.m_nMaxIdle);
    m_address->setItemValue(strSection, "maxidle", strTmp);
    m_address->setItemValue(strSection, "replykey", param.m_strReplyKey);
    m_address->setItemValue(strSection, "antiidlestring", param.m_strAntiString);
    m_address->setItemValue(strSection, "bautoreply", param.m_bAutoReply?"1":"0");
    m_address->setItemValue(strSection, "autoreply", param.m_strAutoReply.toLocal8Bit());
    m_address->setItemValue(strSection, "reconnect", param.m_bReconnect?"1":"0");
    strTmp.setNum(param.m_nReconnectInterval);
    m_address->setItemValue(strSection, "interval", strTmp);
    strTmp.setNum(param.m_nRetry);
    m_address->setItemValue(strSection, "retrytimes", strTmp);

    m_address->setItemValue(strSection, "loadscript", param.m_bLoadScript?"1":"0");
    m_address->setItemValue(strSection, "scriptfile", param.m_strScriptFile);

    strTmp.setNum(param.m_nMenuType);
    m_address->setItemValue(strSection, "menutype", strTmp);
    m_address->setItemValue(strSection, "menucolor",param.m_clrMenu.name());
    m_address->save();

}

QString Global::getOpenFileName(const QString & filter, QWidget * widget)
{
    QString path = m_config->getItemValue("global","openfiledialog");

    QString strOpen = QFileDialog::getOpenFileName(widget, "choose a file", path, filter);

    if(!strOpen.isEmpty())
    {
        // save the path
        QFileInfo fi(strOpen);
        m_config->setItemValue("global","openfiledialog",fi.absolutePath());
        m_config->save();
    }

    return strOpen;
}

// warning for overwrite getSaveFileName
QString Global::getSaveFileName(const QString& filename, QWidget* widget)
{
    // get the previous dir
    QString path = m_config->getItemValue("global","savefiledialog");

    QString strSave = QFileDialog::getSaveFileName(widget, "Choose a file to save under", path+"/"+filename, "*");

    QFileInfo fi(strSave);

    while(fi.exists())
    {
        int yn = QMessageBox::warning(widget, "QTerm", 
                    "File exists. Overwrite?", "Yes", "No");
        if(yn==0)
            break;
        strSave = QFileDialog::getSaveFileName(widget, "Choose a file to save under", path+"/"+filename, "*");
        if(strSave.isEmpty())
            break;
    }

    if(!strSave.isEmpty())
    {
        // save the path
        m_config->setItemValue("global","savefiledialog",fi.absolutePath());
        m_config->save();
    }

    return strSave;
}

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
bool Global::iniWorkingDir( QString param )
{
    char ExeNamePath[MAX_PATH], _fileCfg[MAX_PATH], _addrCfg[MAX_PATH];
    size_t LastSlash = 0;

    if(0 == GetModuleFileNameA(NULL, ExeNamePath, MAX_PATH))
    {
        const char *DefaultModuleName = "./qterm.exe";
        strcpy(ExeNamePath, DefaultModuleName);
    }
    for(size_t i = 0, ModuleNameLen = strlen(ExeNamePath) ;
        i < ModuleNameLen ; ++i)
    {
        if(ExeNamePath[i] == '\\')
        {
            ExeNamePath[i] = '/';
            LastSlash = i;
        }
    }
    ExeNamePath[LastSlash+1] = '\0';
    m_pathLib = QString::fromLocal8Bit(ExeNamePath);
    m_pathPic = QString::fromLocal8Bit(ExeNamePath);
    m_pathCfg = QString::fromLocal8Bit(ExeNamePath);
    strcpy(_fileCfg, ExeNamePath);
    strcat(_fileCfg, "qterm.cfg");
    m_fileCfg = QString::fromLocal8Bit(_fileCfg);
    strcpy(_addrCfg, ExeNamePath);
    strcat(_addrCfg, "address.cfg");
    m_addrCfg = QString::fromLocal8Bit(_addrCfg);

    QString pathSchema=m_pathCfg+"schema";
    if(!isPathExist(pathSchema))
        return false;

}
#else
bool Global::iniWorkingDir( QString param )
{
    QDir dir;
    QFileInfo fi;
#ifdef Q_OS_MACX
    // $HOME/Library/QTerm/
    QString pathHome=QDir::homePath();
    m_pathCfg = pathHome+"/Library/QTerm/";
    if(!isPathExist(m_pathCfg))
        return false;

    // get executive file path 
    fi.setFile(param);
    m_pathLib=fi.path()+'/';
#else
    m_pathCfg=QDir::homePath()+"/.qterm/";
    if(!isPathExist(m_pathCfg))
        return false;

    // pathLib --- where datedir "pic", "cursor", "po"
    if(param.indexOf('/')==-1)
        m_pathLib=QTERM_DATADIR"/";
    else
    {
        QFileInfo fi(param);
        if( fi.isSymLink() )
            param = fi.readLink();
        // get the pathname
        param.truncate( param.lastIndexOf('/') );
        QString oldPath=QDir::currentPath();
        QDir::setCurrent( param );
        dir.setPath(QTERM_BINDIR);
        if( dir == QDir::current() )
            m_pathLib=QTERM_DATADIR;
        else
            m_pathLib=QDir::currentPath();
        QDir::setCurrent( oldPath );
        m_pathLib+='/';
    }
#endif

    QString pathSchema=m_pathCfg+"schema";
    if(!isPathExist(pathSchema))
        return false;


    // picPath --- $HOME/.qterm/pic prefered
    m_pathPic = m_pathCfg+"pic";
    dir.setPath( m_pathPic );
    if( !dir.exists() )
        m_pathPic = m_pathLib;
    else
        m_pathPic = m_pathCfg;

    // copy configuration files
    m_fileCfg=m_pathCfg+"qterm.cfg";
    if(!createLocalFile(m_fileCfg,m_pathLib+"qterm.cfg"))
        return false;
    m_addrCfg=m_pathCfg+"address.cfg";
    if(!createLocalFile(m_addrCfg,m_pathLib+"address.cfg"))
        return false;

    return true;
}
#endif

bool Global::iniSettings()
{
    //install the translator
    QString lang = m_config->getItemValue("global","language");
    if(lang !="eng" && !lang.isEmpty())
    {
        // look in $HOME/.qterm/po/ first
        QString qm=QDir::homePath()+"/.qterm/po/qterm_"+lang+".qm";
        if(!QFile::exists(qm))
            qm=m_pathLib+"po/qterm_"+lang+".qm";
        static QTranslator * translator = new QTranslator(0);
        translator->load(qm);
        qApp->installTranslator(translator);
    }
    //set font
    QString family = m_config->getItemValue("global","font");
    QString pointsize = m_config->getItemValue("global","pointsize");
    QString pixelsize = m_config->getItemValue("global","pixelsize");
    if( !family.isEmpty() )
    {
        QFont font(family);
        if (pointsize.toInt() > 0)
            font.setPointSize(pointsize.toInt());
        if (pixelsize.toInt() > 0)
            font.setPixelSize(pixelsize.toInt());
        #if (QT_VERSION>=300)
        QString bAA = m_config->getItemValue("global", "antialias");
        if(bAA!="0")
            font.setStyleStrategy(QFont::PreferAntialias);
        #endif
        qApp->setFont(font);
        qApp->setFont(font, "QWidget");
    }

    // zmodem and pool directory
    QString pathZmodem = m_config->getItemValue("preference", "zmodem");
    if(pathZmodem.isEmpty())
        pathZmodem = m_pathCfg+"zmodem";
    if(!isPathExist(pathZmodem))
        return false;

    QString pathPool = m_config->getItemValue("preference", "pool");

    if(pathPool.isEmpty())
        pathPool = m_pathCfg+"pool/";

    if (pathPool.right(1) != "/")
        pathPool.append('/');

    QString pathCache = pathPool+"shadow-cache/";

    if((!isPathExist(pathPool))||(!isPathExist(pathCache)))
        return false;

    return true;
}


bool Global::isPathExist( const QString& path )
{
    QDir dir(path);
    if( ! dir.exists() )
    {
        if( !dir.mkdir(path) )
        {
            qDebug("Failed to create directory %s", (const char*)path.toLocal8Bit());
            return false;
        }
    }
    return true;
}

bool Global::createLocalFile( const QString& dst, const QString& src )
{
    QDir dir;
    if(dir.exists(dst))
        return true;

    if( !dir.exists(src) )
    {
        qDebug("QTerm failed to find %s.\n"
               "Please copy the %s from the source tarball to %s yourself.",
            (const char*)src.toLocal8Bit(), (const char*)src.toLocal8Bit(),
            (const char*) dst.toLocal8Bit() );
        return false;
    }

    if (!QFile::copy(src,dst))
    {
        qDebug("QTerm failed to copy %s to %s.\n"
               "Please copy the %s from the source tarball to %s yourself.",
            (const char*)src.toLocal8Bit(), (const char *)dst.toLocal8Bit(),
            (const char*)src.toLocal8Bit(), (const char *)dst.toLocal8Bit());
        return false;
    }
    return true;
}

} // namespace QTerm
