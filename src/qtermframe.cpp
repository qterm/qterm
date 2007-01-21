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
#include <QWorkspace>
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

extern QString fileCfg;
extern QString addrCfg;

extern QString pathLib;
extern QString pathPic;
extern QString pathCfg;

extern void clearDir(const QString& );
extern QStringList loadNameList(QTermConfig *);
extern bool loadAddress(QTermConfig *, int, QTermParam &);
extern void saveAddress(QTermConfig *, int, const QTermParam &);

QTermFrame* QTermFrame::s_instance = 0;

//constructor
QTermFrame::QTermFrame()
    : QMainWindow( 0 )
{
	s_instance = this;
	setAttribute(Qt::WA_DeleteOnClose);
//set the layout
	QFrame * vb = new QFrame(this);
	QVBoxLayout * vbLayout = new QVBoxLayout(vb);
	vbLayout->setMargin(0);
// 	Q3VBox *vb = new Q3VBox( this );
// 	FIXME:QFrame?
	vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setCentralWidget( vb );
	ws = new QWorkspace( vb );
	vbLayout->addWidget(ws);

	tray = 0;
	trayMenu = 0;
	
//set menubar
	addMainMenu();

//setup toolbar
	addMainTool();

// add the custom defined key
	updateKeyToolBar();

// diaable some menu & toolbar
	enableMenuToolBar( false );

// 
	connect(this, SIGNAL(toolBarPositionChanged(QToolBar*)), 
			this, SLOT(toolBarPosChanged(QToolBar*)));
//!!!create a horizonal layout to hold the tabbar,the reason of why not put
//the tabbar directly on the status bar is when no tab in the tabbar,the
//statusbar display a horizonal line,ugly.
//perhaps there is some function in statusbar to solve this.
	QWidget * hb = new QWidget(statusBar());
	QHBoxLayout * hbLayout = new QHBoxLayout(hb);
	hbLayout->setMargin(0);
// 	Q3HBox * hb=new Q3HBox(statusBar());
	statusBar()->addWidget(hb,90);
//create a tabbar in the hbox
	tabBar=new QTabBar(hb);
	hbLayout->addWidget(tabBar);
	connect(tabBar,SIGNAL(selected(int)),this,SLOT(selectionChanged(int)));
	//tabBar->setShape(QTabBar::TriangularBelow);
	tabBar->setShape(QTabBar:: RoundedSouth);

//create a progress bar to notify the download process
	m_pStatusBar = new QTerm::StatusBar(statusBar(), "mainStatusBar");
	statusBar()->addWidget(m_pStatusBar, 0);

//create the window manager to deal with the window-tab-icon pairs
	wndmgr=new QTermWndMgr(this);

// expressly connect sites in addressbook
// 	Q3Accel *accel2 = new Q3Accel(this);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_1, 0);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_2, 1);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_3, 2);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_4, 3);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_5, 4);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_6, 5);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_7, 6);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_8, 7);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_9, 8);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_9, 9);
// 	connect( accel2, SIGNAL(activated(int)), 
// 	this,SLOT(connectMenuActivated(int)));
// 
// // ALT+# to switch between 10 windows
// 	Q3Accel *accel = new Q3Accel(this);
// 	accel->insertItem(Qt::ALT+Qt::Key_1, 1);
// 	accel->insertItem(Qt::ALT+Qt::Key_2, 2);
// 	accel->insertItem(Qt::ALT+Qt::Key_3, 3);
// 	accel->insertItem(Qt::ALT+Qt::Key_4, 4);
// 	accel->insertItem(Qt::ALT+Qt::Key_5, 5);
// 	accel->insertItem(Qt::ALT+Qt::Key_6, 6);
// 	accel->insertItem(Qt::ALT+Qt::Key_7, 7);
// 	accel->insertItem(Qt::ALT+Qt::Key_8, 8);
// 	accel->insertItem(Qt::ALT+Qt::Key_9, 9);
// // ALT+->(<-) to active next(previous) window
// 	accel->insertItem(Qt::ALT+Qt::Key_Left, 200);
// 	accel->insertItem(Qt::ALT+Qt::Key_Up, 200);
// 	accel->insertItem(Qt::ALT+Qt::Key_Right, 201);
// 	accel->insertItem(Qt::ALT+Qt::Key_Down, 201);
// 
// 	connect( accel, SIGNAL(activated(int)), this, SLOT(switchWin(int)) );
	
	//initialize all settings
	iniSetting();

	installEventFilter(this);
}

//destructor
QTermFrame::~QTermFrame()
{
   delete wndmgr; 
}
//initialize setting from qterm.cfg
void QTermFrame::iniSetting()
{
	QTermConfig * conf= new QTermConfig(fileCfg);

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

void QTermFrame::loadPref( QTermConfig * conf )
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
void QTermFrame::saveSetting()
{
	QTermConfig * conf= new QTermConfig(fileCfg);

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
void QTermFrame::addressBook()
{
	addrDialog addr(this,false);
	
	if(addr.exec()==1)
	{
		newWindow(addr.param, addr.ui.nameListWidget->currentRow());
	}
}
//quicklogin
void QTermFrame::quickLogin()
{
	quickDialog quick(this);
	
	QTermConfig *pConf = new QTermConfig(addrCfg);
	loadAddress(pConf, -1, quick.param);
	delete pConf;

	if(quick.exec()==1)
	{
		newWindow(quick.param);
	}
}

void QTermFrame::exitQTerm()
{
	while( wndmgr->count()>0 ) 
	{
		bool closed = ws->activeWindow()->close();
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
//QTermWindow * QTermFrame::newWindow( const QTermParam&  param, int index )
void QTermFrame::newWindow( const QTermParam&  param, int index )
{
	QTermWindow * window=new QTermWindow( this, param, index, ws,
					0 );
	ws->addWindow(window);
	window->setWindowTitle( param.m_strName );
	window->setWindowIcon( QPixmap(pathLib+"pic/tabpad.png") );

	QIcon* icon=new QIcon(QPixmap(pathLib+"pic/tabpad.png"));
	//QTab *qtab=new QTab(*icon,window->caption());
	QString qtab = window->windowTitle();
	tabBar->addTab( *icon, qtab );

	//if no this call, the tab wont display untill you resize the window
	tabBar->updateGeometry();
	tabBar->update();
	
	//add window-tab-icon to window manager
	wndmgr->addWindow(window,qtab,icon);
	
	if( ws->windowList().isEmpty() ){
		window->setFocus();
		window->showMaximized();
	}
	else
	{
		ws->setFocus();
		window->show();
	}

	//activte the window-tab
//	window->setFocus();
//	wndmgr->activateTheTab(window);

//	return window;
}

//the tabbar selection changed
void QTermFrame::selectionChanged(int n)
{
	QString qtab=tabBar->tabText(n);
	wndmgr->activateTheWindow(qtab);	
}
//slot Help->About QTerm
void QTermFrame::aboutQTerm()
{
	aboutDialog about(this);
	
	about.exec();
}

//slot Help->Homepage
void QTermFrame::homepage()
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
void QTermFrame::windowsMenuAboutToShow()
{
	windowsMenu->clear();
	QAction * cascadeAction = windowsMenu->addAction(tr("Cascade"), ws, SLOT(cascade() ) );
	QAction * tileAction = windowsMenu->addAction(tr("Tile"), ws, SLOT(tile() ) );
	if ( ws->windowList().isEmpty() ) 
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

	QWidgetList windows = ws->windowList();
	for ( int i = 0; i < int(windows.count()); ++i ) 
	{
		QAction * idAction = windowsMenu->addAction(windows.at(i)->windowTitle(),
				this, SLOT( windowsMenuActivated( int ) ) );
		idAction->setData(i);
		idAction->setChecked(ws->activeWindow() == windows.at(i));
	}
	
}
//slot activate the window correspond with the menu id 
void QTermFrame::windowsMenuActivated( int id )
{
	QWidget* w = ws->windowList().at( id );
	if ( w ) 
	{
		w->showNormal();
//		wndmgr->activateTheTab((QTermWindow*)w);
	} 
}

void QTermFrame::popupConnectMenu()
{
	connectMenu->clear();

	connectMenu->addAction("Quick Login", this, SLOT(quickLogin()) );
	connectMenu->addSeparator();
	
	QTermConfig conf(addrCfg);
	QStringList listName = loadNameList( &conf );

	for ( int i=0; i<listName.count(); i++ )
	{
		QAction * idAction = connectMenu->addAction( listName[i],
					this, SLOT(connectMenuActivated()));
		idAction->setData(i);
	}
	
	//connectMenu->exec( connectButton->mapToGlobal( connectButton->rect().bottomLeft() ));
}
void QTermFrame::connectMenuAboutToHide()
{
// 	QMouseEvent me( QEvent::MouseButtonRelease, QPoint(0,0), QPoint(0,0), 
// 			Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
// 	QApplication::sendEvent( connectButton, &me );

}
void QTermFrame::connectMenuActivated()
{
	QTermConfig *pConf = new QTermConfig(addrCfg);
	int id = static_cast<QAction *>(sender())->data().toInt();
	QTermParam param;
	// FIXME: don't know the relation with id and param setted by setItemParameter
	if(loadAddress(pConf, id, param))
		newWindow(param, id);
	delete pConf;
}

void QTermFrame::switchWin(int id)
{
	QWidgetList windows = ws->windowList();
	if(windows.count()==0)
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

	QWidget *w = windows.at(id-1);
	if(w == ws->activeWindow() )
		return;

	if(w!=NULL)
		w->showNormal();
}

bool QTermFrame::eventFilter(QObject *o, QEvent *e)
{
/*
	if( o==this && m_pref.bTray)
	{
		if( e->type()==QEvent::ShowMinimized && m_pref.bTray )
		{
			printf("QTermFrame::eventFilter\n");
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
void QTermFrame::paintEvent( QPaintEvent * )
{

}

void QTermFrame::closeEvent(QCloseEvent * clse)
{
        QWidgetList windows = ws->windowList();
        for ( int i = 0; i < int(windows.count()); ++i )
        {

                if( ( (QTermWindow *)windows.at(i) )->isConnected() )
                {
                        if (m_pref.bTray) {
                                trayHide();
                                return;
                        }
                }
        }
        while( wndmgr->count()>0 )
        {
                bool closed = ws->activeWindow()->close();
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

        clse->accept();

}

void QTermFrame::langEnglish()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","eng");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::langSimplified()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","chs");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::langTraditional()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));	
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","cht");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::connectIt()
{
	if( wndmgr->activeWindow()== NULL )
	{
		QTermParam param;
		QTermConfig *pConf = new QTermConfig(addrCfg);
		loadAddress(pConf, -1, param);
		delete pConf;
		newWindow( param );
	}
	else
		if(!wndmgr->activeWindow()->isConnected())
			wndmgr->activeWindow()->reconnect();
}
void QTermFrame::disconnect()
{
	wndmgr->activeWindow()->disconnect();
}

void QTermFrame::copy( )
{
	wndmgr->activeWindow()->copy();
}
void QTermFrame::paste( )
{
	wndmgr->activeWindow()->paste();
}
void QTermFrame::copyRect()
{
	wndmgr->activeWindow()->m_bCopyRect = !wndmgr->activeWindow()->m_bCopyRect;

	m_rectAction->setChecked(wndmgr->activeWindow()->m_bCopyRect);
	//menuBar()->setItemChecked( ID_EDIT_RECT, wndmgr->activeWindow()->m_bCopyRect );

// 	editRect->setChecked( wndmgr->activeWindow()->m_bCopyRect );

	
}
void QTermFrame::copyColor()
{
	wndmgr->activeWindow()->m_bCopyColor = !wndmgr->activeWindow()->m_bCopyColor;
	
	m_colorCopyAction->setChecked(wndmgr->activeWindow()->m_bCopyColor);
	//menuBar()->setItemChecked( ID_EDIT_COLOR, wndmgr->activeWindow()->m_bCopyColor );
// 	qDebug("before crash");
// 	editColor->setChecked( wndmgr->activeWindow()->m_bCopyColor );
// 	qDebug("after crash");
}
void QTermFrame::copyArticle( )
{
	wndmgr->activeWindow()->copyArticle();
}

void QTermFrame::autoCopy()
{
	wndmgr->activeWindow()->m_bAutoCopy = !wndmgr->activeWindow()->m_bAutoCopy;

	m_autoCopyAction->setChecked( wndmgr->activeWindow()->m_bAutoCopy );
	//menuBar()->setItemChecked( ID_EDIT_AUTO, wndmgr->activeWindow()->m_bAutoCopy );

}
void QTermFrame::wordWrap()
{
	wndmgr->activeWindow()->m_bWordWrap = !wndmgr->activeWindow()->m_bWordWrap;

	m_wwrapAction->setChecked(wndmgr->activeWindow()->m_bWordWrap);
// 	menuBar()->setItemChecked( ID_EDIT_WW, wndmgr->activeWindow()->m_bWordWrap );

}

void QTermFrame::noEsc()
{
// 	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "";
	m_noescAction->setChecked(true);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_NO,true); 
}
void QTermFrame::escEsc()
{
// 	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "^[^[[";
	m_escescAction->setChecked(true);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,true);
}
void QTermFrame::uEsc()
{
// 	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "^u[";
	m_uescAction->setChecked(true);
// 	menuBar()->setItemChecked(ID_EDIT_ESC_U,true);
}
void QTermFrame::customEsc()
{
	bool ok;
	QString strEsc = QInputDialog::getText(this, "define escape", "scape string *[",
					QLineEdit::Normal, m_strEscape , &ok);
	if(ok)
	{
		m_strEscape = "";
// 		menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
// 		menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
// 		menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
// 		menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

		m_strEscape = strEsc;
		m_customescAction->setChecked(true);
// 		menuBar()->setItemChecked(ID_EDIT_ESC_CUS,true);
	}
}

void QTermFrame::gbkCodec()
{
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,false);
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,false);

	m_nClipCodec=0;
	m_GBKAction->setChecked(true);
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,true);
}

void QTermFrame::big5Codec()
{
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,false);
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,false);

	m_nClipCodec=1;
	m_BIG5Action->setChecked(true);
// 	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,true);
}

void QTermFrame::font( )
{
	wndmgr->activeWindow()->font();
}

void QTermFrame::color( )
{
	wndmgr->activeWindow()->color();

}
void QTermFrame::refresh( )
{
	wndmgr->activeWindow()->refresh();
}

void QTermFrame::uiFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,qApp->font());
	
	if(m_pref.bAA)
		font.setStyleStrategy(QFont::PreferAntialias);
	
	if(ok==true)
	{
		qApp->setFont(font);
	}
}

void QTermFrame::fullscreen()
{
	m_bFullScreen = ! m_bFullScreen;
	
	if( m_bFullScreen )
	{
		menuBar()->hide();
		mdiTools->hide();
		mdiconnectTools->hide();
		key->hide();
		hideScroll();
		showStatusBar();
		showSwitchBar();
		showFullScreen();
	}
	else
	{
		menuBar()->show();
		mdiTools->show();
		mdiconnectTools->show();
		key->show();
		emit updateScroll();
		showStatusBar();
		showSwitchBar();
		showNormal();
	}

	m_fullAction->setEnabled(m_bFullScreen);
// 	menuBar()->setItemChecked( ID_VIEW_FULL, m_bFullScreen );

}

void QTermFrame::bosscolor()
{
	m_bBossColor = !m_bBossColor;

	emit bossColor();
	
	m_bossAction->setChecked(m_bBossColor);
// 	menuBar()->setItemChecked( ID_VIEW_BOSS, m_bBossColor );
}

void QTermFrame::themesMenuAboutToShow()
{
	themesMenu->clear();
// #if QT_VERSION < 300
// 	insertThemeItem("default");	
// 	#ifndef QT_NO_STYLE_CDE
// 	insertThemeItem("CDE");
// 	#endif
// 	#ifndef QT_NO_STYLE_MOTIF
// 	insertThemeItem("Motif");
// 	#endif
// 	#ifndef QT_NO_STYLE_MOTIFPLUS
// 	insertThemeItem("MotifPlus");
// 	#endif
// 	#ifndef QT_NO_STYLE_PLATINUM
// 	insertThemeItem("Platinum");
// 	#endif
// 	#ifndef QT_NO_STYLE_SGI
// 	insertThemeItem("SGI");
// 	#endif
// 	#ifndef QT_NO_STYLE_WINDOWS
// 	insertThemeItem("Windows");
// 	#endif
// #else
	QStringList styles = QStyleFactory::keys();
	for(QStringList::ConstIterator it=styles.begin(); it!=styles.end(); it++)
		insertThemeItem(*it);
// #endif
}

void QTermFrame::themesMenuActivated()
{

	theme = ((QAction*)QObject::sender())->text();
// 	fprintf(stderr, "Theme: %s\n", theme.ascii());

// #if QT_VERSION < 300
// 	if(theme=="default")
// 	{
// 	}
// 	else if(theme=="CDE")
// 	{
// 		#ifndef QT_NO_STYLE_CDE
// 		qApp->setStyle( new QCDEStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Motif")
// 	{
// 		#ifndef QT_NO_STYLE_MOTIF
// 		qApp->setStyle( new QMotifStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="MotifPlus")
// 	{
// 		#ifndef QT_NO_STYLE_MOTIFPLUS
// 		qApp->setStyle( new QMotifPlusStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Platinum")
// 	{
// 		#ifndef QT_NO_STYLE_PLATINUM
// 		qApp->setStyle( new QPlatinumStyle() );
// 		#endif
// 	}
// 	else if(theme=="SGI")
// 	{
// 		#ifndef QT_NO_STYLE_SGI
// 		qApp->setStyle( new QSGIStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Windows")
// 	{
// 		#ifndef QT_NO_STYLE_WINDOWS
// 		qApp->setStyle( new QWindowsStyle() );
// 		#endif
// 	}
// #else
	QStyle * style = QStyleFactory::create(theme);
	if (style)
		qApp->setStyle(style);
// #endif
	
}

void QTermFrame::hideScroll()
{
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, true );
	m_scrollHideAction->setChecked(true);
	m_nScrollPos = 0;

	emit updateScroll();
}
void QTermFrame::leftScroll()
{
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, true );
	
	m_scrollLeftAction->setChecked(true);
	
	m_nScrollPos = 1;

	emit updateScroll();
}
void QTermFrame::rightScroll()
{
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, false );
// 	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, true );
	
	m_scrollRightAction->setChecked(true);
	
	m_nScrollPos = 2;

	emit updateScroll();
}

void QTermFrame::showSwitchBar()
{

	m_bSwitchBar = !m_bSwitchBar; 
// 	menuBar()->setItemChecked(ID_VIEW_SWITCH,m_bSwitchBar );
	
	m_switchAction->setChecked(m_bSwitchBar);
	
	if(m_bSwitchBar)
		statusBar()->show();
	else
		statusBar()->hide();
}

void QTermFrame::showStatusBar()
{
	m_bStatusBar = !m_bStatusBar;
// 	menuBar()->setItemChecked(ID_VIEW_STATUS,m_bStatusBar );
	
	m_statusAction->setChecked(m_bStatusBar);

	emit updateStatusBar(m_bStatusBar);
}

void QTermFrame::setting( )
{
	wndmgr->activeWindow()->setting();
}
void QTermFrame::defaultSetting()
{
	addrDialog set(this, true);
	
	QTermConfig *pConf = new QTermConfig(addrCfg);

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

void QTermFrame::preference()
{
	prefDialog pref(this);

	if(pref.exec()==1)
	{
		QTermConfig *pConf = new QTermConfig(fileCfg);
		loadPref(pConf);
		delete pConf;
		setUseDock(m_pref.bTray);
	}
}

void QTermFrame::keySetup()
{
	keyDialog keyDlg(this);
	if(keyDlg.exec()==1)
	{
		updateKeyToolBar();
	}
}


void QTermFrame::antiIdle( )
{
	wndmgr->activeWindow()->antiIdle();

// 	menuBar()->setItemChecked( ID_SPEC_ANTI, wndmgr->activeWindow()->m_bAntiIdle );
	
	m_antiIdleAction->setChecked(wndmgr->activeWindow()->m_bAntiIdle);

// 	specAnti->setChecked( wndmgr->activeWindow()->m_bAntiIdle );
}

void QTermFrame::autoReply( )
{
	wndmgr->activeWindow()->autoReply();

// 	menuBar()->setItemChecked( ID_SPEC_AUTO, wndmgr->activeWindow()->m_bAutoReply );
	
	m_autoReplyAction->setChecked(wndmgr->activeWindow()->m_bAutoReply);

// 	specAuto->setChecked( wndmgr->activeWindow()->m_bAutoReply );

}
void QTermFrame::viewMessages( )
{
	wndmgr->activeWindow()->viewMessages();
}
void QTermFrame::enableMouse( )
{
	wndmgr->activeWindow()->m_bMouse = !wndmgr->activeWindow()->m_bMouse;

// 	menuBar()->setItemChecked( ID_SPEC_MOUSE, wndmgr->activeWindow()->m_bMouse );
	
	m_mouseAction->setChecked(wndmgr->activeWindow()->m_bMouse);

// 	specMouse->setChecked( wndmgr->activeWindow()->m_bMouse );

}

void QTermFrame::viewImages()
{
	QTermImage viewer(pathPic+"pic/shadow.png", m_pref.strPoolPath, this);
	viewer.exec();
}

void QTermFrame::beep()
{
	wndmgr->activeWindow()->m_bBeep = !wndmgr->activeWindow()->m_bBeep;

// 	menuBar()->setItemChecked( ID_SPEC_BEEP, wndmgr->activeWindow()->m_bBeep );
	m_beepAction->setChecked(wndmgr->activeWindow()->m_bBeep);

// 	specBeep->setChecked( wndmgr->activeWindow()->m_bBeep );

}

void QTermFrame::reconnect()
{
	wndmgr->activeWindow()->m_bReconnect = !wndmgr->activeWindow()->m_bReconnect;

// 	specReconnect->setChecked( wndmgr->activeWindow()->m_bReconnect );
}

void QTermFrame::runScript()
{
	wndmgr->activeWindow()->runScript();
}
void QTermFrame::stopScript()
{
	wndmgr->activeWindow()->stopScript();
}

void QTermFrame::keyClicked(int id)
{
	if(wndmgr->activeWindow()==NULL)
		return;

	QTermConfig conf(fileCfg);

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

void QTermFrame::toolBarPosChanged(QToolBar*)
{
	QTermConfig conf(fileCfg);
	
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

void QTermFrame::addMainTool()
{
	
	// hte main toolbar	
	mdiTools = addToolBar( "Main ToolBar" );
// 	mdiTools->setLabel("Main ToolBar");
	mdiTools->setIconSize(QSize(22,22));

	QTermConfig conf(fileCfg);
	int hide,dock,index,nl,extra;

	QString strTmp = conf.getItemValue("global","mainbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( mdiTools, Qt::ToolBarDock(dock), nl==1, index, extra );
		if(hide==0)
			mdiTools->hide();
	}
	connectButton = new QToolButton( mdiTools );
	connectButton->setIcon(QPixmap(pathPic+"pic/connect.png"));

// 	connectButton = new QToolButton( QPixmap(pathPic+"pic/connect.png"), tr("Connect"), 
// 			QString::null, NULL, NULL, mdiTools, "Connect" );
 	mdiTools->addWidget(connectButton);
	connectMenu = new QMenu(this);
	//FIXME: autoupdate menu
	connect( connectMenu, SIGNAL(aboutToShow()), this, SLOT(popupConnectMenu()) );
	connectButton->setMenu(connectMenu);
	connectButton->setPopupMode(QToolButton::InstantPopup);
// 	connect( connectMenu, SIGNAL(aboutToHide()), this, SLOT(connectMenuAboutToHide()) );
// 	connect( connectButton, SIGNAL(pressed()), this, SLOT(popupConnectMenu()) );
	
// 	mdiTools->addAction( QPixmap(pathPic+"pic/quick.png"), tr("Quick Login"), 
// 						this, SLOT(quickLogin()));
	mdiTools->addAction( m_quickConnectAction );
	m_quickConnectAction->setIcon(QPixmap(pathPic+"pic/quick.png"));
	// custom define
	key = addToolBar("Custom Key");
// 	key->setLabel(tr("Custom Key"));
	strTmp = conf.getItemValue("global","keybar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( key, Qt::ToolBarDock(dock), nl==1, index, extra );
		if(hide==0)
			key->hide();
	}

	// the toolbar 
	mdiconnectTools = addToolBar( "bbs operations" );
	strTmp = conf.getItemValue("global","bbbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( mdiconnectTools, Qt::ToolBarDock(dock), nl==1, index, extra );
	}

// 	disconnectButton =
// 		new QToolButton( QPixmap(pathPic+"pic/disconnect.png"), tr("Disconnect"), QString::null,
// 			this, SLOT(disconnect()), mdiconnectTools, "Disconnect" );
// 	m_disconnectAction = new QAction(QPixmap(pathPic+"pic/disconnect.png"), tr("Disconnect"), mdiconnectTools);
// 	connect(m_disconnectAction, SIGNAL(triggered()), this, SLOT(disconnect()));
// 	mdiconnectTools->addAction(m_disconnectAction);
	m_disconnectAction = mdiconnectTools->addAction(QPixmap(pathPic+"pic/disconnect.png"), tr("Disconnect"),this, SLOT(disconnect()));
	m_disconnectAction->setEnabled(false);
	mdiconnectTools->addSeparator();

	// Edit (5)
// 	mdiconnectTools->addAction(QPixmap(pathPic+"pic/copy.png"), tr("Copy"), this, SLOT(copy()));
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/paste.png"), tr("Paste"), this, SLOT(paste()));
// 	m_rectAction = mdiconnectTools->addAction(QPixmap(pathPic+"pic/rect.png"), tr("Rectangle Select"),this, SLOT(copyRect()));
	mdiconnectTools->addAction(m_copyAction);
	m_copyAction->setIcon(QPixmap(pathPic+"pic/copy.png"));
	mdiconnectTools->addAction(m_pasteAction);
	m_pasteAction->setIcon(QPixmap(pathPic+"pic/paste.png"));
	mdiconnectTools->addAction(m_rectAction);
	m_rectAction->setIcon(QPixmap(pathPic+"pic/rect.png"));
// 	editRect	=
// 		new QToolButton( QPixmap(pathPic+"pic/rect.png"), tr("Rectangle Select"), QString::null,
// 			this, SLOT(copyRect()), mdiconnectTools, "Rectangle Select" );
// 	editRect->setToggleButton(TRUE);
	m_rectAction->setCheckable(true);
// 	mdiconnectTools->addWidget(editRect);
// 	editColor	=
// 		new QToolButton( QPixmap(pathPic+"pic/color-copy.png"), tr("Copy With Color"), QString::null,
// 			this, SLOT(copyColor()), mdiconnectTools, "Copy With Color" );
// 	m_colorAction = mdiconnectTools->addAction(QPixmap(pathPic+"pic/color-copy.png"), tr("Copy With Color"),this, SLOT(copyColor()));
	mdiconnectTools->addAction(m_colorCopyAction);
	m_colorCopyAction->setIcon(QPixmap(pathPic+"pic/color-copy.png"));
	m_colorCopyAction->setCheckable(true);
// 	editColor->setToggleButton(TRUE);
// 	mdiconnectTools->addWidget(editColor);
	mdiconnectTools->addSeparator();

	//View (3)
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/fonts.png"), tr("Font"),this, SLOT(font()));
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/color.png"), tr("Color"), this, SLOT(color()));
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/refresh.png"), tr("Refresh Screen"), this, SLOT(refresh()));
	mdiconnectTools->addAction(m_fontAction);
	m_fontAction->setIcon(QPixmap(pathPic+"pic/fonts.png"));
	mdiconnectTools->addAction(m_colorAction);
	m_colorAction->setIcon(QPixmap(pathPic+"pic/color.png"));
	mdiconnectTools->addAction(m_refreshAction);
	m_refreshAction->setIcon(QPixmap(pathPic+"pic/refresh.png"));
	mdiconnectTools->addSeparator();
	
	// Option 
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/pref.png"), tr("Advanced Option"), this, SLOT(setting()) );
	mdiconnectTools->addAction(m_currentSessionAction);
	m_currentSessionAction->setIcon(QPixmap(pathPic+"pic/pref.png"));
	mdiconnectTools->addSeparator();

	// Spec (5)
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/article.png"), tr("Copy Article"), this, SLOT(copyArticle()));
	mdiconnectTools->addAction(m_copyArticleAction);
	m_copyArticleAction->setIcon(QPixmap(pathPic+"pic/article.png"));
// 	m_antiIdleAction = mdiconnectTools->addAction( QPixmap(pathPic+"pic/anti-idle.png"), tr("Anti-Idle"), this, SLOT(antiIdle()));
	mdiconnectTools->addAction(m_antiIdleAction);
	m_antiIdleAction->setIcon(QPixmap(pathPic+"pic/anti-idle.png"));
	m_antiIdleAction->setCheckable(true);
// 	mdiconnectTools->addWidget(specAnti);
	mdiconnectTools->addAction( m_autoReplyAction );
	m_autoReplyAction->setIcon(QPixmap(pathPic+"pic/auto-reply.png"));
// 	specAuto->setCheckable(true);
	m_autoReplyAction->setCheckable(true);
// 	mdiconnectTools->addWidget(specAuto);
// 	mdiconnectTools->addAction( QPixmap(pathPic+"pic/message.png"), tr("View Message"), this, SLOT(viewMessages()) );
	mdiconnectTools->addAction(m_viewMessageAction);
	m_viewMessageAction->setIcon(QPixmap(pathPic+"pic/message.png"));
// 	m_mouseAction	= mdiconnectTools->addAction( QPixmap(pathPic+"pic/mouse.png"), tr("Enable Mouse Support"), this, SLOT(enableMouse()) );
	mdiconnectTools->addAction(m_mouseAction);
	m_mouseAction->setIcon(QPixmap(pathPic+"pic/mouse.png"));
	m_mouseAction->setCheckable(true);
// 	mdiconnectTools->addWidget(specMouse);
// 	m_beepAction = mdiconnectTools->addAction( QPixmap(pathPic+"pic/sound.png"), tr("Beep When Message Coming"), this, SLOT(beep()) );
	mdiconnectTools->addAction(m_beepAction);
	m_beepAction->setIcon(QPixmap(pathPic+"pic/sound.png"));
// 	specBeep->setCheckable(true);
	m_beepAction->setCheckable(true);
// 	mdiconnectTools->addWidget(specBeep);
	m_reconnectAction = mdiconnectTools->addAction( QPixmap(pathPic+"pic/reconnect.png"), tr("Reconnect When Disconnected By Host"), this, SLOT(reconnect()) );
	m_reconnectAction->setCheckable(true);
// 	mdiconnectTools->addWidget(specReconnect);
}


void QTermFrame::addMainMenu()
{
	mainMenu = menuBar();//new QMenuBar(this);
	
	QMenu * file = mainMenu->addMenu( tr("&File") );
// 	mainMenu->addAction( tr("&File"), file );
	file->addAction( QPixmap(pathLib+"pic/connect.png"), 
					tr("&Connect"), this, SLOT(connectIt()) );
	file->addAction( QPixmap(pathLib+"pic/disconnect.png"),
					 tr("&Disconnect"), this, SLOT(disconnect()));//, 0, ID_FILE_DISCONNECT );

	file->addSeparator();
	file->addAction( QPixmap(pathLib+"pic/addr.png"), 
					tr("&Address book"), this, SLOT(addressBook()), Qt::Key_F2 );
	m_quickConnectAction = file->addAction( QPixmap(pathLib+"pic/quick.png"), 
					tr("&Quick login"),this, SLOT(quickLogin()), Qt::Key_F3 );
	file->addSeparator();
	file->addAction( tr("&Exit"), this, SLOT(exitQTerm()) );
	
	//Edit Menu
	QMenu * edit = new QMenu( tr("&Edit"), this );
// 	mainMenu->addMenu( tr("&Edit"), edit );
	mainMenu->addMenu( edit );

// 	edit->setCheckable( true );
	m_copyAction = edit->addAction( QPixmap(pathLib+"pic/copy.png"),
					tr("&Copy"), this, SLOT(copy()), Qt::CTRL+Qt::Key_Insert );
	m_pasteAction = edit->addAction( QPixmap(pathLib+"pic/paste.png"),
					tr("&Paste"), this, SLOT(paste()), Qt::SHIFT+Qt::Key_Insert );
	edit->addSeparator();
	m_colorCopyAction = edit->addAction( QPixmap(pathLib+"pic/color-copy.png"),
					tr("C&opy with color"), this, SLOT(copyColor()) );
	m_rectAction = edit->addAction( QPixmap(pathLib+"pic/rect.png"),
					tr("&Rectangle select"), this, SLOT(copyRect()) );
	m_autoCopyAction = edit->addAction( tr("Auto copy &select"), this, SLOT(autoCopy()) );
	m_wwrapAction = edit->addAction( tr("P&aste with wordwrap"), this, SLOT(wordWrap()) );
	
	QMenu * escapeMenu = new QMenu(tr("Paste &with color"), this);
	QActionGroup * escapeGroup = new QActionGroup(this);
	m_noescAction = escapeMenu->addAction( tr("&None"), this, SLOT(noEsc()) );
	m_escescAction = escapeMenu->addAction( tr("&ESC ESC ["), this, SLOT(escEsc()) );
	m_uescAction = escapeMenu->addAction( tr("Ctrl+&U ["), this, SLOT(uEsc()) );
	m_customescAction = escapeMenu->addAction( tr("&Custom..."), this, SLOT(customEsc()) );
	escapeGroup->addAction(m_noescAction);
	escapeGroup->addAction(m_escescAction);
	escapeGroup->addAction(m_uescAction);
	escapeGroup->addAction(m_customescAction);
	edit->addMenu(escapeMenu);

	QMenu * codecMenu = new QMenu(tr("Clipboard &encoding"),this);
	QActionGroup * codecGroup = new QActionGroup(this);
	m_GBKAction = codecMenu->addAction( tr("&GBK"), this, SLOT(gbkCodec()) );
	m_BIG5Action = codecMenu->addAction( tr("&Big5"), this, SLOT(big5Codec()) );
	codecGroup->addAction(m_GBKAction);
	codecGroup->addAction(m_BIG5Action);
	edit->addMenu(codecMenu);

	//View menu
	QMenu * view = new QMenu( tr("&View"), this );
	mainMenu->addMenu( view );
	
	m_fontAction = view->addAction( QPixmap(pathLib+"pic/fonts.png"),
					tr("&Font"), this, SLOT(font()) );
	m_colorAction = view->addAction( QPixmap(pathLib+"pic/color.png"),
					tr("&Color"), this, SLOT(color()) );
	m_refreshAction = view->addAction( QPixmap(pathLib+"pic/refresh.png"),
					tr("&Refresh"), this, SLOT(refresh()), Qt::Key_F5 );
	view->addSeparator();
	//language menu
	langMenu = new QMenu( tr("&Language"), this );
	m_engAction = langMenu->addAction( tr("&English"),this,SLOT(langEnglish()));
	m_chsAction = langMenu->addAction( tr("&Simplified Chinese"),this,SLOT(langSimplified()));
	m_chtAction = langMenu->addAction( tr("&Traditional Chinese"),this,SLOT(langTraditional()));
	view->addMenu( langMenu );
	view->addAction( tr("&UI font"), this, SLOT(uiFont()) );

	themesMenu = new QMenu( tr("&Themes"), this );
	connect( themesMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( themesMenuAboutToShow() ) );
	view->addMenu( themesMenu );

// 	Q3Accel *a = new Q3Accel(this);
// 	a->connectItem(a->addAction(Qt::Key_F6), this, SLOT(fullscreen()) );
	m_fullAction = view->addAction( tr("&Fullscreen"), this, SLOT(fullscreen()), Qt::Key_F6 );

	m_bossAction = view->addAction( tr("Boss &Color"), this, SLOT(bosscolor()), Qt::Key_F12 );

	view->addSeparator();
	QMenu *scrollMenu = new QMenu(tr("&ScrollBar"),this);
	m_scrollHideAction = scrollMenu->addAction( tr("&Hide"), this, SLOT(hideScroll()) );
	m_scrollLeftAction = scrollMenu->addAction( tr("&Left"), this, SLOT(leftScroll()) );
	m_scrollRightAction = scrollMenu->addAction( tr("&Right"), this, SLOT(rightScroll()) );
	view->addMenu( scrollMenu );
	m_statusAction = view->addAction( tr("Status &Bar"), this, SLOT(showStatusBar()) );
	m_switchAction = view->addAction( tr("S&witch Bar"), this, SLOT(showSwitchBar()) );

	
	// Option Menu	
	QMenu * option = new QMenu( tr("&Option"), this );
	mainMenu->addMenu( option );

	m_currentSessionAction = option->addAction( QPixmap(pathLib+"pic/pref.png"), tr("&Setting for currrent session"), this, SLOT(setting()) );
	option->addSeparator();
	option->addAction( tr("&Default setting"), this, SLOT(defaultSetting()) );
	option->addAction( tr("&Preference"), this, SLOT(preference()) );
	
	// Special
	QMenu * spec = new QMenu( tr("&Special"), this );
	mainMenu->addMenu( spec );
	m_copyArticleAction = spec->addAction( QPixmap(pathLib+"pic/article.png"),
					tr("&Copy article"), this, SLOT(copyArticle()), Qt::Key_F9 );
	m_antiIdleAction = spec->addAction( QPixmap(pathLib+"pic/anti-idle.png"),
					tr("Anti &idle"), this, SLOT(antiIdle()) );
	m_autoReplyAction =  spec->addAction(QPixmap(pathLib+"pic/auto-reply.png"), 
					tr("Auto &reply"), this, SLOT(autoReply()) );
	m_viewMessageAction = spec->addAction( QPixmap(pathLib+"pic/message.png"),
					tr("&View messages"), this, SLOT(viewMessages()), Qt::Key_F10 );
	m_beepAction = spec->addAction( QPixmap(pathLib+"pic/sound.png"),
					tr("&Beep "), this, SLOT(beep()) );
	m_mouseAction = spec->addAction( QPixmap(pathLib+"pic/mouse.png"),
					tr("&Mouse support"), this, SLOT(enableMouse()) );
	spec->addAction(tr("&Image viewer"), this, SLOT(viewImages()));

	
	//Script
	QMenu * script = new QMenu( tr("Scrip&t"), this );
	mainMenu->addMenu( script );
	m_scriptRunAction = script->addAction( tr("&Run..."), this, SLOT(runScript()), Qt::Key_F7 );
	m_scriptStopAction = script->addAction( tr("&Stop"), this, SLOT(stopScript()), Qt::Key_F8 );

	//Window menu
	windowsMenu = new QMenu( tr("&Windows"), this );
	connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );

	mainMenu->addMenu( windowsMenu );
	mainMenu->addSeparator();

	//Help menu
	QMenu * help = new QMenu( tr("&Help"), this );
	mainMenu->addMenu( help );
	help->addAction( tr("About &QTerm"), this, SLOT(aboutQTerm()), Qt::Key_F1 );
	help->addAction( tr("QTerm's &Homepage"),this,SLOT(homepage()));

}

void QTermFrame::updateMenuToolBar()
{
	QTermWindow * window = wndmgr->activeWindow();

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

void QTermFrame::enableMenuToolBar( bool enable )
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

void QTermFrame::updateKeyToolBar()
{
	key->clear();
	key->addAction( QPixmap(pathPic+"pic/keys.png"), tr("Key Setup"), this, SLOT(keySetup()) );

	QTermConfig * conf= new QTermConfig(fileCfg);
	QString strItem, strTmp;
	strTmp = conf->getItemValue("key", "num");
	int num = strTmp.toInt();

	for(int i=0; i<num; i++)
	{
		strItem = QString("name%1").arg(i);
		strTmp = conf->getItemValue("key", strItem);
		QTermToolButton *button = new QTermToolButton(key, i, strTmp);
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

QString QTermFrame::valueToString(bool shown, int dock, int index, bool nl, int extra )
{
	QString str="";

	str = QString("%1 %2 %3 %4 %5").arg(shown?1:0).arg(dock).arg(index).arg(nl?1:0).arg(extra);

	return str;
}

void QTermFrame::popupFocusIn(QTermWindow *)
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

void QTermFrame::insertThemeItem(QString themeitem)
{
	//const char *style[]={"Default", "CDE", "Motif", 
			//"Motif Plus", "Platinum", "SGI", "Windows"};
	QAction * idAction;

	idAction = themesMenu->addAction(themeitem, this, SLOT(themesMenuActivated()));
// 	idAction->setObjectName(themeitem);
// 	themesMenu->setItemParameter( id, id );
	idAction->setChecked(themeitem==theme);
// 	themesMenu->setItemChecked( id, themeitem==theme );
}

void QTermFrame::setUseDock(bool use)
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
	connect(tray, SIGNAL(activated(int)), SLOT(trayActived(int)));
// 	connect(tray, SIGNAL(clicked(const QPoint &, int)), SLOT(trayClicked(const QPoint &, int)));
// 	connect(tray, SIGNAL(doubleClicked(const QPoint &)), SLOT(trayDoubleClicked()));
// 	connect(tray, SIGNAL(closed()), this, SLOT(exitQTerm()));

	tray->show();
}

void QTermFrame::buildTrayMenu()
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
void QTermFrame::trayActived(int reason)
{
	if (reason == QSystemTrayIcon::Context)
		return;
	if(!isVisible())
		trayShow();
	else
		trayHide();
}
/*
void QTermFrame::trayClicked(const QPoint &, int)
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}

void QTermFrame::trayDoubleClicked()
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}
*/
void QTermFrame::trayShow()
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

void QTermFrame::trayHide()
{
	hide();
}

void QTermFrame::buzz()
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


#include <qtermframe.moc>
