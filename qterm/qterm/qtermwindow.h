#ifndef QTERMWINDOW_H
#define QTERMWINDOW_H

#include <qmainwindow.h>
#include <qcursor.h>

#include <qthread.h>

#include "qtermparam.h"
#include "qtermconvert.h"

class QTermTextLine;
class QTermScreen;
class QTermTelnet;
class QTermDecode;
class QTermBuffer;
class QTermFrame;
class QTermBBS;
class popWidget;

class QTermThread : public QThread
{
public:
	QTermThread(QObject*, const QCString&);
	~QTermThread();

	void run();
	void stop();
private:
	QObject *receiver;
	QCString script;
};

class QTermWindow: public QMainWindow
{
	Q_OBJECT
public:
	QTermWindow( QTermFrame * frame,QTermParam param, int addr=-1, QWidget* parent = 0, const char* name = 0, int wflags =0 );
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
	QCString downloadArticle();

public:
	void disconnect();
	void refresh();
	void viewMessages();
	void autoReply();
	void antiIdle();

	void runScriptFile(const QCString&);
	void externInput(const QCString&);
protected slots:
	// from QTermTelnet
	void readReady(int);
	void TelnetState(int);
   	void idleProcess();
	void replyProcess();
	void blinkTab();
	void downTimer();

protected:
	void mouseMoveEvent( QMouseEvent * );
	void mousePressEvent( QMouseEvent * );
	void mouseReleaseEvent( QMouseEvent * );
	void wheelEvent( QWheelEvent * );
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );

	void reconnectProcess();
	void connectionClosed();
	void doAutoLogin();
	void saveSetting();	
	void replyMessage();
	QCString parseString( const QCString&, int *len=0);
	bool waitForNewPage(int);
	QString fromBBSCodec(const QCString& cstr);
	QCString stripWhitespace(const QCString& cstr);

	void closeEvent ( QCloseEvent * );
	void keyPressEvent( QKeyEvent * );
	QTermScreen * m_pScreen;
	QTermDecode * m_pDecode;
	QTermBBS	* m_pBBS;
	QPopupMenu *  m_pMenu;
	static char direction[][5];
    QCursor cursor[9];
	bool m_bConnected;
	QTermConvert m_converter;
	QTermParam m_param;
	
	// download article
	bool m_bTimeOut;
	bool m_bPageComplete;

	bool m_bMessage;
	QString m_strMessage;

	// mouse select
	QPoint m_ptSelStart, m_ptSelEnd;	
	bool m_bSelecting;

	// timer
	bool m_bIdling;
	QTimer *m_idleTimer, *m_replyTimer, *m_tabTimer, *m_downTimer, *m_reconnectTimer;
	
	// address setting
	bool m_bSetChanged;
	int m_nAddrIndex;

	bool m_bDoingLogin;

	// url rect
	QRect m_rcUrl;

	// 
	popWidget *m_popWin;

	QTermThread *m_pThread;

public:
	QTermFrame * m_pFrame;

	QTermBuffer * m_pBuffer;

	QTermTelnet * m_pTelnet;
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
};

#endif	//QTERMWINDOW_H

