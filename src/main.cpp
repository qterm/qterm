/*******************************************************************************
FILENAME:      main.cpp
REVISION:      2001.10.4 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/


// remove this when use configure

#include "qterm.h"
//Added by qt3to4:
// #include <Q3CString>
#include <QTranslator>
#include <QFileDialog>
#include <QMessageBox>
#ifdef HAVE_PYTHON
#include <Python.h>
#endif
// #include <qpixmap.h>
// #include <qstringlist.h>
// #include <qapplication.h>
#include <QApplication>
#include "qtermframe.h"
#include "qtermconfig.h"
#include "qtermparam.h"

#include <stdlib.h>
#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#endif
// #include <qdir.h>
// #include <qfile.h>
// #include <qfileinfo.h>
// 
// #include <qobject.h>
// #include <qwidget.h>
namespace QTerm
{
QString fileCfg="./qterm.cfg";
QString addrCfg="./address.cfg";

QString pathLib="./";
QString pathPic="./";
QString pathCfg="./";

void runProgram(const QString & cmd)
{
	QString strCmd=cmd;
	#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
	strCmd += " &";
	#endif
	system(strCmd.toLocal8Bit());
}

QString getOpenFileName(const QString& filter, QWidget * widget)
{
	Config conf(fileCfg);
	QString path = conf.getItemValue("global","openfiledialog");

	QString strOpen = QFileDialog::getOpenFileName(widget, "choose a file", path, filter);

	if(!strOpen.isEmpty())
	{
		// save the path
		QFileInfo fi(strOpen);
		conf.setItemValue("global","openfiledialog",fi.absolutePath());
		conf.save(fileCfg);
	}
	
	return strOpen;
}

// warning for overwrite getSaveFileName
QString getSaveFileName(const QString& filename, QWidget* widget)
{
	// get the previous dir
	Config conf(fileCfg);
	QString path = conf.getItemValue("global","savefiledialog");

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
		conf.setItemValue("global","savefiledialog",fi.absolutePath());
		conf.save(fileCfg);
	}

	return strSave;
}

void clearDir( const QString& path )
{
	QDir dir(path);
	if( dir.exists() )
	{
		const QFileInfoList list = dir.entryInfoList();
		//QFileInfoListIterator it( *list );
		//QFileInfo *fi;
		foreach (QFileInfo fi, list) 
		{
			if(fi.isFile())
				dir.remove(fi.fileName());
		}
	}
}

int checkPath( const QString& path )
{
	QDir dir(path);
	if( ! dir.exists() )
	{
		if( !dir.mkdir(path) )
		{
			printf("Failed to create directory %s\n", (const char*)path.toLocal8Bit());
			return -1;
		}
	}
	return 0;
}

int checkFile( const QString& dst, const QString& src )
{
	QDir dir;
	if(dir.exists(dst))
		return 0;
	
	if( !dir.exists(src) )
	{
		printf("QTerm failed to find %s.\n"
			"Please copy the qterm.cfg from the source tarball to %s\n", 
			(const char*)src.toLocal8Bit(), (const char*) dst.toLocal8Bit() );
		return -1;
	}
		
	QString cmd;
	cmd="/bin/cp -f "+ src+" "+dst;

	if ( system((const char *)cmd.toLocal8Bit())==-1 )
	{
		printf("QTerm failed to %s. \n"
				"Please copy the qterm.cfg from the source tarball to %s\n", 
			(const char*)cmd.toLocal8Bit(), (const char *)dst.toLocal8Bit());
		return -1;
	}
	return 0;
}

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
#ifndef MAX_PATH
#define MAX_PATH 128
#endif
#include <windows.h>
#include <shellapi.h>
int iniWorkingDir( QString param )
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
    pathLib = QString::fromLocal8Bit(ExeNamePath);
    pathPic = QString::fromLocal8Bit(ExeNamePath);
    pathCfg = QString::fromLocal8Bit(ExeNamePath);
    strcpy(_fileCfg, ExeNamePath);
    strcat(_fileCfg, "qterm.cfg");
    fileCfg = QString::fromLocal8Bit(_fileCfg);
    strcpy(_addrCfg, ExeNamePath);
    strcat(_addrCfg, "address.cfg");
    addrCfg = QString::fromLocal8Bit(_addrCfg);

	QString pathSchema=pathCfg+"schema";
	if(checkPath(pathSchema)==-1)
		return -1;

}
#else
int iniWorkingDir( QString param )
{
	QDir dir;
	QFileInfo fi;
#ifdef Q_OS_MACX
	// $HOME/Library/QTerm/
	QString pathHome=QDir::homePath();
	pathCfg = pathHome+"/Library/QTerm/";
	if(checkPath(pathCfg)==-1)
		return -1;

	// get executive file path 
	fi.setFile(param);
	pathLib=fi.path()+'/';
#else
	pathCfg=QDir::homePath()+"/.qterm/";
	if(checkPath(pathCfg)==-1)
		return -1;

	// pathLib --- where datedir "pic", "cursor", "po"
	if(param.indexOf('/')==-1)
		pathLib=QTERM_DATADIR"/";
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
            pathLib=QTERM_DATADIR;
        else
            pathLib=QDir::currentPath();
        QDir::setCurrent( oldPath );
        pathLib+='/';
	}
#endif

	QString pathSchema=pathCfg+"schema";
	if(checkPath(pathSchema)==-1)
		return -1;


	// picPath --- $HOME/.qterm/pic prefered
	pathPic = pathCfg+"pic";
	dir.setPath( pathPic );
	if( !dir.exists() )
		pathPic = pathLib;
	else
		pathPic = pathCfg;
	
	// configuration files
	fileCfg=pathCfg+"qterm.cfg";
	if(checkFile(fileCfg,pathLib+"qterm.cfg")==-1)
		return -1;
	addrCfg=pathCfg+"address.cfg";
	if(checkFile(addrCfg,pathLib+"address.cfg")==-1)
		return -1;
	
	return 0;
}
#endif

int iniSettings()
{
   //read settings from qterm.cfg
	Config * conf= new Config(fileCfg);

#if 0
	// get the version info first
	QString ver = conf->getItemValue("global","version");
	if(ver.toInt()<0000300)
	{
		printf("It seems that you are still using the old formated configuration file. Please update first.\n"
				"The simple way is to backup your directory ~/.qterm, delete it and run qterm again."
				"Then import your old AddressBook by hand\n" 
				"Sorry for any inconvenience. We try not to make this happen again.\n");
		exit(-1);
	}
#endif

	//install the translator
	QString lang = conf->getItemValue("global","language");
	if(lang !="eng" && !lang.isEmpty())
	{
		// look in $HOME/.qterm/po/ first
		QString qm=QDir::homePath()+"/.qterm/po/qterm_"+lang+".qm";
		if(!QFile::exists(qm))
			qm=pathLib+"po/qterm_"+lang+".qm";
		static QTranslator * translator = new QTranslator(0);
		translator->load(qm);
		qApp->installTranslator(translator);
	}
	//set font
	QString family = conf->getItemValue("global","font");
	QString pointsize = conf->getItemValue("global","pointsize");
	QString pixelsize = conf->getItemValue("global","pixelsize");
	if( !family.isEmpty() )
	{
		QFont font(family);
		if (pointsize.toInt() > 0)
			font.setPointSize(pointsize.toInt());
		if (pixelsize.toInt() > 0)
			font.setPixelSize(pixelsize.toInt());
		#if (QT_VERSION>=300)
		QString bAA = conf->getItemValue("global", "antialias");
		if(bAA!="0")
			font.setStyleStrategy(QFont::PreferAntialias);
		#endif
		qApp->setFont(font);
	}

	// zmodem and pool directory
	QString pathZmodem = conf->getItemValue("preference", "zmodem");
	if(pathZmodem.isEmpty())
		pathZmodem = pathCfg+"zmodem";
	if(checkPath(pathZmodem)==-1)
		return -1;
	
	QString pathPool = conf->getItemValue("preference", "pool");

	if(pathPool.isEmpty()) 
		pathPool = pathCfg+"pool/";

	if (pathPool.right(1) != "/")
		pathPool.append('/');

	QString pathCache = pathPool+"shadow-cache/";
	
	if(checkPath(pathPool)==-1 || checkPath(pathCache) == -1)
		return -1;

    delete conf;

	return 0;
}

QStringList loadNameList( Config * pConf )
{
	QStringList listName;
	
	QString strTmp = pConf->getItemValue("bbs list", "num");
	
	QString strSection;
	
	for(int i=0; i<strTmp.toInt(); i++)
	{
		strSection.sprintf("bbs %d", i);
		listName.append(pConf->getItemValue(strSection, "name"));
	}
	return listName;
}

bool loadAddress( Config *pConf, int n, Param& param )
{
	QString strTmp, strSection;
	if(n<0)
		strSection="default";
	else
		strSection.sprintf("bbs %d", n);
	
	// check if larger than existence
	strTmp = pConf->getItemValue("bbs list","num");
	if(n>=strTmp.toInt())
		return false;
	param.m_strName = pConf->getItemValue(strSection, "name");
	param.m_strAddr = pConf->getItemValue(strSection, "addr");
	strTmp = pConf->getItemValue(strSection, "port");
	param.m_uPort = strTmp.toUShort();
	strTmp = pConf->getItemValue(strSection, "hosttype");
	param.m_nHostType = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "autologin"); 
	param.m_bAutoLogin = (strTmp!="0");
	param.m_strPreLogin = pConf->getItemValue(strSection,"prelogin");
	param.m_strUser = pConf->getItemValue(strSection, "user");
	param.m_strPasswd = pConf->getItemValue(strSection, "password");
	param.m_strPostLogin = pConf->getItemValue(strSection, "postlogin");
	
	strTmp = pConf->getItemValue(strSection, "bbscode");
	param.m_nBBSCode = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "displaycode");
	param.m_nDispCode = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "autofont");
	param.m_bAutoFont = (strTmp!="0");
	strTmp = pConf->getItemValue(strSection, "alwayshighlight");
	param.m_bAlwaysHighlight = (strTmp!="0");
	strTmp = pConf->getItemValue(strSection, "ansicolor");
	param.m_bAnsiColor = (strTmp!="0");
	param.m_strFontName = pConf->getItemValue(strSection, "fontname");
	strTmp = pConf->getItemValue(strSection, "fontsize");
	param.m_nFontSize = strTmp.toInt();
	param.m_clrFg.setNamedColor(pConf->getItemValue(strSection, "fgcolor"));
	param.m_clrBg.setNamedColor(pConf->getItemValue(strSection, "bgcolor"));
	param.m_strSchemaFile = pConf->getItemValue(strSection, "schemafile");
	
	param.m_strTerm = pConf->getItemValue(strSection, "termtype");
	strTmp = pConf->getItemValue(strSection, "keytype");
	param.m_nKey = strTmp.toInt();
	strTmp =  pConf->getItemValue(strSection, "column");
	param.m_nCol = strTmp.toInt();
	strTmp =  pConf->getItemValue(strSection, "row" );
	param.m_nRow = strTmp.toInt();
	strTmp =  pConf->getItemValue(strSection, "scroll");
	param.m_nScrollLines = strTmp.toInt();
	strTmp =  pConf->getItemValue(strSection, "cursor");
	param.m_nCursorType = strTmp.toInt();
	param.m_strEscape = pConf->getItemValue(strSection, "escape");
	
	strTmp =  pConf->getItemValue(strSection, "proxytype");
	param.m_nProxyType = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "proxyauth");
	param.m_bAuth = (strTmp!="0");
	param.m_strProxyHost = pConf->getItemValue(strSection, "proxyaddr");
	strTmp = pConf->getItemValue(strSection, "proxyport");
	param.m_uProxyPort = strTmp.toInt();
	param.m_strProxyUser = pConf->getItemValue(strSection, "proxyuser");
	param.m_strProxyPasswd = pConf->getItemValue(strSection, "proxypassword");
	strTmp = pConf->getItemValue(strSection,"protocol");
	param.m_nProtocolType = strTmp.toInt();

	strTmp = pConf->getItemValue(strSection, "maxidle");
	param.m_nMaxIdle = strTmp.toInt();
	param.m_strReplyKey = pConf->getItemValue(strSection, "replykey");
	if(param.m_strReplyKey.isNull())
		printf("loading null\n");
			
	param.m_strAntiString = pConf->getItemValue(strSection, "antiidlestring");
	param.m_strAutoReply = pConf->getItemValue(strSection, "autoreply");
	strTmp = pConf->getItemValue(strSection, "bautoreply");
	param.m_bAutoReply = (strTmp!="0");
			
	strTmp = pConf->getItemValue(strSection, "reconnect");
	param.m_bReconnect = (strTmp!="0");
	strTmp = pConf->getItemValue(strSection, "interval");
	param.m_nReconnectInterval = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "retrytimes");
	param.m_nRetry = strTmp.toInt();

	strTmp = pConf->getItemValue(strSection, "loadscript");
	param.m_bLoadScript = (strTmp!="0");
	param.m_strScriptFile = pConf->getItemValue(strSection, "scriptfile");
	
	strTmp = pConf->getItemValue(strSection, "menutype");
	param.m_nMenuType = strTmp.toInt();
	param.m_clrMenu.setNamedColor(pConf->getItemValue(strSection, "menucolor"));

	return true;
}

void saveAddress(Config *pConf, int n, const Param& param)
{
	QString strTmp, strSection;
	if(n<0)
		strSection="default";
	else
		strSection.sprintf("bbs %d", n);

	pConf->setItemValue(strSection, "name", param.m_strName);
	pConf->setItemValue(strSection, "addr", param.m_strAddr);
	strTmp.setNum(param.m_uPort);
	pConf->setItemValue(strSection, "port", strTmp);
	strTmp.setNum( param.m_nHostType );
	pConf->setItemValue(strSection, "hosttype", strTmp);
	pConf->setItemValue(strSection, "autologin", param.m_bAutoLogin?"1":"0"); 
	pConf->setItemValue(strSection,"prelogin", param.m_strPreLogin);
	pConf->setItemValue(strSection, "user",param.m_strUser);
	pConf->setItemValue(strSection, "password", param.m_strPasswd);
	pConf->setItemValue(strSection, "postlogin",param.m_strPostLogin);
	
	strTmp.setNum( param.m_nBBSCode );
	pConf->setItemValue(strSection, "bbscode", strTmp);
	strTmp.setNum(param.m_nDispCode);
	pConf->setItemValue(strSection, "displaycode", strTmp);
	pConf->setItemValue(strSection, "autofont", param.m_bAutoFont?"1":"0");
	pConf->setItemValue(strSection, "alwayshighlight", param.m_bAlwaysHighlight?"1":"0");
	pConf->setItemValue(strSection, "ansicolor", param.m_bAnsiColor?"1":"0");
	pConf->setItemValue(strSection, "fontname", param.m_strFontName.toLocal8Bit());
	strTmp.setNum(param.m_nFontSize);
	pConf->setItemValue(strSection, "fontsize", strTmp);
	pConf->setItemValue(strSection, "fgcolor", param.m_clrFg.name());
	pConf->setItemValue(strSection, "bgcolor", param.m_clrBg.name());
	pConf->setItemValue(strSection, "schemafile", param.m_strSchemaFile);
	
	pConf->setItemValue(strSection, "termtype",param.m_strTerm);
	strTmp.setNum(param.m_nKey);
	pConf->setItemValue(strSection, "keytype",strTmp);
	strTmp.setNum(param.m_nCol);
	pConf->setItemValue(strSection, "column",strTmp);
	strTmp.setNum(param.m_nRow);
	pConf->setItemValue(strSection, "row",strTmp);
	strTmp.setNum(param.m_nScrollLines);
	pConf->setItemValue(strSection, "scroll",strTmp);
	strTmp.setNum(param.m_nCursorType);
	pConf->setItemValue(strSection, "cursor",strTmp);
	pConf->setItemValue(strSection, "escape",param.m_strEscape);

	strTmp.setNum(param.m_nProxyType);
	pConf->setItemValue(strSection, "proxytype",strTmp);
	pConf->setItemValue(strSection, "proxyauth", param.m_bAuth?"1":"0");
	pConf->setItemValue(strSection, "proxyaddr", param.m_strProxyHost);
	strTmp.setNum(param.m_uProxyPort);
	pConf->setItemValue(strSection, "proxyport",strTmp);
	pConf->setItemValue(strSection, "proxyuser",param.m_strProxyUser);
	pConf->setItemValue(strSection, "proxypassword",param.m_strProxyPasswd);
	strTmp.setNum(param.m_nProtocolType);
	pConf->setItemValue(strSection,"protocol",strTmp);

	strTmp.setNum(param.m_nMaxIdle);
	pConf->setItemValue(strSection, "maxidle", strTmp);
	pConf->setItemValue(strSection, "replykey", param.m_strReplyKey);
	pConf->setItemValue(strSection, "antiidlestring", param.m_strAntiString);
	pConf->setItemValue(strSection, "bautoreply", param.m_bAutoReply?"1":"0");
	pConf->setItemValue(strSection, "autoreply", param.m_strAutoReply.toLocal8Bit());
	pConf->setItemValue(strSection, "reconnect", param.m_bReconnect?"1":"0");
	strTmp.setNum(param.m_nReconnectInterval);
	pConf->setItemValue(strSection, "interval", strTmp);
	strTmp.setNum(param.m_nRetry);
	pConf->setItemValue(strSection, "retrytimes", strTmp);

	pConf->setItemValue(strSection, "loadscript", param.m_bLoadScript?"1":"0");
	pConf->setItemValue(strSection, "scriptfile", param.m_strScriptFile);
					
	strTmp.setNum(param.m_nMenuType);
	pConf->setItemValue(strSection, "menutype", strTmp);
	pConf->setItemValue(strSection, "menucolor",param.m_clrMenu.name());

}

#ifdef HAVE_PYTHON
PyThreadState * mainThreadState;
#endif //HAVE_PYTHON

}

int main( int argc, char ** argv ) 
{

    QApplication a( argc, argv );

	//qApp=&a;

    if( QTerm::iniWorkingDir( argv[0] )<0 )
    {
	    return -1;
    }
    //set font
    if( QTerm::iniSettings()<0 )
	{
		return -1;
	}

#ifdef HAVE_PYTHON
	  // initialize Python
    Py_Initialize();
    // initialize thread support
    PyEval_InitThreads();
    mainThreadState = NULL;
    // save a pointer to the main PyThreadState object
    mainThreadState = PyThreadState_Get();

	
	// add path
	PyRun_SimpleString("import sys\n");
	QString pathCmd;
	// pathLib/script
	pathCmd = "sys.path.insert(0,'";
	pathCmd += QTerm::pathLib+"script')";
	PyRun_SimpleString(strdup(pathCmd));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
	// $HOME/.qterm/script, override 
	pathCmd = "sys.path.insert(0,'";
	pathCmd += pathCfg+"script')";
	PyRun_SimpleString(strdup(pathCmd));
#endif	
	// release the lock
    PyEval_ReleaseLock();
#endif // HAVE_PYTHON

  
    QTerm::Frame * mw = new QTerm::Frame();
    mw->setWindowTitle( "QTerm "+QString(QTERM_VERSION) );
	mw->setWindowIcon( QPixmap(QTerm::pathLib+"pic/qterm.png") );
	//a.setMainWidget(mw);
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();

#ifdef HAVE_PYTHON
	// shut down the interpreter
	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
	// create a thread state object for this thread
	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
	PyThreadState_Swap(myThreadState);
	PyEval_AcquireLock();
	Py_Finalize();
#endif // HAVE_PYTHON
    return res;
}
