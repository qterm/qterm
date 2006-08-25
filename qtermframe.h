#ifndef QTERMFRAME_H
#define QTERMFRAME_H

#include <QMainWindow>

#include "ui_frame.h"

class QTermWindowBase;
class QWorkspace;
class QTabBar;
class QActionGroup;

class QTermFrame : public QMainWindow, public Ui::Frame
{
	Q_OBJECT

public:
	QTermFrame();
	~QTermFrame();

public slots:

protected:
    void closeEvent(QCloseEvent *);
    
    void newWindow(QTermWindowBase*);

    void groupActions();
    void updateActions();
    void createMenus();
    void saveToolbars();
    void popToolbars();

	void loadSettings();
	void saveSettings();

protected slots:

    void on_actionQuit_triggered();

    void on_actionNew_ANSI_triggered();    

    void on_actionStatusbar_toggled(bool);

    void on_actionConfigure_Toolbars_triggered();

    void on_actionAbout_QTerm_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionWhat_s_this_triggered();

	void tabActivated(int index);
    void winActivated(QWidget*);
    void winClosed(QTermWindowBase*);
    void actionsDispatcher(QAction*);

private:
    QActionGroup* actionGroup;
	QWorkspace* workspace;
    QTabBar* tabBar;
	QList<QTermWindowBase*> listWindow;
	QStringList listBasicActions;
};

#endif // QTERMFRAME_H
