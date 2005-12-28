#ifndef QTERMWINDOW_H
#define QTERMWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include "qterm.h"
#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#include <qmainwindow.h>
#include <qcursor.h>

#include <qthread.h>

#include "qtermparam.h"
#include "qtermconvert.h"
#include "qtermsound.h"

class QTermTextLine;
class QTermScreen;
class QTermTelnet;
class QTermDecode;
class QTermBuffer;
class QTermFrame;
class QTermBBS;
class popWidget;
class QTermZmodem;
class QTermWindow;
class zmodemDialog;
class QProgressDialog;
class QTermHttp;
class QTermIPLocation;
class PageViewMessage;
class QProgressBar;

// thread copy article
class QTermDAThread : public QThread
{
public:
	QTermDAThread(QTermWindow*);
	~QTermDAThread();

	virtual void run();
	QCString cstrArticle;
private:
	QTermWindow *pWin;
};

class QTermWindow: public QMainWindow
{
	Q_OBJECT
public:
	QTermWindow( QTermFrame * frame,QTermParam param, int addr=-1, QWidget* parent = 0, 
					const char* name = 0, int wflags =0 );
	~QTermWindow();
	
	void connectHost();
	bool isConnected() { return m_bConnected; }

public slots:
	// ui
	void copy();
	void paste();
	void copyArticle();
	void font();
	void setting();
	void color();
	void runScript();
	void stopScript();
	void showStatusBar(bool);
	void reconnect();
	void sendParsedString(const char*);
	void showIP();
public:
	void disconnect();
	void refresh();
	void viewMessages();
	void autoReply();
	void antiIdle();

	void runScriptFile(const QCString&);
	void externInput(const QCString&);
	QCString stripWhitespace(const QCString& cstr);
	void getHttpHelper(const QString&, bool);

protected slots:
	// from QTermTelnet
	void readReady(int);
	void TelnetState(int);
	void ZmodemState(int,int,const QCString&);
  	// timer
	void idleProcess();
	void replyProcess();
	void blinkTab();
	void inputHandle(QString * text);

	//http menu
	void previewLink();
	void openLink();
	void copyLink();
	void saveLink();
	
	// httpDone
	void httpDone(QObject*);

	// decode
	void setMouseMode(bool);

protected:
	void mouseDoubleClickEvent( QMouseEvent * );
	void mouseMoveEvent( QMouseEvent * );
	void mousePressEvent( QMouseEvent * );
	void mouseReleaseEvent( QMouseEvent * );
	void wheelEvent( QWheelEvent * );
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	void customEvent( QCustomEvent * );
	void imStartEvent (QIMEvent *);
	void imComposeEvent (QIMEvent *);
	void imEndEvent (QIMEvent *);

	void reconnectProcess();
	void connectionClosed();
	void doAutoLogin();
	void saveSetting();	
	void replyMessage();
	
	void pasteHelper(bool);
	QCString unicode2bbs(const QString&);

	QCString parseString( const QCString&, int *len=0);
	QString fromBBSCodec(const QCString& cstr);

	#ifdef HAVE_PYTHON
	bool pythonCallback(const QString &, PyObject*);
	#endif
	int runPythonFile(const char*);
	void pythonMouseEvent(int, ButtonState, ButtonState, const QPoint&, int);
	
	void closeEvent ( QCloseEvent * );
	void keyPressEvent( QKeyEvent * );
	
	void sendMouseState(int, ButtonState, ButtonState, const QPoint&);
		
	QTermScreen * m_pScreen;
	QTermDecode * m_pDecode;
	QPopupMenu *  m_pMenu;
	QPopupMenu *  m_pUrl;
	static char direction[][5];
    QCursor cursor[9];
	bool m_bConnected;

	QTermConvert m_converter;
	
	bool m_bMessage;
	QString m_strMessage;

	// mouse select
	QPoint m_ptSelStart, m_ptSelEnd;	
	bool m_bSelecting;

	// timer
	bool m_bIdling;
	QTimer *m_idleTimer, *m_replyTimer, *m_tabTimer, *m_reconnectTimer, * m_ipTimer;
	
	// address setting
	bool m_bSetChanged;
	int m_nAddrIndex;

	bool m_bDoingLogin;
	bool m_bCheckIP;

	// url rect
	QRect m_rcUrl;

	// 
	popWidget *m_popWin;

	// download article thread
	QTermDAThread *m_pDAThread;

	// python 
#ifdef HAVE_PYTHON
	PyObject *pModule, *pDict;
#endif
	bool m_bPythonScriptLoaded;
	
	// play sound
	QTermSound * m_pSound;
	QTermZmodem *m_pZmodem;

	zmodemDialog *m_pZmDialog;

	// Decode
	bool m_bMouseX11;

	//IP location
	QString location;
	QTermIPLocation * m_pIPLocation;

	//osd
	PageViewMessage * m_pMessage;

	bool m_bMouseClicked;
public:
	QTermFrame * m_pFrame;
	QTermBuffer * m_pBuffer;
	QTermTelnet * m_pTelnet;
	QTermParam m_param;
	QTermBBS	* m_pBBS;
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
	friend class QTermScreen;
	
	QString m_strPythonError;

	QWaitCondition m_wcWaiting;

};

#endif	//QTERMWINDOW_H

