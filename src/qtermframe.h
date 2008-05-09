#ifndef QTERMFRAME_H
#define QTERMFRAME_H


// #include <q3mainwindow.h>
//Added by qt3to4:
#include "statusBar.h"
#include <QPixmap>
#include <QByteArray>
#include <QString>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QMainWindow>
#include <QEvent>
#include <QMenu>
#include <QSystemTrayIcon>

class QLineEdit;
// class Q3ToolBar;
class QToolButton;
class QMdiArea;
class QTabBar;
class QFontDialog;

namespace QTerm
{
class WndMgr;
class Window;
class QTermTimeLabel;
// class Q3VBox;
class Param;
class Config;
//class TrayIcon;
// class QTerm::statusBar;

struct Pref
{
	int 	nXIM;	//	0--GBK	1--BIG5
	int 	nWordWrap;
	bool	bWheel;
	bool	bWarn;
	bool	bBlinkTab;
	QString strHttp;
	int		nBeep;
	QString strWave;
	int		nMethod;
	QString strPlayer;
	bool	bUrl;
	bool	bAutoCopy;
	bool	bAA;
	bool	bTray;
	bool	bClearPool;
	QString strZmPath;
	QString strPoolPath;
	QString strImageViewer;
};
/*
class MTray : public QObject
{
	Q_OBJECT
public:
	MTray(const QString &icon, const QString &tip, QMenu *popup, QObject *parent=0);
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
*/

class Frame : public QMainWindow
{
	Q_OBJECT
public:
	Frame();
	~Frame();
	static Frame * instance(){return s_instance;}

	void updateMenuToolBar();
	void enableMenuToolBar( bool );

	void popupFocusIn( Window * );
	void buzz();
signals:
	void bossColor();
	void scrollChanged();
	void statusBarChanged(bool);

protected slots:
	void keyClicked(int);
	void toolBarPosChanged(QToolBar*);
	// Menu
	void addressBook();
	void quickLogin();
	void exitQTerm();

	void selectionChanged(int);
	void aboutQTerm();
	void homepage();
	void updateLang(QAction* );
	void defaultSetting();
	void preference();
	void runScript();
	void stopScript();
	
	// Toolbar
	void connectIt();
	void disconnect();
	void copy();
	void paste();
	void copyRect(bool);
	void copyColor(bool);
	void copyArticle();
	void autoCopy(bool);
	void wordWrap(bool);
	void updateESC(QAction* );
	void updateCodec(QAction* );
	void updateScroll(QAction* );
	void updateSwitchBar(bool);
	void updateStatusBar(bool);
	void font();
	void color();
	void refresh();
	void fullscreen();
	void bosscolor();
	void uiFont();
	void antiIdle(bool);
	void autoReply(bool);
	void setting();
	void viewMessages();
	void updateMouse(bool);
	void updateBeep(bool);
	void reconnect(bool);
	void keySetup();

	void viewImages();
	
	void initThemesMenu();
	void themesMenuActivated(QAction *);
	void windowsMenuAboutToShow();
	void windowsMenuActivated(int);
	void connectMenuActivated(int);
	void popupConnectMenu();
	void connectMenuAboutToHide();

	//void trayClicked(const QPoint &, int);
	//void trayDoubleClicked();	
	void trayActivated(QSystemTrayIcon::ActivationReason reason);
	void trayHide();
	void trayShow();
	void buildTrayMenu();

	void switchWin(int);
	void paintEvent( QPaintEvent * );
public:
	QTabBar *tabBar;
	WndMgr * wndmgr;

	Pref	m_pref;
	bool m_bBossColor;
	
	QString m_strEscape;

	int m_nClipCodec; // 0--GBK 1--BIG5

	bool m_bStatusBar;
	
	int m_nScrollPos;	// 0--hide 1--LEFT 2--RIGHT

	QMdiArea * m_MdiArea;
protected:   
	//variables
	QTermTimeLabel *labelTime;
	QMenu *windowsMenu;
	QMenu *themesMenu;
// 	int sEng,sChs,sCht;
	QString theme;

	QToolBar * key;

	QMenu * escapeMenu;
	QMenu * langMenu;	
	QMenu * connectMenu;
// 	File
	QAction * m_connectAction;
	QAction * m_disconnectAction;
	QAction * m_addressAction;
	QAction * m_quickConnectAction;
	QAction * m_exitAction;

// 	Edit
	QAction * m_copyAction;
	QAction * m_pasteAction;
	QAction * m_colorCopyAction;
	QAction * m_rectAction;
	QAction * m_autoCopyAction;
	QAction * m_wwrapAction;
	QAction * m_noescAction;
	QAction * m_escescAction;
	QAction * m_uescAction;
	QAction * m_customescAction;
	QAction * m_GBKAction;
	QAction * m_BIG5Action;

	QAction * m_fontAction;
	QAction * m_colorAction;
	QAction * m_refreshAction;
	QAction * m_engAction;
	QAction * m_chsAction;
	QAction * m_chtAction;
	QAction * m_uiFontAction;
	QAction * m_fullAction;
	QAction * m_bossAction;
	QAction * m_scrollHideAction;
	QAction * m_scrollLeftAction;
	QAction * m_scrollRightAction;
	QAction * m_statusAction;
	QAction * m_switchAction;

// 	View
	QAction * m_currentSessionAction;
	QAction * m_defaultAction;
	QAction * m_prefAction;
	QAction * m_copyArticleAction;
	QAction * m_antiIdleAction;
	QAction * m_autoReplyAction;
	QAction * m_viewMessageAction;
	QAction * m_beepAction;
	QAction * m_mouseAction;
	QAction * m_viewImageAction;

	QAction * m_scriptRunAction;
	QAction * m_scriptStopAction;

	QAction * m_aboutAction;
	QAction * m_homepageAction;

	QAction * m_reconnectAction;

	StatusBar * m_pStatusBar;

	QToolButton *connectButton;

	QMenuBar * mainMenu;
	QToolBar *mdiconnectTools, *mdiTools;
	
	bool m_bFullScreen;
	bool m_bSwitchBar;

	QSystemTrayIcon *tray;
	QMenu *trayMenu;

	//function
	//Window * newWindow( const Param& param, int index=-1 );
	void newWindow( const Param& param, int index=-1 );
	void closeEvent(QCloseEvent * );
	void selectStyleMenu(int ,int );
	void iniSetting();
	void initActions();
	void initShortcuts();
	void loadPref(Config *);
	void saveSetting();
	
	void addMainMenu();
	void addMainTool();

	void updateKeyToolBar();
	
	bool eventFilter(QObject*, QEvent *);

	QString valueToString(bool, int, int, bool, int);
	QAction * insertThemeItem(const QString & );
	void setUseDock(bool);
private:
	static Frame * s_instance;
};

} // namespace QTerm

#endif	//QTERMFRAME_H

