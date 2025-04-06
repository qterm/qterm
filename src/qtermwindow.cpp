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
#include "keyboardtranslator.h"

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
#include <QRegularExpression>
#include <QFileDialog>
#include <QTabWidget>
#include <QStringList>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QtCore/QProcess>
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
        QString strTemp = pWin->m_pBuffer->screen(0)->getText().replace(QRegularExpression("\\s+$"),"");
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
                QString str1 = pWin->m_pBuffer->screen(j)->getText().replace(QRegularExpression("\\s+$"),"");
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
            strList += pWin->m_pBuffer->screen(i)->getText().replace(QRegularExpression("\\s+$"),"");

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
Window::Window(Frame * frame, Param param, const QString &uuid, QWidget * parent, const char * name, Qt::WindowFlags wflags)
        : WindowBase(parent, wflags), m_strMessage(), location()
{
	groupActions();

    m_pFrame = frame;
    m_param = param;
    m_strUuid = uuid;
    m_hostInfo = NULL;
    m_translator = NULL;
    QString pathLib = Global::instance()->pathLib();
    setMouseTracking(true);

#ifdef SCRIPT_ENABLED
    m_scriptEngine = NULL;
    m_scriptHelper = NULL;
#endif // SCRIPT_ENABLED
//init the textline list

	QString strTerm = m_param.m_mapParam["termtype"].toString();
	int nColumn = m_param.m_mapParam["column"].toInt();
	int nRow = m_param.m_mapParam["row"].toInt();
	int nScroll = m_param.m_mapParam["scroll"].toInt();

    m_codec = QTextCodec::codecForName(param.m_mapParam["bbscode"].toString().toLatin1());
    m_pBuffer = new Buffer(nRow, nColumn, nScroll);
    if (param.m_mapParam["protocol"] == 0)
        m_pTelnet = new Telnet(strTerm, nRow, nColumn, false);
    else {
#ifndef SSH_ENABLED
        QMessageBox::warning(this, "QTerm",
                             tr("SSH support is not compiled, QTerm can only use Telnet!"));
        m_pTelnet = new Telnet(strTerm, nRow, nColumn, false);
#else
        m_pTelnet = new Telnet(strTerm, nRow, nColumn, true);
#endif
    }
    connect(m_pBuffer, SIGNAL(windowSizeChanged(int, int)),
            m_pTelnet, SLOT(windowSizeChanged(int, int)));
    m_pZmDialog = new zmodemDialog(this);
	m_pZmodem = new Zmodem(this, m_pTelnet, m_codec, param.m_mapParam["protocol"].toInt());

    if (m_codec == 0) {
        qDebug("Fallback to GBK codec");
        m_codec = QTextCodec::codecForName("GBK");
    }

    m_pDecode = new Decode(m_pBuffer, m_codec);
    m_pBBS   = new BBS(m_pBuffer);
    m_pScreen = new Screen(this, m_pBuffer, &m_param, m_pBBS);

    m_pIPLocation = new IPLocation(pathLib);
    m_bCheckIP = m_pIPLocation->haveFile();
    m_pSound = NULL;

    setWidget(m_pScreen);

    connect(m_pFrame, SIGNAL(scrollChanged()), m_pScreen, SLOT(updateScrollBar()));
    connect(m_pScreen, SIGNAL(inputEvent(const QString &)), this, SLOT(inputHandle(const QString &)));
    connect(m_pZmodem, SIGNAL(ZmodemState(int, int, const QString&)),
            this, SLOT(ZmodemState(int, int, const QString&)));
    connect(m_pZmDialog, SIGNAL(canceled()), m_pZmodem, SLOT(zmodemCancel()));

    connect(m_pDecode, SIGNAL(mouseMode(bool)), this, SLOT(setMouseMode(bool)));

    m_popWin = new popWidget(this, m_pFrame);

    m_pScreen->osd()->display(tr("Not Connected"));

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
// timers
    m_idleTimer = new QTimer;
    connect(m_idleTimer, SIGNAL(timeout()), this, SLOT(idleProcess()));
    m_replyTimer = new QTimer;
    connect(m_replyTimer, SIGNAL(timeout()), this, SLOT(replyProcess()));
    m_reconnectTimer = new QTimer;
    connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnect()));
    m_updateTimer = new QTimer;
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateWindow()));


// initial varibles
    m_bColorCopy = false;
    m_bRectCopy = false;
    m_bAntiIdle = true;
	m_bAutoReply = m_param.m_mapParam["bautoreply"].toBool();
    m_bBeep  = !(
#if !defined(PHONON_ENABLED) && !defined(QMEDIAPLAYER_ENABLED)
                   Global::instance()->m_pref.strPlayer.isEmpty() ||
#endif // PHONON_ENABLED
                   Global::instance()->m_pref.strWave.isEmpty());
    m_bMouse = true;
    m_bWordWrap = false;
    m_bAutoCopy = true;
    m_bMessage = false;
	m_bReconnect = m_param.m_mapParam["reconnect"].toBool();

    m_pDAThread = 0;
    m_bConnected = false;
    m_bIdling = false;

    m_bMouseX11 = false;
    m_bMouseClicked = false;
    m_bDoingLogin = false;

    cursor[0] = QCursor(QPixmap(":/cursor/home.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[1] = QCursor(QPixmap(":/cursor/end.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[2] = QCursor(QPixmap(":/cursor/pageup.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[3] = QCursor(QPixmap(":/cursor/pagedown.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[4] = QCursor(QPixmap(":/cursor/prev.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[5] = QCursor(QPixmap(":/cursor/next.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[6] = QCursor(QPixmap(":/cursor/exit.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[7] = QCursor(QPixmap(":/cursor/hand.xpm").scaledToWidth(Global::instance()->m_pref.nCursorSize,Qt::SmoothTransformation));
    cursor[8] = Qt::IBeamCursor;
    cursor[9] = Qt::ArrowCursor;

    // the system wide script
    m_bPythonScriptLoaded = false;

    initScript();

    loadKeyboardTranslator(param.m_mapParam["keyboardprofile"].toString());

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
    delete m_updateTimer;

    delete m_pUrl;
    delete m_pScreen;
    delete m_reconnectTimer;
    delete m_pIPLocation;
    delete m_pSound;
    delete m_hostInfo;
}

//close event received
void Window::closeEvent(QCloseEvent * clse)
{
    if (m_bConnected && Global::instance()->m_pref.bWarn) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "QTerm",
                       tr("Connected,Do you still want to exit?"),
                       QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            m_pTelnet->close();
            clse->accept();
        } else
            clse->ignore();
    } else {
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
        return;
    }

    m_bIdling = true;
    // system script can handle that
#ifdef SCRIPT_ENABLED
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("antiIdle");
        if (func.isCallable()) {
            func.call();
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("antiIdle is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    // the default function
    int length;
    QByteArray cstr = parseString(m_param.m_mapParam["antiidlestring"].toString().toLocal8Bit(), &length);
    m_pTelnet->write(cstr, length);
}

void Window::replyProcess()
{
    // if AutoReply still enabled, then autoreply
    if (m_bAutoReply)
        replyMessage();
    else // else just stop the timer
        m_replyTimer->stop();
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
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint pt = me->pos();
        #else
        QPoint pt = me->position().toPoint();
        #endif
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isCallable()) {
            func.call(QJSValueList() << 3 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << pt.x() << pt.y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
}

void Window::mousePressEvent(QMouseEvent * me)
{
    // Left Button for selecting
    if (me->button()&Qt::LeftButton && !(me->modifiers())) {
        // clear the selected before
        if (m_ptSelStart != m_ptSelEnd) {
            m_pBuffer->clearSelect();
            m_pScreen->updateRegion();
        }

        // set the selecting flag
        m_bSelecting = true;
        m_ptSelStart = me->pos();
        m_ptSelEnd = m_ptSelStart;
    }

#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint pt = me->pos();
        #else
        QPoint pt = me->position().toPoint();
        #endif
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isCallable()) {
            func.call(QJSValueList() << 0 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << pt.x() << pt.y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
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
            #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            QPoint globalPos = me->globalPos();
            #else
            QPoint globalPos = me->globalPosition().toPoint();
            #endif
            if (!m_pBBS->getUrl().isEmpty()) // on Url
                m_pUrl->popup(globalPos);
            else
                m_pMenu->popup(globalPos);
            return;
        }
    }
    // Middle Button for paste
    if (me->button()&Qt::MiddleButton && !(me->modifiers())) {
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
            m_pBuffer->setSelect(m_pScreen->mapToChar(m_ptSelStart), m_pScreen->mapToChar(m_ptSelEnd), m_bRectCopy);
            m_pScreen->updateRegion();
        }
    }

#ifdef SCRIPT_ENABLED
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint pt = me->pos();
        #else
        QPoint pt = me->position().toPoint();
        #endif
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isCallable()) {
            func.call(QJSValueList() << 2 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << pt.x() << pt.y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    if (m_bMouse && m_bConnected) {
        // set cursor pos, repaint if state changed
        QRect rect_old, rect_new;
        if (m_pBBS->setCursorPos(m_pScreen->mapToChar(me->pos()), rect_old, rect_new)) {
            m_pScreen->repaint(m_pScreen->mapToRect(rect_new));
            m_pScreen->repaint(m_pScreen->mapToRect(rect_old));
        }
        // judge if URL
        QRect rcOld;
        QRect rcOld_IP;
        QRect rcUrl_IP = m_rcUrl;
        bool bUrl = false;
        if (Global::instance()->m_pref.bUrl) {
            if (m_pBBS->isIP(rcUrl_IP, rcOld_IP)) {
                if (rcUrl_IP != rcOld_IP && !m_pScreen->osd()->isVisible()) {
                    showIP();
                }
            } else {
                if (m_pScreen->osd()->isVisible() && m_pScreen->osd()->type() == PageViewMessage::IP)
                    m_pScreen->osd()->hide();
            }

            if (m_pBBS->isUrl(m_rcUrl, rcOld)) {
                setCursor(Qt::PointingHandCursor);
                bUrl = true;
            }
        }

        if (!bUrl) {
            int nCursorType = m_pBBS->getCursorType(m_pScreen->mapToChar(me->pos()));
            if (nCursorType <= 9 && nCursorType >= 0)
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
        m_pBuffer->setSelect(m_pScreen->mapToChar(m_ptSelStart), m_pScreen->mapToChar(m_ptSelEnd), m_bRectCopy);
        m_pScreen->updateRegion();
        if (m_bAutoCopy)
            on_actionCopy_triggered();
        m_bSelecting = false;
        return;
    }
    m_bSelecting = false;


#ifdef SCRIPT_ENABLED
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint pt = me->pos();
        #else
        QPoint pt = me->position().toPoint();
        #endif
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onMouseEvent");
        if (func.isCallable()) {
            func.call(QJSValueList() << 1 << (int) me->button() << (int) me->buttons() << (int) me->modifiers() << pt.x() << pt.y());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
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
        QString hint = tr("URL:");
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
                QByteArray cmd;
                cmd.append(cMenu);
                cmd.append(cr);
                m_pTelnet->write(cmd, cmd.size());
            }
            break;
        case 1:
            if (!rc.isEmpty()) {
                // scroll lines
                int n = rc.y() - m_pBuffer->caretY();
                QByteArray cmd;
                for(int i=0; i<abs(n); i++)
                    cmd.append(direction[n>0?5:4]);
                // don't forget to send a CHAR_CR at last to enter in
                cmd.append(cr);
                m_pTelnet->write(cmd, cmd.size());
            }
            break;
        default:
            break;
        }
    } else if (nCursorType == 8) {
        QPoint ptPos = m_pScreen->mapToChar(me->pos());

        int ny = ptPos.y() - m_pBuffer->caretY();
        QByteArray cmd;
        for(int i=0; i<abs(ny); i++)
            cmd.append(direction[ny>0?5:4]);

        int nx = ptPos.x() - m_pBuffer->caretX();
        for(int i=0; i<abs(nx); i++)
            cmd.append(direction[nx>0?7:6]);

        m_pTelnet->write(cmd, cmd.size());
    } else if (nCursorType == 9) {
        char cr = CHAR_CR;
        m_pTelnet->write(&cr, 1);
    }
}

void Window::wheelEvent(QWheelEvent *we)
{
#ifdef SCRIPT_ENABLED
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onWheelEvent");
        if (func.isCallable()) {
            #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
            QPoint pt = we->pos();
            int orientation = (int) we->orientation();
            #else
            QPoint pt = we->position().toPoint();
            int orientation = we->angleDelta().x() != 0 ? 1 : 2;
            #endif
            func.call(QJSValueList() << we->pixelDelta().y() << (int) we->buttons() << (int) we->modifiers() << orientation << pt.x() << pt.y() );
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onWheelEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
        if (Global::instance()->m_pref.bWheel) {
            int j = we->pixelDelta().y() > 0 ? 4 : 5;
            if (!(we->modifiers())) {
                if (Global::instance()->m_pref.bWheel && m_bConnected)
                    m_pTelnet->write(direction[j], sizeof(direction[j]));
            }
        }
        else {
            m_pScreen->scrollLine(-we->pixelDelta().y()/8/15);
        }
}

//keyboard input event
void Window::keyPressEvent(QKeyEvent * e)
{
#ifdef SCRIPT_ENABLED
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onKeyPressEvent");
        if (func.isCallable()) {
            func.call(QJSValueList() << e->key() << (int) e->modifiers() << e->text());
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onMouseEvent is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    if (!m_bConnected) {
        if (e->key() == Qt::Key_Return)
            reconnect();
        return;
    }

    // message replying
    if (m_replyTimer->isActive())
        m_replyTimer->stop();

    Qt::KeyboardModifiers modifiers = e->modifiers();
    KeyboardTranslator::States states = KeyboardTranslator::AnsiState;

    if ( m_translator )
    {
    KeyboardTranslator::Entry entry = m_translator->findEntry(
                                                e->key() ,
                                                modifiers,
                                                states );

        // send result to terminal
        QByteArray textToSend;

        // special handling for the Alt (aka. Meta) modifier.  pressing
        // Alt+[Character] results in Esc+[Character] being sent
        // (unless there is an entry defined for this particular combination
        //  in the keyboard modifier)
        bool wantsAltModifier = entry.modifiers() & entry.modifierMask() & Qt::AltModifier;
        bool wantsAnyModifier = entry.state() &
                                entry.stateMask() & KeyboardTranslator::AnyModifierState;

        if ( modifiers & Qt::AltModifier && !(wantsAltModifier || wantsAnyModifier)
             && !e->text().isEmpty() )
        {
            textToSend.prepend("\033");
        }

        if ( entry.command() != KeyboardTranslator::NoCommand )
        {
            if (entry.command() & KeyboardTranslator::EraseCommand)
                textToSend += "\x1b[3~";

            // TODO command handling
        }
        else if ( !entry.text().isEmpty() )
        {
            textToSend += unicode2bbs(entry.text(true,modifiers));
        }
        else
            textToSend += unicode2bbs(e->text());

        m_pTelnet->write(textToSend, textToSend.length());
    }

}

//connect slot
void Window::connectHost()
{
	int nProtocol = m_param.m_mapParam["protocol"].toInt();
	bool bAutoLogin = m_param.m_mapParam["autologin"].toBool();
	QString strAddr = m_param.m_mapParam["addr"].toString();
	int nPort = m_param.m_mapParam["port"].toInt();
	QString strSSHUser = m_param.m_mapParam["sshuser"].toString();
	QString strSSHPass = m_param.m_mapParam["sshpass"].toString();
	QString strTerm = m_param.m_mapParam["termtype"].toString();
	int nColumn = m_param.m_mapParam["column"].toInt();
	int nRow = m_param.m_mapParam["row"].toInt();

    if (m_hostInfo == NULL) {
        if (nProtocol == 0)
            m_hostInfo = new TelnetInfo(strAddr, nPort, this);
        else {
#ifndef SSH_ENABLED
            m_hostInfo = new TelnetInfo(strAddr, nPort, this);
#else
            SSHInfo * sshInfo = new SSHInfo(strAddr , nPort, this);
            if (strSSHUser.isEmpty()) {
                strSSHUser = m_param.m_mapParam["user"].toString();
                if (strSSHPass.isEmpty()) {
                    strSSHPass = m_param.m_mapParam["password"].toString();
                }
            }
            sshInfo->setUserName(strSSHUser);
            sshInfo->setPassword(strSSHPass);
            sshInfo->setPrivateKeyFile(m_param.m_mapParam["sshprivatekeyfile"].toString());
            sshInfo->setPassphrase(m_param.m_mapParam["sshpassphrase"].toString());
            sshInfo->setHostKey(m_param.m_mapParam["sshhostkey"].toString());
            m_hostInfo = sshInfo;
            connect(m_hostInfo, SIGNAL(hostKeyChanged(const QString &)), this, SLOT(updateHostKey(const QString &)));
#endif
        }
    }
    m_hostInfo->setTermInfo(strTerm, nColumn, nRow);

	m_pTelnet->setProxy(m_param.m_mapParam["proxytype"].toInt(),
						m_param.m_mapParam["proxyauth"].toBool(),
						m_param.m_mapParam["proxyaddr"].toString(), 
						m_param.m_mapParam["proxyport"].toInt(),
						m_param.m_mapParam["proxyuser"].toString(), 
						m_param.m_mapParam["proxypassword"].toString());

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
	if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onZmodemState");
        if (func.isCallable()) {
            func.call(QJSValueList() << type << value << status);
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onZmodemState is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
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
    if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onTelnetState");
        if (func.isCallable()) {
            func.call(QJSValueList() << state);
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onTelnetState is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
    switch (state) {
    case TSRESOLVING:
        m_pScreen->osd()->display(tr("resolving host name"));
        break;
    case TSHOSTFOUND:
        m_pScreen->osd()->display(tr("host found"));
        break;
    case TSHOSTNOTFOUND:
        m_pScreen->osd()->display(tr("host not found"));
        connectionClosed();
        break;
    case TSCONNECTING:
        m_pScreen->osd()->display(tr("connecting..."));
        break;
    case TSHOSTCONNECTED:
        m_pScreen->osd()->display(tr("connected"));
        m_bConnected = true;
        if (m_param.m_mapParam["autologin"].toBool())
            m_bDoingLogin = true;
        break;
    case TSPROXYCONNECTED:
        m_pScreen->osd()->display(tr("connected to proxy"));
        break;
    case TSPROXYAUTH:
        m_pScreen->osd()->display(tr("proxy authentation"));
        break;
    case TSPROXYFAIL:
        m_pScreen->osd()->display(tr("proxy failed"));
        on_actionDisconnect_triggered();
        break;
    case TSREFUSED:
        m_pScreen->osd()->display(tr("connection refused"));
        connectionClosed();
        break;
    case TSREADERROR:
        m_pScreen->osd()->display(tr("error when reading from server"), PageViewMessage::Error);
        on_actionDisconnect_triggered();
        break;
    case TSCLOSED:
        m_pScreen->osd()->display(tr("connection closed"));
        connectionClosed();
        if (m_param.m_mapParam["reconnect"].toBool() && m_bReconnect)
            reconnectProcess();
        break;
    case TSCLOSEFINISH:
        m_pScreen->osd()->display(tr("connection close finished"));
        //connectionClosed();
        break;
    case TSCONNECTVIAPROXY:
        m_pScreen->osd()->display(tr("connect to host via proxy"));
        break;
    case TSEGETHOSTBYNAME:
        m_pScreen->osd()->display(tr("error in gethostbyname"), PageViewMessage::Error);
        connectionClosed();
        break;
    case TSEINIWINSOCK:
        m_pScreen->osd()->display(tr("error in startup winsock"), PageViewMessage::Error);
        connectionClosed();
        break;
    case TSERROR:
        m_pScreen->osd()->display(tr("error in connection"), PageViewMessage::Error);
        on_actionDisconnect_triggered();
        break;
    case TSPROXYERROR:
        m_pScreen->osd()->display(tr("error in proxy"), PageViewMessage::Error);
        on_actionDisconnect_triggered();
        break;
    case TSWRITED:
        // restart the idle timer
        if (m_idleTimer->isActive())
            m_idleTimer->stop();
        if (m_bAntiIdle)
            m_idleTimer->start(m_param.m_mapParam["maxidle"].toInt()*1000);
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


void Window::on_actionCopy_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();

	QString strEscape = m_param.m_mapParam["escape"].toString();

    clipboard->setText(m_pBuffer->getSelectText(m_bRectCopy, m_bColorCopy,
                       parseString((const char *)strEscape.toLatin1())),
                       QClipboard::Clipboard);
    clipboard->setText(m_pBuffer->getSelectText(m_bRectCopy, m_bColorCopy,
                       parseString((const char *)strEscape.toLatin1())),
                       QClipboard::Selection);
}

void Window::on_actionPaste_triggered()
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
                         parseString((const char *)m_param.m_mapParam["escape"].toString().toLatin1()));

    if (m_bWordWrap) {
        // insert '\n' as needed
        for (int i = 0; i < strText.length(); i++) {
            int j = i;
            int k = 0, l = 0;
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
void Window::on_actionCopy_Article_triggered()
{
    //return;

    if (!m_bConnected)
        return;

#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onCopyArticle");
        if (func.isCallable()) {
            QJSValue text = func.call();
            if (m_scriptHelper->accepted()) {
                showArticle(text.toString());
                return;
            }
        } else {
            qDebug("onCopyArticle is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    m_pDAThread = new DAThread(this);
    connect(m_pDAThread, SIGNAL(done(int)), this, SLOT(jobDone(int)));
    m_pDAThread->start();

}

void Window::on_actionCurrent_Session_Setting_triggered()
{
    addrDialog set(this, true);

    Param backup = m_param;

    set.param = m_param;
    set.updateData(false);
    set.ui.tabWidget->setCurrentIndex(1);
    connect(set.ui.asciiFontComboBox, SIGNAL(currentFontChanged(const QFont &)), m_pScreen, SLOT(asciiFontChanged(const QFont &)));
    connect(set.ui.generalFontComboBox, SIGNAL(currentFontChanged(const QFont &)), m_pScreen, SLOT(generalFontChanged(const QFont &)));
    connect(set.ui.fontSizeSpinBox, SIGNAL(valueChanged(int)), m_pScreen, SLOT(fontSizeChanged(int)));
    connect(set.ui.schemeComboBox, SIGNAL(currentIndexChanged(int)), m_pScreen, SLOT(schemeChanged(int)));
    connect(set.ui.opacityHorizontalSlider, SIGNAL(valueChanged(int)),m_pScreen, SLOT(opacityChanged(int)));
    set.ui.asciiFontComboBox->setCurrentFont(m_pScreen->asciiFont());
    set.ui.generalFontComboBox->setCurrentFont(m_pScreen->generalFont());

    if (set.exec() == 1) {
        m_param = set.param;
        if (!m_strUuid.isEmpty()) {
            QDomDocument doc = Global::instance()->addrXml();
            Global::instance()->saveAddress(doc,m_strUuid, m_param);
            Global::instance()->saveAddressXml(doc);
        }
        loadKeyboardTranslator(m_param.m_mapParam["keyboardprofile"].toString());
    } else {
        m_param = backup;
    }
    m_pScreen->setScheme();
    m_pScreen->initFontMetrics();
    QResizeEvent* re = new QResizeEvent(m_pScreen->size(), m_pScreen->size());
    QApplication::postEvent(m_pScreen, re);
}

void Window::on_actionDisconnect_triggered()
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
    if (!m_bCheckIP) {
        m_pScreen->osd()->display(tr("IP database not found"), PageViewMessage::Warning, 0, PageViewMessage::IP);
        return;
    }
    QString country, city;
    QString url = m_pBBS->getIP();
    if (m_pIPLocation->getLocation(url, country, city)) {
        m_pScreen->osd()->display((country + city), PageViewMessage::Info, 0, PageViewMessage::IP);
    }
}

void Window::on_actionBoss_Color_toggled(bool boss)
{
	Global::instance()->setBossColor(boss);
	m_pScreen->bossColor();
}

void Window::on_actionRefresh_triggered()
{
    //m_pScreen->repaint(true);
    m_pScreen->m_ePaintState = Screen::Show;
    m_pScreen->update();
}

void Window::on_actionUnderline_toggled(bool underline)
{
	QString strEscape = m_param.m_mapParam["escape"].toString();
	if (underline)
		strEscape += "4m";
	else
		strEscape += "0m";
	QByteArray sequence = parseString(strEscape.toLocal8Bit());
	m_pTelnet->write(sequence,sequence.length());
}
void Window::on_actionBlink_toggled(bool blink)
{
	QString strEscape = m_param.m_mapParam["escape"].toString();
	if (blink)
		strEscape += "5m";
	else
		strEscape += "0m";
	QByteArray sequence = parseString(strEscape.toLocal8Bit());
	m_pTelnet->write(sequence,sequence.length());
}
void Window::on_actionPallete_triggered(const QVariant& data)
{
	if (data.isNull())
		return;
	int index = data.toInt() >> 4;
	int role  = data.toInt() & 0x0f;
	int color = index;
	QString strEscape = m_param.m_mapParam["escape"].toString();
	// highlight
	if (index > 7) {
		strEscape += "1;";
		color = index - 8;
	}
	// fg or bg
	if (role == 0) //fg
		strEscape += QString::number(30+color);
	else // bg
		strEscape += QString::number(40+color);
	strEscape += "m";
	// write to server
	QByteArray sequence = parseString(strEscape.toLocal8Bit());
	m_pTelnet->write(sequence,sequence.length());
}
void Window::on_actionSymbols_triggered(const QVariant& data)
{
	setFocus(); // steal back focus from symbol table
	QByteArray text = m_codec->fromUnicode(data.toString());
	m_pTelnet->write(text,text.length());
}

void Window::on_actionDebug_Console_triggered()
{
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
#else
	QMessageBox::information(this, "QTerm",
                             tr("You need to enable the script engine debugger to use this feature. \
								Please recompile QTerm with the debugger enabled (need Qt 4.5 or newer version)"));
#endif
}

void Window::on_actionRun_triggered()
{
	runScript();
}

void Window::on_actionStop_triggered()
{
#ifdef SCRIPT_ENABLED
    m_scriptEngine->setInterrupted(true);
#endif
}

void Window::on_actionReload_Script_triggered()
{
	initScript();
}

void Window::on_actionView_Message_triggered()
{
    msgDialog msg(this);
    msg.ui.msgBrowser->setPlainText(m_strMessage);
    msg.exec();
}

void Window::on_actionAnti_Idle_toggled(bool isEnabled)
{
    m_bAntiIdle = isEnabled;
    // disabled
    if (!m_bAntiIdle && m_idleTimer->isActive())
        m_idleTimer->stop();
    // enabled
    if (m_bAntiIdle && !m_idleTimer->isActive())
        m_idleTimer->start(m_param.m_mapParam["maxidle"].toInt()*1000);
}

void Window::on_actionAuto_Reply_toggled(bool isEnabled)
{
    m_bAutoReply = isEnabled;
    // disabled
    if (!m_bAutoReply && m_replyTimer->isActive())
        m_replyTimer->stop();
    // enabled
// if( m_bAutoReply && !m_replyTimer->isActive() )
//  m_replyTimer->start(m_param.m_mapParam["maxidle"].toInt()*1000/2);
}

void Window::connectionClosed()
{
    m_bConnected = false;

    if (m_idleTimer->isActive())
        m_idleTimer->stop();

    m_pScreen->osd()->display(tr("connection closed"));

    setCursor(cursor[9]);

    QString strMsg = "";
    strMsg += "\n\n\n\r";
    strMsg += "\x1b[17C\x1b[0m===========================================\n\r";
    strMsg += "\x1b[17C Connection Closed, Press \x1b[1m\x1b[31;40mEnter\x1b[m\x1b[0m To Connect\n\r";
    strMsg += "\x1b[17C===========================================\n";

    m_pDecode->decode(strMsg.toLatin1(), strMsg.length());
    m_pScreen->updateRegion();
}

void Window::doAutoLogin()
{
	QString strPreLogin = m_param.m_mapParam["prelogin"].toString();
	QString strUser = m_param.m_mapParam["user"].toString();
	QString strPasswd = m_param.m_mapParam["password"].toString();

	QString strPostLogin = m_param.m_mapParam["postlogin"].toString();

    if (!strPreLogin.isEmpty()) {
        QByteArray temp = parseString(strPreLogin.toLatin1());
        m_pTelnet->write((const char *)(temp), temp.length());
    }
    if (!strUser.isEmpty()) {
        QByteArray temp = strUser.toLocal8Bit();
        m_pTelnet->write((const char *)(temp), temp.length());
        char ch = CHAR_CR;
        m_pTelnet->write(&ch, 1);
    }
    if (!strPasswd.isEmpty()) {
        QByteArray temp = strPasswd.toLocal8Bit();
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

    if (!strPostLogin.isEmpty()) {
        QByteArray temp = parseString(strPostLogin.toLatin1());
        m_pTelnet->write((const char *)(temp), temp.length());
    }
    m_bDoingLogin = false;
}

void Window::reconnectProcess()
{
	int nRetry = m_param.m_mapParam["retrytimes"].toInt();
	int nReconnectInterval = m_param.m_mapParam["interval"].toInt();
    static int retry = 0;
    if (retry < nRetry || nRetry == -1) {
        if (nReconnectInterval <= 0)
            reconnect();
        else
            m_reconnectTimer->start(nReconnectInterval*1000);
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
        showArticle(m_pDAThread->strArticle);
    } else if (e == DAE_TIMEOUT) {
        QMessageBox::warning(this, "timeout", "download article timeout, aborted");
    }
}

void Window::showArticle(const QString text)
{
    if (text.isEmpty()) {
        return;
    }
    articleDialog article(this);
    article.restoreGeometry(Global::instance()->fileCfg()->getItemValue("global", "articledialog").toByteArray());
    article.strArticle = text;
    article.ui.textBrowser->setPlainText(article.strArticle);
    article.exec();
    Global::instance()->fileCfg()->setItemValue("global", "articledialog", article.saveGeometry());
    Global::instance()->fileCfg()->save();
}

void Window::updateHostKey(const QString & hostKey)
{
    m_param.m_mapParam["sshhostkey"] = hostKey;
    if (!m_strUuid.isEmpty()) {
        QDomDocument doc = Global::instance()->addrXml();
        Global::instance()->saveAddress(doc,m_strUuid, m_param);
        Global::instance()->saveAddressXml(doc);
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

    for (int i = 0; i < cstr.length(); i++) {
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

	QByteArray cstrTmp = m_param.m_mapParam["replykey"].toString().toLocal8Bit();
    QByteArray cstr = parseString(cstrTmp.isEmpty() ? QByteArray("^Z") : cstrTmp);
	cstr += m_codec->fromUnicode(m_param.m_mapParam["autoreply"].toString());

    cstr += '\n';
    m_pTelnet->write(cstr, cstr.length());
    m_pScreen->osd()->display(tr("You have messages"), PageViewMessage::Info, 0);
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

void Window::initScript()
{
#ifdef SCRIPT_ENABLED
    if (m_scriptEngine != NULL)
        m_scriptEngine->setInterrupted(true);
#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->detach();
#endif
    delete m_scriptEngine;
    delete m_scriptHelper;
    m_scriptEngine = new QQmlEngine(this);
    m_scriptHelper = new ScriptHelper(this, m_scriptEngine);

#ifdef SCRIPTTOOLS_ENABLED
    m_scriptDebugger->attachTo(m_scriptEngine);
#endif

    QJSValue scriptHelper = m_scriptEngine->newQObject(m_scriptHelper);
    m_scriptEngine->setObjectOwnership(m_scriptHelper, QJSEngine::CppOwnership);
    m_scriptEngine->globalObject().setProperty("QTerm", scriptHelper);
	if (!m_param.m_mapParam["loadscript"].toBool())
        return;
    m_pBBS->setScript(m_scriptEngine, m_scriptHelper);
    m_scriptHelper->loadScript(m_param.m_mapParam["scriptfile"].toString());
    QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("init");
    if (!func.isCallable()) {
        qDebug() << "init is not a function";
    }
    func.call();
#endif // SCRIPT_ENABLED
}

void Window::runScript(const QString & filename)
{
#ifdef SCRIPT_ENABLED
    QString file = filename;
    if (file.isEmpty()){
        // get the previous dir
        file= Global::instance()->getOpenFileName("Script Files (*.js *.txt)", this);
    }
    if (file.isEmpty())
        return;

    m_scriptHelper->loadScript(file);
#endif
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
    Http *pHttp = new Http(this, m_codec);
    connect(pHttp, SIGNAL(done(QObject*)), this, SLOT(httpDone(QObject*)));
    connect(pHttp, SIGNAL(message(const QString &)), m_pScreen->osd(), SLOT(showText(const QString &)));
    pHttp->getLink(strUrl, bPreview);
}

void Window::httpDone(QObject *pHttp)
{
    pHttp->deleteLater();
}

void Window::osdMessage(const QString & message, int type, int duration)
{
    m_pScreen->osd()->display(message, (PageViewMessage::Icon)type, duration);
}

void Window::showMessage(const QString & title, const QString & message, int duration)
{
#ifdef DBUS_ENABLED
    if (DBus::instance()->notificationAvailable()) {
        QList<DBus::Action> actions;
        actions.append(DBus::Show_QTerm);
        DBus::instance()->sendNotification(title, message, QImage(), actions);
    } else
#endif //DBUS_ENABLED
    {
        if (!m_pFrame->showMessage(title, message)) {
            m_popWin->setText(message);
            m_popWin->popup();
        }
    }
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
    if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
        m_scriptHelper->setAccepted(false);
        QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("onNewData");
        if (func.isCallable()) {
            func.call();
            if (m_scriptHelper->accepted()) {
                return;
            }
        } else {
            qDebug("onNewData is not a function");
        }
        if (m_scriptEngine->hasError()) {
            QJSValue exception = m_scriptEngine->catchError();
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

        QString strText = pTextLine->getText().replace(QRegularExpression("\\s+$"),"");
        if (m_pBuffer->caret().y() == m_pBuffer->line() - 1 &&
                m_pBuffer->caret().x() >= strText.length() - 1)
            m_wcWaiting.wakeAll();

        //QToolTip::remove(this, m_pScreen->mapToRect(m_rcUrl));

        // message received
        // 03/19/2003. the caret posion removed as a message judgement
        // because smth.org changed
        if (m_bMessage) {
            if (m_bBeep) {
#ifdef QMEDIAPLAYER_ENABLED
                if (Global::instance()->m_pref.strPlayer.isEmpty()) {
                    m_pSound = new QMediaPlayerSound(Global::instance()->m_pref.strWave, this);
                } else
#endif // QMEDIAPLAYER_ENABLED
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

            QString strMsg = m_pBBS->getMessage();
            if (!strMsg.isEmpty())
                m_strMessage += strMsg + "\n\n";


            if (!isActiveWindow()) 
            {
                showMessage("New Message in QTerm", strMsg, -1);
            }
        if (m_bAutoReply) {
#ifdef SCRIPT_ENABLED
            if (m_scriptEngine != NULL && m_param.m_mapParam["loadscript"].toBool()) {
                m_scriptHelper->setAccepted(false);
                QJSValue func = m_scriptEngine->globalObject().property("QTerm").property("autoReply");
                if (func.isCallable()) {
                    func.call();
                    if (m_scriptHelper->accepted()) {
                        return;
                    } else {
                        // TODO: save messages
                        if (m_bIdling)
                            replyMessage();
                        else
                            m_replyTimer->start(m_param.m_mapParam["maxidle"].toInt()*1000 / 2);
                    }
                } else {
                    qDebug("autoReply is not a function");
                }
                if (m_scriptEngine->hasError()) {
                    QJSValue exception = m_scriptEngine->catchError();
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
    m_pScreen->updateRegion();
    m_bMessage = false;
}

void Window::loadKeyboardTranslator(const QString & filename)
{
    //const QString& path = name + ".keytab";
    QFileInfo fi(filename);
    QString path;
    if (fi.exists()) {
        path = filename;
    } else {
        qWarning() << "Fallback to default keyboard layout";
#ifdef Q_OS_MACOS
        path = Global::instance()->pathLib() + "/keyboard_profiles/apple.keytab";
#else
        path = Global::instance()->pathLib() + "/keyboard_profiles/linux.keytab";
#endif
    }
    fi.setFile(path);
    QString name = fi.baseName();
    QFile source(path);
    if (name.isEmpty() || !source.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    // delete the current key tranlator
    delete m_translator;
    m_translator = NULL;
    // create a new key translator
    m_translator = new KeyboardTranslator(name);
    KeyboardTranslatorReader reader(&source);
    m_translator->setDescription( reader.description() );
    while ( reader.hasNextEntry() )
        m_translator->addEntry(reader.nextEntry());

    source.close();

    if ( reader.parseError() )
    {
        delete m_translator;
        m_translator = NULL;
        return;
    }
    qDebug() << "Keyboard translator:" << name << "loaded";
}

void Window :: groupActions() 
{
	listActions << "actionDisconnect"
        << "actionPrint" << "actionPrint_Preview"
		<< "actionRefresh"
		<< "actionPallete" << "actionUnderline" << "actionBlink" << "actionSymbols"
        << "actionCopy" << "actionPaste" 
        << "actionAuto_Copy" << "actionCopy_w_Color"
		<< "actionRectangle_Selection" << "actionPaste_w_Wordwrap"
		<< "actionAnti_Idle" << "actionAuto_Reply"
		<< "actionCopy_Article"
        << "actionView_Message" << "actionBoss_Color"
		<< "actionDebug_Console" << "actionRun" << "actionStop" << "actionReload_Script"
		<< "actionCurrent_Session_Setting";
	mapToggleStates["actionAuto_Copy"]    = &m_bAutoCopy;
	mapToggleStates["actionCopy_w_Color"] = &m_bColorCopy;
	mapToggleStates["actionRectangle_Selection"] = &m_bRectCopy;
	mapToggleStates["actionPast_w_Wordwrap"] = &m_bWordWrap;
	mapToggleStates["actionAnti_Idle"]  = &m_bAntiIdle;
	mapToggleStates["actionAuto_Reply"] = &m_bAutoReply;
}

}
#include <moc_qtermwindow.cpp>
