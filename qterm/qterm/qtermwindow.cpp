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

#include "qterm.h"
#include "qtermscreen.h"
#include "qtermdecode.h"
#include "qtermtelnet.h"
#include "qtermconvert.h"
#include "qtermbuffer.h"
#include "qtermparam.h"
#include "addrdialog.h"
#include "qtermconfig.h"
#include "qtermbbs.h"
#include "msgdialog.h"
#include "qtermtextline.h"
#include "articledialog.h"
#include "popwidget.h"

#ifndef _OS_WIN32_
#include <unistd.h>
#endif

#include <Python.h>

#include <stdlib.h>
#include <stdio.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qfontdialog.h>
#include <qtextcodec.h>
#include <qsound.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qtextbrowser.h>
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qregexp.h>
#include <qfiledialog.h>

extern char fileCfg[];
extern char addrCfg[];
extern QString pathLib;
extern QString pathPic;

extern void saveAddress(QTermConfig*,int,const QTermParam&);

extern int Python_Init();
extern int Python_Finalize();
extern int DoPythonFile(const char *);

// script thread
QTermThread::QTermThread(QObject *o, const QCString& str)
{
	receiver = o;
	script = str;
}

QTermThread::~QTermThread()
{

}

void QTermThread::run()
{
	char str[32];
	sprintf(str,"%ld",receiver);
	
	char *argv[2]={str,NULL};

	if(Python_Init())
	{
		QMessageBox::information( (QTermWindow*)receiver, "QTerm",
			"Failed To Initialized Python");	
		return;
	}

	PySys_SetArgv(1, argv);

	if(DoPythonFile(script)!=0)
	{
		QMessageBox::information( (QTermWindow*)receiver, "QTerm",
		"Failed To Run Script");	
	}
	
	Python_Finalize();

}

void QTermThread::stop()
{
	Python_Finalize();
}

char QTermWindow::direction[][5]=
{
	// 4
	"\x1b[1~",	// 0 HOME
	"\x1b[4~",	// 1 END
	"\x1b[5~",	// 2 PAGE UP
	"\x1b[6~",	// 3 PAGE DOWN
	// 3
	"\x1b[A",	// 4 UP
	"\x1b[B",	// 5 DOWN
	"\x1b[D",	// 6 LEFT
	"\x1b[C"	// 7 RIGHT
};

//constructor
QTermWindow::QTermWindow( QTermFrame * frame, QTermParam param, int addr, QWidget * parent, const char * name, int wflags )
    : QMainWindow( parent, name, wflags )
{

	m_pFrame = frame;
	m_param = param;
	m_nAddrIndex = addr;

	setMouseTracking( true );


//init the textline list

	m_pBuffer = new QTermBuffer( m_param.m_nRow, m_param.m_nCol, m_param.m_nScrollLines );
	m_pTelnet = new QTermTelnet( (const char *)m_param.m_strTerm );
	m_pDecode = new QTermDecode( m_pBuffer );
	m_pBBS	  = new QTermBBS( m_pBuffer );
	m_pScreen = new QTermScreen( this, m_pBuffer, &m_param, m_pBBS );
	setFocusProxy( m_pScreen);
	setCentralWidget( m_pScreen);
	connect(m_pFrame, SIGNAL(bossColor()), m_pScreen, SLOT(bossColor()));
	connect(m_pFrame, SIGNAL(updateScroll()), m_pScreen, SLOT(updateScrollBar()));

	#ifdef _OS_WIN32_
	m_popWin = new popWidget(this,m_pFrame);
	#else
	m_popWin = new popWidget(this);
	#endif

	//set the status bar
	statusBar()->message( tr("Not Connected") );
	statusBar()->setSizeGripEnabled(false);

	if(m_pFrame->m_bStatusBar)
		statusBar()->show();
	else
		statusBar()->hide();

	// disable the dock menu
	setDockMenuEnabled(false);

	m_pMenu = new QPopupMenu(m_pScreen);
	m_pMenu->insertItem( QPixmap(pathLib+"pic/copy.png"), tr("Copy"), this, SLOT(copy()), CTRL+Key_Insert );
	m_pMenu->insertItem( QPixmap(pathLib+"pic/paste.png"), tr("Paste"), this, SLOT(paste()), SHIFT+Key_Insert );
	m_pMenu->insertItem( QPixmap(pathLib+"pic/article.png"), tr("Copy Article"), this, SLOT(copyArticle()), Key_F9 );
	m_pMenu->insertSeparator();
	m_pMenu->insertItem( QPixmap(pathLib+"pic/fonts.png"), tr("Font"), this, SLOT(font()) );
	m_pMenu->insertItem( QPixmap(pathLib+"pic/color.png"), tr("Color"), this, SLOT(color()) );
	m_pMenu->insertSeparator();
	m_pMenu->insertItem( QPixmap(pathLib+"pic/pref.png"), tr("Setting"), this, SLOT(setting()) );

//connect telnet signal to slots
	connect(m_pTelnet,SIGNAL( readyRead(int) ),
		this,SLOT( readReady(int) ) );
	connect(m_pTelnet,SIGNAL( TelnetState(int) ),
		this,SLOT( TelnetState(int) ) );
	connect(m_pFrame, SIGNAL(updateStatusBar(bool)),
		this, SLOT(showStatusBar(bool)) );
// timers 
	m_idleTimer = new QTimer;
	connect( m_idleTimer, SIGNAL(timeout()), this, SLOT(idleProcess()) );
	m_replyTimer = new QTimer;
	connect( m_replyTimer, SIGNAL(timeout()), this, SLOT(replyProcess()) );
	m_tabTimer = new QTimer;
	connect( m_tabTimer, SIGNAL(timeout()), this, SLOT(blinkTab()) );
	m_downTimer = new QTimer;
	connect( m_downTimer, SIGNAL(timeout()), this, SLOT(downTimer()) );
	m_reconnectTimer = new QTimer;
	connect( m_reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnect()) );
	
// initial varibles
	m_bCopyColor= false;
	m_bCopyRect	= false;
	m_bAntiIdle	= true;
	m_bAutoReply= m_param.m_bAutoReply;
	m_bBeep		= (m_pFrame->m_pref.nBeep!=0);
	m_bMouse	= true;
	m_bWordWrap = false;
	m_bAutoCopy = true;
	m_bMessage	= false;
	m_bReconnect = m_param.m_bReconnect;

//init variable
	m_bConnected=false;
	m_bIdling = false;

	m_bSetChanged = false;

	m_bDoingLogin = false;

	cursor[0] = QCursor(QPixmap(pathLib+"cursor/home.xpm"));
	cursor[1] = QCursor(QPixmap(pathLib+"cursor/end.xpm"));
	cursor[2] = QCursor(QPixmap(pathLib+"cursor/pageup.xpm"));
	cursor[3] = QCursor(QPixmap(pathLib+"cursor/pagedown.xpm"));
	cursor[4] = QCursor(QPixmap(pathLib+"cursor/prev.xpm"));
	cursor[5] = QCursor(QPixmap(pathLib+"cursor/next.xpm"));
	cursor[6] = QCursor(QPixmap(pathLib+"cursor/exit.xpm"));
	cursor[7] = QCursor(QPixmap(pathLib+"cursor/hand.xpm"));
	cursor[8] = Qt::arrowCursor;

	connectHost();

}

//destructor
QTermWindow::~QTermWindow()
{
	delete m_pScreen;
	delete m_pTelnet;
	delete m_pDecode;
	delete m_pBuffer;
	
	delete m_popWin;
	
	delete m_idleTimer;
	delete m_replyTimer;
	delete m_tabTimer;
}

//close event received
void QTermWindow::closeEvent ( QCloseEvent * clse)
{
	if( m_bConnected && m_pFrame->m_pref.bWarn )
	{
		QMessageBox mb( "QTerm",
			"Connected,Do you still want to exit?",
			QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No  | QMessageBox::Escape ,
			0,this,0,true);
		if ( mb.exec() == QMessageBox::Yes )
		{
			m_pTelnet->close();
			saveSetting();
			m_pFrame->wndmgr->removeWindow(this);
			clse->accept();
		}
		else
			clse->ignore();
	}
	else
	{
		saveSetting();
		m_pFrame->wndmgr->removeWindow(this);
		clse->accept();
	}
}
/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Timer                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermWindow::idleProcess()
{
	// do as autoreply when it is enabled
	if(m_replyTimer->isActive() && m_bAutoReply)
	{
		replyMessage();
		if(m_tabTimer->isActive())
	   	{
			m_tabTimer->stop();
			m_pFrame->wndmgr->blinkTheTab(this,TRUE);
    	}
		return;
	}

	int length;	
	QCString cstr = parseString( (const char *)m_param.m_strAntiString, &length );

	m_pTelnet->write( cstr, length );
	
	m_bIdling = true;
}

void QTermWindow::replyProcess()
{
	// if AutoReply still enabled, then autoreply
	if(m_bAutoReply)
		replyMessage();
	else // else just stop the timer
		m_replyTimer->stop();

	if(m_tabTimer->isActive())
    {
		m_tabTimer->stop();
		m_pFrame->wndmgr->blinkTheTab(this,TRUE);
    }
}

void QTermWindow::blinkTab()
{
	static bool bVisible=TRUE;
	m_pFrame->wndmgr->blinkTheTab(this,bVisible);
	bVisible=!bVisible;
}

void QTermWindow::downTimer()
{
	m_bTimeOut = true;
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Mouse & Key                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermWindow::enterEvent( QEvent * )
{
}

void QTermWindow::leaveEvent( QEvent * )
{
}

void QTermWindow::mousePressEvent( QMouseEvent * me )
{
	// Right Button for context menu
	if(me->button() & RightButton)
		m_pMenu->popup(me->globalPos());
		
	// stop  the tab blinking
    if(m_tabTimer->isActive())
    {
		m_tabTimer->stop();
		m_pFrame->wndmgr->blinkTheTab(this,TRUE);
    }

	m_bIdling = false;

	// Left Button for selecting
	if(me->button() & LeftButton)
	{
		// clear the selected before
		m_pBuffer->clearSelect();
		m_pScreen->refreshScreen();
	
		// set the selecting flag
		m_bSelecting = true;
		m_ptSelStart = m_pScreen->mapToChar( me->pos() );
		m_ptSelEnd = m_ptSelStart;
	}

}
void QTermWindow::mouseMoveEvent( QMouseEvent * me)
{
	// selecting by leftbutton
	if( (me->state() & LeftButton) && m_bSelecting )
	{
        if(me->pos().y()< childrenRect().top())
			m_pScreen->scrollLine(-1);
        if(me->pos().y()> childrenRect().bottom())
			m_pScreen->scrollLine(1);
	
		m_ptSelEnd = m_pScreen->mapToChar( me->pos() );
		if( m_ptSelEnd!=m_ptSelStart )
		{
			m_pBuffer->setSelect( m_ptSelStart, m_ptSelEnd, m_bCopyRect );
			m_pScreen->refreshScreen();
		}
	}

	
	if(!m_bMouse || !m_bConnected)
		return;
	// set cursor pos, repaint if state changed
	QRect rect;
	if( m_pBBS->setCursorPos( m_pScreen->mapToChar(me->pos()),rect ) )
	{
		m_pScreen->repaint( m_pScreen->mapToRect(rect), true );
	}
	// judge if URL
	QRect rcOld;
	if(m_pFrame->m_pref.bUrl)
	{
		if(m_pBBS->isUrl(m_rcUrl, rcOld))
		{
			setCursor(pointingHandCursor);
			if(m_rcUrl!=rcOld)
			{
				QToolTip::remove(this, m_pScreen->mapToRect(rcOld));
				QToolTip::add(this, m_pScreen->mapToRect(m_rcUrl), m_pBBS->getUrl());
			}
			return;
		}
		else
		QToolTip::remove(this, m_pScreen->mapToRect(rcOld));
	}

	// normal
	int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
	if( nCursorType<=8 && nCursorType>=0 )
		setCursor(cursor[nCursorType]);
}

void QTermWindow::mouseReleaseEvent( QMouseEvent * me )
{	
	// restart the idle timer	
	if(m_idleTimer->isActive())
		m_idleTimer->stop();
	if( m_bAntiIdle )
		m_idleTimer->start( m_param.m_nMaxIdle*1000 );
	m_bIdling = false;

	// Middle Button for paste
	if( me->button() & MidButton )
	{
		if(m_bConnected)
		{
			if(m_pFrame->m_pref.bWheel)
			{
				char ch = CHAR_CR;
				m_pTelnet->write(&ch,1);
			}
			else
				paste();
		}
		return;
	}

	// other than Left Button ignored
	if( !(me->button() & LeftButton) )
		return;
	
	// Left Button for selecting
	m_ptSelEnd = m_pScreen->mapToChar( me->pos() );
	if( m_ptSelEnd != m_ptSelStart && m_bSelecting )
	{
		m_pBuffer->setSelect( m_ptSelStart, m_ptSelEnd, m_bCopyRect );
		m_pScreen->refreshScreen();
		if( m_bAutoCopy )
			copy();
		m_bSelecting = false;
		return;
	}
	m_bSelecting = false;

	
	if(!m_bMouse || !m_bConnected)
		return;

	// url
	if(!m_pBBS->getUrl().isEmpty())
	{
		bool ok;
		QString caption = tr("Open URL");
		QString hint = "url";
		#if (QT_VERSION>=300)
		QString strUrl = QInputDialog::getText(caption, hint,QLineEdit::Normal, QString(m_pBBS->getUrl()), &ok);
		#else
		QString strUrl = QInputDialog::getText(caption, hint, QString(m_pBBS->getUrl()), &ok);
		#endif
		if (ok)
		{
			//QApplication::clipboard()->setText(strUrl);
			QCString cstrCmd = m_pFrame->m_pref.strHttp.local8Bit() + " \"" + strUrl.local8Bit() +"\"";
			#ifndef _OS_WIN32_
			cstrCmd += " &";
			#endif
			system(cstrCmd);
		}
		return;
	}
	// mouse menu
	int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
	if( nCursorType<4 && nCursorType>=0 )
		m_pTelnet->write(direction[nCursorType], 4);
	else if( nCursorType<7 && nCursorType>=4 )
		m_pTelnet->write(direction[nCursorType], 3);
	else if( nCursorType==7 )
	{
		char cr = CHAR_CR;
		QRect rc = m_pBBS->getSelectRect();
		switch(m_pBBS->getPageState())
		{
			case 0:
				if( !rc.isEmpty() )
				{
					char cMenu = m_pBBS->getMenuChar();
					m_pTelnet->write( &cMenu, 1 );
					m_pTelnet->write( &cr, 1 );
				}
				break;
			case 1:
				if( !rc.isEmpty() )
				{
					int n = rc.y() - m_pBuffer->caretY();
					// scroll lines
					if( n>0 )
						while( n )
						{
							m_pTelnet->write( direction[5], 4 );
							n--;
						}
					if( n<0 )
					{
						n = -n;
						while( n )
						{
							m_pTelnet->write( direction[4], 4 );
							n--;
						}
					}
					// don't forget to send a CHAR_CR at last to enter in
					m_pTelnet->write( &cr, 1 );
				}
				break;
			default:
				break;
		}
	}
}

void QTermWindow::wheelEvent( QWheelEvent *we)
{
	if(!m_bConnected)
		return;

	if(we->delta()>0)
		m_pTelnet->write(direction[4], sizeof(direction[4]));
	else
		m_pTelnet->write(direction[5], sizeof(direction[5]));
}

//keyboard input event
void QTermWindow::keyPressEvent( QKeyEvent * e )
{

    if ( !m_bConnected )
	{
		if(e->key() == Key_Return)
			reconnect();
		return;
	}

	// stop  the tab blinking
    if(m_tabTimer->isActive())
    {
		m_tabTimer->stop();
		m_pFrame->wndmgr->blinkTheTab(this,TRUE);
    }

	// restart the idle timer	
	if(m_idleTimer->isActive())
		m_idleTimer->stop();
	if( m_bAntiIdle )
		m_idleTimer->start( m_param.m_nMaxIdle*1000 );
	m_bIdling = false;

	// message replying
	if(m_replyTimer->isActive())
		m_replyTimer->stop();

	if( e->state() & ControlButton )
	{
		if( e->key()>=Key_A && e->key()<=Key_Z )
		{	
			char ch = e->key() & 0x1f;
			m_pTelnet->write(&ch,1);
		}
		return;
	}

	// TODO get the input messages
//	if( m_bMessage && e->key()==Key_Return && m_pBuffer->caret().y()==1 )
//	{
//		m_strMessage += m_pBuffer->screen(1)->getText()+"\n";
//	}

	switch( e->key() )
	{
		case Key_Home:
			m_pTelnet->write( direction[0],4 );
		return;
		case Key_End:
			m_pTelnet->write( direction[1],4 );
		return;
		case Key_PageUp:
			m_pTelnet->write( direction[2],4 );
		return;
		case Key_PageDown:
			m_pTelnet->write( direction[3],4 );
		return;
		case Key_Up:
			m_pTelnet->write( direction[4],3 );
		return;
		case Key_Down:
			m_pTelnet->write( direction[5],3 );
		return;
		case Key_Left:
			m_pTelnet->write( direction[6],3 );
		return;
		case Key_Right:
			m_pTelnet->write( direction[7],3 );
		return;
		default:	
			break;
	}

	if( e->text().length() )
	{		
		QCString strTmp;		

		if( m_pFrame->m_pref.nXIM == 0 )
		{
			strTmp = U2G(e->text());
			if( m_param.m_nBBSCode == 1)
			{
				char * str = m_converter.G2B( strTmp, strTmp.length() );
				strTmp = str;
				delete []str;
			}
		}
		else
		{
			strTmp = U2B(e->text());
			if( m_param.m_nBBSCode == 0 )
			{
				char * str = m_converter.B2G( strTmp, strTmp.length() );
				strTmp = str;
				delete []str;
			}
		}

		m_pTelnet->write( strTmp, strTmp.length() );
	}
}


//connect slot
void QTermWindow::connectHost()
{

	m_pTelnet->setProxy( m_param.m_nProxyType, m_param.m_bAuth,
		m_param.m_strProxyHost, m_param.m_uProxyPort,
		m_param.m_strProxyUser, m_param.m_strProxyPasswd);
	
	m_pTelnet->connectHost( m_param.m_strAddr , m_param.m_uPort );
}
/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Telnet State                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
//read slot
void QTermWindow::readReady( int size )
{
	//read it
	char * str = new char[size+1];
	m_pTelnet->read( str,size );
	str[size]='\0';
	//decode
	m_pDecode->decode(str,size);
	
	if(m_bDoingLogin)
	{
		int n = m_pBuffer->caret().y();
		for(int y=n-5;y<n+5;y++ )
		{
			y=QMAX(0,y);
			QTermTextLine *pTextLine=m_pBuffer->screen(y);
			if(pTextLine==NULL)
				continue;
			QCString cstr=pTextLine->getText();
			if(cstr.find("guest")!=-1 && cstr.find("new")!=-1)
			{
				doAutoLogin();
				break;
			}
		}
	}
	// page complete when caret at the right corner
	// this works for most but not for all
	QTermTextLine * pTextLine = m_pBuffer->screen(m_pBuffer->line()-1);
	QCString cstrText = stripWhitespace(pTextLine->getText());
	m_bPageComplete = (m_pBuffer->caret().y()==m_pBuffer->line()-1 &&
						m_pBuffer->caret().x()>=cstrText.length()-1 );

	QToolTip::remove(this, m_pScreen->mapToRect(m_rcUrl));

	// message received
	// 03/19/2003. the caret posion removed as a message judgement
	// because smth.org changed
    if( m_pDecode->bellReceive() ) //&& m_pBuffer->caret().y()==1 )
    {
		if( m_bBeep )
			if(m_pFrame->m_pref.strWave.isEmpty()||m_pFrame->m_pref.nBeep==1)
				qApp->beep();
			else
				QSound::play(m_pFrame->m_pref.strWave);

		if(m_pFrame->m_pref.bBlinkTab)
			m_tabTimer->start(500);

		QCString cstrMsg = m_pBBS->getMessage();
		if(!cstrMsg.isEmpty())
			m_strMessage += cstrMsg + "\n\n";

		m_bMessage = true;
	
		if(!isActiveWindow() || m_pFrame->wndmgr->activeWindow()!=this)
		{
			m_popWin->setText(fromBBSCodec(cstrMsg));
			m_popWin->popup();
		}
		if(m_bAutoReply)
		{	
			// TODO: save messages
	        if ( m_bIdling )
				replyMessage();
			else
				m_replyTimer->start(m_param.m_nMaxIdle*1000/2);
		}
	}
	
	// set page state
	m_pBBS->setPageState();
	
	//refresh screen
	m_pScreen->refreshScreen();

	//delete the buf
	delete []str;
}

// telnet state slot
void QTermWindow::TelnetState(int state)
{
	switch( state )
	{
	case TSRESOLVING:
		statusBar()->message( tr("resolving host name") );
		break;
	case TSHOSTFOUND:
		statusBar()->message( tr("host found") );
		break;
	case TSHOSTNOTFOUND:
		statusBar()->message( tr("host not found") );
		connectionClosed();
		break;
	case TSCONNECTING:
		statusBar()->message( tr("connecting...") );
		break;
	case TSHOSTCONNECTED:
		statusBar()->message( tr("connected") );
		m_bConnected = true;
		m_pFrame->updateMenuToolBar();
		if(m_param.m_bAutoLogin)
			m_bDoingLogin = true;
		break;
	case TSPROXYCONNECTED:
		statusBar()->message( tr("connected to proxy" ) );
		break;
	case TSPROXYAUTH:
		statusBar()->message( tr("proxy authentation") );
		break;
	case TSPROXYFAIL:
		statusBar()->message( tr("proxy failed") );
		disconnect();
		break;
	case TSREFUSED:
		statusBar()->message( tr("connection refused") );
		connectionClosed();
		break;
	case TSREADERROR:
		statusBar()->message( tr("error when reading from server") );
		disconnect();
		break;
	case TSCLOSED:
		statusBar()->message( tr("connection closed") );
		connectionClosed();
		if( m_param.m_bReconnect && m_bReconnect )
			reconnectProcess();
		break;
	case TSCLOSEFINISH:
		statusBar()->message( tr("connection close finished") );
		connectionClosed();
		break;
	case TSCONNECTVIAPROXY:
		statusBar()->message( tr("connect to host via proxy") );
		break;
	case TSEGETHOSTBYNAME:
		statusBar()->message( tr("error in gethostbyname") );
		connectionClosed();
		break;
	case TSEINIWINSOCK:
		statusBar()->message( tr("error in startup winsock") );
		connectionClosed();
		break;
	case TSERROR:
		statusBar()->message( tr("error in connection") );
		disconnect();
		break;
	case TSPROXYERROR:
		statusBar()->message( tr("eoor in proxy") );
		disconnect();
		break;
	default:
		break;
	}

}
/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         UI Slots                                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */


void QTermWindow::copy( )
{
	QClipboard *clipboard = QApplication::clipboard();
	
	#if (QT_VERSION>=0x030100)
	if( m_param.m_nBBSCode==0 )
		clipboard->setText(G2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))), 
						QClipboard::Selection );
	else
		clipboard->setText(B2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))), 
						QClipboard::Selection );
	#else
	if( m_param.m_nBBSCode==0 )
		clipboard->setText(G2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))));
	else
		clipboard->setText(B2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))));
	#endif
}
void QTermWindow::paste( )
{
	if( !m_bConnected )
		return;
	
	QClipboard *clipboard = QApplication::clipboard();
	QCString cstrText;
	
	if(m_pFrame->m_nClipCodec==0)
	{
		#if (QT_VERSION>=0x030100)
		cstrText=U2G( clipboard->text(QClipboard::Selection) );
		#else
		cstrText=U2G( clipboard->text() );
		#endif
		if( m_param.m_nBBSCode==1 )
		{
			char * str = m_converter.G2B( cstrText, cstrText.length() );
			cstrText = str;
			delete []str;
		}
	}
	else
	{
		#if (QT_VERSION>=0x030100)
		cstrText=U2B( clipboard->text(QClipboard::Selection) );
		#else
		cstrText=U2B( clipboard->text() );
		#endif
		if( m_param.m_nBBSCode==0 )
		{
			char * str = m_converter.B2G( cstrText, cstrText.length() );
			cstrText = str;
			delete []str;
		}
	}

	if(!m_pFrame->m_cstrEscape.isEmpty())
	#if (QT_VERSION>=0x030100)
		cstrText.replace(parseString(m_pFrame->m_cstrEscape), 
						parseString((const char *)m_param.m_strEscape));
	#else
		cstrText.replace(QRegExp(parseString(m_pFrame->m_cstrEscape)), 
						parseString((const char *)m_param.m_strEscape));
	#endif

	if( m_bWordWrap )
	{
		// convert to unicode for word wrap
		QString strText;
		if( m_param.m_nBBSCode==0 )
			strText = G2U(cstrText);
		else
			strText = B2U(cstrText);

		// insert '\n' as needed
		for( uint i=0; i< strText.length(); i++ )
		{
			uint j=i;
			uint k=0, l=0;
			while(strText.at(j)!=QChar('\n') && j<strText.length())
			{
				if(m_pFrame->m_pref.nWordWrap-(l-k)>=0 &&
					m_pFrame->m_pref.nWordWrap-(l-k)<2)
				{
					strText.insert(j,QChar('\n'));
					k=l;
					j++;
					break;
				}
				// double byte or not
				if(strText.constref(j).row()=='\0')
					l++;
				else
					l+=2;
				j++;
			}
			i = j;
		}

		if( m_param.m_nBBSCode==0 )
			cstrText = U2G(strText);
		else
			cstrText = U2B(strText);
	}
	
	m_pTelnet->write(cstrText, cstrText.length());
}
void QTermWindow::copyArticle( )
{
	if(!m_bConnected)
		return;
	
	QCString cstrArticle = downloadArticle();	

	articleDialog article(this);
	QTermConfig conf(fileCfg);
	char * size = conf.getItemValue("global","articledialog");
	if(size!=NULL)
	{
		int x,y,cx,cy;
		sscanf(size,"%d %d %d %d",&x,&y,&cx,&cy);
		article.resize(QSize(cx,cy));
		article.move(QPoint(x,y));	
	}
	article.cstrArticle = cstrArticle;
	if(m_param.m_nBBSCode==0)
		article.textBrowser2->setText(G2U(cstrArticle));
	else
		article.textBrowser2->setText(B2U(cstrArticle));
	article.exec();
	QCString cstrSize;
	cstrSize.sprintf("%d %d %d %d",article.x(),article.y(),article.width(),article.height());
	conf.setItemValue("global","articledialog",cstrSize);
	conf.save(fileCfg);
}
void QTermWindow::font()
{		
	bool ok;

	QResizeEvent* re =new QResizeEvent( m_pScreen->size(),m_pScreen->size());
	
	QFont font=QFontDialog::getFont( &ok,m_pScreen->getDispFont() );
	if(ok==true)
	{
		m_pScreen->setDispFont( font );
		QApplication::postEvent( m_pScreen,re );
	}
}


void QTermWindow::color()
{
	QMessageBox::information( this, "QTerm",
		"Not Implemented Yet :(");
}

void QTermWindow::disconnect()
{
	m_pTelnet->close();

	connectionClosed();
}

void QTermWindow::reconnect()
{
	if(!m_bConnected)
		m_pTelnet->connectHost( m_param.m_strAddr , m_param.m_uPort );

}
void QTermWindow::refresh( )
{
	m_pScreen->repaint(true);
}

void QTermWindow::showStatusBar(bool bShow)
{
	if(bShow)
		statusBar()->show();
	else
		statusBar()->hide();
}

void QTermWindow::runScript()
{

	QString file = QFileDialog::getOpenFileName(
							"/home",
							"Python File (*.py *.txt)", this,
							"open file dialog"
							"choose a script file" );
	if(file.isEmpty())
		return;

	runScriptFile(file.local8Bit());
}

void QTermWindow::stopScript()
{
	if(m_pThread!=0)
		m_pThread->stop();
}

void QTermWindow::viewMessages( )
{
	msgDialog msg(this);

	QTermConfig conf(fileCfg);
	char * size = conf.getItemValue("global","msgdialog");
	if(size!=NULL)
	{
		int x,y,cx,cy;
		sscanf(size,"%d %d %d %d",&x,&y,&cx,&cy);
		msg.resize(QSize(cx,cy));
		msg.move(QPoint(x,y));	
	}

	if( m_param.m_nBBSCode==0 )
		msg.msgBrowser->setText(G2U(m_strMessage));
	else
		msg.msgBrowser->setText(B2U(m_strMessage));
	
	msg.exec();

	QCString cstrSize;
	cstrSize.sprintf("%d %d %d %d", msg.x(),msg.y(),msg.width(),msg.height());
	conf.setItemValue("global","msgdialog",cstrSize);
	conf.save(fileCfg);

}

void QTermWindow::setting( )
{
	addrDialog set(this, true);
	
	set.param = m_param;
	set.updateData(false);		 

	if(set.exec()==1)
	{
		m_param = set.param;
		m_bSetChanged = true;
	}
}

void QTermWindow::antiIdle()
{
	m_bAntiIdle = !m_bAntiIdle;
	//enabled
	if( !m_bAntiIdle && m_idleTimer->isActive() )
		m_idleTimer->stop();
	//disabled
	if( m_bAntiIdle && !m_idleTimer->isActive() )
		m_idleTimer->start(m_param.m_nMaxIdle*1000);
}

void QTermWindow::autoReply()
{
	m_bAutoReply = !m_bAutoReply;
	// disabled
	if( !m_bAutoReply && m_replyTimer->isActive() )
		m_replyTimer->stop();
	// enabled
	if( m_bAutoReply && !m_replyTimer->isActive() )
		m_replyTimer->start(m_param.m_nMaxIdle*1000/2);
}

void QTermWindow::connectionClosed()
{
	m_bConnected = false;

	if(m_idleTimer->isActive())
		m_idleTimer->stop();
	
	statusBar()->message( tr("connection closed") );

	m_pFrame->updateMenuToolBar();

	setCursor(cursor[8]);
	
	QCString cstrMsg = "";
	cstrMsg += "\n\n\n\r";
	cstrMsg += "\x1b[17C\x1b[0m===========================================\n\r"; 
	cstrMsg += "\x1b[17C Connection Closed, Press \x1b[1m\x1b[31;40mEnter\x1b[m\x1b[0m To Connect\n\r"; 
	cstrMsg += "\x1b[17C===========================================\n"; 

	m_pDecode->decode( cstrMsg, cstrMsg.length() );

	m_pScreen->refreshScreen();
}

void QTermWindow::doAutoLogin()
{
	if( !m_param.m_strPreLogin.isEmpty() ) 
	{
		QCString temp = parseString(m_param.m_strPreLogin.local8Bit());
		m_pTelnet->write( (const char *)(temp), temp.length() );
	}
	if( !m_param.m_strUser.isEmpty() ) 
	{
		QCString temp = m_param.m_strUser.local8Bit();
		m_pTelnet->write( (const char *)(temp), temp.length() );
		char ch=CHAR_CR;
		m_pTelnet->write( &ch, 1 );
	}
	if( !m_param.m_strPasswd.isEmpty() ) 
	{
		QCString temp = m_param.m_strPasswd.local8Bit();
		m_pTelnet->write( (const char *)(temp), temp.length() );
		char ch=CHAR_CR;
		m_pTelnet->write( &ch, 1 );
	}
	// smth ignore continous input, so sleep 1 sec :)
	#ifdef _OS_WIN32_
	Sleep(1);
	#else
	sleep(1);
	#endif

	if( !m_param.m_strPostLogin.isEmpty() ) 
	{
		QCString temp = parseString(m_param.m_strPostLogin.local8Bit());
		m_pTelnet->write( (const char *)(temp), temp.length() );
	}
	m_bDoingLogin = false;
}

void QTermWindow::reconnectProcess()
{
	static int retry = 0;
	if( retry<m_param.m_nRetry || m_param.m_nRetry==-1 )
	{
		if( m_param.m_nReconnectInterval<=0)
			reconnect();
		else
			m_reconnectTimer->start(m_param.m_nReconnectInterval*1000);
		retry++;
	}
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Aux Func                                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

QCString  QTermWindow::parseString(const QCString& cstr, int *len)
{
	QCString parsed="";

	if(len!=0)	
		*len=0;

	for( uint i=0; i<cstr.length(); i++ )
	{
		if(cstr.at(i)=='^')
		{
			i++;
			if(i<cstr.length())
			{
				parsed += CTRL(cstr.at(i));
				if(len!=0)	
					*len=*len+1;
			}
			
		}else
		if(cstr.at(i)=='\\')
		{
			i++;
			if(i<cstr.length())
			{
				if(cstr.at(i)=='n')
					parsed += CHAR_CR;
				else if(cstr.at(i)=='r')
					parsed += CHAR_LF;
				else if(cstr.at(i)=='t')
					parsed += CHAR_TAB;
				if(len!=0)	
					*len=*len+1;
			}
		}else	
		{	
			parsed += cstr.at(i);
			if(len!=0)	
				*len=*len+1;
		}
	}

	return parsed;
}

void QTermWindow::replyMessage()
{
	if(m_replyTimer->isActive())
		m_replyTimer->stop();

	QCString cstr = "r ";
	cstr += m_param.m_strAutoReply.local8Bit();
	cstr += '\n';
	m_pTelnet->write( cstr, cstr.length() );
}

void QTermWindow::saveSetting()
{
	if(m_nAddrIndex == -1 || !m_bSetChanged)
		return;

	QMessageBox mb( "QTerm",
		"Setting changed do you want to save it?",
		QMessageBox::Warning,
		QMessageBox::Yes | QMessageBox::Default,
		QMessageBox::No  | QMessageBox::Escape ,
			0,this,0,true);
	if ( mb.exec() == QMessageBox::Yes )
	{
		QTermConfig *pConf = new QTermConfig(addrCfg);
		saveAddress(pConf, m_nAddrIndex, m_param);
		pConf->save(addrCfg);
		delete pConf;
	}
}

void QTermWindow::externInput( const QCString& cstrText )
{
	QCString cstrParsed = parseString( cstrText );
	m_pTelnet->write( cstrParsed, cstrParsed.length() );

}
bool QTermWindow::waitForNewPage( int timeout )
{
	m_downTimer->start( timeout );
	m_bTimeOut = false;

	while ( !m_bPageComplete && !m_bTimeOut )
		qApp->processEvents( timeout );
	m_downTimer->stop();

	bool res = m_bPageComplete;

	m_bPageComplete = false;

	return res && !m_bTimeOut; 
}

QString QTermWindow::fromBBSCodec( const QCString& cstr )
{
	if(m_param.m_nBBSCode==0)
		return G2U(cstr);
	else
		return B2U(cstr);
}

QCString QTermWindow::stripWhitespace( const QCString& cstr )
{
	QCString cstrText=cstr;
	
	#if (QT_VERSION>=300)
	int pos=cstrText.findRev(QRegExp("[\\S]"));
	#else
	int pos=cstrText.findRev(QRegExp("[^\\s]"));
	#endif

	if(pos==-1)
		cstrText="";
	else
		cstrText.truncate(pos+1);
	return cstrText;
}

void QTermWindow::sendParsedString( const char * str)
{
    int length;
	QCString cstr = parseString( str, &length );
	m_pTelnet->write( cstr, length );
}

void QTermWindow::runScriptFile( const QCString & cstr )
{
	m_pThread = new QTermThread(this, cstr);

	m_pThread->run();
}


QCString QTermWindow::downloadArticle()
{
    QCString cstrArticle;
    QCString cstrCurrent0,cstrCurrent1;
    QCString cstrTemp;

    int pos0=-1,pos1=-1;

    cstrArticle = m_pBuffer->screen(0)->getText();
    #ifdef _OS_WIN32_
    cstrArticle += '\r';
    #endif
    cstrArticle += '\n';

//    m_bPageComplete = true;

    while(waitForNewPage(10000))
    {
        cstrCurrent0=m_pBuffer->screen(0)->getText();
        cstrCurrent1=m_pBuffer->screen(1)->getText();

        cstrTemp = cstrCurrent0;
        #ifdef _OS_WIN32_
        cstrTemp += '\r';
        #endif
        cstrTemp +='\n';
        cstrTemp +=cstrCurrent1;

        pos0=cstrArticle.findRev(cstrTemp);

        if(pos0!=-1)
        {
            pos1=cstrArticle.find(cstrCurrent1,pos0);
            if(pos1!=-1)
                cstrArticle.truncate(pos1);
        }

        for(int i=1;i<m_pBuffer->line()-1;i++)
        {
            cstrArticle+=m_pBuffer->screen(i)->getText();
            #ifdef _OS_WIN32_
            cstrArticle += '\r';
            #endif
            cstrArticle+='\n';
        }

        if( m_pBuffer->screen(m_pBuffer->line()-1)->getText().find("%") == -1 )
            break;
        m_pTelnet->write(direction[7], 3);
    }
    return cstrArticle;
}

