/*******************************************************************************
FILENAME:      qtermframe.cpp
REVISION:      2001.10.4 first created.

AUTHOR:        kingson fiasco hooey
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qtermwindow.h"
#include "qtermframe.h"
#include "qtermtimelabel.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "qterm.h"
#include "qtermparam.h"
#include "qtermtoolbutton.h"
#include "statusBar.h"
#include "aboutdialog.h"
#include "addrdialog.h"
#include "prefdialog.h"
#include "quickdialog.h"
#include "keydialog.h"
#include "imageviewer.h"
#include "shortcutsdialog.h"
#include "toolbardialog.h"
#include "closedialog.h"

#ifdef DBUS_ENABLED
#include "dbus.h"
#endif //DBUS_ENABLED

#include <QPaintEvent>
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QByteArray>
#include <QCloseEvent>
#include <QTime>
#include <QSystemTrayIcon>
#include <QShortcut>
#include <QSignalMapper>

#include <QTextCodec>
#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMenu>
#include <QMenuBar>
#include <QTabBar>
#include <QMessageBox>
#include <QRegExp>
#include <QStyle>
#include <QStyleFactory>
#include <QFont>
#include <QFontDialog>
#include <QPalette>
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QLabel>
#include <QMovie>
#include <QLineEdit>
#include <QInputDialog>
#include <QStatusBar>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtDebug>

namespace QTerm
{

Frame* Frame::s_instance = 0;

//constructor
Frame::Frame()
        : QMainWindow(0)
{
    s_instance = this;
    setAttribute(Qt::WA_DeleteOnClose);

	setupUi(this);

    mdiArea->setViewMode(QMdiArea::TabbedView);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                this, SLOT(windowActivated(QMdiSubWindow*)) );
    setCentralWidget(mdiArea);
    

    tray = 0;
    trayMenu = 0;
    Global::instance()->setParent(this);

#ifdef DBUS_ENABLED
    DBus::instance()->setParent(this);
    connect(DBus::instance(), SIGNAL(showQTerm()), this, SLOT(slotShowQTerm()));
#endif //DBUS_ENABLED


//create a progress bar to notify the download process
    m_pStatusBar = new QTerm::StatusBar(statusBar(), "mainStatusBar");
    statusBar()->addWidget(m_pStatusBar, 0);

    initShortcuts();

//setup toolbar
    loadToolbars();

// add the custom defined key
    updateKeyToolBar();

//initialize all settings
    iniSetting();

    loadShortcuts();

    initThemeMenu();
	initToolbarMenu();

    connect(menuWindow, SIGNAL(aboutToShow()), 
            this, SLOT(windowsMenuAboutToShow()));

	connect(actionQuit, SIGNAL(triggered()), 
		    this, SLOT(confirmExitQTerm()));

	connectMenu = new QMenu(this);
    QToolButton * connectButton = qobject_cast<QToolButton *> (terminalToolBar->widgetForAction(actionConnect));
    connectButton->setObjectName("buttonConnect");

    connect(connectMenu, SIGNAL(aboutToShow()), this, SLOT(connectMenuAboutToShow()));
    connectButton->setMenu(connectMenu);
    connectButton->setPopupMode(QToolButton::InstantPopup);

	groupActions();

    installEventFilter(this);
}

//destructor
Frame::~Frame()
{
}

QMenu * Frame::createPopupMenu()
{
	QMenu * menuContext = QMainWindow::createPopupMenu();
	menuContext->addAction(actionMenubar);
	return menuContext;
}

/*********************************************************
 *      Initialize and Finalize Routines                 *
 *********************************************************/
void Frame::iniSetting()
{
    Global::instance()->loadConfig();

    restoreGeometry(Global::instance()->loadGeometry());
    restoreState(Global::instance()->loadState());
    if (Global::instance()->isFullScreen()) {
        actionFullscreen->setChecked(true);
        showFullScreen();
    }

    QString theme = Global::instance()->style();
    QStyle * style = QStyleFactory::create(theme);
    if (style)
        qApp->setStyle(style);

    //language
    if (Global::instance()->language() == Global::English)
        actionEnglish->setChecked(true);
    else if (Global::instance()->language() == Global::SimplifiedChinese)
        actionSimplified_Chinese->setChecked(true);
    else if (Global::instance()->language() == Global::TraditionalChinese)
        actionTraditional_Chinese->setChecked(true);
    else
        actionEnglish->setChecked(true);

    actionNone_Color->setChecked(true);

    switch (Global::instance()->clipConversion()) {
    case Global::No_Conversion:
        actionNone->setChecked(true);
        break;
    case Global::Simplified_To_Traditional:
        actionCHS_CHT->setChecked(true);
        break;
    case Global::Traditional_To_Simplified:
        actionCHT_CHS->setChecked(true);
        break;
    default:
        qDebug("ClipboardConversion: we should not be here");
        break;
    }

    if (Global::instance()->scrollPosition() == Global::Hide) {
        actionScroll_Hide->setChecked(true);
    } else if (Global::instance()->scrollPosition() == Global::Left) {
        actionScroll_Left->setChecked(true);
    } else {
        actionScroll_Right->setChecked(true);
    }


	actionStatusbar->setChecked(Global::instance()->showStatusBar());
	statusBar()->setVisible(Global::instance()->showStatusBar());
	
	actionMenubar->setChecked(Global::instance()->showMenuBar());
	menuBar()->setVisible(Global::instance()->showMenuBar());

    setUseTray(Global::instance()->m_pref.bTray);
}

void Frame::saveSetting()
{
    Global::instance()->saveGeometry(saveGeometry());
    Global::instance()->saveState(saveState());
    Global::instance()->saveConfig();
    saveShortcuts();
    saveToolbars();
}

/*********************************************************
 *                       SLOTS                           *
 *********************************************************/
//addressbook
void Frame::on_actionAddressBook_triggered()
{
    addrDialog addr(this, false);

    if (addr.exec() == 1) {
        newWindow(addr.param, addr.ui.nameListWidget->currentRow());
    }
}

//quicklogin
void Frame::on_actionQuick_Login_triggered()
{
    quickDialog quick(this);

    Global::instance()->loadAddress(-2, quick.param);

    if (quick.exec() == 1) {
        newWindow(quick.param);
    }
}

//quit
bool Frame::confirmExitQTerm()
{
    QList<QVariant> sites;
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    QStringList titleList;
    for (int i = 0; i < int(windows.count()); ++i) {
        if ((qobject_cast<Window *>(windows.at(i)))->isConnected()) {
            titleList << windows.at(i)->windowTitle();
            sites << qobject_cast<Window *>(windows.at(i))->index();
        }
    }
    if ((!titleList.isEmpty())&&(Global::instance()->m_pref.bWarn)) {
        CloseDialog close(this);
        close.setSiteList(titleList);
        if (close.exec() == 0) {
            return false;
        }
    }
    saveAndDisconnect();

    setUseTray(false);
    qApp->quit();
    // We should never reach here;
    return true;
}
void Frame::saveAndDisconnect()
{
    QList<QVariant> sites;
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    QStringList titleList;
    for (int i = 0; i < int(windows.count()); ++i) {
        if ((qobject_cast<Window *>(windows.at(i)))->isConnected()) {
            titleList << windows.at(i)->windowTitle();
            sites << qobject_cast<Window *>(windows.at(i))->index();
        }
    }

    Global::instance()->saveSession(sites);
    saveSetting();
    // clear zmodem and pool if needed
    if (Global::instance()->m_pref.bClearPool) {
        Global::instance()->clearDir(Global::instance()->m_pref.strZmPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath + "shadow-cache/");
    }
}

//create a new display window
void Frame::newWindow(const Param&  param, int index)
{
    Window * window = new Window(this, param, index, mdiArea, 0);
	window->setWindowTitle(param.m_mapParam["name"].toString());
    window->setWindowIcon(QIcon(":/pic/tabpad.png"));
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->showMaximized();
	mdiArea->addSubWindow(window);
	mdiArea->setActiveSubWindow(window);
	connect(window, SIGNAL(destroyed(QObject*)),
            this, SLOT(windowClosed(QObject*)));
}

//slot Help->About QTerm
void Frame::on_actionAbout_QTerm_triggered()
{
    aboutDialog about(this);
    about.exec();
}

//slot Help->About Qt
void Frame::on_actionAbout_Qt_triggered()
{
	qApp->aboutQt();
}

//slot Help->Homepage
void Frame::on_actionQTerm_Online_triggered()
{
    QString strUrl = "http://www.qterm.org";
    Global::instance()->openUrl(strUrl);
}

//slot Help->Contents
void Frame::on_actionContents_triggered()
{
	QString errorMessage;
    if (!m_assistant.showPage("qthelp://org.qterm/doc/index.html", &errorMessage))
        QMessageBox::warning(this, tr("Assistant"), errorMessage);
}

//slot Windows menu aboutToShow
void Frame::windowsMenuAboutToShow()
{
    menuWindow->clear();
    QAction * cascadeAction = menuWindow->addAction(tr("&Cascade"), mdiArea, SLOT(cascadeSubWindows()));
    QAction * tileAction = menuWindow->addAction(tr("&Tile"), mdiArea, SLOT(tileSubWindows()));
    if (mdiArea->subWindowList().isEmpty()) {
        cascadeAction->setEnabled(false);
        tileAction->setEnabled(false);
    }
    menuWindow->addSeparator();

#ifdef Q_OS_MACX
    // used to dock the programe
    if (isHidden())
        windowsMenu->addAction(tr("&Main Window"), this, SLOT(trayShow()));
#endif

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    QActionGroup * windowsGroup = new QActionGroup(this);
    windowsGroup->setExclusive(true);
    for (int i = 0; i < int(windows.count()); ++i) {
        QAction * idAction = menuWindow->addAction(windows.at(i)->windowTitle(),
                             this, SLOT(windowsMenuActivated()));
        idAction->setCheckable(true);
        idAction->setData(i);
        windowsGroup->addAction(idAction);
        idAction->setChecked(mdiArea->activeSubWindow() == windows.at(i));
    }

}
//slot activate the window correspond with the menu id
void Frame::windowsMenuActivated()
{
    QObject * action = sender();
    if (action == 0) {
        qDebug("Frame::windowsMenuActivated: No sender found");
        return;
    }
    int id = qobject_cast<QAction *>(action)->data().toInt();
    Window * w = qobject_cast<Window *>(mdiArea->subWindowList().at(id));
    if (w) {
        w->setFocus();
        w->showMaximized();
    } else {
        qDebug() << "Frame::windowsMenuActivated: No window found, id: " << id;
    }
}

void Frame::connectMenuAboutToShow()
{
    connectMenu->clear();

    connectMenu->addAction(tr("Quick Login"), this, SLOT(quickLogin()));
    connectMenu->addSeparator();

    QStringList listName = Global::instance()->loadNameList();
    QSignalMapper * connectMapper = new QSignalMapper(this);

    for (int i = 0; i < listName.count(); i++) {
        QAction * idAction = new QAction(listName[i], connectMenu);
        connectMenu->addAction(idAction);
        connect(idAction, SIGNAL(triggered()), connectMapper, SLOT(map()));
        connectMapper->setMapping(idAction, i);
    }
    connect(connectMapper, SIGNAL(mapped(int)), this, SLOT(connectMenuActivated(int)));
}

void Frame::connectMenuActivated(int id)
{
    Param param;
    // FIXME: don't know the relation with id and param setted by setItemParameter
    if (Global::instance()->loadAddress(id, param))
        newWindow(param, id);
}

/*********************************************************
 *              Window Switch Management                 *
 *********************************************************/
void Frame::windowClosed(QObject*w)
{
    QList<QMdiSubWindow *> listWindow = mdiArea->subWindowList();
    // if no window left, only show basic actions
    if(listWindow.count()==0) {
        QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
        foreach(QAction* action, actions)
            action->setVisible(listBasicActions.contains(action->objectName())
					|| action->actionGroup()!=actionsExtra);
    }
}

void Frame::windowActivated(QMdiSubWindow * w)
{
	WindowBase * wb=qobject_cast<WindowBase*>(w);
	if(wb==0)
        return;

    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    foreach(QAction* action, actions)
    {
		// only show actions belonging to BasicActions or WindowActions
		action->setVisible(listBasicActions.contains(action->objectName())
					|| action->actionGroup()!=actionsExtra
					|| wb->hasAction(action->objectName()));
		// update checkable actions status
		if (wb->hasAction(action->objectName()) && action->isCheckable())
			action->setChecked(wb->isActionChecked(action->objectName()));
	}
}

void Frame :: actionsDispatcher(QAction* action)
{
    WindowBase * wb=qobject_cast<WindowBase*>(mdiArea->activeSubWindow());
    if(wb==0)
        return;
    if (action->isCheckable())
    {
        QString nameSlot="on_"+action->objectName()+"_toggled";
        if(wb->hasAction(action->objectName()))
        {
            bool ret=QMetaObject::invokeMethod(wb, 
                nameSlot.toLatin1().constData(), Q_ARG(bool, action->isChecked()));
            if(!ret)
                qWarning("Failed to execute %s",
                nameSlot.toLatin1().constData());
        }
    }
    else
    {
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
}

void Frame::switchWin(int id)
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    if (windows.count() <= id && id < 200)
        return;

    if (id == 200) {
        mdiArea->activatePreviousSubWindow();
        return;
    }
    if (id == 201 || id == 202) {
        mdiArea->activateNextSubWindow();
        return;
    }

    QMdiSubWindow *w = windows.at(id);
    if (w != NULL) {
        mdiArea->setActiveSubWindow(w);
    }
}


void Frame::closeEvent(QCloseEvent * clse)
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    for (int i = 0; i < int(windows.count()); ++i) {

        if (((Window *)windows.at(i))->isConnected()) {
            if (Global::instance()->m_pref.bTray) {
                trayHide();
                clse->ignore();
                return;
            }
        }
    }
    if (confirmExitQTerm()) {
        clse->accept();
    } else {
        clse->ignore();
    }
}

void Frame::updateLang(QAction * action)
{
	if (action == actionEnglish)
		Global::instance()->setLanguage(Global::English);
	else if (action == actionSimplified_Chinese)
		Global::instance()->setLanguage(Global::SimplifiedChinese);
	else if (action == actionTraditional_Chinese)
		Global::instance()->setLanguage(Global::TraditionalChinese);
}

void Frame::connectIt()
{
    if (mdiArea->activeSubWindow() == NULL) {
        Param param;
        Global::instance()->loadAddress(-1, param);
        newWindow(param);
    } else
        if (!qobject_cast<Window *>(mdiArea->activeSubWindow())->isConnected())
            qobject_cast<Window *>(mdiArea->activeSubWindow())->reconnect();
}

void Frame::updateESC(QAction * action)
{
    if (action == actionNone_Color) {
        Global::instance()->setEscapeString("");
    } else if (action == actionESC_ESC) {
        Global::instance()->setEscapeString("^[^[[");
    } else if (action == actionCtrl_U) {
        Global::instance()->setEscapeString("^u[");
    } else if (action == actionCustom) {
        bool ok;
        QString strEsc = QInputDialog::getText(this, "define escape", "scape string *[", QLineEdit::Normal, Global::instance()->escapeString(), &ok);
        if (ok)
            Global::instance()->setEscapeString(strEsc);
    } else {
        qDebug("updateESC: should not be here");
    }
}

void Frame::updateCodec(QAction * action)
{
    if (action == actionNone) {
        Global::instance()->setClipConversion(Global::No_Conversion);
    } else if (action == actionCHS_CHT) {
        Global::instance()->setClipConversion(Global::Simplified_To_Traditional);
    } else if (action == actionCHT_CHS) {
        Global::instance()->setClipConversion(Global::Traditional_To_Simplified);
    } else {
        qDebug("updateCodec: should not be here");
    }
}


void Frame::on_actionFont_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, qApp->font());

    if (Global::instance()->m_pref.bAA)
        font.setStyleStrategy(QFont::PreferAntialias);

    if (ok == true) {
        qApp->setFont(font);
    }
}

void Frame::on_actionMenubar_toggled(bool show)
{
	Global::instance()->setMenuBar(show);
	menuBar()->setVisible(show);
}

void Frame::on_actionFullscreen_triggered(bool isFullScreen)
{
    Global::instance()->setFullScreen(isFullScreen);

    //TODO: add an item to the popup menu so we can go back to normal without touch the keyboard
    if (isFullScreen) {
        Global::instance()->saveGeometry(saveGeometry());
        Global::instance()->saveState(saveState());
        mainToolBar->hide();
        terminalToolBar->hide();
        keyToolBar->hide();
        showFullScreen();
    } else {
        restoreGeometry(Global::instance()->loadGeometry());
        restoreState(Global::instance()->loadState());
        emit scrollChanged();
        showNormal();
    }

    actionFullscreen->setChecked(isFullScreen);

}

void Frame::initToolbarMenu()
{
	menuToolbar->clear();
	foreach(QToolBar *toolbar, findChildren<QToolBar *>())
		menuToolbar->addAction(toolbar->toggleViewAction());
}

void Frame::initThemeMenu()
{
    menuTheme->clear();
    QActionGroup * themesGroup = new QActionGroup(this);
    QStringList styles = QStyleFactory::keys();
    for (QStringList::ConstIterator it = styles.begin(); it != styles.end(); it++)
        themesGroup->addAction(insertThemeItem(*it));
    connect(themesGroup, SIGNAL(triggered(QAction*)), 
		this, SLOT(themesMenuActivated(QAction*)));
}
QAction * Frame::insertThemeItem(const QString & themeitem)
{
    QAction * idAction = new QAction(themeitem, this);
    menuTheme->addAction(idAction);
    idAction->setCheckable(true);
	idAction->setChecked(
        themeitem.compare(qApp->style()->objectName(), Qt::CaseInsensitive)==0);
    return idAction;
}
void Frame::themesMenuActivated(QAction * action)
{
    QString theme = action->text();
    QStyle * style = QStyleFactory::create(theme);
    if (style) {
        qApp->setStyle(style);
        Global::instance()->setStyle(theme);
    }
}

void Frame::updateScroll(QAction * action)
{
    if (action == actionScroll_Hide) {
        Global::instance()->setScrollPosition(Global::Hide);
    } else if (action == actionScroll_Left) {
        Global::instance()->setScrollPosition(Global::Left);
    } else if (action == actionScroll_Right) {
        Global::instance()->setScrollPosition(Global::Right);
    } else {
        qDebug("updateScroll: should not be here");
    }
    emit scrollChanged();
}

void Frame::on_actionStatusbar_toggled(bool isEnabled)
{
    Global::instance()->setStatusBar(isEnabled);
	statusBar()->setVisible(isEnabled);
    emit statusBarChanged(isEnabled);
}

void Frame::on_actionDefault_Session_Setting_triggered()
{
    addrDialog set(this, true);

    if (Global::instance()->addrCfg()->hasSection("default"))
        Global::instance()->loadAddress(-1, set.param);

    set.updateData(false);

    if (set.exec() == 1) {
        Global::instance()->saveAddress(-1, set.param);
        Global::instance()->addrCfg()->save();
    }
}

void Frame::on_actionPreference_triggered()
{
    prefDialog pref(this);

    if (pref.exec() == 1) {
        Global::instance()->loadPrefence();
        setUseTray(Global::instance()->m_pref.bTray);
    }
}

void Frame::on_actionKey_Setup_triggered()
{
    keyDialog keyDlg(this);
    if (keyDlg.exec() == 1) {
        updateKeyToolBar();
    }
}

void Frame::on_actionPrint_triggered()
{
     QPrinter printer(QPrinter::HighResolution);
     QPrintDialog *dialog = new QPrintDialog(&printer, this);
     dialog->setWindowTitle(tr("Print Document"));
     if (dialog->exec() != QDialog::Accepted)
         return;
     QPainter painter;
     painter.begin(&printer);
     QPixmap screen = QPixmap::grabWidget(qobject_cast<Window *>(mdiArea->activeSubWindow()));
     QPixmap target = screen.scaled(printer.pageRect().width(),printer.pageRect().height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
     painter.drawPixmap(0,0,target);
     painter.end();
}

void Frame::on_actionImage_Viewer_triggered()
{
    Image *pViewer = new Image(Global::instance()->m_pref.strPoolPath);
    pViewer->show();
}

void Frame::keyClicked(int id)
{
    if (qobject_cast<Window *>(mdiArea->activeSubWindow()) == NULL)
        return;

    Config * conf = Global::instance()->fileCfg();

    QString strItem = QString("key%1").arg(id);

    QString strTmp = conf->getItemValue("key", strItem).toString();

    if (strTmp[0] == '0') { // key
        qobject_cast<Window *>(mdiArea->activeSubWindow())->externInput(strTmp.mid(1));
    } else if (strTmp[0] == '1') { // script
        qobject_cast<Window *>(mdiArea->activeSubWindow())->runScript(strTmp.mid(1));
    } else if (strTmp[0] == '2') { // program
        system((strTmp.mid(1) + " &").toLocal8Bit());
    }
}

void Frame::initShortcuts()
{
    int i = 0;
    QShortcut * shortcut = NULL;

    // shortcuts to addressbook entries
    QSignalMapper * addrMapper = new QSignalMapper(this);
    for (i = 0; i < 9; i++) {
        shortcut = new QShortcut(Qt::CTRL + Qt::ALT + 0x30 + 1 + i, this);
        shortcut->setObjectName(QString("Open addressbook enetry %1").arg(i+1));
        connect(shortcut, SIGNAL(activated()), addrMapper, SLOT(map()));
        addrMapper->setMapping(shortcut, i);
    }
    connect(addrMapper, SIGNAL(mapped(int)), this, SLOT(connectMenuActivated(int)));

    // shortcuts to swtch windows
    QSignalMapper * windowMapper = new QSignalMapper(this);
    for (i = 0; i < 9; i++) {
        shortcut = new QShortcut(Qt::ALT + 0x30 + 1 + i, this);
        shortcut->setObjectName(QString("Switch to window %1").arg(i+1));
        connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
        windowMapper->setMapping(shortcut, i);
    }
    
    shortcut = new QShortcut(Qt::ALT + Qt::Key_Left, this);
    shortcut->setObjectName("Previous window");
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 200);

    shortcut = new QShortcut(Qt::ALT + Qt::Key_Right, this);
    shortcut->setObjectName("Next window");
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 201);

    connect(windowMapper, SIGNAL(mapped(int)), this, SLOT(switchWin(int)));
}

QMenu * Frame::genPopupMenu(QWidget * owner)
{
    QMenu * popupMenu = new QMenu(owner);
    popupMenu->addAction(actionMenubar);
    popupMenu->addAction(actionFullscreen);
    popupMenu->addSeparator();
    popupMenu->addAction(actionCopy);
    popupMenu->addAction(actionPaste);
    popupMenu->addAction(actionCopy_Article);
    popupMenu->addSeparator();
    popupMenu->addAction(actionCurrent_Session_Setting);
    return popupMenu;
}

void Frame::updateKeyToolBar()
{
    keyToolBar->clear();
    keyToolBar->addAction(actionKey_Setup);

    Config * conf = Global::instance()->fileCfg();
    QString strItem, strTmp;
    strTmp = conf->getItemValue("key", "num").toString();
    int num = strTmp.toInt();

    for (int i = 0; i < num; i++) {
        strItem = QString("name%1").arg(i);
        strTmp = conf->getItemValue("key", strItem).toString();
        ToolButton *button = new ToolButton(keyToolBar, i, strTmp);
        button->setText(strTmp);
        strItem = QString("key%1").arg(i);
        strTmp = (conf->getItemValue("key", strItem).toString());
        connect(button, SIGNAL(buttonClicked(int)), this, SLOT(keyClicked(int)));
        keyToolBar->addWidget(button);
    }
}

void Frame::popupFocusIn(Window *)
{
    // bring to font
    if (isHidden()) {
        show();
    }
    if (isMinimized()) {
        if (isMaximized())
            showMaximized();
        else
            showNormal();
    }
    raise();
    activateWindow();
}


void Frame::setUseTray(bool use)
{
    if (use == false) {
        if (tray) {
            delete tray;
            tray = 0;
            delete trayMenu;
            trayMenu = 0;
        }
        return;
    }

    if (tray || !QSystemTrayIcon::isSystemTrayAvailable())
        return;

    trayMenu = new QMenu;
    connect(trayMenu, SIGNAL(aboutToShow()), SLOT(buildTrayMenu()));


    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/pic/qterm_tray.png"));
    tray->setContextMenu(trayMenu);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    tray->show();
}

void Frame::buildTrayMenu()
{
    if (!trayMenu)
        return;
    trayMenu->clear();

    if (isHidden())
        trayMenu->addAction(tr("&Show"), this, SLOT(trayShow()));
    else
        trayMenu->addAction(tr("&Hide"), this, SLOT(trayHide()));
    trayMenu->addSeparator();
    trayMenu->addAction(tr("&About"), this, SLOT(aboutQTerm()));
    trayMenu->addAction(tr("&Exit"), this, SLOT(confirmExitQTerm()));
}

void Frame::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isHidden()) {
            trayShow();
        } else {
            trayHide();
        }
        break;
    case QSystemTrayIcon::Context:
        return;
    default:
        return;
    }
}

void Frame::trayShow()
{
    show();
    // bring to font
    if (isHidden()) {
        show();
    }
    if (isMinimized()) {
        if (isMaximized())
            showMaximized();
        else
#ifdef Q_OS_MACX
            showMaximized();
#else
            showNormal();
#endif
    }
    restoreGeometry(Global::instance()->loadGeometry());
    restoreState(Global::instance()->loadState());
    raise();
    activateWindow();
}

void Frame::trayHide()
{
    Global::instance()->saveGeometry(saveGeometry());
    Global::instance()->saveState(saveState());
    hide();
}

void Frame::buzz()
{
    int xp = x();
    int yp = y();
    QTime t;

    t.start();
    for (int i = 32; i > 0;) {
        if (t.elapsed() >= 1) {
            int delta = i >> 2;
            int dir = i & 3;
            int dx = ((dir == 1) || (dir == 2)) ? delta : -delta;
            int dy = (dir < 2) ? delta : -delta;
            move(xp + dx, yp + dy);
            t.restart();
            i--;
        }
    }
    move(xp, yp);
}

void Frame::saveShortcuts()
{
    Config * conf = Global::instance()->fileCfg();
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    foreach (QAction* action, actions) {
        conf->setItemValue("Shortcuts", action->objectName(), action->shortcut().toString());
    }
    QList<QShortcut*> shortcuts = findChildren<QShortcut*>();
    foreach (QShortcut* shortcut, shortcuts) {
        conf->setItemValue("Shortcuts", shortcut->objectName(), shortcut->key().toString());
    }
    conf->save();
}

void Frame::loadShortcuts()
{
    Config * conf = Global::instance()->fileCfg();
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    foreach(QAction* action, actions)
    {
        QString keyseq=conf->getItemValue("Shortcuts", action->objectName()).toString();
        if (!keyseq.isEmpty())
            action->setShortcut(QKeySequence(keyseq));
    }
    QList<QShortcut*> shortcuts = findChildren<QShortcut*>();
    foreach (QShortcut* shortcut, shortcuts) {
        QString keyseq=conf->getItemValue("Shortcuts", shortcut->objectName()).toString();
        if (!keyseq.isEmpty())
            shortcut->setKey(QKeySequence(keyseq));
    }
}

void Frame::on_actionConfigure_Shortcuts_triggered()
{
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));
    QList<QShortcut*> shortcuts = findChildren<QShortcut*>();
    ShortcutsDialog sd(this,actions,shortcuts);
    sd.exec();
    saveShortcuts();
}

void Frame::saveToolbars()
{
    Config * conf = Global::instance()->fileCfg();
    QSize toolButtonIconSize;
    foreach (QToolBar *toolbar, findChildren<QToolBar*>())
    {
        toolButtonIconSize = toolbar->iconSize();
        QStringList listActions;
        if (toolbar == keyToolBar)
            continue;
        foreach(QAction* action, toolbar->actions())
        {
            if (action->objectName() == "actionConnectButton")
                continue;
            if(action->isSeparator())
                listActions+="Separator";
            else if (action->objectName().isEmpty()==false)
                listActions+=action->objectName();
            else
                qDebug() << toolbar->objectName() << ": Action without an object name cannot be saved";
        }
        conf->setItemValue("ToolBars", toolbar->objectName(), listActions);
    }
    conf->setItemValue("ToolBars", "ButtonStyle", int(toolButtonStyle()));
    conf->setItemValue("ToolBars", "IconSize", toolButtonIconSize);
    conf->save();
}

void Frame::loadToolbars()
{
    Config * conf = Global::instance()->fileCfg();
    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    QToolBar * toolbar;
    foreach (toolbar, toolbars)
    {
        QStringList actions=conf->getItemValue("ToolBars", toolbar->objectName()).toStringList();
        if (toolbar == keyToolBar)
            continue;
        foreach(QString action, actions)
        {
            QAction* act;
            if (action.isEmpty())
                continue;
            if(action=="Separator")
                toolbar->addSeparator();
            else {
                act=findChild<QAction*>(action);
                if (act != 0 ) // kingson: I forgot why && act->actionGroup()==0)
                    toolbar->addAction(act);
            }
        }
    }
    setToolButtonStyle(Qt::ToolButtonStyle(conf->getItemValue("ToolBars", "ButtonStyle").toInt()));
    setIconSize(conf->getItemValue("ToolBars", "IconSize").toSize());
}

void Frame::on_actionConfigure_Toolbars_triggered()
{
    ToolbarDialog td(this);
    td.exec();

    saveToolbars();
}

void Frame::slotShowQTerm()
{
    popupFocusIn(NULL);
}

void Frame::keyPressEvent(QKeyEvent * e)
{
    if (mdiArea->subWindowList().count() == 0 && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)) {
        loadSession();
        e->accept();
    } else {
        e->ignore();
    }
}

void Frame::mouseReleaseEvent(QMouseEvent * e)
{
    if (mdiArea->subWindowList().count() == 0 && (e->button() == Qt::LeftButton)) {
        loadSession();
        e->accept();
    } else {
        e->ignore();
    }

}
void Frame::loadSession()
{
    QList<QVariant> sites = Global::instance()->loadSession();
    if (sites.empty()) {
        connectMenuActivated(0);
    } else {
        for (int i = 0; i < sites.size(); i++) {
            int index = sites.at(i).toInt();
            connectMenuActivated(index);
        }
    }
}

bool Frame::showMessage(const QString & title, const QString & message, int millisecondsTimeoutHint)
{
    if (tray == 0 || !QSystemTrayIcon::isSystemTrayAvailable() || !QSystemTrayIcon::supportsMessages()) {
        return false;
    }
    tray->showMessage(title, message, QSystemTrayIcon::Information, millisecondsTimeoutHint);
    return true;
}

void Frame::groupActions()
{
	// These are actions which are enabled without any subwindow
	listBasicActions 
		<< "actionNew_Console" 
		<< "actionQuick_Login" << "actionAddressBook"  << "actionQuit"
		<< "actionNew_ANSI"     << "actionOpen_ANSI"
		<< "actionFont" << "actionStatusbar" << "actionMenubar" << "actionFullscreen"
		<< "actionImage_Viewer"
		<< "actionManage_Favarites" << "actionArticle_Manager" 
		<< "actionDefault_Session_Setting" << "actionPreference"
		<< "actionConfigure_Toolbars" << "actionConfigure_Shortcuts" << "actionKey_Setup"
		<< "actionQTerm_Online" << "actionContents" <<"actionWhat_s_this"
		<< "actionAbout_QTerm" << "actionAbout_Qt";
	QActionGroup *group;
	// These actions belong to Edit->Past with Color submenu
	group = new QActionGroup(this);
	group->addAction(actionNone_Color);
	group->addAction(actionESC_ESC);
	group->addAction(actionCtrl_U);
	group->addAction(actionCustom);
	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(updateESC(QAction*)));
	// These actions belong to Edit->Clipboard Conversion submenu
	group = new QActionGroup(this);
	group->addAction(actionNone);
	group->addAction(actionCHS_CHT);
	group->addAction(actionCHT_CHS);
	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(updateCodec(QAction*)));
	// These actions belong to View->Scrollbar submenu
	group = new QActionGroup(this);
	group->addAction(actionScroll_Hide);
	group->addAction(actionScroll_Left);
	group->addAction(actionScroll_Right);
	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(updateScroll(QAction*)));
	// These actions belong to View->Language submenu
	group = new QActionGroup(this);
	group->addAction(actionEnglish);
	group->addAction(actionSimplified_Chinese);
	group->addAction(actionTraditional_Chinese);
	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(updateLang(QAction*)));

    // The other actions are grouped and redirected to subwindow
    QList<QAction*> actions = findChildren<QAction*>(QRegExp("action*"));

    actionsExtra = new QActionGroup(this);
    actionsExtra->setExclusive(false);
    connect(actionsExtra, SIGNAL(triggered(QAction*)),
		this, SLOT(actionsDispatcher(QAction*)));

    foreach(QAction* action, actions) {
		action->setVisible(listBasicActions.contains(action->objectName())
			|| action->actionGroup() != 0);
		if (action->actionGroup() == 0)
			actionsExtra->addAction(action);
	}
}

}

#include <qtermframe.moc>
