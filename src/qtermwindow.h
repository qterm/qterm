#ifndef QTERMWINDOW_H
#define QTERMWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include "qterm.h"
#ifdef HAVE_PYTHON
#include <Python.h>
#endif

// #include <q3mainwindow.h>
// #include <qcursor.h>
// 
// #include <qthread.h>

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
class QScriptEngine;

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
class Script;
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
	Window( Frame * frame,Param param, int addr=-1, QWidget* parent = 0, 
					const char* name = 0, Qt::WFlags wflags = Qt::Window );
	~Window();
	
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
	void autoReply(bool);
	void antiIdle(bool);

	void runScriptFile(const QString&);
	void externInput(const QByteArray&);
	QByteArray stripWhitespace(const QByteArray& cstr);
	void getHttpHelper(const QString&, bool);

protected slots:
	// from Telnet
	void readReady(int);
	void TelnetState(int);
	void ZmodemState(int,int,const QString&);
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
	void jobDone(int);

protected:
	void mouseDoubleClickEvent( QMouseEvent * );
	void mouseMoveEvent( QMouseEvent * );
	void mousePressEvent( QMouseEvent * );
	void mouseReleaseEvent( QMouseEvent * );
	void wheelEvent( QWheelEvent * );
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	bool event(QEvent * );
//  	void customEvent( QCustomEvent * );
	/*
	void imStartEvent (QIMEvent *);
	void imComposeEvent (QIMEvent *);
	void imEndEvent (QIMEvent *);
*/
	void reconnectProcess();
	void connectionClosed();
	void doAutoLogin();
	void saveSetting();	
	void replyMessage();
	
	void pasteHelper(bool);
	QByteArray unicode2bbs(const QString&);

	QByteArray parseString( const QByteArray&, int *len=0);
	QString fromBBSCodec(const QByteArray& cstr);

	#ifdef HAVE_PYTHON
	bool pythonCallback(const QString &, PyObject*);
	#endif
	int runPythonFile(const char*);
	void pythonMouseEvent(int, Qt::KeyboardModifier, Qt::KeyboardModifier, const QPoint&, int);
	
	void closeEvent ( QCloseEvent * );
	void keyPressEvent( QKeyEvent * );
	
	void sendMouseState(int, Qt::KeyboardModifier, Qt::KeyboardModifier, const QPoint&);
		
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
public:
	Frame * m_pFrame;
	Buffer * m_pBuffer;
	Telnet * m_pTelnet;
	Param m_param;
	BBS	* m_pBBS;
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

#endif	//QTERMWINDOW_H

