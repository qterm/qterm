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
#include "qtermhttp.h"
#include "qtermiplocation.h"
#include "osdmessage.h"
#include "statusBar.h"
#include "progressBar.h"
#include "qtermglobal.h"
#include "hostinfo.h"

#ifdef SCRIPT_ENABLED
#include "scripthelper.h"
#endif // SCRIPT_ENABLED

#ifdef DBUS_ENABLED
#include "dbus.h"
#endif // DBUS_ENABLED

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QByteArray>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QPixmap>
#include <QApplication>
#include <QClipboard>
#include <QToolButton>
#include <QMessageBox>
#include <QStatusBar>
#include <QFontDialog>
#include <QTextCodec>
#include <QTimer>
#include <QMenu>
#include <QTextBrowser>
#include <QInputDialog>
#include <QRegExp>
#include <QFileDialog>
#include <QTabWidget>
#include <QStringList>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QtCore/QProcess>
#ifdef SCRIPT_ENABLED
#include <QtScript>
#ifdef SCRIPTTOOLS_ENABLED
#include <QtScriptTools/QScriptEngineDebugger>
#endif
#endif
#include <QtDebug>

namespace QTerm
{

// script thread
DAThread::DAThread(Window *win)
{
    pWin = win;
}

DAThread::~DAThread()
{

}

void DAThread::run()
{
    mutex.lock();
    QStringList strList;
    while (1) {
        // check it there is duplicated string
        // it starts from the end in the range of one screen height
        // so this is a non-greedy match
        QString strTemp = pWin->m_pBuffer->screen(0)->getText().replace(QRegExp("\\s+$"),"");
        int i = 0;
        int start = 0;
        QStringList::Iterator it = strList.end();
        while (it != strList.begin() && i < pWin->m_pBuffer->line() - 1) {
//   for(QStringList::Iterator it=strList.end();
//    it!=strList.begin(), i < pWin->m_pBuffer->line()-1; // not exceeeding the last screen
//    --it, i++)
            --it;
            i++;
            if (*it != strTemp)
                continue;
            QStringList::Iterator it2 = it;
            bool dup = true;
            // match more to see if its duplicated
            for (int j = 0; j <= i && it2 != strList.end(); j++, it2++) {
                QString str1 = pWin->m_pBuffer->screen(j)->getText().replace(QRegExp("\\s+$"),"");
                if (*it2 != str1) {
                    dup = false;
                    break;
                }
            }
            if (dup) {
                // set the start point
                start = i + 1;
                break;
            }
        }
        // add new lines
        for (i = start;i < pWin->m_pBuffer->line() - 1;i++)
            strList += pWin->m_pBuffer->screen(i)->getText().replace(QRegExp("\\s+$"),"");

        // the end of article
        if (pWin->m_pBuffer->screen(
                    pWin->m_pBuffer->line() - 1)->getText().indexOf("%") == -1)
            break;
        // continue
        pWin->m_pTelnet->write(" ", 1);
        if (!pWin->m_wcWaiting.wait(&mutex, 10000)) { // timeout
            //qApp->postEvent(pWin, new QCustomEvent(DAE_TIMEOUT));
            emit done(DAE_TIMEOUT);
            break;
        }
    }
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    strArticle = strList.join("\r\n");
#else
    strArticle = strList.join("\n");
#endif
    //qApp->postEvent(pWin, new QCustomEvent(DAE_FINISH));
    emit done(DAE_FINISH);
    mutex.unlock();
}

/*
void DAThread::run()
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

  if(!pWin->m_wcWaiting.wait(10000)) // timeout
  {
   postEvent(pWin, new QCustomEvent(DAE_TIMEOUT));
   break;
  }
    }

 postEvent(pWin, new QCustomEvent(DAE_FINISH));
}
*/

char Window::direction[][5] = {
    // 4
    "\x1b[1~", // 0 HOME
    "\x1b[4~", // 1 END
    "\x1b[5~", // 2 PAGE UP
    "\x1b[6~", // 3 PAGE DOWN
    // 3
    "\x1b[A", // 4 UP
    "\x1b[B", // 5 DOWN
    "\x1b[D", // 6 LEFT
    "\x1b[C" // 7 RIGHT
};

//constructor
Window::Window(Frame * frame, Param param, int addr, QWidget * parent, const char * name, Qt::WFlags wflags)
        : QMainWindow(parent, wflags), m_strMessage(), location()
{

    m_pFrame = frame;
    m_param = param;
    m_nAddrIndex = addr;
    m_hostInfo = NULL;
    QString pathLib = Global::instance()->pathLib();
    setMouseTracking(true);

#ifdef SCRIPT_ENABLED
    m_scriptEngine = NULL;
    m_scriptHelper = NULL;
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger = new QScriptEngineDebugger;
#endif // SCRIPTTOOLS_ENABLED
#endif // SCRIPT_ENABLED
//init the textline list

    m_pBuffer = new Buffer(m_param.m_nRow, m_param.m_nCol, m_param.m_nScrollLines);
    if (param.m_nProtocolType == 0)
        m_pTelnet = new Telnet(m_param.m_strTerm,
                               m_param.m_nRow, m_param.m_nCol, false);
    else {
#ifndef SSH_ENABLED
        QMessageBox::warning(this, "QTerm",
                             tr("SSH support is not compiled, QTerm can only use Telnet!"));
        m_pTelnet = new Telnet(m_param.m_strTerm,
                               m_param.m_nRow, m_param.m_nCol, false);
#else
        m_pTelnet = new Telnet(m_param.m_strTerm,
                               m_param.m_nRow, m_param.m_nCol, true);
#endif
    }
    connect(m_pBuffer, SIGNAL(windowSizeChanged(int, int)),
            m_pTelnet, SLOT(windowSizeChanged(int, int)));
    m_pZmDialog = new zmodemDialog(this);
    m_pZmodem = new Zmodem(m_pTelnet, param.m_nProtocolType);

    m_codec = QTextCodec::codecForName(param.m_BBSCode.toLatin1());
    if (m_codec == 0) {
        qDebug("Fallback to GBK codec");
        m_codec = QTextCodec::codecForName("GBK");
    }

    m_pDecode = new Decode(m_pBuffer, m_codec);
    m_pBBS   = new BBS(m_pBuffer);
    m_pScreen = new Screen(this, m_pBuffer, &m_param, m_pBBS);

    m_pIPLocation = new IPLocation(pathLib);
    m_pMessage = new PageViewMessage(this);
    m_bCheckIP = m_pIPLocation->haveFile();
    m_pSound = NULL;

    setFocusProxy(m_pScreen);
    setCentralWidget(m_pScreen);
    connect(m_pFrame, SIGNAL(bossColor()), m_pScreen, SLOT(bossColor()));
    connect(m_pFrame, SIGNAL(scrollChanged()), m_pScreen, SLOT(updateScrollBar()));
    connect(m_pScreen, SIGNAL(inputEvent(const QString &)), this, SLOT(inputHandle(const QString &)));
    connect(m_pZmodem, SIGNAL(ZmodemState(int, int, const QString&)),
            this, SLOT(ZmodemState(int, int, const QString&)));
    connect(m_pZmDialog, SIGNAL(canceled()), m_pZmodem, SLOT(zmodemCancel()));

    connect(m_pDecode, SIGNAL(mouseMode(bool)), this, SLOT(setMouseMode(bool)));

    m_popWin = new popWidget(this, m_pFrame);

    m_pMessage->display(tr("Not Connected"));
    statusBar()->setSizeGripEnabled(false);

    if (Global::instance()->showStatusBar())
        statusBar()->show();
    else
        statusBar()->hide();

    // disable the dock menu
//  setDockMenuEnabled(false);

    m_pUrl = new QMenu(m_pScreen);
    /*
    m_pUrl->insertItem( tr("Preview image"), this, SLOT(previewLink()) );
    m_pUrl->insertItem( tr("Open link"), this, SLOT(openLink()) );
    m_pUrl->insertItem( tr("Copy link address"), this, SLOT(copyLink()) );
    m_pUrl->insertItem( tr("Save target as..."), this, SLOT(saveLink()) );
    */

    m_pUrl->addAction(tr("Preview image"), this, SLOT(previewLink()));
    m_pUrl->addAction(tr("Open link"), this, SLOT(openLink()));
    m_pUrl->addAction(tr("Copy link address"), this, SLOT(copyLink()));

    m_pMenu = m_pFrame->genPopupMenu(this);

//connect telnet signal to slots
    connect(m_pTelnet, SIGNAL(readyRead(int)),
            this, SLOT(readReady(int)));
    connect(m_pTelnet, SIGNAL(TelnetState(int)),
            this, SLOT(TelnetState(int)));
    connect(m_pFrame, SIGNAL(statusBarChanged(bool)),
            this, SLOT(showStatusBar(bool)));
// timers
    m_idleTimer = new QTimer;
    connect(m_idleTimer, SIGNAL(timeout()), this, SLOT(idleProcess()));
    m_replyTimer = new QTimer;
    connect(m_replyTimer, SIGNAL(timeout()), this, SLOT(replyProcess()));
    m_tabTimer = new QTimer;
    connect(m_tabTimer, SIGNAL(timeout()), this, SLOT(blinkTab()));
    m_reconnectTimer = new QTimer;
    connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnect()));
    m_ipTimer = new QTimer;
    connect(m_ipTimer, SIGNAL(timeout()), this, SLOT(showIP()));
    m_updateTimer = new QTimer;
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateWindow()));


// initial varibles
    m_bCopyColor = false;
    m_bCopyRect = false;
    m_bAntiIdle = true;
    m_bAutoReply = m_param.m_bAutoReply;
    m_bBeep  = !(
#ifndef PHONON_ENABLED
                   Global::instance()->m_pref.strPlayer.isEmpty() ||
#endif // PHONON_ENABLED
                   Global::instance()->m_pref.strWave.isEmpty());
    m_bMouse = true;
    m_bWordWrap = false;
    m_bAutoCopy = true;
    m_bMessage = false;
    m_bReconnect = m_param.m_bReconnect;

    m_pDAThread = 0;
    m_bConnected = false;
    m_bIdling = false;

    m_bSetChanged = false;

    m_bMouseX11 = false;
    m_bMouseClicked = false;
#ifdef SSH_ENABLED
    if (param.m_nProtocolType != 0)
        m_bDoingLogin = true;
    else
#endif
        m_bDoingLogin = false;

    cursor[0] = QCursor(QPixmap(pathLib + "cursor/home.xpm"));
    cursor[1] = QCursor(QPixmap(pathLib + "cursor/end.xpm"));
    cursor[2] = QCursor(QPixmap(pathLib + "cursor/pageup.xpm"));
    cursor[3] = QCursor(QPixmap(pathLib + "cursor/pagedown.xpm"));
    cursor[4] = QCursor(QPixmap(pathLib + "cursor/prev.xpm"));
    cursor[5] = QCursor(QPixmap(pathLib + "cursor/next.xpm"));
    cursor[6] = QCursor(QPixmap(pathLib + "cursor/exit.xpm"), 0, 10);
    cursor[7] = QCursor(QPixmap(pathLib + "cursor/hand.xpm"));
    cursor[8] = Qt::ArrowCursor;

    // the system wide script
    m_bPythonScriptLoaded = false;

    initScript();

    connectHost();
}

//destructor
Window::~Window()
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
    delete m_updateTimer;

    delete m_pUrl;
    delete m_pScreen;
    delete m_reconnectTimer;
    delete m_pIPLocation;
    delete m_pMessage;
    delete m_pSound;
    delete m_hostInfo;
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptEngine->abortEvaluation();
    delete m_scriptDebugger;
#endif
}

//close event received
void Window::closeEvent(QCloseEvent * clse)
{
    if (m_bConnected && Global::instance()->m_pref.bWarn) {
        QMessageBox mb("QTerm",
                       tr("Connected,Do you still want to exit?"),
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this);
        if (mb.exec() == QMessageBox::Yes) {
            m_pTelnet->close();
            saveSetting();
            m_pFrame->wndmgr->removeWindow(this);
            clse->accept();
        } else
            clse->ignore();
    } else {
        saveSetting();
        m_pFrame->wndmgr->removeWindow(this);
        clse->accept();
    }
}
/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           Timer                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void Window::idleProcess()
{
    // do as autoreply when it is enabled
    if (m_replyTimer->isActive() && m_bAutoReply) {
        replyMessage();
        if (m_tabTimer->isActive()) {
            m_tabTimer->stop();
            m_pFrame->wndmgr->blinkTheTab(this, TRUE);
        }
        return;
    }

    m_bIdling = true;
    // system script can handle that
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("antiIdle");
        if (func.isFunction()) {
            func.call();
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("antiIdle is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    // the default function
    int length;
    QByteArray cstr = parseString(m_param.m_strAntiString.toLocal8Bit(), &length);
    m_pTelnet->write(cstr, length);
}

void Window::replyProcess()
{
    // if AutoReply still enabled, then autoreply
    if (m_bAutoReply)
        replyMessage();
    else // else just stop the timer
        m_replyTimer->stop();

    if (m_tabTimer->isActive()) {
        m_tabTimer->stop();
        m_pFrame->wndmgr->blinkTheTab(this, TRUE);
    }
}

void Window::blinkTab()
{
    static bool bVisible = TRUE;
    m_pFrame->wndmgr->blinkTheTab(this, bVisible);
    bVisible = !bVisible;
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           Mouse & Key                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void Window::enterEvent(QEvent *)
{
}

void Window::leaveEvent(QEvent *)
{
}

void Window::mouseDoubleClickEvent(QMouseEvent * me)
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << 3 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << me->x() << me->y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
}

void Window::mousePressEvent(QMouseEvent * me)
{
    // stop  the tab blinking
    if (m_tabTimer->isActive()) {
        m_tabTimer->stop();
        m_pFrame->wndmgr->blinkTheTab(this, TRUE);
    }

    // Left Button for selecting
    if (me->button()&Qt::LeftButton && !(me->modifiers())) {
        // clear the selected before
        m_pBuffer->clearSelect();
        m_pScreen->m_ePaintState = Screen::NewData;
        m_pScreen->update();

        // set the selecting flag
        m_bSelecting = true;
        m_ptSelStart = me->pos();
        m_ptSelEnd = m_ptSelStart;
    }

#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << 0 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << me->x() << me->y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    // Right Button
    if ((me->button()&Qt::RightButton)) {
        if (me->modifiers()&Qt::ControlModifier) {
            if (!m_pBBS->getUrl().isEmpty()) // on Url
                previewLink();
            return;
        }

        if (!(me->modifiers())) {
            if (!m_pBBS->getUrl().isEmpty()) // on Url
                m_pUrl->popup(me->globalPos());
            else
                m_pMenu->popup(me->globalPos());
            return;
        }
    }
    // Middle Button for paste
    if (me->button()&Qt::MidButton && !(me->modifiers())) {
        if (m_bConnected) {
            if (!m_pBBS->getUrl().isEmpty())        // on Url
                previewLink();
            else
                pasteHelper(false);
        }
        return;
    }

    // If it is a click, there should be a press event and a release event.
    m_bMouseClicked = true;

    // xterm mouse event
    //sendMouseState(0, me->button(), me->state(), me->pos());
}


void Window::mouseMoveEvent(QMouseEvent * me)
{
    m_ptMouse = me->pos();
    // selecting by leftbutton
    if ((me->buttons()&Qt::LeftButton) && m_bSelecting) {
        if (me->pos().y() < childrenRect().top())
            m_pScreen->scrollLine(-1);
        if (me->pos().y() > childrenRect().bottom())
            m_pScreen->scrollLine(1);

        m_ptSelEnd = me->pos();
        QPoint point = m_ptSelEnd - m_ptSelStart;
        if (point.manhattanLength() > 3) {
            m_pBuffer->setSelect(m_pScreen->mapToChar(m_ptSelStart), m_pScreen->mapToChar(m_ptSelEnd), m_bCopyRect);
            m_pScreen->m_ePaintState = Screen::NewData;
            m_pScreen->update();
        }
    }

#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << 2 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << me->x() << me->y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    if (m_bMouse && m_bConnected) {
        // set cursor pos, repaint if state changed
        QRect rect;
        if (m_pBBS->setCursorPos(m_pScreen->mapToChar(me->pos()), rect))
            m_pScreen->repaint(m_pScreen->mapToRect(rect));
        // judge if URL
        QRect rcOld;
        QRect rcOld_IP;
        QRect rcUrl_IP = m_rcUrl;
        bool bUrl = false;
        if (Global::instance()->m_pref.bUrl) {
            if (m_pBBS->isIP(rcUrl_IP, rcOld_IP) && m_bCheckIP) {
                if (rcUrl_IP != rcOld_IP) {
                    if (!m_ipTimer->isActive()) {
                        m_ipTimer->setSingleShot(false);
                        m_ipTimer->start(100);
                    }
                }
            } else {
                if (m_ipTimer->isActive())
                    m_ipTimer->stop();
            }

            if (m_pBBS->isUrl(m_rcUrl, rcOld)) {
                setCursor(Qt::PointingHandCursor);
                bUrl = true;
            }
        }

        if (!bUrl) {
            int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
            if (nCursorType <= 8 && nCursorType >= 0)
                setCursor(cursor[nCursorType]);
        }
    }
}

void Window::mouseReleaseEvent(QMouseEvent * me)
{
    if (!m_bMouseClicked)
        return;
    m_bMouseClicked = false;

    // Left Button for selecting
    m_ptSelEnd = me->pos();
    if (m_ptSelEnd != m_ptSelStart && m_bSelecting) {
        m_pBuffer->setSelect(m_pScreen->mapToChar(m_ptSelStart), m_pScreen->mapToChar(m_ptSelEnd), m_bCopyRect);
        m_pScreen->m_ePaintState = Screen::NewData;
        m_pScreen->update();
        if (m_bAutoCopy)
            copy();
        m_bSelecting = false;
        return;
    }
    m_bSelecting = false;


#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << 1 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << me->x() << me->y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    if (!m_bMouse || !m_bConnected)
        return;

    // url
    if (!m_pBBS->getUrl().isEmpty()) {
        bool ok;
        QString caption = tr("Open URL");
        QString hint = "url";
        QString strUrl = QInputDialog::getText(this, caption, hint,
                                               QLineEdit::Normal, QString(m_pBBS->getUrl()), &ok);
        if (ok) {
            Global::instance()->openUrl(strUrl);
        }
        return;
    }
    // mouse menu
    int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
    if (nCursorType<4 && nCursorType >= 0)
        m_pTelnet->write(direction[nCursorType], 4);
    else if (nCursorType<7 && nCursorType >= 4)
        m_pTelnet->write(direction[nCursorType], 3);
    else if (nCursorType == 7) {
        char cr = CHAR_CR;
        QRect rc = m_pBBS->getSelectRect();
        switch (m_pBBS->getPageState()) {
        case 0:
            if (!rc.isEmpty()) {
                char cMenu = m_pBBS->getMenuChar();
                m_pTelnet->write(&cMenu, 1);
                m_pTelnet->write(&cr, 1);
            }
            break;
        case 1:
            if (!rc.isEmpty()) {
                int n = rc.y() - m_pBuffer->caretY();
                // scroll lines
                if (n > 0)
                    while (n) {
                        m_pTelnet->write(direction[5], 4);
                        n--;
                    }
                if (n < 0) {
                    n = -n;
                    while (n) {
                        m_pTelnet->write(direction[4], 4);
                        n--;
                    }
                }
                // don't forget to send a CHAR_CR at last to enter in
                m_pTelnet->write(&cr, 1);
            }
            break;
        default:
            break;
        }
    } else if (nCursorType == 8) {
        char cr = CHAR_CR;
        m_pTelnet->write(&cr, 1);
    }
}

void Window::wheelEvent(QWheelEvent *we)
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        m_scriptHelper->setAccepted(false);
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onWheelEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << we->delta() << (int) we->buttons() << (int) we->modifiers() << (int) we->orientation() << we->x() << we->y() );
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onWheelEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    int j = we->delta() > 0 ? 4 : 5;
    if (!(we->modifiers())) {
        if (Global::instance()->m_pref.bWheel && m_bConnected)
            m_pTelnet->write(direction[j], sizeof(direction[j]));
        return;
    }
}

//keyboard input event
void Window::keyPressEvent(QKeyEvent * e)
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        m_scriptHelper->setAccepted(false);
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onKeyPressEvent");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << e->key() << (int) e->modifiers() << e->text());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    if (!m_bConnected) {
        if (e->key() == Qt::Key_Return)
            reconnect();
        return;
    }

    // stop  the tab blinking
    if (m_tabTimer->isActive()) {
        m_tabTimer->stop();
        m_pFrame->wndmgr->blinkTheTab(this, TRUE);
    }

    // message replying
    if (m_replyTimer->isActive())
        m_replyTimer->stop();

    if (e->modifiers() & Qt::MetaModifier) {
        if (e->key() >= Qt::Key_A && e->key() <= Qt::Key_Z) {
            char ch = e->key() & 0x1f;
            m_pTelnet->write(&ch, 1);
        }
        return;
    }

    // TODO get the input messages
// if( m_bMessage && e->key()==Key_Return && m_pBuffer->caret().y()==1 )
// {
//  m_strMessage += m_pBuffer->screen(1)->getText()+"\n";
// }

    switch (e->key()) {
    case Qt::Key_Home:
        m_pTelnet->write(direction[0], 4);
        return;
    case Qt::Key_End:
        m_pTelnet->write(direction[1], 4);
        return;
    case Qt::Key_PageUp:
        m_pTelnet->write(direction[2], 4);
        return;
    case Qt::Key_PageDown:
        m_pTelnet->write(direction[3], 4);
        return;
    case Qt::Key_Up:
        m_pTelnet->write(direction[4], 3);
        return;
    case Qt::Key_Down:
        m_pTelnet->write(direction[5], 3);
        return;
    case Qt::Key_Left:
        m_pTelnet->write(direction[6], 3);
        return;
    case Qt::Key_Right:
        m_pTelnet->write(direction[7], 3);
        return;
    case Qt::Key_Delete: // stupid
        m_pTelnet->write("\x1b[3~", 4);
        return;
    default:
        break;
    }


    if (e->text().length()) {
        QByteArray cstrTmp = unicode2bbs(e->text());
        m_pTelnet->write(cstrTmp, cstrTmp.length());
    }
}

//connect slot
void Window::connectHost()
{
    if (m_hostInfo == NULL) {
        if (m_param.m_nProtocolType == 0)
            m_hostInfo = new TelnetInfo(m_param.m_strAddr , m_param.m_uPort);
        else {
#ifndef SSH_ENABLED
            m_hostInfo = new TelnetInfo(m_param.m_strAddr , m_param.m_uPort);
#else
            SSHInfo * sshInfo = new SSHInfo(m_param.m_strAddr , m_param.m_uPort);
            if (m_param.m_bAutoLogin) {
                sshInfo->setUserName(m_param.m_strUser);
                sshInfo->setPassword(m_param.m_strPasswd);
            }
            m_hostInfo = sshInfo;
#endif
        }
    }
    m_hostInfo->setTermType(m_param.m_strTerm);

    m_pTelnet->setProxy(m_param.m_nProxyType, m_param.m_bAuth,
                        m_param.m_strProxyHost, m_param.m_uProxyPort,
                        m_param.m_strProxyUser, m_param.m_strProxyPasswd);

    m_pTelnet->connectHost(m_hostInfo);
}
/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           Telnet State                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
//read slot
void Window::readReady(int size)
{
//  qDebug("readReady");
    //read it
    char * str = new char[size+1];
    m_pTelnet->read(str, size);
    str[size] = '\0';

    int raw_size = m_pTelnet->raw_len();
    char * raw_str = new char[raw_size];
    m_pTelnet->read_raw(raw_str, raw_size);

    // read raw buffer
    m_pZmodem->ZmodemRcv((uchar *)raw_str, raw_size, &(m_pZmodem->info));

    if (m_pZmodem->transferstate == NoTransfer) {
        //decode
        m_pDecode->decode(str, size);
        if (m_pDecode->bellReceive()) // Better way to determing the message?
            m_bMessage = true;
    }

    //delete the buf
    delete []str;
    delete []raw_str;

    if (m_pZmodem->transferstate == TransferStop)
        m_pZmodem->transferstate = NoTransfer;

    m_updateTimer->start(0);

}


void Window::ZmodemState(int type, int value, const QString& msg)
{
    QString status = m_codec->toUnicode(msg.toLatin1());
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        m_scriptHelper->setAccepted(false);
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onZmodemState");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << type << value << status);
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onZmodemState is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    QString strMsg;
    //to be completed
    switch (type) {
    case    RcvByteCount:
        m_pZmDialog->setProgress(value);
        break;
    case    SndByteCount:
        m_pZmDialog->setProgress(value);
        break;
    case    RcvTimeout:
        /* receiver did not respond, aborting */
        strMsg = QString("time out!");
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    SndTimeout:
        /* value is # of consecutive send timeouts */
        strMsg = QString("time out after trying %1 times").arg(value);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    RmtCancel:
        /* remote end has cancelled */
        strMsg = QString("canceled by remote peer %1").arg(status);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    ProtocolErr:
        /* protocol error has occurred, val=hdr */
        strMsg = QString("unhandled header %1 at state %2").arg(value).arg(status);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    RemoteMessage:
        /* message from remote end */
        strMsg = QString("msg from remote peer: %1").arg(status);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    DataErr:
        /* data error, val=error count */
        strMsg = QString("data errors %1").arg(value);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    FileErr:
        /* error writing file, val=errno */
        strMsg = QString("falied to write file");
        m_pZmDialog->addErrorLog(strMsg);
        break;
    case    FileBegin:
        /* file transfer begins, str=name */
//                qWarning("starting file %s", status);
        m_pZmDialog->setFileInfo(status, value);
        m_pZmDialog->setProgress(0);
        m_pZmDialog->clearErrorLog();
        m_pZmDialog->show();
        m_pZmDialog->setModal(true);
        break;
    case    FileEnd:
        /* file transfer ends, str=name */
//                qWarning("finishing file %s", status);
        m_pZmDialog->hide();
        break;
    case    FileSkip:
        /* file being skipped, str=name */
        strMsg = QString("skipping file %1").arg(status);
        m_pZmDialog->addErrorLog(strMsg);
        break;
    }

}

// telnet state slot
void Window::TelnetState(int state)
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        m_scriptHelper->setAccepted(false);
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onTelnetState");
        if (func.isFunction()) {
            func.call(QScriptValue(), QScriptValueList() << state);
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onTelnetState is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    switch (state) {
    case TSRESOLVING:
        //statusBar()->message( tr("resolving host name") );
        m_pMessage->display(tr("resolving host name"));
        break;
    case TSHOSTFOUND:
        //statusBar()->message( tr("host found") );
        m_pMessage->display(tr("host found"));
        break;
    case TSHOSTNOTFOUND:
        //statusBar()->message( tr("host not found") );
        m_pMessage->display(tr("host not found"));
        connectionClosed();
        break;
    case TSCONNECTING:
        //statusBar()->message( tr("connecting...") );
        m_pMessage->display(tr("connecting..."));
        break;
    case TSHOSTCONNECTED:
        //statusBar()->message( tr("connected") );
        m_pMessage->display(tr("connected"));
        m_bConnected = true;
        m_pFrame->updateMenuToolBar();
        if (m_param.m_bAutoLogin)
            m_bDoingLogin = true;
        break;
    case TSPROXYCONNECTED:
        //statusBar()->message( tr("connected to proxy" ) );
        m_pMessage->display(tr("connected to proxy"));
        break;
    case TSPROXYAUTH:
        //statusBar()->message( tr("proxy authentation") );
        m_pMessage->display(tr("proxy authentation"));
        break;
    case TSPROXYFAIL:
        //statusBar()->message( tr("proxy failed") );
        m_pMessage->display(tr("proxy failed"));
        disconnect();
        break;
    case TSREFUSED:
        //statusBar()->message( tr("connection refused") );
        m_pMessage->display(tr("connection refused"));
        connectionClosed();
        break;
    case TSREADERROR:
        //statusBar()->message( tr("error when reading from server") );
        m_pMessage->display(tr("error when reading from server"), PageViewMessage::Error);
        disconnect();
        break;
    case TSCLOSED:
        //statusBar()->message( tr("connection closed") );
        m_pMessage->display(tr("connection closed"));
        connectionClosed();
        if (m_param.m_bReconnect && m_bReconnect)
            reconnectProcess();
        break;
    case TSCLOSEFINISH:
        //statusBar()->message( tr("connection close finished") );
        m_pMessage->display(tr("connection close finished"));
        //connectionClosed();
        break;
    case TSCONNECTVIAPROXY:
        //statusBar()->message( tr("connect to host via proxy") );
        m_pMessage->display(tr("connect to host via proxy"));
        break;
    case TSEGETHOSTBYNAME:
        //statusBar()->message( tr("error in gethostbyname") );
        m_pMessage->display(tr("error in gethostbyname"), PageViewMessage::Error);
        connectionClosed();
        break;
    case TSEINIWINSOCK:
        //statusBar()->message( tr("error in startup winsock") );
        m_pMessage->display(tr("error in startup winsock"), PageViewMessage::Error);
        connectionClosed();
        break;
    case TSERROR:
        //statusBar()->message( tr("error in connection") );
        m_pMessage->display(tr("error in connection"), PageViewMessage::Error);
        disconnect();
        break;
    case TSPROXYERROR:
        //statusBar()->message( tr("eoor in proxy") );
        m_pMessage->display(tr("error in proxy"), PageViewMessage::Error);
        disconnect();
        break;
    case TSWRITED:
        // restart the idle timer
        if (m_idleTimer->isActive())
            m_idleTimer->stop();
        if (m_bAntiIdle)
            m_idleTimer->start(m_param.m_nMaxIdle*1000);
        m_bIdling = false;
        break;
    default:
        break;
    }

}
/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           UI Slots                                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */


void Window::copy()
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor,
                       parseString((const char *)m_param.m_strEscape.toLatin1())),
                       QClipboard::Clipboard);
    clipboard->setText(m_pBuffer->getSelectText(m_bCopyRect, m_bCopyColor,
                       parseString((const char *)m_param.m_strEscape.toLatin1())),
                       QClipboard::Selection);
}

void Window::paste()
{
    pasteHelper(true);
}

void Window::pasteHelper(bool clip)
{
    if (!m_bConnected)
        return;

    QClipboard *clipboard = QApplication::clipboard();
    QString strText;
    QString strTmp;
    QByteArray cstrText;

    if (clip)
        strTmp = clipboard->text(QClipboard::Clipboard);
    else
        strTmp = clipboard->text(QClipboard::Selection);
    strText = Global::instance()->convert(strTmp, (Global::Conversion)Global::instance()->clipConversion());

    if (!Global::instance()->escapeString().isEmpty())
        strText.replace(parseString(Global::instance()->escapeString().toLatin1()),
                         parseString((const char *)m_param.m_strEscape.toLatin1()));

    if (m_bWordWrap) {
        // insert '\n' as needed
        for (uint i = 0; i < strText.length(); i++) {
            uint j = i;
            uint k = 0, l = 0;
            while (strText.at(j) != QChar('\n') && j < strText.length()) {
                if (Global::instance()->m_pref.nWordWrap - (l - k) >= 0 &&
                        Global::instance()->m_pref.nWordWrap - (l - k) < 2) {
                    strText.insert(j, QChar('\n'));
                    k = l;
                    j++;
                    break;
                }
                // double byte or not
                if (strText.at(j).row() == '\0')
                    l++;
                else
                    l += 2;
                j++;
            }
            i = j;
        }
    }

    cstrText = m_codec->fromUnicode(strText);
    m_pTelnet->write(cstrText, cstrText.length());
}
void Window::copyArticle()
{
    //return;

    if (!m_bConnected)
        return;

    m_pDAThread = new DAThread(this);
    connect(m_pDAThread, SIGNAL(done(int)), this, SLOT(jobDone(int)));
    m_pDAThread->start();

}

void Window::appearance()
{
    addrDialog set(this, true);
    int fontSize = m_pScreen->fontSize();

    set.param = m_param;
    set.updateData(false);
    set.ui.tabWidget->setCurrentIndex(1);
    connect(set.ui.asciiFontComboBox, SIGNAL(currentFontChanged(const QFont &)), m_pScreen, SLOT(asciiFontChanged(const QFont &)));
    connect(set.ui.generalFontComboBox, SIGNAL(currentFontChanged(const QFont &)), m_pScreen, SLOT(generalFontChanged(const QFont &)));
    connect(set.ui.fontSizeSpinBox, SIGNAL(valueChanged(int)), m_pScreen, SLOT(fontSizeChanged(int)));

    if (set.exec() == 1) {
        m_param = set.param;
        m_bSetChanged = true;
        m_pScreen->setScheme();
    } else {
        m_pScreen->fontSizeChanged(fontSize);
    }
    m_pScreen->initFontMetrics();
    QResizeEvent* re = new QResizeEvent(m_pScreen->size(), m_pScreen->size());
    QApplication::postEvent(m_pScreen, re);
}

void Window::disconnect()
{
    m_pTelnet->close();
}

void Window::reconnect()
{
    if (!m_bConnected)
        connectHost();

}

void Window::showIP()
{
    QString country, city;
    QString url = m_pBBS->getIP();
    if (m_pIPLocation->getLocation(url, country, city)) {
        m_pMessage->display(m_codec->toUnicode((country + city).toLatin1()), PageViewMessage::Info, 100);
    }
}

void Window::refresh()
{
    //m_pScreen->repaint(true);
    m_pScreen->m_ePaintState = Screen::Show;
    m_pScreen->update();
}

void Window::showStatusBar(bool bShow)
{
    if (bShow)
        statusBar()->show();
    else
        statusBar()->hide();
}

void Window::debugConsole()
{
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
#endif
}

void Window::runScript(const QString & filename)
{
    QString file = filename;
    if (file.isEmpty()){
        // get the previous dir
        file= Global::instance()->getOpenFileName("Script Files (*.js *.txt)", this);
    }
    if (file.isEmpty())
        return;

    m_scriptHelper->loadScriptFile(file);
}

void Window::stopScript()
{
#ifdef SCRIPT_ENABLED
    m_scriptEngine->abortEvaluation();
#endif
}

void Window::viewMessages()
{
    msgDialog msg(this);

//    const char * size = Global::instance()->fileCfg()->getItemValue("global", "msgdialog").toString().toLatin1();
//    if (size != NULL) {
//        int x, y, cx, cy;
//        sscanf(size, "%d %d %d %d", &x, &y, &cx, &cy);
//        msg.resize(QSize(cx, cy));
//        msg.move(QPoint(x, y));
//    }

    msg.ui.msgBrowser->setPlainText(m_strMessage);
    msg.exec();

//    QString strSize = QString("%1 %2 %3 %4").arg(msg.x()).arg(msg.y()).arg(msg.width()).arg(msg.height());
//    Global::instance()->fileCfg()->setItemValue("global", "msgdialog", strSize);
//    Global::instance()->fileCfg()->save();

}

void Window::setting()
{
    addrDialog set(this, true);

    set.param = m_param;
    set.updateData(false);

    if (set.exec() == 1) {
        m_param = set.param;
        m_bSetChanged = true;
        if (m_param.m_bAutoFont) {
            m_pBuffer->setSize(m_param.m_nCol, m_param.m_nRow);
            m_pScreen->updateFont();
            m_pScreen->m_ePaintState = Screen::Show;
            m_pScreen->update();
        }
    }
}

void Window::antiIdle(bool isEnabled)
{
    m_bAntiIdle = isEnabled;
    // disabled
    if (!m_bAntiIdle && m_idleTimer->isActive())
        m_idleTimer->stop();
    // enabled
    if (m_bAntiIdle && !m_idleTimer->isActive())
        m_idleTimer->start(m_param.m_nMaxIdle*1000);
}

void Window::autoReply(bool isEnabled)
{
    m_bAutoReply = isEnabled;
    // disabled
    if (!m_bAutoReply && m_replyTimer->isActive())
        m_replyTimer->stop();
    // enabled
// if( m_bAutoReply && !m_replyTimer->isActive() )
//  m_replyTimer->start(m_param.m_nMaxIdle*1000/2);
}

void Window::connectionClosed()
{
    m_bConnected = false;

    if (m_idleTimer->isActive())
        m_idleTimer->stop();

    //statusBar()->message( tr("connection closed") );
    m_pMessage->display(tr("connection closed"));

    m_pFrame->updateMenuToolBar();

    setCursor(cursor[8]);

    QString strMsg = "";
    strMsg += "\n\n\n\r";
    strMsg += "\x1b[17C\x1b[0m===========================================\n\r";
    strMsg += "\x1b[17C Connection Closed, Press \x1b[1m\x1b[31;40mEnter\x1b[m\x1b[0m To Connect\n\r";
    strMsg += "\x1b[17C===========================================\n";

    m_pDecode->decode(strMsg.toLatin1(), strMsg.length());
    m_pScreen->m_ePaintState = Screen::NewData;
    m_pScreen->update();
}

void Window::doAutoLogin()
{
    if (!m_param.m_strPreLogin.isEmpty()) {
        QByteArray temp = parseString(m_param.m_strPreLogin.toLatin1());
        m_pTelnet->write((const char *)(temp), temp.length());
    }
    if (!m_param.m_strUser.isEmpty()) {
        QByteArray temp = m_param.m_strUser.toLocal8Bit();
        m_pTelnet->write((const char *)(temp), temp.length());
        char ch = CHAR_CR;
        m_pTelnet->write(&ch, 1);
    }
    if (!m_param.m_strPasswd.isEmpty()) {
        QByteArray temp = m_param.m_strPasswd.toLocal8Bit();
        m_pTelnet->write((const char *)(temp), temp.length());
        char ch = CHAR_CR;
        m_pTelnet->write(&ch, 1);
    }
    // smth ignore continous input, so sleep 1 sec :)
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    Sleep(1);
#else
    sleep(1);
#endif

    if (!m_param.m_strPostLogin.isEmpty()) {
        QByteArray temp = parseString(m_param.m_strPostLogin.toLatin1());
        m_pTelnet->write((const char *)(temp), temp.length());
    }
    m_bDoingLogin = false;
}

void Window::reconnectProcess()
{
    static int retry = 0;
    if (retry < m_param.m_nRetry || m_param.m_nRetry == -1) {
        if (m_param.m_nReconnectInterval <= 0)
            reconnect();
        else
            m_reconnectTimer->start(m_param.m_nReconnectInterval*1000);
        retry++;
    }
}
/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           Events                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void Window::jobDone(int e)
{
    if (e == DAE_FINISH) {
        articleDialog article(this);
        const char * size = Global::instance()->fileCfg()->getItemValue("global", "articledialog").toString().toLatin1().data();
        if (size != NULL) {
            int x, y, cx, cy;
            sscanf(size, "%d %d %d %d", &x, &y, &cx, &cy);
            article.resize(QSize(cx, cy));
            article.move(QPoint(x, y));
        }
        article.strArticle = m_pDAThread->strArticle;
        article.ui.textBrowser->setPlainText(article.strArticle);
        article.exec();
        QString strSize = QString("%1 %2 %3 %4").arg(article.x()).arg(article.y()).arg(article.width()).arg(article.height());
        Global::instance()->fileCfg()->setItemValue("global", "articledialog", strSize);
        Global::instance()->fileCfg()->save();
    } else if (e == DAE_TIMEOUT) {
        QMessageBox::warning(this, "timeout", "download article timeout, aborted");
    }
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           Aux Func                                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

QByteArray  Window::parseString(const QByteArray& cstr, int *len)
{
    QByteArray parsed = "";

    if (len != 0)
        *len = 0;

    for (uint i = 0; i < cstr.length(); i++) {
        if (cstr.at(i) == '^') {
            i++;
            if (i < cstr.length()) {
                parsed += CTRL(cstr.at(i));
                if (len != 0)
                    *len = *len + 1;
            }

        } else
            if (cstr.at(i) == '\\') {
                i++;
                if (i < cstr.length()) {
                    if (cstr.at(i) == 'n')
                        parsed += CHAR_CR;
                    else if (cstr.at(i) == 'r')
                        parsed += CHAR_LF;
                    else if (cstr.at(i) == 't')
                        parsed += CHAR_TAB;
                    if (len != 0)
                        *len = *len + 1;
                }
            } else {
                parsed += cstr.at(i);
                if (len != 0)
                    *len = *len + 1;
            }
    }

    return parsed;
}

void Window::replyMessage()
{
    if (m_replyTimer->isActive())
        m_replyTimer->stop();

    QByteArray cstrTmp = m_param.m_strReplyKey.toLocal8Bit();
    QByteArray cstr = parseString(cstrTmp.isEmpty() ? QByteArray("^Z") : cstrTmp);
    cstr += m_codec->fromUnicode(m_param.m_strAutoReply);

    cstr += '\n';
    m_pTelnet->write(cstr, cstr.length());
    m_pMessage->display(tr("You have messages"), PageViewMessage::Info, 0);
}

void Window::saveSetting()
{
    if (m_nAddrIndex == -1 || !m_bSetChanged)
        return;

    QMessageBox mb("QTerm",
                   tr("Setting changed do you want to save it?"),
                   QMessageBox::Warning,
                   QMessageBox::Yes | QMessageBox::Default,
                   QMessageBox::No  | QMessageBox::Escape ,
                   0, this);
    if (mb.exec() == QMessageBox::Yes) {
        Global::instance()->saveAddress(m_nAddrIndex, m_param);
    }
}

void Window::externInput(const QString & strText)
{
    QByteArray cstrText = unicode2bbs(strText);
    QByteArray cstrParsed = parseString(cstrText);
    m_pTelnet->write(cstrParsed, cstrParsed.length());

}

QByteArray Window::unicode2bbs(const QString& text)
{
    QString strTmp = Global::instance()->convert(text, Global::instance()->m_pref.XIM);
    return m_codec->fromUnicode(strTmp);
}

void Window::sendParsedString(const QString& str)
{
    int length = 0;
    QByteArray bbsText = unicode2bbs(str);
    QByteArray cstr = parseString(bbsText, &length);
    m_pTelnet->write(cstr, length);
}

void Window::setMouseMode(bool on)
{
    m_bMouseX11 = on;
}


void Window::initScript()
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL)
        m_scriptEngine->abortEvaluation();
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->detach();
#endif
    delete m_scriptEngine;
    delete m_scriptHelper;
    m_scriptEngine = new QScriptEngine(this);
    m_scriptHelper = new ScriptHelper(this, m_scriptEngine);

#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->attachTo(m_scriptEngine);
#endif

    QScriptValue scriptHelper = m_scriptEngine->newQObject(m_scriptHelper);
    m_scriptEngine->globalObject().setProperty("QTerm", scriptHelper);
    if (!m_param.m_bLoadScript)
        return;
    m_pBBS->setScript(m_scriptEngine, m_scriptHelper);
    m_scriptHelper->import(m_param.m_strScriptFile);
    QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("init");
    if (!func.isFunction()) {
        qDebug() << "init is not a function";
    }
    func.call();
#endif // SCRIPT_ENABLED
}

void Window::inputHandle(const QString & text)
{
    if (text.length() > 0) {
        QByteArray cstrTmp = unicode2bbs(text);
        m_pTelnet->write(cstrTmp, cstrTmp.length());
    }
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                           HTTP Func                                      */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void Window::openLink()
{
    Global::instance()->openUrl(m_pBBS->getUrl());
}

void Window::previewLink()
{
    getHttpHelper(m_pBBS->getUrl(), true);
}

void Window::copyLink()
{
    QString strUrl;
    strUrl = m_pBBS->getUrl();

    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(strUrl, QClipboard::Selection);
    clipboard->setText(strUrl, QClipboard::Clipboard);
}

void Window::saveLink()
{
    getHttpHelper(m_pBBS->getUrl(), false);
}

void Window::getHttpHelper(const QString& strUrl, bool bPreview)
{
    Http *pHttp = new Http(this);
    connect(pHttp, SIGNAL(done(QObject*)), this, SLOT(httpDone(QObject*)));
    connect(pHttp, SIGNAL(message(const QString &)), m_pMessage, SLOT(showText(const QString &)));
    pHttp->getLink(strUrl, bPreview);
}

void Window::httpDone(QObject *pHttp)
{
    pHttp->deleteLater();
}

void Window::showMessage(const QString & message, int type, int duration)
{
    m_pMessage->display(message, (PageViewMessage::Icon)type, duration);
}

QMenu * Window::popupMenu()
{
    return m_pMenu;
}

QMenu * Window::urlMenu()
{
    return m_pUrl;
}

void Window::updateWindow()
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
        m_scriptHelper->setAccepted(false);
        QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("onNewData");
        if (func.isFunction()) {
            func.call();
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onNewData is not a function");
        }
        if (m_scriptEngine->hasUncaughtException()) {
            QScriptValue exception = m_scriptEngine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

        if (m_bDoingLogin) {
            int n = m_pBuffer->caret().y();
            for (int y = n - 5;y < n + 5;y++) {
                y = qMax(0, y);
                TextLine *pTextLine = m_pBuffer->screen(y);
                if (pTextLine == NULL)
                    continue;
                QString str = pTextLine->getText();
                if (str.indexOf("guest") != -1 && str.indexOf("new") != -1) {
                    doAutoLogin();
                    break;
                }
            }
        }
        // page complete when caret at the right corner
        // this works for most but not for all
        TextLine * pTextLine = m_pBuffer->screen(m_pBuffer->line() - 1);

        QString strText = pTextLine->getText().replace(QRegExp("\\s+$"),"");
        if (m_pBuffer->caret().y() == m_pBuffer->line() - 1 &&
                m_pBuffer->caret().x() >= strText.length() - 1)
            m_wcWaiting.wakeAll();

        //QToolTip::remove(this, m_pScreen->mapToRect(m_rcUrl));

        // message received
        // 03/19/2003. the caret posion removed as a message judgement
        // because smth.org changed
        if (m_bMessage) {
            if (m_bBeep) {
#ifdef PHONON_ENABLED
                if (Global::instance()->m_pref.strPlayer.isEmpty()) {
                    m_pSound = new PhononSound(Global::instance()->m_pref.strWave, this);
                } else
#endif // PHONON_ENABLED
                {
                    m_pSound = new ExternalSound(Global::instance()->m_pref.strPlayer,
                                                 Global::instance()->m_pref.strWave, this);
                }
                if (m_pSound)
                    m_pSound->play();
                delete m_pSound;
                m_pSound = NULL;
            }
            if (Global::instance()->m_pref.bBlinkTab)
                m_tabTimer->start(500);

            QString strMsg = m_pBBS->getMessage();
            if (!strMsg.isEmpty())
                m_strMessage += strMsg + "\n\n";


            if (!isActiveWindow() || m_pFrame->wndmgr->activeWindow() != this)
            {
#ifdef DBUS_ENABLED
                if (DBus::instance()->notificationAvailable()) {
                    QList<DBus::Action> actions;
                    actions.append(DBus::Show_QTerm);
                    DBus::instance()->sendNotification("New Message in QTerm", strMsg, actions);
                } else
#endif //DBUS_ENABLED
                {
                    m_popWin->setText(strMsg);
                    m_popWin->popup();
                }
            }
        if (m_bAutoReply) {
#ifdef SCRIPT_ENABLED
            if (m_scriptEngine != NULL && m_param.m_bLoadScript) {
                m_scriptHelper->setAccepted(false);
                QScriptValue func = m_scriptEngine->globalObject().property("QTerm").property("autoReply");
                if (func.isFunction()) {
                    func.call();
                    if (m_scriptHelper->accepted()) {
                        return;
                    } else {
                        // TODO: save messages
                        if (m_bIdling)
                            replyMessage();
                        else
                            m_replyTimer->start(m_param.m_nMaxIdle*1000 / 2);
                    }
                } else {
                    qDebug("autoReply is not a function");
                }
                if (m_scriptEngine->hasUncaughtException()) {
                    QScriptValue exception = m_scriptEngine->uncaughtException();
                    qDebug() << "Exception: " << exception.toString();
                }
            }
#endif
        }
        //m_pFrame->buzz();
    }

    m_pBBS->setPageState();
    m_pBBS->updateSelectRect();
    // set page state
    m_pBBS->updateUrlList();
    //m_updateTimer->start(100);
    //refresh screen
    m_pScreen->m_ePaintState = Screen::NewData;
    m_pScreen->update();
    m_bMessage = false;
}

}
#include <qtermwindow.moc>
