#ifndef QTERMFRAME_H
#define QTERMFRAME_H

#include "ui_mainframe.h"

#include "assistantclient.h"
#include "statusBar.h"
#include <QPixmap>
#include <QActionGroup>
#include <QByteArray>
#include <QString>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QMainWindow>
#include <QEvent>
#include <QMenu>
#include <QSystemTrayIcon>

class QLineEdit;
class QToolButton;
class QMdiArea;
class QTabBar;
class QFontDialog;

namespace QTerm
{
class Window;
class QTermTimeLabel;
class Param;
class Config;

class Frame : public QMainWindow, public Ui::Frame
{
    Q_OBJECT
public:
    Frame();
    ~Frame();
    static Frame * instance() {
        return s_instance;
    }
	QMenu * createPopupMenu();
    void popupFocusIn(Window * window = 0);
    void buzz();
    QMenu * genPopupMenu(QWidget * owner);
    bool showMessage(const QString & title, const QString & message, int millisecondsTimeoutHint = 10000);

public slots:
    bool confirmExitQTerm();
    void saveAndDisconnect();

signals:
    void scrollChanged();
    void statusBarChanged(bool);

protected slots:
	// custum key
    void on_actionKey_Setup_triggered();
    void keyClicked(int);
	// theme submenu
	void initThemeMenu();
    void themesMenuActivated(QAction *);
	// toolbar submenu
	void initToolbarMenu();
	// File
	void on_actionAddressBook_triggered();
	void on_actionQuick_Login_triggered();
	void on_actionPrint_triggered();
	// View
	void on_actionFont_triggered();
	void on_actionMenubar_toggled(bool);
	void on_actionStatusbar_toggled(bool);
	void on_actionFullscreen_triggered(bool);
	// BBS
	void on_actionImage_Viewer_triggered();
	// Options
	void on_actionDefault_Session_Setting_triggered();
	void on_actionPreference_triggered();
	void on_actionConfigure_Shortcuts_triggered();
	void on_actionConfigure_Toolbars_triggered();
    // Help
	void on_actionAbout_QTerm_triggered();
	void on_actionQTerm_Online_triggered();
	void on_actionAbout_Qt_triggered();
	void on_actionContents_triggered();

protected slots:
    // Action groups
    void connectIt();
    void updateESC(QAction*);
    void updateCodec(QAction*);
    void updateScroll(QAction*);
	void updateLang(QAction*);

    void windowsMenuAboutToShow();
    void windowsMenuActivated();
    void connectMenuAboutToShow();
    void connectMenuActivated(const QString &uuid);

    void closeWindowByIndex(int index);
	void windowActivated(QMdiSubWindow*);
    void windowClosed(QObject*);
    void actionsDispatcher(QAction*);

    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void trayHide();
    void trayShow();
    void buildTrayMenu();

    void switchWin(int);

    void slotShowQTerm();
	void palleteColorChanged(int index, int role);
	void characterSelected(QString str);

protected:
    //variables
    QMenu * connectMenu;
    QToolButton *connectButton;

	QMenu *trayMenu;
    QSystemTrayIcon *tray;

	QAction *actionPallete;
	QAction *actionSymbols;

	StatusBar *m_pStatusBar;

    //function
    void newWindow(const Param& param, const QString& uuid="");
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void selectStyleMenu(int , int);
    void iniSetting();

    void initShortcuts();

	void groupActions();

    void saveSetting();
    void saveShortcuts();
    void loadShortcuts();
    void saveToolbars();
    void loadToolbars();
    void loadSession();

    void updateKeyToolBar();

    QAction * insertThemeItem(const QString &);
    void setUseTray(bool);
    void updateTabBar();
private:
    static Frame * s_instance;  
    AssistantClient m_assistant;

	QStringList listBasicActions;
    QActionGroup* actionsExtra;
};

} // namespace QTerm

#endif //QTERMFRAME_H
