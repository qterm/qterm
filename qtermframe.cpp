#include "qterm.h"
#include "qtermframe.h"
#include "qtermwindowbase.h"
#include "qtermeditor.h"

#include "toolbardialog.h"

#include <QtGui>

QTermFrame :: QTermFrame()
{
	setWindowTitle( "QTerm "+QString(VERSION) );
	setWindowIcon(QIcon(":/pic/qterm_32x32.png"));

	workspace=new QWorkspace();
	setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget*)),
            this, SLOT(winActivated(QWidget*)));
    
    setupUi(this);
    createMenus();    
    groupActions();
    
    tabBar = new QTabBar();
    PagerBar->addWidget(tabBar);
    connect(tabBar, SIGNAL(currentChanged(int)),
            this, SLOT(tabActivated(int)));

	loadSettings();
}

QTermFrame :: ~QTermFrame()
{

}

/********************
******** EVENTS *****
*********************/

void QTermFrame :: closeEvent(QCloseEvent *ce)
{
    saveSettings();
}

/********************
******** SLOTS *****
*********************/
void QTermFrame :: winClosed(QTermWindowBase*w)
{
    int index=listWindow.indexOf(w);
    if(index!=-1)
    {
        listWindow.removeAt(index);
        tabBar->removeTab(index);
    }
    
    // if no window left, only show basic actions
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    if(listWindow.count()==0)
        foreach(QAction* action, actions)
            action->setVisible(listBasicActions.contains(action->objectName()));
}

void QTermFrame :: winActivated(QWidget * w)
{
    QTermWindowBase * wb=qobject_cast<QTermWindowBase*>(w);
    if(wb==0)
        return;
    int index = listWindow.indexOf(wb);
    tabBar->setCurrentIndex(index);

    // only show actions belonging to BasicActions or WindowActions
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    foreach(QAction* action, actions)
    {
        if(listBasicActions.contains(action->objectName())
            || wb->hasAction(action->objectName()))
            action->setVisible(true);
        else
            action->setVisible(false);
    }
}

void QTermFrame :: tabActivated(int index)
{
	QTermWindowBase * w = listWindow.at(index);
    workspace->setActiveWindow(w);
}

void QTermFrame :: on_actionStatusbar_toggled(bool checked)
{
    statusBar()->setVisible(checked);
}

void QTermFrame :: on_actionNew_ANSI_triggered()
{
    QTermEditor* editor = new QTermEditor();
    
    newWindow(editor);
}

void QTermFrame :: on_actionQuit_triggered()
{
    close();
}
void QTermFrame :: on_actionConfigure_Toolbars_triggered()
{
    toolbarDialog td(this);
    td.exec();
}
void QTermFrame :: on_actionWhat_s_this_triggered()
{
    QWhatsThis:: enterWhatsThisMode();
}

void QTermFrame :: on_actionAbout_QTerm_triggered()
{
    QApplication::aboutQt();
}

void QTermFrame :: on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void QTermFrame :: actionsDispatcher(QAction* action)
{
    QTermWindowBase * wb=qobject_cast<QTermWindowBase*>(workspace->activeWindow());
    if(wb==0)
        return;
    QString nameSlot="on_"+action->objectName()+"_triggered";
    if(wb->hasAction(action->objectName()))
    {
        bool ret=QMetaObject::invokeMethod(wb, 
                nameSlot.toLatin1().constData());
        if(!ret)
            qWarning("Failed to execute %s",
            nameSlot.toLatin1().constData());
    }
}

/********************
***** FUNCTIONS *****
*********************/

void QTermFrame :: newWindow(QTermWindowBase*w)
{
    // Do this before showing window, to receive activated singal.
    workspace->addWindow(w);
    listWindow.append(w);
    tabBar->addTab(w->windowTitle());
    connect(w, SIGNAL(windowClosed(QTermWindowBase*)),
            this, SLOT(winClosed(QTermWindowBase*)));

	if( workspace->windowList().isEmpty() )
		w->showMaximized();
	else
	    w->showNormal();
    
}
void QTermFrame :: saveToolbars()
{
    QSettings setting("qterm.cfg",QSettings::IniFormat);    
    setting.beginGroup("Toolbars");
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    QToolBar * toolbar;
    foreach (toolbar, toolbars)
    {
        if(toolbar==PagerBar)
            continue;
        QStringList listActions;
        foreach(QAction* action, toolbar->actions())
        {
            if(action->isSeparator())
                listActions+="Separator";
            else
                listActions+=action->objectName();
        }
        setting.setValue(toolbar->objectName(),listActions);
    }
    setting.setValue("ButtonStyle", int(toolButtonStyle()));
    setting.setValue("IconSize", iconSize());
    setting.endGroup();    
}

// populate Toolbars
void QTermFrame :: popToolbars()
{
    QSettings setting("qterm.cfg",QSettings::IniFormat);
/*
// This code actually works. It will load actions by name, but not
able to load "Seprator" as its not an action. I leave it for now to
remind me this is a good test. Maybe I will come back to this later.
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    QToolBar * toolbar;
    QList<QAction*> actions = findChildren<QAction*>();
    QAction *action;
    foreach(action, actions)
    {
        foreach (toolbar, toolbars)
        {
            setting.beginGroup(toolbar->objectName());
            if(setting.contains(action->objectName()))
            {
                toolbar->addAction(action);
                setting.endGroup();            
                break;
            }
            setting.endGroup();
        }
    }
*/
// This code simply stores all actions belonging to a toolbar under its own key
// identified by objectName
    setting.beginGroup("Toolbars");
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    QToolBar * toolbar;
    foreach (toolbar, toolbars)
    {
        if(toolbar==PagerBar)
            continue;
        QStringList actions=setting.value(toolbar->objectName()).toStringList();
        foreach(QString action, actions)
        {
            QAction* act;
            if(action=="Separator")
                toolbar->addSeparator();
            else if(act=findChild<QAction*>(action))
                toolbar->addAction(act);
        }
    }
    setToolButtonStyle(Qt::ToolButtonStyle(setting.value("ButtonStyle").toInt()));
    setIconSize(setting.value("IconSize").toSize());
}

void QTermFrame :: createMenus()
{
    // Menu: View->Toolbars
    menuToolbars = new QMenu("Toolbars");
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    QToolBar * toolbar;
    foreach (toolbar, toolbars)
        menuToolbars->addAction(toolbar->toggleViewAction());
    
    menuView->insertMenu(actionStatusbar,menuToolbars);
}

void QTermFrame :: loadSettings()
{
    popToolbars();
    QSettings setting("qterm.cfg", QSettings::IniFormat);
    setting.beginGroup("MainWindow");

    restoreState(setting.value("ToolBars").toByteArray());

    // FIXME: when ws=Qt::WindowMaximized|Qt::WindowMinimized,
    // cannot restore it to be maximized
    uint ws= setting.value("State").toUInt();
    setWindowState(Qt::WindowState(ws));
    if((ws & Qt::WindowMaximized) == 0x0)
    {
        resize(setting.value("Size").toSize());
        move(setting.value("Position").toPoint());
    }
    actionStatusbar->setChecked(true);
    statusBar()->setVisible(setting.value("StatusBar").toBool());
    setting.endGroup();

}
void QTermFrame :: saveSettings()
{
    saveToolbars();
    QSettings setting("qterm.cfg",QSettings::IniFormat);
    setting.beginGroup("MainWindow");
    setting.setValue("ToolBars", saveState());
    if((windowState() & Qt::WindowMaximized)==0x0)
    {
        setting.setValue("Position",pos());
        setting.setValue("Size", size());
    }
    setting.setValue("State", uint(windowState()));
    setting.setValue("StatusBar", statusBar()->isVisible());
    setting.endGroup();
}

void QTermFrame :: groupActions()
{
    listBasicActions << "actionNew_Console" << "actionQuick_Login"
                    << "actionAddressBook"  << "actionQuit"
                    << "actionNew_ANSI"     << "actionOpen_ANSI"
                    << "actionView_Message" << "actionStatusbar"
                    << "actionFullscreen"   << "actionManage_Favarites"
                    << "actionArticle_Manager" << "actionClipboard_Encoding"
                    << "actionDefault_Session_Setting"
                    << "actionPreference"
                    << "actionConfigure_Toolbars"
                    << "actionConfigure_Shortcuts"
                    << "actionRun"  << "actionStop" << "actionDebug_Console"
                    << "actionContents" <<"actionWhat_s_this"
                    << "actionAbout_QTerm" << "actionAbout_Qt";

    // show only basic actions and add others to actionGroup
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));

    actionGroup = new QActionGroup(this);
    connect(actionGroup, SIGNAL(triggered(QAction*)),
    this, SLOT(actionsDispatcher(QAction*)));

    foreach(QAction* action, actions)
        if(listBasicActions.contains(action->objectName()))
            action->setVisible(true);
        else
        {
            action->setVisible(false);
            actionGroup->addAction(action);
        }
}
