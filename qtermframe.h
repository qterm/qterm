#ifndef QTERMFRAME_H
#define QTERMFRAME_H

#include <QtGui>
#include "ui_frame.h"

class QTermWindowBase;

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

    void menuToolbars_aboutToShow();
    void menuToolbars_triggered(QAction*);

	void tabActivated(int index);
    void winActivated(QWidget*);
    void winClosed(QTermWindowBase*);

private:
	QWorkspace* workspace;
    QMenu* menuToolbars;
    QTabBar* tabBar;
	QList<QTermWindowBase*> listWindow;
	QStringList listBasicActions;
};

#endif // QTERMFRAME_H
