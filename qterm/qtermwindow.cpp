/*******************************************************************************
FILENAME:      qtermframe.cpp
REVISION:      2001.10.4 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qterm.h"
#include "qtermwindow.h"
#include "qtermframe.h"
#include "qtermwndmgr.h"

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
#include "qtermzmodem.h"
#include "zmodemdialog.h"
#include "qtermpython.h"
#include "qtermhttp.h"

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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
#include <qtabwidget.h>
#include <qstringlist.h>

extern QString fileCfg;
extern QString addrCfg;
extern QString pathLib;
extern QString pathPic;
extern QString pathCfg;

extern void saveAddress(QTermConfig*,int,const QTermParam&);
extern void runProgram(const QCString &);
extern QString getOpenFileName(const QString&, QWidget*);

// script thread
QTermDAThread::QTermDAThread(QTermWindow *win)
{
	pWin = win;
}

QTermDAThread::~QTermDAThread()
{

}

void QTermDAThread::run()
{
	QStringList strList;
	while(1)
	{
		// check it there is duplicated string
		// it starts from the end in the range of one screen height
		// so this is a non-greedy match
		QString strTemp = pWin->stripWhitespace(pWin->m_pBuffer->screen(0)->getText());
		int i=0;
		int start=0;
		for(QStringList::Iterator it=strList.fromLast();
			it!=strList.begin(), i < pWin->m_pBuffer->line()-1; // not exceeeding the last screen
			--it, i++)
		{
			if(*it!=strTemp)
				continue;
			QStringList::Iterator it2 = it;
			bool dup=true;
			// match more to see if its duplicated
			for(int j=0; j<=i; j++, it2++)
			{
				QString str1 = pWin->stripWhitespace(
								pWin->m_pBuffer->screen(j)->getText());
				if(*it2!=str1)
				{
					dup = false;
					break;
				}
			}
			if(dup)
			{
				// set the start point
				start = i+1;
				break;
			}
		}
		// add new lines
		for(i=start;i<pWin->m_pBuffer->line()-1;i++)
			strList+=pWin->stripWhitespace(
			pWin->m_pBuffer->screen(i)->getText());

		// the end of article
		if( pWin->m_pBuffer->screen(
		pWin->m_pBuffer->line()-1)->getText().find("%") == -1 )
			break;
		// continue
		pWin->m_pTelnet->write(" ", 1);
		
		if(!pWin->m_wcWaiting.wait(10000))	// timeout
		{
			postEvent(pWin, new QCustomEvent(DAE_TIMEOUT));
			break;
		}
	}
	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
	cstrArticle = strList.join("\r\n");
	#else
	cstrArticle = strList.join("\n");
	#endif
	postEvent(pWin, new QCustomEvent(DAE_FINISH));
}

/*
void QTermDAThread::run()
{
    QCString cstrCurrent0,cstrCurrent1;
    QCString cstrTemp;

    int pos0=-1,pos1=-1;

    cstrArticle =pWin->stripWhitespace(pWin->m_pBuffer->screen(0)->getText());
    #if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    cstrArticle += '\r';
    #endif
    cstrArticle += '\n';
	
    while(1)
    {
        cstrCurrent0=pWin->stripWhitespace(pWin->m_pBuffer->screen(0)->getText());
        cstrCurrent1=pWin->stripWhitespace(pWin->m_pBuffer->screen(1)->getText());

        cstrTemp = cstrCurrent0;
        #if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
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

        for(int i=1;i<pWin->m_pBuffer->line()-1;i++)
        {
            cstrArticle+=pWin->stripWhitespace(pWin->m_pBuffer->screen(i)->getText());
            #if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
            cstrArticle += '\r';
            #endif
            cstrArticle+='\n';
        }

        if( pWin->m_pBuffer->screen(pWin->m_pBuffer->line()-1)->getText().find("%") == -1 )
            break;
        pWin->m_pTelnet->write(" ", 1);
		
		if(!pWin->m_wcWaiting.wait(10000))	// timeout
		{
			postEvent(pWin, new QCustomEvent(DAE_TIMEOUT));
			break;
		}
    }

	postEvent(pWin, new QCustomEvent(DAE_FINISH));
}
*/

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
	if (param.m_nProtocolType == 0)
		m_pTelnet = new QTermTelnet( (const char *)m_param.m_strTerm, 
						m_param.m_nRow, m_param.m_nCol, false );
	else {
		#if defined(_NO_SSH_COMPILED)
		QMessageBox::warning(this, "sorry", 
						"SSH support is not compiled, QTerm can only use Telnet!");
		m_pTelnet = new QTermTelnet( (const char *)m_param.m_strTerm, 
						m_param.m_nRow, m_param.m_nCol,false );
		#else
		m_pTelnet = new QTermTelnet( (const char *)m_param.m_strTerm, 
						m_param.m_nRow, m_param.m_nCol,true, 
						(const char *)m_param.m_strSSHUser, 
						(const char *)m_param.m_strSSHPasswd );
		#endif
	}
	connect( m_pBuffer, SIGNAL(windowSizeChanged(int,int)), 
					m_pTelnet, SLOT(windowSizeChanged(int,int)) );
	m_pZmDialog = new zmodemDialog(this);
	m_pZmodem = new QTermZmodem( m_pTelnet, param.m_nProtocolType);
	m_pDecode = new QTermDecode( m_pBuffer );
	m_pBBS	  = new QTermBBS( m_pBuffer );
	m_pScreen = new QTermScreen( this, m_pBuffer, &m_param, m_pBBS );
	setFocusProxy( m_pScreen);
	setCentralWidget( m_pScreen);
	connect(m_pFrame, SIGNAL(bossColor()), m_pScreen, SLOT(bossColor()));
	connect(m_pFrame, SIGNAL(updateScroll()), m_pScreen, SLOT(updateScrollBar()));
	connect(m_pScreen, SIGNAL(inputEvent(QString *)), this, SLOT(inputHandle(QString *)));
	connect(m_pZmodem, SIGNAL(ZmodemState(int,int,const QCString&)), 
					this, SLOT(ZmodemState(int,int,const QCString&)));
	connect(m_pZmDialog, SIGNAL(canceled()), m_pZmodem, SLOT(zmodemCancel()));

	connect(m_pDecode, SIGNAL(mouseMode(bool)), this, SLOT(setMouseMode(bool)));

	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
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

	m_pUrl = new QPopupMenu(m_pScreen);
	m_pUrl->insertItem( tr("Preview image"), this, SLOT(previewLink()) );
	m_pUrl->insertItem( tr("Open link"), this, SLOT(openLink()) );
	m_pUrl->insertItem( tr("Copy link address"), this, SLOT(copyLink()) );
	m_pUrl->insertItem( tr("Save target as..."), this, SLOT(saveLink()) );

	
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

	m_pDAThread = 0;
	m_bConnected=false;
	m_bIdling = false;

	m_bSetChanged = false;
	
	m_bMouseX11 = false;
#ifndef _NO_SSH_COMPILED
	if (param.m_nProtocolType != 0)
		m_bDoingLogin = true;
	else
#endif
		m_bDoingLogin = false;

	cursor[0] = QCursor(QPixmap(pathLib+"cursor/home.xpm"));
	cursor[1] = QCursor(QPixmap(pathLib+"cursor/end.xpm"));
	cursor[2] = QCursor(QPixmap(pathLib+"cursor/pageup.xpm"));
	cursor[3] = QCursor(QPixmap(pathLib+"cursor/pagedown.xpm"));
	cursor[4] = QCursor(QPixmap(pathLib+"cursor/prev.xpm"));
	cursor[5] = QCursor(QPixmap(pathLib+"cursor/next.xpm"));
	cursor[6] = QCursor(QPixmap(pathLib+"cursor/exit.xpm"),0,10);
	cursor[7] = QCursor(QPixmap(pathLib+"cursor/hand.xpm"));
	cursor[8] = Qt::arrowCursor;

	// the system wide script
	m_bPythonScriptLoaded = false;
#ifdef HAVE_PYTHON
	pModule = NULL;
// python thread module
	// get the global python thread lock
    PyEval_AcquireLock();

    // get a reference to the PyInterpreterState
    extern PyThreadState * mainThreadState;
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;

    // create a thread state object for this thread
    PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
    PyThreadState_Swap(myThreadState);

	PyImport_AddModule("qterm");
	Py_InitModule4("qterm", qterm_methods,
			NULL,(PyObject*)NULL,PYTHON_API_VERSION);


	if(m_param.m_bLoadScript && !m_param.m_strScriptFile.isEmpty() )
	{
		PyObject *pName = PyString_FromString( m_param.m_strScriptFile );
		pModule = PyImport_Import(pName);
		Py_DECREF(pName);
		if (pModule != NULL) 
			pDict = PyModule_GetDict(pModule);
		else
		{
			printf("Failed to PyImport_Import\n");
		}

		if(pDict != NULL )
			m_bPythonScriptLoaded = true;
		else
		{
			printf("Failed to PyModule_GetDict\n");
		}
	}
	
	//Clean up this thread's python interpreter reference
    PyThreadState_Swap(NULL);
    PyThreadState_Clear(myThreadState);
    PyThreadState_Delete(myThreadState);
    PyEval_ReleaseLock();
#endif //HAVE_PYTHON
	
	connectHost();
}

//destructor
QTermWindow::~QTermWindow()
{
	delete m_pTelnet;
	delete m_pBBS;
	delete m_pDecode;
	delete m_pBuffer;
	delete m_pZmodem;
	
	delete m_popWin;
	
	delete m_idleTimer;
	delete m_replyTimer;
	delete m_tabTimer;

	delete m_pMenu;
	delete m_pUrl;
	delete m_pScreen;
	delete m_reconnectTimer;

#ifdef HAVE_PYTHON
	// get the global python thread lock
	PyEval_AcquireLock();

	// get a reference to the PyInterpreterState
	extern PyThreadState * mainThreadState;
	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
      
	// create a thread state object for this thread
	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
	PyThreadState_Swap(myThreadState);
      
	//Displose of current python module so we can reload in constructor.
	if(pModule!=NULL)
		Py_DECREF(pModule);

	//Clean up this thread's python interpreter reference
	PyThreadState_Swap(NULL);
	PyThreadState_Clear(myThreadState);
	PyThreadState_Delete(myThreadState);
	PyEval_ReleaseLock();
#endif // HAVE_PYTHON
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

	m_bIdling = true;
	// system script can handle that
	#ifdef HAVE_PYTHON
	if(pythonCallback("antiIdle",Py_BuildValue("(l)",this)))
		return;
	#endif
	// the default function
	int length;	
	QCString cstr = parseString( (const char *)m_param.m_strAntiString, &length );
	m_pTelnet->write( cstr, length );
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
void QTermWindow::mouseDoubleClickEvent( QMouseEvent * me)
{
	pythonMouseEvent(3, me->button(), me->state(), me->pos(),0);
}

void QTermWindow::mousePressEvent( QMouseEvent * me )
{
	// stop  the tab blinking
    if(m_tabTimer->isActive())
    {
		m_tabTimer->stop();
		m_pFrame->wndmgr->blinkTheTab(this,TRUE);
    }

	// Left Button for selecting
	if(me->button()&LeftButton && !(me->state()&KeyButtonMask))
	{
		// clear the selected before
		m_pBuffer->clearSelect();
		m_pScreen->refreshScreen();
	
		// set the selecting flag
		m_bSelecting = true;
		m_ptSelStart = m_pScreen->mapToChar( me->pos() );
		m_ptSelEnd = m_ptSelStart;
	}

	// Right Button
	if((me->button()&RightButton))
	{
		if(me->state()&ControlButton)
		{
			if(!m_pBBS->getUrl().isEmpty())		// on Url
				previewLink();
			return;
		}
		
		if(!(me->state()&KeyButtonMask))
		{
			if(!m_pBBS->getUrl().isEmpty())		// on Url
				m_pUrl->popup(me->globalPos());
			else
				m_pMenu->popup(me->globalPos());
			return;
		}
	}
	// Middle Button for paste
	if( me->button()&MidButton && !(me->state()&KeyButtonMask))
	{
		if( m_bConnected )
			pasteHelper(false);
		return;
	}

	// xterm mouse event
	sendMouseState(0, me->button(), me->state(), me->pos());

	// python mouse event
	pythonMouseEvent(0, me->button(), me->state(), me->pos(),0);
}


void QTermWindow::mouseMoveEvent( QMouseEvent * me)
{
	// selecting by leftbutton
	if( (me->state()&LeftButton) && m_bSelecting )
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

	if(m_bMouse && m_bConnected)
	{
		// set cursor pos, repaint if state changed
		QRect rect;
		if( m_pBBS->setCursorPos( m_pScreen->mapToChar(me->pos()),rect ) )
			m_pScreen->repaint( m_pScreen->mapToRect(rect), true );
		// judge if URL
		QRect rcOld;
		bool bUrl=false;
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
				bUrl = true;
			}
			else
			QToolTip::remove(this, m_pScreen->mapToRect(rcOld));
		}

		if(!bUrl)
		{
			int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
			if( nCursorType<=8 && nCursorType>=0 )
				setCursor(cursor[nCursorType]);
		}
	}
	// python mouse event
	pythonMouseEvent(2, me->button(), me->state(), me->pos(),0);
}

void QTermWindow::mouseReleaseEvent( QMouseEvent * me )
{	
	// other than Left Button ignored
	if( !(me->button()&LeftButton) || (me->state()&KeyButtonMask))
	{
		pythonMouseEvent(1, me->button(), me->state(), me->pos(),0);
		// no local mouse event
		sendMouseState(3, me->button(), me->state(), me->pos());
		return;
	}
	
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
		QString strUrl = QInputDialog::getText(caption, hint,
						QLineEdit::Normal, QString(m_pBBS->getUrl()), &ok);
		#else
		QString strUrl = QInputDialog::getText(caption, hint, 
						QString(m_pBBS->getUrl()), &ok);
		#endif
		if (ok)
		{
			QCString cstrCmd=m_pFrame->m_pref.strHttp.local8Bit();
			if(cstrCmd.find("%L")==-1) // no replace
				//QApplication::clipboard()->setText(strUrl);
				cstrCmd += " \"" + strUrl.local8Bit() +"\"";
			else
				cstrCmd.replace("%L", strUrl.local8Bit());
			
			runProgram(cstrCmd);
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
	int j = we->delta()>0 ? 4 : 5;
	if(!(we->state()&KeyButtonMask))
	{
		if(m_pFrame->m_pref.bWheel && m_bConnected)
			m_pTelnet->write(direction[j], sizeof(direction[j]));
		return;
	}

	pythonMouseEvent(4, NoButton, we->state(), we->pos(),we->delta());

	sendMouseState(j, NoButton, we->state(), we->pos());
}

//keyboard input event
void QTermWindow::keyPressEvent( QKeyEvent * e )
{
#ifdef HAVE_PYTHON
	int state=0;
	if(e->state()&AltButton)
		state |= 0x08;
	if(e->state()&ControlButton)
		state |= 0x10;
	if(e->state()&ShiftButton)
		state |= 0x20;
	pythonCallback("keyEvent",
					Py_BuildValue("liii", this, 0, state, e->key()));
#endif

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
		case Key_Delete: // stupid
			m_pTelnet->write( "\x1b[3~",4 );
		return;
		default:	
			break;
	}

	if( e->text().length() )
	{
		QCString cstrTmp = unicode2bbs(e->text());
		m_pTelnet->write( cstrTmp, cstrTmp.length() );
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

    int raw_size = m_pTelnet->raw_len();
    char * raw_str = new char[raw_size];
    m_pTelnet->read_raw(raw_str, raw_size);

    // read raw buffer
    m_pZmodem->ZmodemRcv( (uchar *)raw_str, raw_size, &(m_pZmodem->info));

if(m_pZmodem->transferstate == notransfer)
{	
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
	if( m_pBuffer->caret().y()==m_pBuffer->line()-1 &&
						m_pBuffer->caret().x()>=cstrText.length()-1 )
		m_wcWaiting.wakeAll();

	QToolTip::remove(this, m_pScreen->mapToRect(m_rcUrl));

	// message received
	// 03/19/2003. the caret posion removed as a message judgement
	// because smth.org changed
    if( m_pDecode->bellReceive() ) //&& m_pBuffer->caret().y()==1 )
    {
		if( m_bBeep )
			if(m_pFrame->m_pref.strWave.isEmpty()||m_pFrame->m_pref.nBeep==3)
				qApp->beep();
			else {
				//QSound::play(m_pFrame->m_pref.strWave);
				switch (m_pFrame->m_pref.nMethod){
				case 0:
					sound = new QTermInternalSound(m_pFrame->m_pref.strWave);
					break;
				#ifndef _NO_ARTS_COMPILED
				case 1:
					sound = new QTermArtsSound(m_pFrame->m_pref.strWave);
					break;
				#endif
				#ifndef _NO_ESD_COMPILED
				case 2:
					sound = new QTermEsdSound(m_pFrame->m_pref.strWave);
					break;
				#endif
				case 3:
					sound = new QTermExternalSound(m_pFrame->m_pref.strPlayer,
									m_pFrame->m_pref.strWave);
					break;
				}
				sound->play();
				delete sound;
			}

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
			#ifdef HAVE_PYTHON
			if(!pythonCallback("autoReply",Py_BuildValue("(l)",this)))
			{
			#endif
			// TODO: save messages
	        if ( m_bIdling )
				replyMessage();
			else
				m_replyTimer->start(m_param.m_nMaxIdle*1000/2);
			#ifdef HAVE_PYTHON
			}
			#endif
		}
	}
	
	// set page state
	m_pBBS->setPageState();
	//refresh screen
	m_pScreen->refreshScreen();

	#ifdef HAVE_PYTHON
	// python 
	pythonCallback("dataEvent",Py_BuildValue("(l)",this));
	#endif
}
	
    //delete the buf
    delete []str;
    delete []raw_str;

    if (m_pZmodem->transferstate == transferstop)
        m_pZmodem->transferstate = notransfer;
}

void QTermWindow::ZmodemState(int type, int value, const QCString& status)
{
	QCString strMsg;
    //to be completed
    switch(type)
    {
        case    RcvByteCount:
				m_pZmDialog->setProgress( value );
                break;
        case    SndByteCount:
				m_pZmDialog->setProgress( value );
                break;
        case    RcvTimeout:
                /* receiver did not respond, aborting */
                strMsg.sprintf("time out!");
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    SndTimeout:
                /* value is # of consecutive send timeouts */
                strMsg.sprintf("time out after trying %d times", value);
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    RmtCancel:
                /* remote end has cancelled */
				strMsg.sprintf("canceled by remote peer %s", (const char*)status);
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    ProtocolErr:
                /* protocol error has occurred, val=hdr */
                strMsg.sprintf("unhandled header %d at state %s", value, (const char *)status);
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    RemoteMessage:
                /* message from remote end */
                strMsg.sprintf("msg from remote peer: %s",(const char *)status);
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    DataErr:
                /* data error, val=error count */
				strMsg.sprintf("data errors %d", value);
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    FileErr:
                /* error writing file, val=errno */
                strMsg.sprintf("falied to write file");
				m_pZmDialog->addErrorLog(strMsg);
                break;
        case    FileBegin:
                /* file transfer begins, str=name */
//                qWarning("starting file %s", status);
				m_pZmDialog->setFileInfo(G2U(status),value);
				m_pZmDialog->setProgress(0);
				m_pZmDialog->clearErrorLog();
				m_pZmDialog->show();
				#if (QT_VERSION>=0x030200)
				m_pZmDialog->setModal(true);
				#endif
                break;
        case    FileEnd:
                /* file transfer ends, str=name */
//                qWarning("finishing file %s", status);
				m_pZmDialog->hide();
                break;
        case    FileSkip:
               /* file being skipped, str=name */
				strMsg.sprintf("skipping file %s", (const char *)status);
				m_pZmDialog->addErrorLog(strMsg);
                break;
    }

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
	case TSWRITED:
		// restart the idle timer	
		if(m_idleTimer->isActive())
			m_idleTimer->stop();
		if( m_bAntiIdle )
			m_idleTimer->start( m_param.m_nMaxIdle*1000 );
		m_bIdling = false;
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
   // as most users complain they cant copy from qterm to other program by context ment, we copy
   // the text to clipboard except mouse selection
	if( m_param.m_nBBSCode==0 )
	{
		clipboard->setText(G2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
									 parseString((const char *)m_param.m_strEscape))), 
					QClipboard::Clipboard );
		clipboard->setText(G2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
									 parseString((const char *)m_param.m_strEscape))), 
					QClipboard::Selection );
	}
	else
	{
		clipboard->setText(B2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
									 parseString((const char *)m_param.m_strEscape))), 
					QClipboard::Clipboard );
		clipboard->setText(B2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
									 parseString((const char *)m_param.m_strEscape))), 
					QClipboard::Selection );
	}
	#else
	if( m_param.m_nBBSCode==0 )
		clipboard->setText(G2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))));
	else
		clipboard->setText(B2U(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor, 
										 parseString((const char *)m_param.m_strEscape))));
	#endif
}

void QTermWindow::paste()
{
	pasteHelper(true);
}

void QTermWindow::pasteHelper( bool clip )
{
	if( !m_bConnected )
		return;
	
	QClipboard *clipboard = QApplication::clipboard();
	QCString cstrText;
	
	if(m_pFrame->m_nClipCodec==0)
	{
		#if (QT_VERSION>=0x030100)
		if(clip)
			cstrText=U2G( clipboard->text(QClipboard::Clipboard) );
		else
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
		if(clip)
			cstrText=U2B( clipboard->text(QClipboard::Clipboard) );
		else
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
	
	m_pDAThread = new QTermDAThread(this);
	m_pDAThread->start();
		
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
	addrDialog set(this, true);
	
	set.param = m_param;
	set.updateData(false);		 
	set.tabWidget->setCurrentPage(1);

	if(set.exec()==1)
	{
		m_param = set.param;
		m_bSetChanged = true;

		QResizeEvent* re =new QResizeEvent( m_pScreen->size(),m_pScreen->size());
		m_pScreen->setSchema();
		QApplication::postEvent( m_pScreen,re );
	}

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
	// get the previous dir
	QString file = getOpenFileName("Python File (*.py *.txt)", this);

	if(file.isEmpty())
		return;

	runScriptFile(file.local8Bit());
}

void QTermWindow::stopScript()
{
}

void QTermWindow::viewMessages( )
{
	msgDialog msg(this);

	QTermConfig conf(fileCfg);
	const char * size = conf.getItemValue("global","msgdialog");
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
	// disabled
	if( !m_bAntiIdle && m_idleTimer->isActive() )
		m_idleTimer->stop();
	// enabled
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
	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
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
/* 	                         Events                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermWindow::customEvent(QCustomEvent*e)
{

	if( e->type() == DAE_FINISH )
	{
		articleDialog article(this);
		QTermConfig conf(fileCfg);
		const char * size = conf.getItemValue("global","articledialog");
		if(size!=NULL)
		{
			int x,y,cx,cy;
			sscanf(size,"%d %d %d %d",&x,&y,&cx,&cy);
			article.resize(QSize(cx,cy));
			article.move(QPoint(x,y));	
		}
		article.cstrArticle = m_pDAThread->cstrArticle;
		if(m_param.m_nBBSCode==0)
			article.textBrowser2->setText(G2U(article.cstrArticle));
		else
			article.textBrowser2->setText(B2U(article.cstrArticle));
		article.exec();
		QCString cstrSize;
		cstrSize.sprintf("%d %d %d %d",article.x(),article.y(),article.width(),article.height());
		conf.setItemValue("global","articledialog",cstrSize);
		conf.save(fileCfg);
	}
	else if(e->type() == DAE_TIMEOUT)
	{
		QMessageBox::warning(this,"timeout","download article timeout, aborted");
	}	
	else if(e->type() == PYE_ERROR)
	{
		QMessageBox::warning(this,"Python script error", m_strPythonError);
	}
	else if(e->type() == PYE_FINISH)
	{
		QMessageBox::information(this,"Python script finished", "Python script file executed successfully");
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
	
	QCString cstrTmp = m_param.m_strReplyKey.local8Bit();
	QCString cstr = parseString(cstrTmp.isEmpty()?QCString("^Z"):cstrTmp);
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

QCString QTermWindow::unicode2bbs(const QString& text)
{
	QCString strTmp;

	if( m_pFrame->m_pref.nXIM == 0 )
	{
		strTmp = U2G(text);
		if( m_param.m_nBBSCode == 1)
		{
			char * str = m_converter.G2B( strTmp, strTmp.length() );
			strTmp = str;
			delete []str;
		}
	}
	else
	{
		strTmp = U2B(text);
		if( m_param.m_nBBSCode == 0 )
		{
			char * str = m_converter.B2G( strTmp, strTmp.length() );
			strTmp = str;
			delete []str;
		}
	}

	return strTmp;
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

void QTermWindow::setMouseMode(bool on)
{
	m_bMouseX11 = on;
}

/* 0-left 1-middle 2-right 3-relsase 4/5-wheel
 *
 */
void QTermWindow::sendMouseState( int num, 
				ButtonState btnstate, ButtonState keystate, const QPoint& pt )
{
	if(!m_bMouseX11)
		return;
	
	QPoint ptc = m_pScreen->mapToChar(pt);

	if(btnstate&LeftButton)
		num = num==0?0:num;
	else if(btnstate&MidButton)
		num = num==0?1:num;
	else if(btnstate&RightButton)
		num = num==0?2:num;

	int state = 0;
	if(keystate&ShiftButton)
		state |= 0x04;
	if(keystate&MetaButton)
		state |= 0x08;
	if(keystate&ControlButton)
		state |= 0x10;

	// normal buttons are passed as 0x20 + button,
	// mouse wheel (buttons 4,5) as 0x5c + button
	if(num>3)	num += 0x3c;

	char mouse[10];
	sprintf(mouse, "\033[M%c%c%c", 
			num+state+0x20,
			ptc.x()+1+0x20,
			ptc.y()+1+0x20);
	m_pTelnet->write( mouse, strlen(mouse) );
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Python Func                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */


void QTermWindow::runScriptFile( const QCString & cstr )
{
	char str[32];
	sprintf(str,"%ld",this);
	
	char *argv[2]={str,NULL};

#ifdef HAVE_PYTHON
    // get the global python thread lock
    PyEval_AcquireLock();

    // get a reference to the PyInterpreterState
    extern PyThreadState * mainThreadState;
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;

    // create a thread state object for this thread
    PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
    PyThreadState_Swap(myThreadState);


	PySys_SetArgv(1, argv);

	runPythonFile(cstr);

	//Clean up this thread's python interpreter reference
    PyThreadState_Swap(NULL);
    PyThreadState_Clear(myThreadState);
    PyThreadState_Delete(myThreadState);
    PyEval_ReleaseLock(); 
#endif // HAVE_PYTHON
}

#ifdef HAVE_PYTHON
bool QTermWindow::pythonCallback(const char* func, PyObject* pArgs)
{
	if(!m_bPythonScriptLoaded) {
		Py_DECREF(pArgs);
		return false;
	};
	
	bool done = false;
	// get the global lock
	 PyEval_AcquireLock();
	// get a reference to the PyInterpreterState
      
	//Python thread references
	extern PyThreadState * mainThreadState;
      
	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
	// create a thread state object for this thread
	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
	PyThreadState_Swap(myThreadState);
    
	PyObject *pF = PyString_FromString(func);
	PyObject *pFunc = PyDict_GetItem(pDict, pF);
 	Py_DECREF(pF);

	if (pFunc && PyCallable_Check(pFunc)) 
	{
		PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
		Py_DECREF(pArgs);
		if (pValue != NULL) 
		{
			done = true;
			Py_DECREF(pValue);
		}
		else 
		{
			QMessageBox::warning(this,"Python script error", getException());
		}
      }
	else 
	{
		PyErr_Print();
		printf("Cannot find python %s callback function\n", func);
	}
      
	// swap my thread state out of the interpreter
	PyThreadState_Swap(NULL);
	// clear out any cruft from thread state object
	PyThreadState_Clear(myThreadState);
   // delete my thread state object
	PyThreadState_Delete(myThreadState);
	// release the lock
	PyEval_ReleaseLock();

	return done;
}
#endif //HAVE_PYTHON

int QTermWindow::runPythonFile( const char * filename )
{
#ifdef HAVE_PYTHON
    static char buffer[1024];
    const char *file = filename;

    char *p;

    /* Have to do it like this. PyRun_SimpleFile requires you to pass a
     * stdio file pointer, but Vim and the Python DLL are compiled with
     * different options under Windows, meaning that stdio pointers aren't
     * compatible between the two. Yuk.
     *
     * Put the string "execfile('file')" into buffer. But, we need to
     * escape any backslashes or single quotes in the file name, so that
     * Python won't mangle the file name.
	 * ---- kafa
     */
	strcpy(buffer, "def work_thread():\n"
					"\ttry:\n\t\texecfile('");
    p = buffer + 37; /* size of above  */

    while (*file && p < buffer + (1024 - 3))
    {
	if (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    /* If we didn't finish the file name, we hit a buffer overflow */
    if (*file != '\0')
	return -1;

    /* Put in the terminating "')" and a null */
    *p++ = '\'';
	*p++ = ',';
	*p++ = '{';
	*p++ = '}';
	*p++ = ')';
    *p++ = '\n';
	*p++ = '\0';

	QCString cstr;
	
//	cstr.sprintf("\t\tqterm.formatError(%ld,'')\n",this);
//	strcat(buffer, cstr);
	
	cstr.sprintf("\texcept:\n"
				"\t\texc, val, tb = sys.exc_info()\n"
				"\t\tlines = traceback.format_exception(exc, val, tb)\n"
				"\t\terr = string.join(lines)\n"
				"\t\tprint err\n"
				"\t\tf=open('%s','w')\n"
				"\t\tf.write(err)\n"
				"\t\tf.close()\n"
				,getErrOutputFile(this).data());
	strcat(buffer, cstr);

	cstr.sprintf("\t\tqterm.formatError(%ld)\n",this);
	strcat(buffer, cstr);

	strcat(buffer, "\t\texit\n");

    /* Execute the file */
	PyRun_SimpleString("import thread,string,sys,traceback,qterm");
	PyRun_SimpleString(buffer);
	PyRun_SimpleString(	"thread.start_new_thread(work_thread,())\n");

#endif // HAVE_PYTHON

	return 0;
}

void QTermWindow::pythonMouseEvent(int type, ButtonState btnstate, ButtonState keystate, 
				const QPoint& pt, int delta  )
{
	int state=0;

	if(btnstate&LeftButton)
		state |= 0x01;
	if(btnstate&RightButton)
		state |= 0x02;
	if(btnstate&MidButton)
		state |= 0x04;

	if(keystate&AltButton)
		state |= 0x08;
	if(keystate&ControlButton)
		state |= 0x10;
	if(keystate&ShiftButton)
		state |= 0x20;

	QPoint ptc = m_pScreen->mapToChar(pt);

#ifdef HAVE_PYTHON
	pythonCallback("mouseEvent", 
					Py_BuildValue("liiiii", this, type, state, ptc.x(), ptc.y(),delta));
#endif
}

void QTermWindow::inputHandle(QString * text)
{
	if (text->length() > 0) {
		QCString cstrTmp = unicode2bbs(*text);
		m_pTelnet->write( cstrTmp, cstrTmp.length() );
	}
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         HTTP Func                                      */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void QTermWindow::openLink()
{
	QCString cstrCmd=m_pFrame->m_pref.strHttp.local8Bit();
	if(cstrCmd.find("%L")==-1) // no replace
	//QApplication::clipboard()->setText(strUrl);
		cstrCmd += " \"" + m_pBBS->getUrl() +"\"";
	else
		cstrCmd.replace(QRegExp("%L",false), m_pBBS->getUrl());
	runProgram(cstrCmd);
}

void QTermWindow::previewLink()
{	
	getHttpHelper(m_pBBS->getUrl(), true);
}

void QTermWindow::copyLink()
{
	QString strUrl;
	if( m_param.m_nBBSCode==0 )
		strUrl=G2U(m_pBBS->getUrl());
	else
		strUrl=B2U(m_pBBS->getUrl());
				
	QClipboard *clipboard = QApplication::clipboard();

	#if (QT_VERSION>=0x030100)
	clipboard->setText(strUrl, QClipboard::Selection);
	clipboard->setText(strUrl, QClipboard::Clipboard);
	#else
	clipboard->setText(strUrl);
	#endif
}

void QTermWindow::saveLink()
{
	getHttpHelper(m_pBBS->getUrl(), false);
}

void QTermWindow::getHttpHelper(const QString& strUrl, bool bPreview)
{
	QTermHttp *pHttp = new QTermHttp(this);
	connect(pHttp, SIGNAL(done(QTermHttp*)), this, SLOT(httpDone(QTermHttp*)));
	pHttp->getLink(strUrl, bPreview);
}

void QTermWindow::httpDone(QTermHttp *pHttp)
{
	pHttp->deleteLater();
}
