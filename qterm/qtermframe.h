#ifndef QTERMFRAME_H
#define QTERMFRAME_H


#include <qmainwindow.h>


class QLineEdit;
class QToolBar;
class QToolButton;
class QWorkspace;
class QTabBar;
class QTermWndMgr;
class QTermWindow;
class QTermTimeLabel;
class QVBox;
class QFontDialog;
class QTermParam;
class QTermConfig;
class TrayIcon;

struct QTermPref
{
	int 	nXIM;	//	0--GBK	1--BIG5
	int 	nWordWrap;
	bool 	bSmartWW;
	bool	bWheel;
	bool	bWarn;
	bool	bBlinkTab;
	bool	bLogMsg;
	QString strHttp;
	int		nBeep;
	QString strWave;
	int		nMethod;
	QString strPlayer;
	bool	bUrl;
	bool	bAutoCopy;
	bool	bAA;
	bool	bTray;
};

class MTray : public QObject
{
	Q_OBJECT
public:
	MTray(const QString &icon, const QString &tip, QPopupMenu *popup, QObject *parent=0);
	~MTray();

	void setToolTip(const QString &);
	void setImage(const QImage &);

signals:
	void clicked(const QPoint &, int);
	void doubleClicked(const QPoint &);
	void closed();

public slots:
	void show();
	void hide();

private:
	TrayIcon *ti;
	QTimer *trayTimer;

	QPixmap makeIcon(const QImage &_in);
};


class QTermFrame : public QMainWindow
{
	Q_OBJECT
public:
	QTermFrame();
	~QTermFrame();

	void updateMenuToolBar();
	void enableMenuToolBar( bool );

	void popupFocusIn( QTermWindow * );
signals:
	void bossColor();
	void updateScroll();
	void updateStatusBar(bool);

protected slots:
	void dialogInput();
	void keyClicked(int);
	void toolBarPosChanged(QToolBar*);
	// Menu
	void addressBook();
	void quickLogin();
	void exitQTerm();

	void selectionChanged(int);
	void aboutQTerm();
	void homepage();
	void langSimplified();
	void langTraditional();
	void langEnglish();
	void defaultSetting();
	void preference();
	void runScript();
	void stopScript();
	
	// Toolbar
    void connectIt();
    void disconnect();
	void copy();
    void paste();
	void copyRect();
	void copyColor();
	void copyArticle();
 	void autoCopy();
	void wordWrap();
	void noEsc();
	void escEsc();
	void uEsc();
	void customEsc();
	void gbkCodec();
	void big5Codec();
	void hideScroll();
	void leftScroll();
	void rightScroll();
	void showSwitchBar();
	void showStatusBar();
	void font();
    void color();
    void refresh();
	void fullscreen();
	void bosscolor();
	void uiFont();
    void antiIdle();
    void autoReply();
    void setting();
	void viewMessages();
    void enableMouse();
    void beep();
	void reconnect();
	void keySetup();
	
	void themesMenuAboutToShow();
	void themesMenuActivated(int);
	void windowsMenuAboutToShow();
	void windowsMenuActivated(int);
	void connectMenuActivated(int);
	void popupConnectMenu();
	void connectMenuAboutToHide();
	
	void trayClicked(const QPoint &, int);
	void trayDoubleClicked();	
	void trayHide();
	void trayShow();
	void buildTrayMenu();

	void switchWin(int);
	void paintEvent( QPaintEvent * );
public:
	QTabBar *tabBar;
	QTermWndMgr * wndmgr;

	QTermPref	m_pref;
	bool m_bBossColor;
	
	QCString m_cstrEscape;

	int m_nClipCodec; // 0--GBK 1--BIG5

	bool m_bStatusBar;
	
	int m_nScrollPos;	// 0--hide 1--LEFT 2--RIGHT
protected:   
	//variables
	QWorkspace* ws;
	QTermTimeLabel *labelTime;
	QPopupMenu *windowsMenu;
	QPopupMenu *themesMenu;
	int sEng,sChs,sCht;
	QString theme;

//	QDockWindow * dock;
	QLineEdit * input;
	QToolBar * key;

	QPopupMenu * escapeMenu;
	QPopupMenu * langMenu;	
	QPopupMenu * connectMenu;

	QToolButton *connectButton, *disconnectButton,
				*editRect, *editColor,
				*specAnti, *specAuto, *specMouse, *specBeep, *specReconnect;

	QMenuBar * mainMenu;
	QToolBar *mdiconnectTools, *mdiTools;
	
	bool m_bFullScreen;
	bool m_bSwitchBar;

	MTray *tray;
	QPopupMenu *trayMenu;

	//function
	//QTermWindow * newWindow( const QTermParam& param, int index=-1 );
	void newWindow( const QTermParam& param, int index=-1 );
	void closeEvent(QCloseEvent * );
	void selectStyleMenu(int ,int );
	void iniSetting();
	void loadPref(QTermConfig *);
	void saveSetting();
	
	void addMainMenu();
	void addMainTool();

	void updateKeyToolBar();
	
	bool eventFilter(QObject*, QEvent *);

	QCString valueToString(bool, int, int, bool, int);
	void insertThemeItem(QString);
	void setUseDock(bool);
};

#endif	//QTERMFRAME_H

