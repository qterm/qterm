/*******************************************************************************
FILENAME:      main.cpp
REVISION:      2001.10.4 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#else 
#define VERSION	"0.2.1"
#define QTERM_DATADIR "/usr/share/qterm"
#define QTERM_BINDIR  "/usr/bin"
#endif

// remove this when use configure

#include <qpixmap.h>
#include <qstringlist.h>
#include <qapplication.h>
#include "qtermframe.h"
#include "qtermconfig.h"
#include "qtermparam.h"

#ifndef _OS_WIN32_
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>
#endif
#include <qdir.h>
#include <qfile.h>

char fileCfg[128]="./qterm.cfg";
char addrCfg[128]="./address.cfg";

QString pathLib="./";
QString pathPic="./";

#ifndef _OS_WIN32_
int iniWorkingDir( QString param )
{
	// pathLib --- where datedir "pic", "cursor", "po"
	if(param.find('/')==-1)
		pathLib=QTERM_DATADIR"/";
	else
	{
		param.truncate( param.findRev('/') );
		QString oldPath=QDir::currentDirPath();
		QDir::setCurrent( param );
		if( QDir::currentDirPath()==QTERM_BINDIR )
			pathLib=QTERM_DATADIR;
		else
			pathLib=QDir::currentDirPath();
		QDir::setCurrent( oldPath );
		pathLib+='/';
	}


	QDir dir;
	
	QString pathHome=QDir::homeDirPath()+"/.qterm";

	dir.setPath( pathHome );
	if( ! dir.exists() )
	{
		if( !dir.mkdir(pathHome) )
		{
			return -1;
		}
	}

	// picPath --- $HOME/.qterm/pic prefered
	pathPic = QDir::homeDirPath()+"/.qterm/pic";
	dir.setPath( pathPic );
	if( !dir.exists() )
		pathPic = pathLib;
	else
		pathPic = pathHome+"/";

	QString file=QDir::homeDirPath()+"/.qterm/qterm.cfg";
	QString addr=QDir::homeDirPath()+"/.qterm/address.cfg";

	sprintf( fileCfg,file.ascii(),file.length() );
	sprintf( addrCfg,addr.ascii(),addr.length() );

	
	if( !dir.exists(file) || !dir.exists(addr) )
	{
		QString srcCfg=pathLib+"qterm.cfg";
		if( !dir.exists( srcCfg ) )
		{
			printf(" Failed to find %s\n Make sure you have this file there\n"
				"There may be two reasons for this:\n"
				" 1. QTerm is not install properly\n"
				" 2. You have a corrupted source package.\n"
				"Solution:\n"
				"Re-Get source tarball and copy the qterm.cfg manually\n", srcCfg.ascii() );
			exit(-1);
		}
		
		QString cmd;
		cmd="/bin/cp -f "+ pathLib+"qterm.cfg ";
		cmd+=pathHome;
		

		if ( system(cmd)==-1 )
		{
			printf(" Failed to %s \nMake sure you have qterm.cfg in %s\n"
				"There may be two reasons for this:\n"
				" 1. QTerm is not install properly\n"
				" 2. You have a corrupted source package.\n"
				"Solution:\n"
				"Re-Get source tarball and copy the qterm.cfg manually\n",cmd.ascii(),pathLib.ascii());
			exit(-1);
		}

		cmd="/bin/cp -f "+ pathLib+"address.cfg ";
		cmd+=pathHome;
		

		if ( system(cmd)==-1 )
		{
			printf(" Failed to %s \nMake sure you have address.cfg in %s\n"
				"There may be two reasons for this:\n"
				" 1. QTerm is not install properly\n"
				" 2. You have a corrupted source package.\n"
				"Solution:\n Re-Get source tarball and copy the address.cfg manually\n",cmd.ascii(),pathLib.ascii());
			exit(-1);
		}


	}

	return 1;
}
#endif

void iniSettings()
{
   //read settings from qterm.cfg
	QTermConfig * conf= new QTermConfig(fileCfg);

	//install the translator
	QString lang = conf->getItemValue("global","language");
	if(lang !="eng" && !lang.isEmpty())
	{
		// look in $HOME/.qterm/po/ first
		QString qm=QDir::homeDirPath()+"/.qterm/po/qterm_"+lang+".qm";
		if(!QFile::exists(qm))
			qm=pathLib+"po/qterm_"+lang+".qm";
		static QTranslator * translator = new QTranslator(0);
		translator->load(qm);
		qApp->installTranslator(translator);
	}
	//set font
	QString family = conf->getItemValue("global","font");
	QString pointsize = conf->getItemValue("global","pointsize");
	if( family!=NULL && pointsize.toInt()!=0 )
	{
		QFont font(QString::fromLocal8Bit(family),pointsize.toInt(),QFont::Normal,false);
		qApp->setFont(font,true);
	}

    delete conf;
}

QStringList loadNameList( QTermConfig * pConf )
{
	QStringList listName;
	
	QString strTmp = pConf->getItemValue("bbs list", "num");
	
	QString strSection;
	
	for(int i=0; i<strTmp.toInt(); i++)
	{
		strSection.sprintf("bbs %d", i);
		listName.append(QString::fromLocal8Bit(pConf->getItemValue(strSection, "name")));
	}
	return listName;
}

void loadAddress( QTermConfig *pConf, int n, QTermParam& param )
{
	QString strTmp, strSection;
	if(n<0)
		strSection="default";
	else
		strSection.sprintf("bbs %d", n);
	
	param.m_strName = QString::fromLocal8Bit(pConf->getItemValue(strSection, "name"));
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
	param.m_strFontName = QString::fromLocal8Bit(pConf->getItemValue(strSection, "fontname"));
	strTmp = pConf->getItemValue(strSection, "fontsize");
	param.m_nFontSize = strTmp.toInt();
	param.m_clrFg.setNamedColor(pConf->getItemValue(strSection, "fgcolor"));
	param.m_clrBg.setNamedColor(pConf->getItemValue(strSection, "bgcolor"));
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
	param.m_strSSHUser = pConf->getItemValue(strSection, "sshuser");
	param.m_strSSHPasswd = pConf->getItemValue(strSection, "sshpassword");

	strTmp = pConf->getItemValue(strSection, "maxidle");
	param.m_nMaxIdle = strTmp.toInt();
	param.m_strAntiString = pConf->getItemValue(strSection, "antiidlestring");
	param.m_strAutoReply = QString::fromLocal8Bit(pConf->getItemValue(strSection, "autoreply"));
	strTmp = pConf->getItemValue(strSection, "bautoreply");
	param.m_bAutoReply = (strTmp!="0");
			
	strTmp = pConf->getItemValue(strSection, "reconnect");
	param.m_bReconnect = (strTmp!="0");
	strTmp = pConf->getItemValue(strSection, "interval");
	param.m_nReconnectInterval = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "retrytimes");
	param.m_nRetry = strTmp.toInt();
	strTmp = pConf->getItemValue(strSection, "menutype");
	param.m_nMenuType = strTmp.toInt();
	param.m_clrMenu.setNamedColor(pConf->getItemValue(strSection, "menucolor"));
}

void saveAddress(QTermConfig *pConf, int n, const QTermParam& param)
{
	QString strTmp, strSection;
	if(n<0)
		strSection="default";
	else
		strSection.sprintf("bbs %d", n);

	pConf->setItemValue(strSection, "name", param.m_strName.local8Bit());
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
	pConf->setItemValue(strSection, "fontname", param.m_strFontName.local8Bit());
	strTmp.setNum(param.m_nFontSize);
	pConf->setItemValue(strSection, "fontsize",strTmp);
	pConf->setItemValue(strSection, "fgcolor",param.m_clrFg.name());
	pConf->setItemValue(strSection, "bgcolor", param.m_clrBg.name());
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
	pConf->setItemValue(strSection, "sshuser", param.m_strSSHUser);
	pConf->setItemValue(strSection, "sshpassword",param.m_strSSHPasswd);

	strTmp.setNum(param.m_nMaxIdle);
	pConf->setItemValue(strSection, "maxidle", strTmp);
	pConf->setItemValue(strSection, "antiidlestring", param.m_strAntiString);
	pConf->setItemValue(strSection, "bautoreply", param.m_bAutoReply?"1":"0");
	pConf->setItemValue(strSection, "autoreply", param.m_strAutoReply.local8Bit());
	pConf->setItemValue(strSection, "reconnect", param.m_bReconnect?"1":"0");
	strTmp.setNum(param.m_nReconnectInterval);
	pConf->setItemValue(strSection, "interval", strTmp);
	strTmp.setNum(param.m_nRetry);
	pConf->setItemValue(strSection, "retrytimes", strTmp);

	strTmp.setNum(param.m_nMenuType);
	pConf->setItemValue(strSection, "menutype", strTmp);
	pConf->setItemValue(strSection, "menucolor",param.m_clrMenu.name());

}

int main( int argc, char ** argv ) 
{

    QApplication a( argc, argv );

	qApp=&a;

#ifndef _OS_WIN32_
    if( iniWorkingDir( argv[0] )<0 )
    {
	    return -1;
    }
#endif
      //set font
    iniSettings();

    QTermFrame * mw = new QTermFrame();
    mw->setCaption( "QTerm "+QString(VERSION) );
	mw->setIcon( QPixmap(pathLib+"pic/qterm_32x32.png") );
	a.setMainWidget(mw);
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();
    return res;
}
