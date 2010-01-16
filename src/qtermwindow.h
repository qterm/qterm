#ifndef QTERMWINDOW_H
#define QTERMWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qtermparam.h"
#include "qtermconvert.h"
#include "qtermsound.h"
//Added by qt3to4:
#include <QMainWindow>
#include <QCursor>
#include <QThread>
//#include <QCustomEvent>
// #include <Q3PopupMenu>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
// #include <Q3CString>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QWaitCondition>
#include <QMutex>

class QProgressDialog;
class QTextCodec;
#ifdef SCRIPT_ENABLED
class QScriptEngine;
#ifdef SCRIPTTOOLS_ENABLED
class QScriptEngineDebugger;
#endif
#endif

namespace QTerm
{
class TextLine;
class Screen;
class Telnet;
class Decode;
class Buffer;
class Frame;
class BBS;
class popWidget;
class Zmodem;
class Window;
class zmodemDialog;
class Http;
class IPLocation;
class PageViewMessage;
#ifdef SCRIPT_ENABLED
class ScriptHelper;
#endif
class HostInfo;
class KeyboardTranslator;
// thread copy article
class DAThread : public QThread
{
    Q_OBJECT
public:
    DAThread(Window*);
    ~DAThread();

    virtual void run();
    QString strArticle;
    QMutex mutex;
signals:
    void done(int);
private:
    Window *pWin;
};

class Window: public QMainWindow
{
    Q_OBJECT
public:
    Window(Frame * frame, Param param, int addr = -1, QWidget* parent = 0,
           const char* name = 0, Qt::WFlags wflags = Qt::Window);
    ~Window();

    void connectHost();
    bool isConnected() {
        return m_bConnected;
    }

    int index() {
        return m_nAddrIndex;
    }

public slots:
    // ui
    void copy();
    void paste();
    void copyArticle();
    void setting();
    void runScript(const QString & filename="");
    void stopScript();
    void debugConsole();
    void reconnect();
    void sendParsedString(const QString &);
    void showIP();
    void inputHandle(const QString & text);
public:
    void disconnect();
    void refresh();
    void viewMessages();
    void autoReply(bool);
    void antiIdle(bool);

    void initScript();
    void externInput(const QString &);
    void getHttpHelper(const QString&, bool);
    void osdMessage(const QString &, int type, int duration);
    void showMessage(const QString & title, const QString & message, int duration = -1);
    Screen * screen()
    {
        return m_pScreen;
    }
    Zmodem * zmodem()
    {
        return m_pZmodem;
    }
    QMenu * popupMenu();
    QMenu * urlMenu();
    QPoint mousePos() const
    {
        return m_ptMouse;
    }

protected slots:
    // from Telnet
    void readReady(int);
    void TelnetState(int);
    void ZmodemState(int, int, const QString&);
    // timer
    void idleProcess();
    void replyProcess();
    void updateWindow();
    void blinkTab();

    //http menu
    void previewLink();
    void openLink();
    void copyLink();
    void saveLink();

    // httpDone
    void httpDone(QObject*);

    // decode
    void setMouseMode(bool);
    void jobDone(int);
    void showArticle(const QString text);

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

//   void customEvent( QCustomEvent * );
    /*
    void imStartEvent (QIMEvent *);
    void imComposeEvent (QIMEvent *);
    void imEndEvent (QIMEvent *);
    */
    void reconnectProcess();
    void connectionClosed();
    void doAutoLogin();
    void replyMessage();

    void pasteHelper(bool);
    QByteArray unicode2bbs(const QString&);

    QByteArray parseString(const QByteArray&, int *len = 0);

    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);

    void loadKeyboardTranslator(const QString & profile);

    Screen * m_pScreen;
    Decode * m_pDecode;
    QMenu *  m_pMenu;
    QMenu *  m_pUrl;
    static char direction[][5];
    QCursor cursor[9];
    bool m_bConnected;

    Convert m_converter;

    bool m_bMessage;
    QString m_strMessage;

    // mouse select
    QPoint m_ptSelStart, m_ptSelEnd;
    QPoint m_ptMouse;
    bool m_bSelecting;

    // timer
    bool m_bIdling;
    QTimer *m_idleTimer, *m_replyTimer, *m_tabTimer, *m_reconnectTimer, * m_ipTimer;
    QTimer * m_updateTimer;

    // address setting
    int m_nAddrIndex;

    bool m_bDoingLogin;
    bool m_bCheckIP;

    // url rect
    QRect m_rcUrl;

    //
    popWidget *m_popWin;

    // download article thread
    DAThread *m_pDAThread;

    // python
#ifdef HAVE_PYTHON
    PyObject *pModule, *pDict;
#endif
    bool m_bPythonScriptLoaded;

    // play sound
    Sound * m_pSound;
    Zmodem *m_pZmodem;

    zmodemDialog *m_pZmDialog;

    // Decode
    bool m_bMouseX11;

    //IP location
    QString location;
    IPLocation * m_pIPLocation;

    //osd
    PageViewMessage * m_pMessage;

    bool m_bMouseClicked;

    QTextCodec * m_codec;
    KeyboardTranslator * m_translator;
#ifdef SCRIPT_ENABLED
    QScriptEngine * m_scriptEngine;
    ScriptHelper * m_scriptHelper;
#ifdef SCRIPTTOOLS_ENABLED
    QScriptEngineDebugger * m_scriptDebugger;
#endif
#endif
public:
    Frame * m_pFrame;
    Buffer * m_pBuffer;
    Telnet * m_pTelnet;
    Param m_param;
    BBS * m_pBBS;
    HostInfo * m_hostInfo;
    // menu and toolbar state
    bool m_bCopyColor;
    bool m_bCopyRect;
    bool m_bAutoCopy;
    bool m_bWordWrap;
    bool m_bAntiIdle;
    bool m_bAutoReply;
    bool m_bBeep;
    bool m_bMouse;
    bool m_bReconnect;
    friend class Screen;

    QString m_strPythonError;

    QWaitCondition m_wcWaiting;

};

} // namespace QTerm

#endif //QTERMWINDOW_H

