/*******************************************************************************
FILENAME:      qtermframe.cpp
REVISION:      2001.10.4 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qtermwindow.h"
#include "qtermframe.h"
#include "qtermwndmgr.h"
#include "qtermtimelabel.h"
#include "qtermconfig.h"
#include "qterm.h"
#include "qtermparam.h"
#include "qtermtoolbutton.h"
#include "statusBar.h"
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QByteArray>
#include <QCloseEvent>
#include <QTime>
#include <QSystemTrayIcon>
#include <QShortcut>
#include <QSignalMapper>

#include "aboutdialog.h"
#include "addrdialog.h"
#include "prefdialog.h"
#include "quickdialog.h"
#include "keydialog.h"
#include "trayicon.h"
#include "imageviewer.h"

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <QTextCodec>
#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMenu>
#include <QMenuBar>
#include <QTabBar>
#include <QMessageBox>
#include <QRegExp>
#include <QStyle>
#include <QStyleFactory>
#include <QFont>
#include <QFontDialog>
#include <QPalette>
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QLabel>
#include <QMovie>
#include <QLineEdit>
#include <QInputDialog>
#include <QStatusBar>

namespace QTerm
{
extern QString fileCfg;
extern QString addrCfg;

extern QString pathLib;
extern QString pathPic;
extern QString pathCfg;

extern void clearDir(const QString& );
extern QStringList loadNameList(Config *);
extern bool loadAddress(Config *, int, Param &);
extern void saveAddress(Config *, int, const Param &);

Frame* Frame::s_instance = 0;

//constructor
Frame::Frame()
    : QMainWindow( 0 )
{
	s_instance = this;
	setAttribute(Qt::WA_DeleteOnClose);
//set the layout
	//QFrame * vb = new QFrame(this);
	//QVBoxLayout * vbLayout = new QVBoxLayout(this);
	//vbLayout->setMargin(0);
// 	Q3VBox *vb = new Q3VBox( this );
// 	FIXME:QFrame?
	//vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	m_MdiArea = new QMdiArea( this );
	setCentralWidget( m_MdiArea );
	//vbLayout->addWidget(ws);

	tray = 0;
	trayMenu = 0;
	connect(this, SIGNAL(toolBarPositionChanged(QToolBar*)), 
			this, SLOT(toolBarPosChanged(QToolBar*)));

//create a tabbar in the hbox
	tabBar=new QTabBar(statusBar());
	statusBar()->addWidget(tabBar,0);
	connect(tabBar,SIGNAL(selected(int)),this,SLOT(selectionChanged(int)));
	//tabBar->setShape(QTabBar::TriangularBelow);
	tabBar->setShape(QTabBar:: RoundedSouth);
	tabBar->setDrawBase(false);

//create a progress bar to notify the download process
	m_pStatusBar = new QTerm::StatusBar(statusBar(), "mainStatusBar");
	statusBar()->addWidget(m_pStatusBar, 0);

//create the window manager to deal with the window-tab-icon pairs
	wndmgr=new WndMgr(this);

	initShortcuts();

//set menubar
	initActions();

	addMainMenu();

//setup toolbar
	addMainTool();

// add the custom defined key
	updateKeyToolBar();

// diaable some menu & toolbar
	enableMenuToolBar( false );

//initialize all settings
	iniSetting();

	initThemesMenu();

	installEventFilter(this);
}

//destructor
Frame::~Frame()
{
   delete wndmgr; 
}
//initialize setting from qterm.cfg
void Frame::iniSetting()
{
	Config * conf= new Config(fileCfg);

	QString strTmp;
	

	strTmp = conf->getItemValue("global","fullscreen");
	if(strTmp=="1")
	{
		m_bFullScreen = true;
		m_fullAction->setChecked( true );
		showFullScreen();
	}
	else
	{
		m_bFullScreen = false;
	//window size
		strTmp = conf->getItemValue("global","max");
		if(strTmp=="1")
			showMaximized();
		else
		{
			QString size = conf->getItemValue("global","size");
			if( !size.isNull() )
			{
				int x,y,cx,cy;
				sscanf(size.toLatin1(),"%d %d %d %d",&x,&y,&cx,&cy);
				resize(QSize(cx,cy));
				move(QPoint(x,y));
			}
		}
	}

	theme = conf->getItemValue("global","theme");
	QStyle * style = QStyleFactory::create(theme);
	if (style)
		qApp->setStyle(style);
	
	//language
	strTmp = conf->getItemValue("global","language");
	if(strTmp=="eng")	
		m_engAction->setChecked( true );	
	else if(strTmp=="chs")	
		m_chsAction->setChecked( true );	
	else if(strTmp=="cht")	
		m_chtAction->setChecked( true );	
	else
		m_engAction->setChecked( true );	

	m_noescAction->setChecked( true );
	m_strEscape = "";

	strTmp = conf->getItemValue("global","clipcodec");
	if(strTmp=="0")
	{
		m_nClipCodec=0;
		m_GBKAction->setChecked(true);
	}
	else
	{
		m_nClipCodec=1;
		m_BIG5Action->setChecked(true);
	}

	strTmp = conf->getItemValue("global","vscrollpos");
	if(strTmp=="0")
	{
		m_nScrollPos=0;
		m_scrollHideAction->setChecked(true);
	}
	else if(strTmp=="1")
	{
		m_nScrollPos=1;
		m_scrollLeftAction->setChecked(true);
	}
	else
	{
		m_nScrollPos=2;
		m_scrollRightAction->setChecked(true);
	}

	strTmp = conf->getItemValue("global","statusbar");
	m_bStatusBar = (strTmp!="0");
	m_statusAction->setChecked( m_bStatusBar );

	
	strTmp = conf->getItemValue("global","switchbar");
	m_bSwitchBar = (strTmp!="0");
	m_switchAction->setChecked( m_bSwitchBar );
	if(m_bSwitchBar)
		statusBar()->show();
	else
		statusBar()->hide();

	m_bBossColor = false;

	loadPref( conf );

	setUseDock(m_pref.bTray);

	delete conf;
}

void Frame::loadPref( Config * conf )
{
	QString strTmp;
	strTmp = conf->getItemValue("preference","xim");
	m_pref.nXIM = strTmp.toInt();
	strTmp = conf->getItemValue("preference","wordwrap");
	m_pref.nWordWrap = strTmp.toInt();
	strTmp = conf->getItemValue("preference","smartww");
	m_pref.bSmartWW=(strTmp!="0");
	strTmp = conf->getItemValue("preference","wheel");
	m_pref.bWheel=(strTmp!="0");
	strTmp = conf->getItemValue("preference","url");
	m_pref.bUrl=(strTmp!="0");
	strTmp = conf->getItemValue("preference","logmsg");
	m_pref.bLogMsg=(strTmp!="0");
	strTmp = conf->getItemValue("preference","blinktab");
	m_pref.bBlinkTab=(strTmp!="0");
	strTmp = conf->getItemValue("preference","warn");
	m_pref.bWarn=(strTmp!="0");
	strTmp = conf->getItemValue("preference","beep");
	m_pref.nBeep=strTmp.toInt();
	m_pref.strWave = conf->getItemValue("preference","wavefile");
	strTmp = conf->getItemValue("preference","http");
	m_pref.strHttp = strTmp;
	strTmp = conf->getItemValue("preference","antialias");
	m_pref.bAA=(strTmp!="0");
	strTmp = conf->getItemValue("preference","tray");
	m_pref.bTray=(strTmp!="0");
	strTmp = conf->getItemValue("preference","playmethod");
	m_pref.nMethod=strTmp.toInt();
	strTmp = conf->getItemValue("preference","externalplayer");
	m_pref.strPlayer=strTmp;

	strTmp = conf->getItemValue("preference","clearpool");
	m_pref.bClearPool=(strTmp!="0");
	strTmp = conf->getItemValue("preference","pool");
	m_pref.strPoolPath=strTmp.isEmpty()?pathCfg+"pool/":strTmp;
	if( m_pref.strPoolPath.right(1) != "/" )
		m_pref.strPoolPath.append('/');
	strTmp = conf->getItemValue("preference","zmodem");
	m_pref.strZmPath=strTmp.isEmpty()?pathCfg+"zmodem/":strTmp;
	if( m_pref.strZmPath.right(1) != "/" )
		m_pref.strZmPath.append('/');
	strTmp = conf->getItemValue("preference","image");
	m_pref.strImageViewer=strTmp;
}

//save current setting to qterm.cfg
void Frame::saveSetting()
{
	Config * conf= new Config(fileCfg);

	QString strTmp;
	//save font
	conf->setItemValue("global","font",qApp->font().family().toLocal8Bit());
	strTmp.setNum(QFontInfo(qApp->font()).pointSize());
	conf->setItemValue("global","pointsize",strTmp);
	strTmp.setNum(QFontInfo(qApp->font()).pixelSize());
	conf->setItemValue("global","pixelsize",strTmp);

	//save window position and size
	if(isMaximized())
	{
		conf->setItemValue("global","max","1");
	}
	else
	{
		strTmp = QString("%1 %2 %3 %4").arg(x()).arg(y()).arg(width()).arg(height());
// 		cstrTmp.sprintf("%d %d %d %d",x(),y(),width(),height());
		conf->setItemValue("global","size",strTmp);
		conf->setItemValue("global","max","0");
	}
	
	if(m_bFullScreen)
		conf->setItemValue("global","fullscreen","1");
	else
		conf->setItemValue("global","fullscreen","0");
	
	// cstrTmp.setNum(theme);
	conf->setItemValue("global","theme",theme);

	int hide,dock,index,nl,extra;

	strTmp = conf->getItemValue("global", "bbsbar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","bbsbar", valueToString(mdiconnectTools->isVisibleTo(this),
							(int)dock, index, nl==1?true:false, extra));

	strTmp = conf->getItemValue("global", "keybar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","keybar", valueToString(key->isVisibleTo(this), 
							(int)dock, index, nl==1?true:false, extra));

	strTmp = conf->getItemValue("global", "mainbar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","mainbar",valueToString(mdiTools->isVisibleTo(this),
							(int)dock, index, nl==1?true:false, extra) );

	
	strTmp.setNum(m_nClipCodec);
	conf->setItemValue("global","clipcodec",strTmp);

	strTmp.setNum(m_nScrollPos);
	conf->setItemValue("global","vscrollpos",strTmp);

	conf->setItemValue("global","statusbar", m_bStatusBar?"1":"0");
	conf->setItemValue("global","switchbar", m_bSwitchBar?"1":"0");

	conf->save(fileCfg);
	delete conf;
}

//addressbook
void Frame::addressBook()
{
	addrDialog addr(this,false);
	
	if(addr.exec()==1)
	{
		newWindow(addr.param, addr.ui.nameListWidget->currentRow());
	}
}
//quicklogin
void Frame::quickLogin()
{
	quickDialog quick(this);
	
	Config *pConf = new Config(addrCfg);
	loadAddress(pConf, -1, quick.param);
	delete pConf;

	if(quick.exec()==1)
	{
		newWindow(quick.param);
	}
}

void Frame::exitQTerm()
{
	while( wndmgr->count()>0 ) 
	{
		bool closed = m_MdiArea->activeSubWindow()->close();
		if(!closed)
		{
			return;
		}
	}

	saveSetting();
	// clear zmodem and pool if needed
	if(m_pref.bClearPool)
	{
		clearDir(m_pref.strZmPath);
		clearDir(m_pref.strPoolPath);
		clearDir(m_pref.strPoolPath+"shadow-cache/");
	}

	setUseDock(false);
	qApp->quit();
}

//create a new display window
//Window * Frame::newWindow( const Param&  param, int index )
void Frame::newWindow( const Param&  param, int index )
{
	Window * window=new Window( this, param, index, m_MdiArea,
					0 );
	m_MdiArea->addSubWindow(window);
	window->setWindowTitle( param.m_strName );
	window->setWindowIcon( QPixmap(pathLib+"pic/tabpad.png") );
	window->setAttribute(Qt::WA_DeleteOnClose);

	QIcon* icon = new QIcon(QPixmap(pathLib+"pic/tabpad.png"));
	//QTab *qtab=new QTab(*icon,window->caption());
	QString qtab = window->windowTitle();

	//add window-tab-icon to window manager
	wndmgr->addWindow(window);
	tabBar->addTab( *icon, qtab );
	
	window->setFocus();
	window->showMaximized();

	//activte the window-tab
//	window->setFocus();
//	wndmgr->activateTheTab(window);

//	return window;
}

//the tabbar selection changed
void Frame::selectionChanged(int n)
{
	wndmgr->activateTheWindow(n);
}
//slot Help->About QTerm
void Frame::aboutQTerm()
{
	aboutDialog about(this);
	
	about.exec();
}

//slot Help->Homepage
void Frame::homepage()
{
    QString strCmd = m_pref.strHttp;
	QString strUrl = "http://qterm.sourceforge.net";

    if(strCmd.indexOf("%L")==-1)
        strCmd += " \"" + strUrl +"\"";
    else
        strCmd.replace(QRegExp("%L",Qt::CaseInsensitive), strUrl);

	#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
	strCmd += " &";
	#endif
	system(strCmd.toLocal8Bit());
}
//slot Windows menu aboutToShow
void Frame::windowsMenuAboutToShow()
{
	windowsMenu->clear();
	QAction * cascadeAction = windowsMenu->addAction(tr("Cascade"), m_MdiArea, SLOT(cascadeSubWindows() ) );
	QAction * tileAction = windowsMenu->addAction(tr("Tile"), m_MdiArea, SLOT(tileSubWindows() ) );
	if ( m_MdiArea->subWindowList().isEmpty() )
	{
		cascadeAction->setEnabled(false);
		tileAction->setEnabled(false);
	}
	windowsMenu->addSeparator();

#ifdef Q_OS_MACX
	// used to dock the programe
	if(isHidden())
		windowsMenu->addAction(tr("Main Window"), this, SLOT(trayShow()));
#endif

	QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
	for ( int i = 0; i < int(windows.count()); ++i ) 
	{
		QAction * idAction = windowsMenu->addAction(windows.at(i)->windowTitle(),
				this, SLOT( windowsMenuActivated( int ) ) );
		idAction->setData(i);
		idAction->setChecked(m_MdiArea->activeSubWindow() == windows.at(i));
	}
	
}
//slot activate the window correspond with the menu id 
void Frame::windowsMenuActivated( int id )
{
	QWidget* w = m_MdiArea->subWindowList().at( id );
	if ( w ) 
	{
		w->showNormal();
//		wndmgr->activateTheTab((Window*)w);
	} 
}

void Frame::popupConnectMenu()
{
	connectMenu->clear();

	connectMenu->addAction("Quick Login", this, SLOT(quickLogin()) );
	connectMenu->addSeparator();
	
	Config conf(addrCfg);
	QStringList listName = loadNameList( &conf );
	QSignalMapper * connectMapper = new QSignalMapper(this);

	for ( int i=0; i<listName.count(); i++ )
	{
		QAction * idAction = new QAction(listName[i], connectMenu);
		connectMenu->addAction(idAction);
		connect(idAction, SIGNAL(triggered()), connectMapper, SLOT(map()));
		connectMapper->setMapping(idAction, i);
	}
	connect(connectMapper, SIGNAL(mapped(int)), this, SLOT(connectMenuActivated(int)));
	//connectMenu->exec( connectButton->mapToGlobal( connectButton->rect().bottomLeft() ));
}

void Frame::connectMenuAboutToHide()
{
// 	QMouseEvent me( QEvent::MouseButtonRelease, QPoint(0,0), QPoint(0,0), 
// 			Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
// 	QApplication::sendEvent( connectButton, &me );

}
void Frame::connectMenuActivated(int id)
{
	Config *pConf = new Config(addrCfg);
	Param param;
	// FIXME: don't know the relation with id and param setted by setItemParameter
	if(loadAddress(pConf, id, param))
		newWindow(param, id);
	delete pConf;
}

void Frame::switchWin(int id)
{
	QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
	if(windows.count() <= id)
		return;

	if(id==200)
	{
		wndmgr->activeNextPrev(false);
		return;
	}
	if(id==201 || id==202)
	{
		wndmgr->activeNextPrev(true);
		return;
	}

	QWidget *w = windows.at(id);
	if(w == m_MdiArea->activeSubWindow() )
		return;

	if(w!=NULL) {
		w->setFocus();
	}
}

bool Frame::eventFilter(QObject *o, QEvent *e)
{
/*
	if( o==this && m_pref.bTray)
	{
		if( e->type()==QEvent::ShowMinimized && m_pref.bTray )
		{
			printf("Frame::eventFilter\n");
			trayHide();
			return true;
		}
	}
	return false;
*/
	return false;
}

//slot draw something e.g. logo in the background
//TODO : draw a pixmap in the background
void Frame::paintEvent( QPaintEvent * )
{

}

void Frame::closeEvent(QCloseEvent * clse)
{
        QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
        for ( int i = 0; i < int(windows.count()); ++i )
        {

                if( ( (Window *)windows.at(i) )->isConnected() )
                {
                        if (m_pref.bTray) {
                                trayHide();
                                clse->ignore();
                                return;
                        }
                }
        }
        while( wndmgr->count()>0 )
        {
                bool closed = m_MdiArea->activeSubWindow()->close();
                if(!closed)
                {
                        clse->ignore();
                        return;
                }
        }

        saveSetting();
        // clear zmodem and pool if needed
        if(m_pref.bClearPool)
        {
                clearDir(m_pref.strZmPath);
                clearDir(m_pref.strPoolPath);
		clearDir(m_pref.strPoolPath+"shadow-cache/");

        }

        setUseDock(false);

        clse->accept();

}

void Frame::updateLang(QAction * action)
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));
	Config * conf= new Config(fileCfg);
	conf->setItemValue("global","language",action->objectName());
	conf->save(fileCfg);
	delete conf;
}

void Frame::connectIt()
{
	if( wndmgr->activeWindow()== NULL )
	{
		Param param;
		Config *pConf = new Config(addrCfg);
		loadAddress(pConf, -1, param);
		delete pConf;
		newWindow( param );
	}
	else
		if(!wndmgr->activeWindow()->isConnected())
			wndmgr->activeWindow()->reconnect();
}
void Frame::disconnect()
{
	wndmgr->activeWindow()->disconnect();
}

void Frame::copy( )
{
	wndmgr->activeWindow()->copy();
}
void Frame::paste( )
{
	wndmgr->activeWindow()->paste();
}
void Frame::copyRect(bool isEnabled)
{
	wndmgr->activeWindow()->m_bCopyRect = isEnabled;
}

void Frame::copyColor(bool isEnabled)
{
	wndmgr->activeWindow()->m_bCopyColor = isEnabled;
}

void Frame::copyArticle( )
{
	wndmgr->activeWindow()->copyArticle();
}

void Frame::autoCopy(bool isEnabled)
{
	wndmgr->activeWindow()->m_bAutoCopy = isEnabled;
}

void Frame::wordWrap(bool isEnabled)
{
	wndmgr->activeWindow()->m_bWordWrap = isEnabled;
}

void Frame::updateESC(QAction * action)
{
	if (action->objectName() == "noesc") {
		m_strEscape = "";
	} else if (action->objectName() == "escesc") {
		m_strEscape = "^[^[[";
	} else if (action->objectName() == "uesc") {
		m_strEscape = "^u[";
	} else if (action->objectName() == "custom") {
		bool ok;
		QString strEsc = QInputDialog::getText(this, "define escape", "scape string *[", QLineEdit::Normal, m_strEscape , &ok);
		if(ok)
			m_strEscape = strEsc;
	} else {
		qDebug("updateESC: should not be here");
	}
}

void Frame::updateCodec(QAction * action)
{
	if (action->objectName() == "GBK") {
		m_nClipCodec=0;
	} else if (action->objectName() == "Big5") {
		m_nClipCodec=1;
	} else {
		qDebug("updateCodec: should not be here");
	}
}

void Frame::font( )
{
	wndmgr->activeWindow()->font();
}

void Frame::color( )
{
	wndmgr->activeWindow()->color();

}
void Frame::refresh( )
{
	wndmgr->activeWindow()->refresh();
}

void Frame::uiFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,qApp->font());
	
	if(m_pref.bAA)
		font.setStyleStrategy(QFont::PreferAntialias);
	
	if(ok==true)
	{
		qApp->setFont(font, "QWidget");
	}
}

void Frame::fullscreen()
{
	m_bFullScreen = ! m_bFullScreen;
	
	if( m_bFullScreen )
	{
		menuBar()->hide();
		mdiTools->hide();
		mdiconnectTools->hide();
		key->hide();
		//showStatusBar();
		//showSwitchBar();
		showFullScreen();
	}
	else
	{
		menuBar()->show();
		mdiTools->show();
		mdiconnectTools->show();
		key->show();
		emit scrollChanged();
		//showStatusBar();
		//showSwitchBar();
		showNormal();
	}

	m_fullAction->setChecked(m_bFullScreen);
// 	menuBar()->setItemChecked( ID_VIEW_FULL, m_bFullScreen );

}

void Frame::bosscolor()
{
	m_bBossColor = !m_bBossColor;

	emit bossColor();
	
	m_bossAction->setChecked(m_bBossColor);
// 	menuBar()->setItemChecked( ID_VIEW_BOSS, m_bBossColor );
}

void Frame::initThemesMenu()
{
	themesMenu->clear();
	QActionGroup * themesGroup = new QActionGroup(this);
	QStringList styles = QStyleFactory::keys();
	for(QStringList::ConstIterator it=styles.begin(); it!=styles.end(); it++)
		themesGroup->addAction(insertThemeItem(*it));
	connect(themesGroup, SIGNAL(triggered(QAction* )), this, SLOT(themesMenuActivated(QAction*)));
}

void Frame::themesMenuActivated(QAction * action)
{
	theme = action->text();
	QStyle * style = QStyleFactory::create(theme);
	if (style)
		qApp->setStyle(style);
}

void Frame::updateScroll(QAction * action)
{
	if (action->objectName() == "Hide") {
		m_nScrollPos = 0;
	} else if (action->objectName() == "Left") {
		m_nScrollPos = 1;
	} else if (action->objectName() == "Right") {
		m_nScrollPos = 2;
	} else {
		qDebug("updateScroll: should not be here");
	}
	emit scrollChanged();
}

void Frame::updateSwitchBar(bool isEnabled)
{
	m_bSwitchBar = isEnabled;
	
	if(m_bSwitchBar)
		statusBar()->show();
	else
		statusBar()->hide();
}

void Frame::updateStatusBar(bool isEnabled)
{
	m_bStatusBar = isEnabled;
	
	emit statusBarChanged(m_bStatusBar);
}

void Frame::setting( )
{
	wndmgr->activeWindow()->setting();
}
void Frame::defaultSetting()
{
	addrDialog set(this, true);

	
	Config *pConf = new Config(addrCfg);

	if( pConf->hasSection("default") )
		loadAddress(pConf,-1,set.param);

	set.updateData(false);

	if(set.exec()==1)
	{
		saveAddress(pConf,-1,set.param);
		pConf->save(addrCfg);
	}
	
	delete pConf;
}

void Frame::preference()
{
	prefDialog pref(this);

	if(pref.exec()==1)
	{
		Config *pConf = new Config(fileCfg);
		loadPref(pConf);
		delete pConf;
		setUseDock(m_pref.bTray);
	}
}

void Frame::keySetup()
{
	keyDialog keyDlg(this);
	if(keyDlg.exec()==1)
	{
		updateKeyToolBar();
	}
}


void Frame::antiIdle(bool isEnabled)
{
	wndmgr->activeWindow()->antiIdle(isEnabled);
}

void Frame::autoReply(bool isEnabled)
{
	wndmgr->activeWindow()->autoReply(isEnabled);
}

void Frame::viewMessages( )
{
	wndmgr->activeWindow()->viewMessages();
}

void Frame::updateMouse(bool isEnabled)
{
	wndmgr->activeWindow()->m_bMouse = isEnabled;
	m_mouseAction->setChecked(wndmgr->activeWindow()->m_bMouse);
}

void Frame::viewImages()
{
	Image viewer(pathPic+"pic/shadow.png", m_pref.strPoolPath, this);
	viewer.exec();
}

void Frame::updateBeep(bool isEnabled)
{
	wndmgr->activeWindow()->m_bBeep = isEnabled;
	m_beepAction->setChecked(wndmgr->activeWindow()->m_bBeep);
}

void Frame::reconnect(bool isEnabled)
{
	wndmgr->activeWindow()->m_bReconnect = isEnabled;
}

void Frame::runScript()
{
	wndmgr->activeWindow()->runScript();
}
void Frame::stopScript()
{
	wndmgr->activeWindow()->stopScript();
}

void Frame::keyClicked(int id)
{
	if(wndmgr->activeWindow()==NULL)
		return;

	Config conf(fileCfg);

	QString strItem = QString("key%1").arg(id);
// 	strItem.sprintf("key%d",id);
	QString strTmp = conf.getItemValue("key", strItem);

	if(strTmp[0]=='0')	// key
	{
		wndmgr->activeWindow()->externInput(strTmp.mid(1).toLatin1());
	}else if(strTmp[0]=='1')	// script
	{
		wndmgr->activeWindow()->runScriptFile(strTmp.mid(1).toLatin1());
	}
	else if(strTmp[0]=='2')	// program
	{
		system((strTmp.mid(1)+" &").toLocal8Bit());
	}
}

void Frame::toolBarPosChanged(QToolBar*)
{
	Config conf(fileCfg);
	
// 	Qt::ToolBarDock dock;
	int index;
	bool nl;
	int extra;
	
//	if(bar==mdiTools)
	{
// 		getLocation(mdiTools, dock, index, nl, extra);
// 		conf.setItemValue("global","mainbar",valueToString(mdiTools->isVisible(), (int)dock, index, nl, extra));
	}
//	else if(bar==key)
	{
// 		getLocation(key, dock, index, nl, extra);
// 		conf.setItemValue("global","keybar",valueToString(key->isVisible(), (int)dock, index, nl, extra));
	}
//	else if(bar==mdiconnectTools)
	{
// 		getLocation(mdiconnectTools, dock, index, nl, extra);
// 		conf.setItemValue("global","bbsbar", valueToString(mdiconnectTools->isVisible(), (int)dock, index, nl, extra));
	}

	conf.save(fileCfg);
}

void Frame::addMainTool()
{
	mdiTools = addToolBar( "Main ToolBar" );

	Config conf(fileCfg);
	int hide,dock,index,nl,extra;

	QString strTmp = conf.getItemValue("global","mainbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
		if(hide==0)
			mdiTools->hide();
	}
	connectButton = new QToolButton( mdiTools );
	connectButton->setIcon(QPixmap(pathPic+"pic/connect.png"));

 	mdiTools->addWidget(connectButton);
	connectMenu = new QMenu(this);

	connect( connectMenu, SIGNAL(aboutToShow()), this, SLOT(popupConnectMenu()) );
	connectButton->setMenu(connectMenu);
	connectButton->setPopupMode(QToolButton::InstantPopup);
	
	mdiTools->addAction( m_quickConnectAction );
	// custom define
	key = addToolBar("Custom Key");
	strTmp = conf.getItemValue("global","keybar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
		if(hide==0)
			key->hide();
	}

	// the toolbar 
	mdiconnectTools = addToolBar( "bbs operations" );
	strTmp = conf.getItemValue("global","bbbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	}

	mdiconnectTools->addAction(m_disconnectAction);
	mdiconnectTools->addSeparator();

	mdiconnectTools->addAction(m_copyAction);
	mdiconnectTools->addAction(m_pasteAction);
	mdiconnectTools->addAction(m_rectAction);
	mdiconnectTools->addAction(m_colorCopyAction);
	mdiconnectTools->addSeparator();

	mdiconnectTools->addAction(m_fontAction);
	mdiconnectTools->addAction(m_colorAction);
	mdiconnectTools->addAction(m_refreshAction);
	mdiconnectTools->addSeparator();
	
	mdiconnectTools->addAction(m_currentSessionAction);
	mdiconnectTools->addSeparator();

	mdiconnectTools->addAction(m_copyArticleAction);
	mdiconnectTools->addAction(m_antiIdleAction);
	mdiconnectTools->addAction(m_autoReplyAction);
	mdiconnectTools->addAction(m_viewMessageAction);
	mdiconnectTools->addAction(m_mouseAction);
	mdiconnectTools->addAction(m_beepAction);
	mdiconnectTools->addAction(m_reconnectAction);
}

void Frame::initShortcuts()
{
	int i = 0;
	QShortcut * shortcut = NULL;
	QSignalMapper * addrMapper = new QSignalMapper(this);
	for (i = 0; i < 9; i++) {
		shortcut = new QShortcut(Qt::CTRL+Qt::ALT+0x30+1+i,this);
		connect(shortcut, SIGNAL(activated()), addrMapper, SLOT(map()));
		addrMapper->setMapping(shortcut, i);
	}
	connect(addrMapper, SIGNAL(mapped(int)), this, SLOT(connectMenuActivated(int)));
	QSignalMapper * windowMapper = new QSignalMapper(this);
	for (i = 0; i < 10; i++) {
		shortcut = new QShortcut(Qt::ALT+0x30+1+i,this);
		connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
		windowMapper->setMapping(shortcut, i);
	}
	shortcut = new QShortcut(Qt::ALT+Qt::Key_Left, this);
	connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
	windowMapper->setMapping(shortcut, 200);
	shortcut = new QShortcut(Qt::ALT+Qt::Key_Up, this);
	connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
	windowMapper->setMapping(shortcut, 200);
	shortcut = new QShortcut(Qt::ALT+Qt::Key_Right, this);
	connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
	windowMapper->setMapping(shortcut, 201);
	shortcut = new QShortcut(Qt::ALT+Qt::Key_Down, this);
	connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
	windowMapper->setMapping(shortcut, 201);
	connect(windowMapper, SIGNAL(mapped(int)), this, SLOT(switchWin(int)));
}

void Frame::initActions()
{
	m_connectAction = new QAction(QPixmap(pathLib+"pic/connect.png"), tr("&Connect"),this);
	m_disconnectAction = new QAction(QPixmap(pathLib+"pic/disconnect.png"), tr("&Disconnect"), this);
	m_addressAction = new QAction(QPixmap(pathLib+"pic/addr.png"), tr("&Address book"), this);
	m_addressAction->setShortcut(Qt::Key_F2);
	m_quickConnectAction = new QAction(QPixmap(pathLib+"pic/quick.png"), tr("&Quick login"),this);
	m_quickConnectAction->setShortcut(Qt::Key_F3);
	m_exitAction = new QAction(tr("&Exit"), this);

	m_copyAction = new QAction(QPixmap(pathLib+"pic/copy.png"), tr("&Copy"), this);
	m_copyAction->setShortcut(Qt::CTRL+Qt::Key_Insert);
	m_pasteAction = new QAction(QPixmap(pathLib+"pic/paste.png"),tr("&Paste"), this);
	m_pasteAction->setShortcut(Qt::SHIFT+Qt::Key_Insert);
	m_colorCopyAction = new QAction(QPixmap(pathLib+"pic/color-copy.png"), tr("C&opy with color"), this);
	m_colorCopyAction->setCheckable(true);
	m_rectAction = new QAction(QPixmap(pathLib+"pic/rect.png"), tr("&Rectangle select"), this);
	m_rectAction->setCheckable(true);
	m_autoCopyAction = new QAction(tr("Auto copy &select"), this);
	m_autoCopyAction->setCheckable(true);
	m_wwrapAction = new QAction(tr("P&aste with wordwrap"), this);
	m_wwrapAction->setCheckable(true);

	QActionGroup * escapeGroup = new QActionGroup(this);
	m_noescAction = new QAction(tr("&None"), this);
	m_noescAction->setObjectName("noesc");
	m_noescAction->setCheckable(true);
	m_escescAction = new QAction(tr("&ESC ESC ["), this);
	m_escescAction->setObjectName("escesc");
	m_escescAction->setCheckable(true);
	m_uescAction = new QAction(tr("Ctrl+&U ["), this);
	m_uescAction->setObjectName("uesc");
	m_uescAction->setCheckable(true);
	m_customescAction = new QAction(tr("&Custom..."), this);
	m_customescAction->setObjectName("custom");
	m_customescAction->setCheckable(true);
	escapeGroup->addAction(m_noescAction);
	escapeGroup->addAction(m_escescAction);
	escapeGroup->addAction(m_uescAction);
	escapeGroup->addAction(m_customescAction);

	QActionGroup * codecGroup = new QActionGroup(this);
	m_GBKAction = new QAction(tr("&GBK"), this);
	m_GBKAction->setObjectName("GBK");
	m_GBKAction->setCheckable(true);
	m_BIG5Action = new QAction(tr("&Big5"), this);
	m_BIG5Action->setObjectName("Big5");
	m_BIG5Action->setCheckable(true);
	codecGroup->addAction(m_GBKAction);
	codecGroup->addAction(m_BIG5Action);

	m_fontAction = new QAction(QPixmap(pathLib+"pic/fonts.png"), tr("&Font"), this);
	m_colorAction = new QAction(QPixmap(pathLib+"pic/color.png"), tr("&Color"), this);
	m_refreshAction = new QAction(QPixmap(pathLib+"pic/refresh.png"), tr("&Refresh"), this);
	m_refreshAction->setShortcut(Qt::Key_F5);

	QActionGroup * langGroup = new QActionGroup(this);
	m_engAction = new QAction(tr("&English"),this);
	m_engAction->setObjectName("eng");
	m_engAction->setCheckable(true);
	m_chsAction = new QAction(tr("&Simplified Chinese"),this);
	m_chsAction->setObjectName("chs");
	m_chsAction->setCheckable(true);
	m_chtAction = new QAction(tr("&Traditional Chinese"),this);
	m_chtAction->setObjectName("cht");
	m_chtAction->setCheckable(true);
	langGroup->addAction(m_engAction);
	langGroup->addAction(m_chsAction);
	langGroup->addAction(m_chtAction);

	m_uiFontAction = new QAction(tr("&UI font"), this);
	m_fullAction = new QAction(tr("&Fullscreen"), this);
	m_fullAction->setShortcut(Qt::Key_F6);
	addAction(m_fullAction);
	m_bossAction = new QAction(tr("Boss &Color"), this);
	m_bossAction->setShortcut(Qt::Key_F12);

	QActionGroup * scrollGroup = new QActionGroup(this);
	m_scrollHideAction = new QAction(tr("&Hide"), this);
	m_scrollHideAction->setObjectName("Hide");
	m_scrollHideAction->setCheckable(true);
	m_scrollLeftAction = new QAction(tr("&Left"), this);
	m_scrollLeftAction->setObjectName("Left");
	m_scrollLeftAction->setCheckable(true);
	m_scrollRightAction = new QAction(tr("&Right"), this);
	m_scrollRightAction->setObjectName("Right");
	m_scrollRightAction->setCheckable(true);
	scrollGroup->addAction(m_scrollHideAction);
	scrollGroup->addAction(m_scrollLeftAction);
	scrollGroup->addAction(m_scrollRightAction);

	m_statusAction = new QAction(tr("Status &Bar"), this);
	m_statusAction->setCheckable(true);
	m_switchAction = new QAction(tr("S&witch Bar"), this);
	m_switchAction->setCheckable(true);

	m_currentSessionAction = new QAction(QPixmap(pathLib+"pic/pref.png"), tr("&Setting for currrent session"), this);
	m_defaultAction = new QAction(tr("&Default setting"), this);
	m_prefAction = new QAction(tr("&Preference"), this);

	m_copyArticleAction = new QAction(QPixmap(pathLib+"pic/article.png"), tr("&Copy article"), this);
	m_copyArticleAction->setShortcut(Qt::Key_F9);
	m_antiIdleAction = new QAction(QPixmap(pathLib+"pic/anti-idle.png"), tr("Anti &idle"), this);
	m_antiIdleAction->setCheckable(true);
	m_autoReplyAction = new QAction(QPixmap(pathLib+"pic/auto-reply.png"), tr("Auto &reply"), this);
	m_autoReplyAction->setCheckable(true);
	m_viewMessageAction = new QAction(QPixmap(pathLib+"pic/message.png"), tr("&View messages"), this);
	m_viewMessageAction->setShortcut(Qt::Key_F10);
	m_beepAction = new QAction(QPixmap(pathLib+"pic/sound.png"), tr("&Beep "), this);
	m_beepAction->setCheckable(true);
	m_mouseAction = new QAction(QPixmap(pathLib+"pic/mouse.png"), tr("&Mouse support"), this);
	m_mouseAction->setCheckable(true);
	m_viewImageAction = new QAction(tr("&Image viewer"), this);

	m_scriptRunAction = new QAction(tr("&Run..."), this);
	m_scriptRunAction->setShortcut(Qt::Key_F7);
	m_scriptStopAction = new QAction(tr("&Stop"), this);
	m_scriptStopAction->setShortcut(Qt::Key_F8);

	m_aboutAction = new QAction(tr("About &QTerm"), this);
	m_aboutAction->setShortcut(Qt::Key_F1);
	m_homepageAction = new QAction(tr("QTerm's &Homepage"),this);

	m_reconnectAction = new QAction(QPixmap(pathPic+"pic/reconnect.png"),tr("Reconnect When Disconnected By Host"), this);
	m_reconnectAction->setCheckable(true);

	connect(m_connectAction, SIGNAL(triggered()), this, SLOT(connectIt()));
	connect(m_disconnectAction, SIGNAL(triggered()), this, SLOT(disconnect()));
	connect(m_addressAction, SIGNAL(triggered()), this, SLOT(addressBook()));
	connect(m_quickConnectAction, SIGNAL(triggered()), this, SLOT(quickLogin()));
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(exitQTerm()));

	connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
	connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
	connect(m_colorCopyAction, SIGNAL(toggled(bool)), this, SLOT(copyColor(bool)));
	connect(m_rectAction, SIGNAL(toggled(bool)), this, SLOT(copyRect(bool)));
	connect(m_autoCopyAction, SIGNAL(toggled(bool)), this, SLOT(autoCopy(bool)));
	connect(m_wwrapAction, SIGNAL(toggled(bool)), this, SLOT(wordWrap(bool)));

	connect(escapeGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateESC(QAction*)));
	connect(codecGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateCodec(QAction*)));

	connect(m_fontAction, SIGNAL(triggered()), this, SLOT(font()));
	connect(m_colorAction, SIGNAL(triggered()), this, SLOT(color()));
	connect(m_refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

	connect(langGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateLang(QAction*)));

	connect(m_uiFontAction, SIGNAL(triggered()), this, SLOT(uiFont()));
	connect(m_fullAction, SIGNAL(triggered()), this, SLOT(fullscreen()));
	connect(m_bossAction, SIGNAL(triggered()), this, SLOT(bosscolor()));

	connect(scrollGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateScroll(QAction*)));

	connect(m_statusAction, SIGNAL(toggled(bool)), this, SLOT(updateStatusBar(bool)));
	connect(m_switchAction, SIGNAL(toggled(bool)), this, SLOT(updateSwitchBar(bool)));

	connect(m_currentSessionAction, SIGNAL(triggered()), this, SLOT(setting()));
	connect(m_defaultAction, SIGNAL(triggered()), this, SLOT(defaultSetting()));
	connect(m_prefAction, SIGNAL(triggered()), this, SLOT(preference()));

	connect(m_copyArticleAction, SIGNAL(triggered()), this, SLOT(copyArticle()));
	connect(m_antiIdleAction, SIGNAL(toggled(bool)), this, SLOT(antiIdle(bool)));
	connect(m_autoReplyAction, SIGNAL(toggled(bool)), this, SLOT(autoReply(bool)));
	connect(m_viewMessageAction, SIGNAL(triggered()), this, SLOT(viewMessages()));
	connect(m_beepAction, SIGNAL(toggled(bool)), this, SLOT(updateBeep(bool)));
	connect(m_mouseAction, SIGNAL(toggled(bool)), this, SLOT(updateMouse(bool)));
	connect(m_viewImageAction, SIGNAL(triggered()), this, SLOT(viewImages()));

	connect(m_scriptRunAction, SIGNAL(triggered()), this, SLOT(runScript()));
	connect(m_scriptStopAction, SIGNAL(triggered()), this, SLOT(stopScript()));

	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(aboutQTerm()));
	connect(m_homepageAction, SIGNAL(triggered()), this, SLOT(homepage()));

	connect(m_reconnectAction, SIGNAL(toggled(bool)), this, SLOT(reconnect(bool)));
}

void Frame::addMainMenu()
{
	mainMenu = menuBar();
	
	QMenu * file = mainMenu->addMenu( tr("&File") );
	file->addAction( m_connectAction );
	file->addAction( m_disconnectAction );

	file->addSeparator();
	file->addAction( m_addressAction );
	file->addAction( m_quickConnectAction );
	file->addSeparator();
	file->addAction( m_exitAction );
	
	//Edit Menu
	QMenu * edit = new QMenu( tr("&Edit"), this );
	mainMenu->addMenu( edit );

	edit->addAction( m_copyAction );
	edit->addAction( m_pasteAction );
	edit->addSeparator();
	edit->addAction( m_colorCopyAction );
	edit->addAction( m_rectAction );
	edit->addAction( m_autoCopyAction );
	edit->addAction( m_wwrapAction );
	
	QMenu * escapeMenu = new QMenu(tr("Paste &with color"), this);
	escapeMenu->addAction( m_noescAction);
	escapeMenu->addAction( m_escescAction );
	escapeMenu->addAction( m_uescAction );
	escapeMenu->addAction( m_customescAction );
	edit->addMenu(escapeMenu);

	QMenu * codecMenu = new QMenu(tr("Clipboard &encoding"),this);
	codecMenu->addAction( m_GBKAction );
	codecMenu->addAction( m_BIG5Action );
	edit->addMenu(codecMenu);

	//View menu
	QMenu * view = new QMenu( tr("&View"), this );
	mainMenu->addMenu( view );
	
	view->addAction( m_fontAction );
	view->addAction( m_colorAction );
	view->addAction( m_refreshAction );
	view->addSeparator();

	//language menu
	langMenu = new QMenu( tr("&Language"), this );
	langMenu->addAction( m_engAction );
	langMenu->addAction( m_chsAction );
	langMenu->addAction( m_chtAction );
	view->addMenu( langMenu );
	view->addAction( m_uiFontAction );

	themesMenu = new QMenu( tr("&Themes"), this );
	view->addMenu( themesMenu );

	view->addAction( m_fullAction );
	view->addAction( m_bossAction );

	view->addSeparator();
	QMenu *scrollMenu = new QMenu(tr("&ScrollBar"),this);
	scrollMenu->addAction( m_scrollHideAction );
	scrollMenu->addAction( m_scrollLeftAction );
	scrollMenu->addAction( m_scrollRightAction );
	view->addMenu( scrollMenu );
	view->addAction( m_statusAction );
	view->addAction( m_switchAction );

	
	// Option Menu	
	QMenu * option = new QMenu( tr("&Option"), this );
	mainMenu->addMenu( option );

	option->addAction( m_currentSessionAction );
	option->addSeparator();
	option->addAction( m_defaultAction );
	option->addAction( m_prefAction );
	
	// Special
	QMenu * spec = new QMenu( tr("&Special"), this );
	mainMenu->addMenu( spec );
	spec->addAction( m_copyArticleAction );
	spec->addAction( m_antiIdleAction );
	spec->addAction( m_autoReplyAction );
	spec->addAction( m_viewMessageAction );
	spec->addAction( m_beepAction );
	spec->addAction( m_mouseAction );
	spec->addAction( m_viewImageAction );

	
	//Script
	QMenu * script = new QMenu( tr("Scrip&t"), this );
	mainMenu->addMenu( script );
	script->addAction( m_scriptRunAction );
	script->addAction( m_scriptStopAction );

	//Window menu
	windowsMenu = new QMenu( tr("&Windows"), this );
	connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );

	mainMenu->addMenu( windowsMenu );
	mainMenu->addSeparator();

	//Help menu
	QMenu * help = new QMenu( tr("&Help"), this );
	mainMenu->addMenu( help );
	help->addAction( m_aboutAction );
	help->addAction( m_homepageAction );

}

void Frame::updateMenuToolBar()
{
	Window * window = wndmgr->activeWindow();

	if(window==NULL)
		return;

	// update menu
// 	mainMenu->setItemEnabled( ID_FILE_DISCONNECT, window->isConnected() );
	m_disconnectAction->setEnabled(window->isConnected());
	
// 	mainMenu->setItemChecked( ID_EDIT_COLOR, window->m_bCopyColor );
// 	mainMenu->setItemChecked( ID_EDIT_RECT, window->m_bCopyRect );
// 	mainMenu->setItemChecked( ID_EDIT_AUTO,	window->m_bAutoCopy );
// 	mainMenu->setItemChecked( ID_EDIT_WW,	window->m_bWordWrap );
	
	m_colorCopyAction->setChecked(window->m_bCopyColor);
	m_rectAction->setChecked(window->m_bCopyRect);
	m_autoCopyAction->setChecked(window->m_bAutoCopy);
	m_wwrapAction->setChecked(window->m_bWordWrap);

// 	mainMenu->setItemChecked( ID_VIEW_FULL, m_bFullScreen );
	
	m_fullAction->setChecked(m_bFullScreen);


// 	mainMenu->setItemChecked( ID_SPEC_ANTI,	window->m_bAntiIdle );
// 	mainMenu->setItemChecked( ID_SPEC_AUTO,	window->m_bAutoReply );
// 	mainMenu->setItemChecked( ID_SPEC_BEEP,	window->m_bBeep );
// 	mainMenu->setItemChecked( ID_SPEC_MOUSE, window->m_bMouse );

	m_antiIdleAction->setChecked(window->m_bAntiIdle);
	m_autoReplyAction->setChecked(window->m_bAutoReply);
	m_beepAction->setChecked(window->m_bBeep);
	m_mouseAction->setChecked(window->m_bMouse);
	
	// update toolbar
	
// 	disconnectButton->setEnabled( window->isConnected() );
// 
// 	editColor->setOn( window->m_bCopyColor );
// 	editRect->setOn( window->m_bCopyRect );
// 
// 	specAnti->setOn( window->m_bAntiIdle );
// 	specAuto->setOn( window->m_bAutoReply );
// 	specBeep->setOn( window->m_bBeep );
// 	specMouse->setOn( window->m_bMouse );
// 	specReconnect->setOn( window->m_bReconnect );
}

void Frame::enableMenuToolBar( bool enable )
{
// 	mainMenu->setItemEnabled( ID_FILE_DISCONNECT,	enable );
	
	m_disconnectAction->setEnabled(enable);
	
// 	mainMenu->setItemEnabled( ID_EDIT_COPY, 	enable );
// 	mainMenu->setItemEnabled( ID_EDIT_PASTE, 	enable );
// 	mainMenu->setItemEnabled( ID_EDIT_COLOR,	enable );
// 	mainMenu->setItemEnabled( ID_EDIT_RECT,		enable );
// 	mainMenu->setItemEnabled( ID_EDIT_AUTO, 	enable );
// 	mainMenu->setItemEnabled( ID_EDIT_WW, 		enable );
	
	m_copyAction->setEnabled(enable);
	m_pasteAction->setEnabled(enable);
	m_colorCopyAction->setEnabled(enable);
	m_rectAction->setEnabled(enable);
	m_autoCopyAction->setEnabled(enable);
	m_wwrapAction->setEnabled(enable);
	
// 	mainMenu->setItemEnabled( ID_VIEW_FONT, 	enable );
// 	mainMenu->setItemEnabled( ID_VIEW_COLOR, 	enable );
// 	mainMenu->setItemEnabled( ID_VIEW_REFRESH, 	enable );
	
	m_fontAction->setEnabled(enable);
	m_colorAction->setEnabled(enable);
	m_refreshAction->setEnabled(enable);
	
	
// 	mainMenu->setItemEnabled( ID_OPTION_CURRENT,enable );
	
	m_currentSessionAction->setEnabled(enable);

// 	mainMenu->setItemEnabled( ID_SPEC_ARTICLE, 	enable );
// 	mainMenu->setItemEnabled( ID_SPEC_ANTI,		enable );
// 	mainMenu->setItemEnabled( ID_SPEC_AUTO,		enable );
// 	mainMenu->setItemEnabled( ID_SPEC_MESSAGE,	enable );
// 	mainMenu->setItemEnabled( ID_SPEC_BEEP,		enable );
// 	mainMenu->setItemEnabled( ID_SPEC_MOUSE,	enable );
	
	m_copyArticleAction->setEnabled(enable);
	m_antiIdleAction->setEnabled(enable);
	m_autoReplyAction->setEnabled(enable);
	m_viewMessageAction->setEnabled(enable);
	m_beepAction->setEnabled(enable);
	m_mouseAction->setEnabled(enable);

// 	mainMenu->setItemEnabled( ID_SCRIPT_RUN,	enable );
// 	mainMenu->setItemEnabled( ID_SCRIPT_STOP,	enable );
	
	m_scriptRunAction->setEnabled(enable);
	m_scriptStopAction->setEnabled(enable);

	if( enable )
		mdiconnectTools->show();
	else
		mdiconnectTools->hide();

	return;
}

void Frame::updateKeyToolBar()
{
	key->clear();
	key->addAction( QPixmap(pathPic+"pic/keys.png"), tr("Key Setup"), this, SLOT(keySetup()) );

	Config * conf= new Config(fileCfg);
	QString strItem, strTmp;
	strTmp = conf->getItemValue("key", "num");
	int num = strTmp.toInt();

	for(int i=0; i<num; i++)
	{
		strItem = QString("name%1").arg(i);
		strTmp = conf->getItemValue("key", strItem);
		ToolButton *button = new ToolButton(key, i, strTmp);
// 		button->setUsesTextLabel(true);
 		button->setText(strTmp);
// 		button->setTextPosition(QToolButton::BesideIcon);
		strItem = QString("key%1").arg(i);
		strTmp = (conf->getItemValue("key", strItem));
// 		QToolTip::add( button, strTmp.mid(1) );
// 		button->addToolTip(strTmp.mid(1));
		connect(button, SIGNAL(buttonClicked(int)), this, SLOT(keyClicked(int)));
		key->addWidget(button);
	}

	delete conf;
}

QString Frame::valueToString(bool shown, int dock, int index, bool nl, int extra )
{
	QString str="";

	str = QString("%1 %2 %3 %4 %5").arg(shown?1:0).arg(dock).arg(index).arg(nl?1:0).arg(extra);

	return str;
}

void Frame::popupFocusIn(Window *)
{
	// bring to font
	if(isHidden()) {
		show();
	}
	if(isMinimized()) {
		if(isMaximized())
			showMaximized();
		else
			showNormal();
	}
	raise();
	activateWindow();
}

QAction * Frame::insertThemeItem(const QString & themeitem)
{
	QAction * idAction = new QAction(themeitem, this);
	themesMenu->addAction(idAction);
	idAction->setCheckable(true);
	idAction->setChecked(themeitem==theme);
	return idAction;
}

void Frame::setUseDock(bool use)
{
	if(use == false) {
		if(tray) {
			delete tray;
			tray = 0;
			delete trayMenu;
			trayMenu = 0;
		}
		return;
	}

	if(tray || !QSystemTrayIcon::isSystemTrayAvailable ())
		return;

	trayMenu = new QMenu;
	connect(trayMenu, SIGNAL(aboutToShow()), SLOT(buildTrayMenu()));

	
	tray = new QSystemTrayIcon( this ); //pathLib+"pic/qterm_tray.png", "QTerm", trayMenu, this);
	tray->setIcon(QPixmap(pathPic+"pic/qterm_tray.png"));
	tray->setContextMenu(trayMenu);
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
// 	connect(tray, SIGNAL(closed()), this, SLOT(exitQTerm()));

	tray->show();
}

void Frame::buildTrayMenu()
{
	if(!trayMenu)
		return;
	trayMenu->clear();

	if(isHidden())
		trayMenu->addAction(tr("Show"), this, SLOT(trayShow()));
	else
		trayMenu->addAction(tr("Hide"), this, SLOT(trayHide()));
	trayMenu->addSeparator();	
	trayMenu->addAction(tr("About"), this, SLOT(aboutQTerm()));
	trayMenu->addAction(tr("Exit"), this, SLOT(exitQTerm()));
}

void Frame::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		if (isHidden()) {
			trayShow();
		} else {
			trayHide();
		}
		break;
	case QSystemTrayIcon::Context:
		return;
	default:
		return;
	}
}
/*
void Frame::trayClicked(const QPoint &, int)
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}

void Frame::trayDoubleClicked()
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}
*/
void Frame::trayShow()
{
	show();
	// bring to font
	if(isHidden()) {
		show();
	}
	if(isMinimized()) {
		if(isMaximized())
			showMaximized();
		else
			#ifdef Q_OS_MACX
			showMaximized();
			#else
			showNormal();
			#endif
	}
	raise();
	activateWindow();
}

void Frame::trayHide()
{
	hide();
}

void Frame::buzz()
{
	int xp = x();
	int yp = y();
	QTime t;

	t.start();
	for ( int i = 32; i > 0; )
	{
		//QApplication::processEvents();
		qApp->processEvents();
		if ( t.elapsed() >= 1 )
	 	{
			int delta = i >> 2;
			int dir = i & 3;
			int dx = ((dir==1)||(dir==2)) ? delta : -delta;
			int dy = (dir<2) ? delta : -delta;
			move( xp+dx, yp+dy );
			t.restart();
			i--;
			}
		}
	move( xp, yp );
}

//----------------------------------------------------------------------------
// MTray
//----------------------------------------------------------------------------
// MTray::MTray(const QString &icon, const QString &tip, QMenu *popup, QObject *parent)
// :QObject(parent)
// {
// 	ti = new TrayIcon(QPixmap(icon), tip, popup);
// 	connect(ti, SIGNAL(clicked(const QPoint &, int)), SIGNAL(clicked(const QPoint &, int)));
// 	connect(ti, SIGNAL(doubleClicked(const QPoint &)), SIGNAL(doubleClicked(const QPoint &)));
// 	connect(ti, SIGNAL(closed()), SIGNAL(closed()));
// 	ti->show();
// }
// 
// MTray::~MTray()
// {
// 	delete ti;
// }
// 
// void MTray::setToolTip(const QString &str)
// {
// 	ti->setToolTip(str);
// }
// 
// void MTray::setImage(const QImage &i)
// {
// 	ti->setIcon(makeIcon(i));
// }
// 
// void MTray::show()
// {
// 	ti->show();
// }
// 
// void MTray::hide()
// {
// 	ti->hide();
// }
// 
// QPixmap MTray::makeIcon(const QImage &_in)
// {
// #ifdef Q_WS_X11
// 		// on X11, the KDE dock is 22x22.  let's make our icon "seem" bigger.
// 		QImage real(22,22,QImage::Format_ARGB32);
// 		//QImage in = _in.convertToImage();
// 		QImage in = _in;
// 		in.detach();
// 		real.fill(0);
// // 		real.setAlphaBuffer(true);
// 
// 		// make sure it is no bigger than 16x16
// 		if(in.width() > 16 || in.height() > 16)
// 			in = in.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
// 
// 		int xo = (real.width() - in.width()) / 2;
// 		int yo = (real.height() - in.height()) / 2;
// 
// 		int n, n2;
// 
// 		// draw a dropshadow
// 		for(n2 = 0; n2 < in.height(); ++n2) {
// 			for(n = 0; n < in.width(); ++n) {
// 				if(qAlpha(in.pixel(n,n2))) {
// 					int x = n + xo + 2;
// 					int y = n2 + yo + 2;
// 					real.setPixel(x, y, qRgba(0x80,0x80,0x80,0xff));
// 				}
// 			}
// 		}
// 		// draw the image
// 		for(n2 = 0; n2 < in.height(); ++n2) {
// 			for(n = 0; n < in.width(); ++n) {
// 				if(qAlpha(in.pixel(n,n2))) {
// 					QRgb c = in.pixel(n, n2);
// 					real.setPixel(n + xo, n2 + yo, qRgba(qRed(c), qGreen(c), qBlue(c), 0xff));
// 				}
// 			}
// 		}
// 		// create the alpha layer
// 		for(n2 = real.height()-2; n2 >= 0; --n2) {
// 			for(n = 0; n < real.width(); ++n) {
// 				uint c = real.pixel(n, n2);
// 				if(c > 0) {
// 					QRgb old = real.pixel(n, n2);
// 					real.setPixel(n, n2, qRgba(qRed(old), qGreen(old), qBlue(old), 0xff));
// 				}
// 			}
// 		}
// 
// 		QPixmap icon;
// 		icon = QPixmap::fromImage(real);
// 		return icon;
// 	//}
// #else
// 	QPixmap icon;
// 	icon.convertFromImage(_in);
// 	return icon;
// #endif
// }
}

#include <qtermframe.moc>
