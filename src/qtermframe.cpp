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
#include "qtermwndmgr.h"
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
#include "trayicon.h"
#include "imageviewer.h"

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

namespace QTerm
{

Frame* Frame::s_instance = 0;

//constructor
Frame::Frame()
        : QMainWindow(0)
{
    s_instance = this;
    setAttribute(Qt::WA_DeleteOnClose);

    m_MdiArea = new QMdiArea(this);
    setCentralWidget(m_MdiArea);

    tray = 0;
    trayMenu = 0;

//create a tabbar in the hbox
    tabBar = new QTabBar(statusBar());
    statusBar()->addWidget(tabBar, 0);
    connect(tabBar, SIGNAL(selected(int)), this, SLOT(selectionChanged(int)));
    //tabBar->setShape(QTabBar::TriangularBelow);
    tabBar->setShape(QTabBar:: RoundedSouth);
    tabBar->setDrawBase(false);

//create a progress bar to notify the download process
    m_pStatusBar = new QTerm::StatusBar(statusBar(), "mainStatusBar");
    statusBar()->addWidget(m_pStatusBar, 0);

//create the window manager to deal with the window-tab-icon pairs
    wndmgr = new WndMgr(this);

    initShortcuts();

//set menubar
    initActions();

    addMainMenu();

//setup toolbar
    addMainTool();

// add the custom defined key
    updateKeyToolBar();

// diaable some menu & toolbar
    enableMenuToolBar(false);

//initialize all settings
    iniSetting();

    initThemesMenu();

    installEventFilter(this);
}

//destructor
Frame::~Frame()
{
    delete wndmgr;
}

//initialize setting from qterm.cfg

void Frame::iniSetting()
{
    Global::instance()->loadConfig();

    restoreGeometry(Global::instance()->loadGeometry());
    restoreState(Global::instance()->loadState());
    if (Global::instance()->isFullScreen()) {
        m_fullAction->setChecked(true);
        showFullScreen();
    }

    QStyle * style = QStyleFactory::create(Global::instance()->style());
    if (style)
        qApp->setStyle(style);

    //language
    if (Global::instance()->language() == Global::English)
        m_engAction->setChecked(true);
    else if (Global::instance()->language() == Global::SimpilifiedChinese)
        m_chsAction->setChecked(true);
    else if (Global::instance()->language() == Global::TraditionalChinese)
        m_chtAction->setChecked(true);
    else
        m_engAction->setChecked(true);

    m_noescAction->setChecked(true);

    if (Global::instance()->clipCodec() == Global::GBK) {
        m_GBKAction->setChecked(true);
    } else {
        m_BIG5Action->setChecked(true);
    }

    if (Global::instance()->scrollPosition() == Global::Hide) {
        m_scrollHideAction->setChecked(true);
    } else if (Global::instance()->scrollPosition() == Global::Left) {
        m_scrollLeftAction->setChecked(true);
    } else {
        m_scrollRightAction->setChecked(true);
    }

    m_statusAction->setChecked(Global::instance()->showStatusBar());

    m_switchAction->setChecked(Global::instance()->showSwitchBar());

    if (Global::instance()->showSwitchBar())
        statusBar()->show();
    else
        statusBar()->hide();

    setUseTray(Global::instance()->m_pref.bTray);
}

void Frame::saveSetting()
{
    Global::instance()->saveGeometry(saveGeometry());
    Global::instance()->saveState(saveState());
    Global::instance()->saveConfig();
}

//addressbook
void Frame::addressBook()
{
    addrDialog addr(this, false);

    if (addr.exec() == 1) {
        newWindow(addr.param, addr.ui.nameListWidget->currentRow());
    }
}

//quicklogin
void Frame::quickLogin()
{
    quickDialog quick(this);

    Global::instance()->loadAddress(-1, quick.param);

    if (quick.exec() == 1) {
        newWindow(quick.param);
    }
}

void Frame::exitQTerm()
{
    while (wndmgr->count() > 0) {
        bool closed = m_MdiArea->activeSubWindow()->close();
        if (!closed) {
            return;
        }
    }

    saveSetting();
    // clear zmodem and pool if needed
    if (Global::instance()->m_pref.bClearPool) {
        Global::instance()->clearDir(Global::instance()->m_pref.strZmPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath + "shadow-cache/");
    }

    setUseTray(false);
    qApp->quit();
}

//create a new display window
void Frame::newWindow(const Param&  param, int index)
{
    Window * window = new Window(this, param, index, m_MdiArea,
                                 0);
    QString pathLib = Global::instance()->pathLib();
    m_MdiArea->addSubWindow(window);
    window->setWindowTitle(param.m_strName);
    window->setWindowIcon(QPixmap(pathLib + "pic/tabpad.png"));
    window->setAttribute(Qt::WA_DeleteOnClose);

    QIcon* icon = new QIcon(QPixmap(pathLib + "pic/tabpad.png"));
    QString qtab = window->windowTitle();

    //add window-tab-icon to window manager
    wndmgr->addWindow(window);
    tabBar->addTab(*icon, qtab);

    window->setFocus();
    window->showMaximized();

}

//the tabbar selection changed
void Frame::selectionChanged(int n)
{
    wndmgr->activateTheWindow(n);
}

//slot Help->About QTerm
void Frame::aboutQTerm()
{
    aboutDialog about(this);

    about.exec();
}

//slot Help->Homepage
void Frame::homepage()
{
    QString strCmd = Global::instance()->m_pref.strHttp;
    QString strUrl = "http://www.qterm.org";

    if (strCmd.indexOf("%L") == -1)
        strCmd += " \"" + strUrl + "\"";
    else
        strCmd.replace(QRegExp("%L", Qt::CaseInsensitive), strUrl);

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    strCmd += " &";
#endif
    system(strCmd.toLocal8Bit());
}

//slot Windows menu aboutToShow
void Frame::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    QAction * cascadeAction = windowsMenu->addAction(tr("Cascade"), m_MdiArea, SLOT(cascadeSubWindows()));
    QAction * tileAction = windowsMenu->addAction(tr("Tile"), m_MdiArea, SLOT(tileSubWindows()));
    if (m_MdiArea->subWindowList().isEmpty()) {
        cascadeAction->setEnabled(false);
        tileAction->setEnabled(false);
    }
    windowsMenu->addSeparator();

#ifdef Q_OS_MACX
    // used to dock the programe
    if (isHidden())
        windowsMenu->addAction(tr("Main Window"), this, SLOT(trayShow()));
#endif

    QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
    for (int i = 0; i < int(windows.count()); ++i) {
        QAction * idAction = windowsMenu->addAction(windows.at(i)->windowTitle(),
                             this, SLOT(windowsMenuActivated(int)));
        idAction->setData(i);
        idAction->setChecked(m_MdiArea->activeSubWindow() == windows.at(i));
    }

}
//slot activate the window correspond with the menu id
void Frame::windowsMenuActivated(int id)
{
    QWidget* w = m_MdiArea->subWindowList().at(id);
    if (w) {
        w->showNormal();
    }
}

void Frame::popupConnectMenu()
{
    connectMenu->clear();

    connectMenu->addAction("Quick Login", this, SLOT(quickLogin()));
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

void Frame::switchWin(int id)
{
    QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
    if (windows.count() <= id)
        return;

    if (id == 200) {
        wndmgr->activeNextPrev(false);
        return;
    }
    if (id == 201 || id == 202) {
        wndmgr->activeNextPrev(true);
        return;
    }

    QWidget *w = windows.at(id);
    if (w == m_MdiArea->activeSubWindow())
        return;

    if (w != NULL) {
        w->setFocus();
    }
}


//slot draw something e.g. logo in the background
//TODO : draw a pixmap in the background
void Frame::paintEvent(QPaintEvent *)
{

}

void Frame::closeEvent(QCloseEvent * clse)
{
    QList<QMdiSubWindow *> windows = m_MdiArea->subWindowList();
    for (int i = 0; i < int(windows.count()); ++i) {

        if (((Window *)windows.at(i))->isConnected()) {
            if (Global::instance()->m_pref.bTray) {
                trayHide();
                clse->ignore();
                return;
            }
        }
    }
    while (wndmgr->count() > 0) {
        bool closed = m_MdiArea->activeSubWindow()->close();
        if (!closed) {
            clse->ignore();
            return;
        }
    }

    saveSetting();
    // clear zmodem and pool if needed
    // FIXME: cleanup the code
    if (Global::instance()->m_pref.bClearPool) {
        Global::instance()->clearDir(Global::instance()->m_pref.strZmPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath);
        Global::instance()->clearDir(Global::instance()->m_pref.strPoolPath + "shadow-cache/");
    }

    setUseTray(false);

    clse->accept();

}

void Frame::updateLang(QAction * action)
{
    QMessageBox::information(this, "QTerm",
                             tr("This will take effect after restart,\nplease close all windows and restart."));
    Config * conf = Global::instance()->fileCfg();
    conf->setItemValue("global", "language", action->objectName());
    conf->save();
}

void Frame::connectIt()
{
    if (wndmgr->activeWindow() == NULL) {
        Param param;
        Global::instance()->loadAddress(-1, param);
        newWindow(param);
    } else
        if (!wndmgr->activeWindow()->isConnected())
            wndmgr->activeWindow()->reconnect();
}
void Frame::disconnect()
{
    wndmgr->activeWindow()->disconnect();
}

void Frame::copy()
{
    wndmgr->activeWindow()->copy();
}
void Frame::paste()
{
    wndmgr->activeWindow()->paste();
}
void Frame::copyRect(bool isEnabled)
{
    wndmgr->activeWindow()->m_bCopyRect = isEnabled;
}

void Frame::copyColor(bool isEnabled)
{
    wndmgr->activeWindow()->m_bCopyColor = isEnabled;
}

void Frame::copyArticle()
{
    wndmgr->activeWindow()->copyArticle();
}

void Frame::autoCopy(bool isEnabled)
{
    wndmgr->activeWindow()->m_bAutoCopy = isEnabled;
}

void Frame::wordWrap(bool isEnabled)
{
    wndmgr->activeWindow()->m_bWordWrap = isEnabled;
}

void Frame::updateESC(QAction * action)
{
    if (action->objectName() == "actionNoESC") {
        Global::instance()->setEscapeString("");
    } else if (action->objectName() == "actionESCESC") {
        Global::instance()->setEscapeString("^[^[[");
    } else if (action->objectName() == "actionUESC") {
        Global::instance()->setEscapeString("^u[");
    } else if (action->objectName() == "actionCustomESC") {
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
    if (action->objectName() == "actionGBK") {
        Global::instance()->setClipCodec(Global::GBK);
    } else if (action->objectName() == "actionBig5") {
        Global::instance()->setClipCodec(Global::Big5);
    } else {
        qDebug("updateCodec: should not be here");
    }
}

void Frame::font()
{
    wndmgr->activeWindow()->font();
}

void Frame::color()
{
    wndmgr->activeWindow()->color();

}
void Frame::refresh()
{
    wndmgr->activeWindow()->refresh();
}

void Frame::uiFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, qApp->font());

    if (Global::instance()->m_pref.bAA)
        font.setStyleStrategy(QFont::PreferAntialias);

    if (ok == true) {
        qApp->setFont(font);
    }
}

void Frame::triggerFullScreen(bool isFullScreen)
{
    Global::instance()->setFullScreen(isFullScreen);

    //TODO: add an item to the popup menu so we can go back to normal without touch the keyboard
    if (isFullScreen) {
        Global::instance()->saveGeometry(saveGeometry());
        Global::instance()->saveState(saveState());
        menuBar()->hide();
        mdiTools->hide();
        mdiconnectTools->hide();
        key->hide();
        //showStatusBar();
        //showSwitchBar();
        showFullScreen();
    } else {
        menuBar()->show();
        restoreGeometry(Global::instance()->loadGeometry());
        restoreState(Global::instance()->loadState());
        emit scrollChanged();
        showNormal();
        //showStatusBar();
        //showSwitchBar();
    }

    m_fullAction->setChecked(isFullScreen);

}

void Frame::bosscolor()
{
    bool tmp = !Global::instance()->isBossColor();
    Global::instance()->setBossColor(tmp);

    emit bossColor();

    m_bossAction->setChecked(tmp);
}

void Frame::initThemesMenu()
{
    themesMenu->clear();
    QActionGroup * themesGroup = new QActionGroup(this);
    QStringList styles = QStyleFactory::keys();
    for (QStringList::ConstIterator it = styles.begin(); it != styles.end(); it++)
        themesGroup->addAction(insertThemeItem(*it));
    connect(themesGroup, SIGNAL(triggered(QAction*)), this, SLOT(themesMenuActivated(QAction*)));
}

void Frame::themesMenuActivated(QAction * action)
{
    theme = action->text();
    QStyle * style = QStyleFactory::create(theme);
    if (style)
        qApp->setStyle(style);
}

void Frame::updateScroll(QAction * action)
{
    if (action->objectName() == "actionHide") {
        Global::instance()->setScrollPosition(Global::Hide);
    } else if (action->objectName() == "actionLeft") {
        Global::instance()->setScrollPosition(Global::Left);
    } else if (action->objectName() == "actionRight") {
        Global::instance()->setScrollPosition(Global::Right);
    } else {
        qDebug("updateScroll: should not be here");
    }
    emit scrollChanged();
}

void Frame::updateSwitchBar(bool isEnabled)
{
    Global::instance()->setSwitchBar(isEnabled);

    if (Global::instance()->showSwitchBar())
        statusBar()->show();
    else
        statusBar()->hide();
}

void Frame::updateStatusBar(bool isEnabled)
{
    Global::instance()->setStatusBar(isEnabled);

    emit statusBarChanged(isEnabled);
}

void Frame::setting()
{
    wndmgr->activeWindow()->setting();
}
void Frame::defaultSetting()
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

void Frame::preference()
{
    prefDialog pref(this);

    if (pref.exec() == 1) {
        Global::instance()->loadPrefence();
        setUseTray(Global::instance()->m_pref.bTray);
    }
}

void Frame::keySetup()
{
    keyDialog keyDlg(this);
    if (keyDlg.exec() == 1) {
        updateKeyToolBar();
    }
}


void Frame::antiIdle(bool isEnabled)
{
    wndmgr->activeWindow()->antiIdle(isEnabled);
}

void Frame::autoReply(bool isEnabled)
{
    wndmgr->activeWindow()->autoReply(isEnabled);
}

void Frame::viewMessages()
{
    wndmgr->activeWindow()->viewMessages();
}

void Frame::updateMouse(bool isEnabled)
{
    wndmgr->activeWindow()->m_bMouse = isEnabled;
    m_mouseAction->setChecked(wndmgr->activeWindow()->m_bMouse);
}

void Frame::viewImages()
{
    Image viewer(Global::instance()->pathPic() + "pic/shadow.png", Global::instance()->m_pref.strPoolPath, this);
    viewer.exec();
}

void Frame::updateBeep(bool isEnabled)
{
    wndmgr->activeWindow()->m_bBeep = isEnabled;
    m_beepAction->setChecked(wndmgr->activeWindow()->m_bBeep);
}

void Frame::reconnect(bool isEnabled)
{
    wndmgr->activeWindow()->m_bReconnect = isEnabled;
}

void Frame::runScript()
{
    wndmgr->activeWindow()->runScript();
}
void Frame::stopScript()
{
    wndmgr->activeWindow()->stopScript();
}

void Frame::keyClicked(int id)
{
    if (wndmgr->activeWindow() == NULL)
        return;

    Config * conf = Global::instance()->fileCfg();

    QString strItem = QString("key%1").arg(id);

    QString strTmp = conf->getItemValue("key", strItem);

    if (strTmp[0] == '0') { // key
        wndmgr->activeWindow()->externInput(strTmp.mid(1).toLatin1());
    } else if (strTmp[0] == '1') { // script
        wndmgr->activeWindow()->runScriptFile(strTmp.mid(1).toLatin1());
    } else if (strTmp[0] == '2') { // program
        system((strTmp.mid(1) + " &").toLocal8Bit());
    }
}

void Frame::addMainTool()
{
    mdiTools = addToolBar("Main ToolBar");
    mdiTools->setObjectName("mainToolBar");

    connectButton = new QToolButton(mdiTools);
    connectButton->setIcon(QPixmap(Global::instance()->pathPic() + "pic/connect.png"));

    mdiTools->addWidget(connectButton);
    connectMenu = new QMenu(this);

    connect(connectMenu, SIGNAL(aboutToShow()), this, SLOT(popupConnectMenu()));
    connectButton->setMenu(connectMenu);
    connectButton->setPopupMode(QToolButton::InstantPopup);

    mdiTools->addAction(m_quickConnectAction);
    // custom define
    key = addToolBar("Custom Key");
    key->setObjectName("customKeyToolBar");

    // the toolbar
    mdiconnectTools = addToolBar("bbs operations");
    mdiconnectTools->setObjectName("bbsOperationsToolBar");

    mdiconnectTools->addAction(m_disconnectAction);
    mdiconnectTools->addSeparator();

    mdiconnectTools->addAction(m_copyAction);
    mdiconnectTools->addAction(m_pasteAction);
    mdiconnectTools->addAction(m_rectAction);
    mdiconnectTools->addAction(m_colorCopyAction);
    mdiconnectTools->addSeparator();

    mdiconnectTools->addAction(m_fontAction);
    mdiconnectTools->addAction(m_colorAction);
    mdiconnectTools->addAction(m_refreshAction);
    mdiconnectTools->addSeparator();

    mdiconnectTools->addAction(m_currentSessionAction);
    mdiconnectTools->addSeparator();

    mdiconnectTools->addAction(m_copyArticleAction);
    mdiconnectTools->addAction(m_antiIdleAction);
    mdiconnectTools->addAction(m_autoReplyAction);
    mdiconnectTools->addAction(m_viewMessageAction);
    mdiconnectTools->addAction(m_mouseAction);
    mdiconnectTools->addAction(m_beepAction);
    mdiconnectTools->addAction(m_reconnectAction);
}

void Frame::initShortcuts()
{
    int i = 0;
    QShortcut * shortcut = NULL;
    QSignalMapper * addrMapper = new QSignalMapper(this);
    for (i = 0; i < 9; i++) {
        shortcut = new QShortcut(Qt::CTRL + Qt::ALT + 0x30 + 1 + i, this);
        connect(shortcut, SIGNAL(activated()), addrMapper, SLOT(map()));
        addrMapper->setMapping(shortcut, i);
    }
    connect(addrMapper, SIGNAL(mapped(int)), this, SLOT(connectMenuActivated(int)));
    QSignalMapper * windowMapper = new QSignalMapper(this);
    for (i = 0; i < 10; i++) {
        shortcut = new QShortcut(Qt::ALT + 0x30 + 1 + i, this);
        connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
        windowMapper->setMapping(shortcut, i);
    }
    shortcut = new QShortcut(Qt::ALT + Qt::Key_Left, this);
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 200);
    shortcut = new QShortcut(Qt::ALT + Qt::Key_Up, this);
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 200);
    shortcut = new QShortcut(Qt::ALT + Qt::Key_Right, this);
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 201);
    shortcut = new QShortcut(Qt::ALT + Qt::Key_Down, this);
    connect(shortcut, SIGNAL(activated()), windowMapper, SLOT(map()));
    windowMapper->setMapping(shortcut, 201);
    connect(windowMapper, SIGNAL(mapped(int)), this, SLOT(switchWin(int)));
}

void Frame::initActions()
{
    QString pathLib = Global::instance()->pathLib();

    m_connectAction = new QAction(QPixmap(pathLib + "pic/connect.png"), tr("&Connect"), this);
    m_connectAction->setObjectName("actionConnect");
    m_disconnectAction = new QAction(QPixmap(pathLib + "pic/disconnect.png"), tr("&Disconnect"), this);
    m_connectAction->setObjectName("actionDisconnect");
    m_addressAction = new QAction(QPixmap(pathLib + "pic/addr.png"), tr("&Address book"), this);
    m_addressAction->setObjectName("actionAddress");
    m_addressAction->setShortcut(Qt::Key_F2);
    m_quickConnectAction = new QAction(QPixmap(pathLib + "pic/quick.png"), tr("&Quick login"), this);
    m_quickConnectAction->setObjectName("actionQuickConnect");
    m_quickConnectAction->setShortcut(Qt::Key_F3);
    m_exitAction = new QAction(tr("&Exit"), this);
    m_exitAction->setObjectName("actionExit");

    m_copyAction = new QAction(QPixmap(pathLib + "pic/copy.png"), tr("&Copy"), this);
    m_copyAction->setObjectName("actionCopy");
    m_copyAction->setShortcut(Qt::CTRL + Qt::Key_Insert);
    m_pasteAction = new QAction(QPixmap(pathLib + "pic/paste.png"), tr("&Paste"), this);
    m_pasteAction->setObjectName("actionPaste");
    m_pasteAction->setShortcut(Qt::SHIFT + Qt::Key_Insert);
    m_colorCopyAction = new QAction(QPixmap(pathLib + "pic/color-copy.png"), tr("C&opy with color"), this);
    m_colorCopyAction->setObjectName("actionColorCopy");
    m_colorCopyAction->setCheckable(true);
    m_rectAction = new QAction(QPixmap(pathLib + "pic/rect.png"), tr("&Rectangle select"), this);
    m_rectAction->setObjectName("actionRect");
    m_rectAction->setCheckable(true);
    m_autoCopyAction = new QAction(tr("Auto copy &select"), this);
    m_autoCopyAction->setObjectName("actionAutoCopy");
    m_autoCopyAction->setCheckable(true);
    m_wwrapAction = new QAction(tr("P&aste with wordwrap"), this);
    m_wwrapAction->setObjectName("actionWordWrap");
    m_wwrapAction->setCheckable(true);

    QActionGroup * escapeGroup = new QActionGroup(this);
    m_noescAction = new QAction(tr("&None"), this);
    m_noescAction->setObjectName("actionNoESC");
    m_noescAction->setCheckable(true);
    m_escescAction = new QAction(tr("&ESC ESC ["), this);
    m_escescAction->setObjectName("actionESCESC");
    m_escescAction->setCheckable(true);
    m_uescAction = new QAction(tr("Ctrl+&U ["), this);
    m_uescAction->setObjectName("actionUESC");
    m_uescAction->setCheckable(true);
    m_customescAction = new QAction(tr("&Custom..."), this);
    m_customescAction->setObjectName("actionCustomESC");
    m_customescAction->setCheckable(true);
    escapeGroup->addAction(m_noescAction);
    escapeGroup->addAction(m_escescAction);
    escapeGroup->addAction(m_uescAction);
    escapeGroup->addAction(m_customescAction);

    QActionGroup * codecGroup = new QActionGroup(this);
    m_GBKAction = new QAction(tr("&GBK"), this);
    m_GBKAction->setObjectName("actionGBK");
    m_GBKAction->setCheckable(true);
    m_BIG5Action = new QAction(tr("&Big5"), this);
    m_BIG5Action->setObjectName("actionBig5");
    m_BIG5Action->setCheckable(true);
    codecGroup->addAction(m_GBKAction);
    codecGroup->addAction(m_BIG5Action);

    m_fontAction = new QAction(QPixmap(pathLib + "pic/fonts.png"), tr("&Font"), this);
    m_fontAction->setObjectName("actionFont");
    m_colorAction = new QAction(QPixmap(pathLib + "pic/color.png"), tr("&Color"), this);
    m_colorAction->setObjectName("actionColor");
    m_refreshAction = new QAction(QPixmap(pathLib + "pic/refresh.png"), tr("&Refresh"), this);
    m_refreshAction->setObjectName("actionRefresh");
    m_refreshAction->setShortcut(Qt::Key_F5);

    QActionGroup * langGroup = new QActionGroup(this);
    m_engAction = new QAction(tr("&English"), this);
    m_engAction->setObjectName("actionEng");
    m_engAction->setCheckable(true);
    m_chsAction = new QAction(tr("&Simplified Chinese"), this);
    m_chsAction->setObjectName("actionChs");
    m_chsAction->setCheckable(true);
    m_chtAction = new QAction(tr("&Traditional Chinese"), this);
    m_chtAction->setObjectName("actionCht");
    m_chtAction->setCheckable(true);
    langGroup->addAction(m_engAction);
    langGroup->addAction(m_chsAction);
    langGroup->addAction(m_chtAction);

    m_uiFontAction = new QAction(tr("&UI font"), this);
    m_uiFontAction->setObjectName("actionUiFont");
    m_fullAction = new QAction(tr("&Fullscreen"), this);
    m_fullAction->setObjectName("actionFull");
    m_fullAction->setCheckable(true);
    m_fullAction->setShortcut(Qt::Key_F6);
    addAction(m_fullAction);
    m_bossAction = new QAction(tr("Boss &Color"), this);
    m_bossAction->setObjectName("action Boss");
    m_bossAction->setShortcut(Qt::Key_F12);

    QActionGroup * scrollGroup = new QActionGroup(this);
    m_scrollHideAction = new QAction(tr("&Hide"), this);
    m_scrollHideAction->setObjectName("actionHide");
    m_scrollHideAction->setCheckable(true);
    m_scrollLeftAction = new QAction(tr("&Left"), this);
    m_scrollLeftAction->setObjectName("actionLeft");
    m_scrollLeftAction->setCheckable(true);
    m_scrollRightAction = new QAction(tr("&Right"), this);
    m_scrollRightAction->setObjectName("actionRight");
    m_scrollRightAction->setCheckable(true);
    scrollGroup->addAction(m_scrollHideAction);
    scrollGroup->addAction(m_scrollLeftAction);
    scrollGroup->addAction(m_scrollRightAction);

    m_statusAction = new QAction(tr("Status &Bar"), this);
    m_statusAction->setObjectName("actionStatus");
    m_statusAction->setCheckable(true);
    m_switchAction = new QAction(tr("S&witch Bar"), this);
    m_switchAction->setObjectName("actionSwitch");
    m_switchAction->setCheckable(true);

    m_currentSessionAction = new QAction(QPixmap(pathLib + "pic/pref.png"), tr("&Setting for currrent session"), this);
    m_currentSessionAction->setObjectName("actionCurrentSession");
    m_defaultAction = new QAction(tr("&Default setting"), this);
    m_defaultAction->setObjectName("actionDefault");
    m_prefAction = new QAction(tr("&Preference"), this);
    m_prefAction->setObjectName("actionPref");

    m_copyArticleAction = new QAction(QPixmap(pathLib + "pic/article.png"), tr("&Copy article"), this);
    m_copyArticleAction->setObjectName("actionCopyArticle");
    m_copyArticleAction->setShortcut(Qt::Key_F9);
    m_antiIdleAction = new QAction(QPixmap(pathLib + "pic/anti-idle.png"), tr("Anti &idle"), this);
    m_antiIdleAction->setObjectName("actionAntiIdle");
    m_antiIdleAction->setCheckable(true);
    m_autoReplyAction = new QAction(QPixmap(pathLib + "pic/auto-reply.png"), tr("Auto &reply"), this);
    m_autoReplyAction->setObjectName("actionAutoReply");
    m_autoReplyAction->setCheckable(true);
    m_viewMessageAction = new QAction(QPixmap(pathLib + "pic/message.png"), tr("&View messages"), this);
    m_viewMessageAction->setObjectName("actionViewMessage");
    m_viewMessageAction->setShortcut(Qt::Key_F10);
    m_beepAction = new QAction(QPixmap(pathLib + "pic/sound.png"), tr("&Beep "), this);
    m_beepAction->setObjectName("actionBeep");
    m_beepAction->setCheckable(true);
    m_mouseAction = new QAction(QPixmap(pathLib + "pic/mouse.png"), tr("&Mouse support"), this);
    m_mouseAction->setObjectName("actionMouse");
    m_mouseAction->setCheckable(true);
    m_viewImageAction = new QAction(tr("&Image viewer"), this);
    m_viewImageAction->setObjectName("actionViewImage");

    m_scriptRunAction = new QAction(tr("&Run..."), this);
    m_scriptRunAction->setObjectName("actionScriptRun");
    m_scriptRunAction->setShortcut(Qt::Key_F7);
    m_scriptStopAction = new QAction(tr("&Stop"), this);
    m_scriptStopAction->setObjectName("actionScriptStop");
    m_scriptStopAction->setShortcut(Qt::Key_F8);

    m_aboutAction = new QAction(tr("About &QTerm"), this);
    m_aboutAction->setObjectName("actionAbout");
    m_aboutAction->setShortcut(Qt::Key_F1);
    m_homepageAction = new QAction(tr("QTerm's &Homepage"), this);
    m_homepageAction->setObjectName("actionHomepage");

    m_reconnectAction = new QAction(QPixmap(Global::instance()->pathPic() + "pic/reconnect.png"), tr("Reconnect When Disconnected By Host"), this);
    m_reconnectAction->setObjectName("actionReconnect");
    m_reconnectAction->setCheckable(true);

    connect(m_connectAction, SIGNAL(triggered()), this, SLOT(connectIt()));
    connect(m_disconnectAction, SIGNAL(triggered()), this, SLOT(disconnect()));
    connect(m_addressAction, SIGNAL(triggered()), this, SLOT(addressBook()));
    connect(m_quickConnectAction, SIGNAL(triggered()), this, SLOT(quickLogin()));
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(exitQTerm()));

    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    connect(m_colorCopyAction, SIGNAL(toggled(bool)), this, SLOT(copyColor(bool)));
    connect(m_rectAction, SIGNAL(toggled(bool)), this, SLOT(copyRect(bool)));
    connect(m_autoCopyAction, SIGNAL(toggled(bool)), this, SLOT(autoCopy(bool)));
    connect(m_wwrapAction, SIGNAL(toggled(bool)), this, SLOT(wordWrap(bool)));

    connect(escapeGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateESC(QAction*)));
    connect(codecGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateCodec(QAction*)));

    connect(m_fontAction, SIGNAL(triggered()), this, SLOT(font()));
    connect(m_colorAction, SIGNAL(triggered()), this, SLOT(color()));
    connect(m_refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

    connect(langGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateLang(QAction*)));

    connect(m_uiFontAction, SIGNAL(triggered()), this, SLOT(uiFont()));
    connect(m_fullAction, SIGNAL(toggled(bool)), this, SLOT(triggerFullScreen(bool)));
    connect(m_bossAction, SIGNAL(triggered()), this, SLOT(bosscolor()));

    connect(scrollGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateScroll(QAction*)));

    connect(m_statusAction, SIGNAL(toggled(bool)), this, SLOT(updateStatusBar(bool)));
    connect(m_switchAction, SIGNAL(toggled(bool)), this, SLOT(updateSwitchBar(bool)));

    connect(m_currentSessionAction, SIGNAL(triggered()), this, SLOT(setting()));
    connect(m_defaultAction, SIGNAL(triggered()), this, SLOT(defaultSetting()));
    connect(m_prefAction, SIGNAL(triggered()), this, SLOT(preference()));

    connect(m_copyArticleAction, SIGNAL(triggered()), this, SLOT(copyArticle()));
    connect(m_antiIdleAction, SIGNAL(toggled(bool)), this, SLOT(antiIdle(bool)));
    connect(m_autoReplyAction, SIGNAL(toggled(bool)), this, SLOT(autoReply(bool)));
    connect(m_viewMessageAction, SIGNAL(triggered()), this, SLOT(viewMessages()));
    connect(m_beepAction, SIGNAL(toggled(bool)), this, SLOT(updateBeep(bool)));
    connect(m_mouseAction, SIGNAL(toggled(bool)), this, SLOT(updateMouse(bool)));
    connect(m_viewImageAction, SIGNAL(triggered()), this, SLOT(viewImages()));

    connect(m_scriptRunAction, SIGNAL(triggered()), this, SLOT(runScript()));
    connect(m_scriptStopAction, SIGNAL(triggered()), this, SLOT(stopScript()));

    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(aboutQTerm()));
    connect(m_homepageAction, SIGNAL(triggered()), this, SLOT(homepage()));

    connect(m_reconnectAction, SIGNAL(toggled(bool)), this, SLOT(reconnect(bool)));
}

void Frame::addMainMenu()
{
    mainMenu = menuBar();

    QMenu * file = mainMenu->addMenu(tr("&File"));
    file->addAction(m_connectAction);
    file->addAction(m_disconnectAction);

    file->addSeparator();
    file->addAction(m_addressAction);
    file->addAction(m_quickConnectAction);
    file->addSeparator();
    file->addAction(m_exitAction);

    //Edit Menu
    QMenu * edit = new QMenu(tr("&Edit"), this);
    mainMenu->addMenu(edit);

    edit->addAction(m_copyAction);
    edit->addAction(m_pasteAction);
    edit->addSeparator();
    edit->addAction(m_colorCopyAction);
    edit->addAction(m_rectAction);
    edit->addAction(m_autoCopyAction);
    edit->addAction(m_wwrapAction);

    QMenu * escapeMenu = new QMenu(tr("Paste &with color"), this);
    escapeMenu->addAction(m_noescAction);
    escapeMenu->addAction(m_escescAction);
    escapeMenu->addAction(m_uescAction);
    escapeMenu->addAction(m_customescAction);
    edit->addMenu(escapeMenu);

    QMenu * codecMenu = new QMenu(tr("Clipboard &encoding"), this);
    codecMenu->addAction(m_GBKAction);
    codecMenu->addAction(m_BIG5Action);
    edit->addMenu(codecMenu);

    //View menu
    QMenu * view = new QMenu(tr("&View"), this);
    mainMenu->addMenu(view);

    view->addAction(m_fontAction);
    view->addAction(m_colorAction);
    view->addAction(m_refreshAction);
    view->addSeparator();

    //language menu
    langMenu = new QMenu(tr("&Language"), this);
    langMenu->addAction(m_engAction);
    langMenu->addAction(m_chsAction);
    langMenu->addAction(m_chtAction);
    view->addMenu(langMenu);
    view->addAction(m_uiFontAction);

    themesMenu = new QMenu(tr("&Themes"), this);
    view->addMenu(themesMenu);

    view->addAction(m_fullAction);
    view->addAction(m_bossAction);

    view->addSeparator();
    QMenu *scrollMenu = new QMenu(tr("&ScrollBar"), this);
    scrollMenu->addAction(m_scrollHideAction);
    scrollMenu->addAction(m_scrollLeftAction);
    scrollMenu->addAction(m_scrollRightAction);
    view->addMenu(scrollMenu);
    view->addAction(m_statusAction);
    view->addAction(m_switchAction);


    // Option Menu
    QMenu * option = new QMenu(tr("&Option"), this);
    mainMenu->addMenu(option);

    option->addAction(m_currentSessionAction);
    option->addSeparator();
    option->addAction(m_defaultAction);
    option->addAction(m_prefAction);

    // Special
    QMenu * spec = new QMenu(tr("&Special"), this);
    mainMenu->addMenu(spec);
    spec->addAction(m_copyArticleAction);
    spec->addAction(m_antiIdleAction);
    spec->addAction(m_autoReplyAction);
    spec->addAction(m_viewMessageAction);
    spec->addAction(m_beepAction);
    spec->addAction(m_mouseAction);
    spec->addAction(m_viewImageAction);


    //Script
    QMenu * script = new QMenu(tr("Scrip&t"), this);
    mainMenu->addMenu(script);
    script->addAction(m_scriptRunAction);
    script->addAction(m_scriptStopAction);

    //Window menu
    windowsMenu = new QMenu(tr("&Windows"), this);
    connect(windowsMenu, SIGNAL(aboutToShow()),
            this, SLOT(windowsMenuAboutToShow()));

    mainMenu->addMenu(windowsMenu);
    mainMenu->addSeparator();

    //Help menu
    QMenu * help = new QMenu(tr("&Help"), this);
    mainMenu->addMenu(help);
    help->addAction(m_aboutAction);
    help->addAction(m_homepageAction);

}

void Frame::updateMenuToolBar()
{
    Window * window = wndmgr->activeWindow();

    if (window == NULL)
        return;

    m_disconnectAction->setEnabled(window->isConnected());

    m_colorCopyAction->setChecked(window->m_bCopyColor);
    m_rectAction->setChecked(window->m_bCopyRect);
    m_autoCopyAction->setChecked(window->m_bAutoCopy);
    m_wwrapAction->setChecked(window->m_bWordWrap);


    m_fullAction->setChecked(Global::instance()->isFullScreen());

    m_antiIdleAction->setChecked(window->m_bAntiIdle);
    m_autoReplyAction->setChecked(window->m_bAutoReply);
    m_beepAction->setChecked(window->m_bBeep);
    m_mouseAction->setChecked(window->m_bMouse);

}

void Frame::enableMenuToolBar(bool enable)
{
    m_disconnectAction->setEnabled(enable);

    m_copyAction->setEnabled(enable);
    m_pasteAction->setEnabled(enable);
    m_colorCopyAction->setEnabled(enable);
    m_rectAction->setEnabled(enable);
    m_autoCopyAction->setEnabled(enable);
    m_wwrapAction->setEnabled(enable);

    m_fontAction->setEnabled(enable);
    m_colorAction->setEnabled(enable);
    m_refreshAction->setEnabled(enable);

    m_currentSessionAction->setEnabled(enable);

    m_copyArticleAction->setEnabled(enable);
    m_antiIdleAction->setEnabled(enable);
    m_autoReplyAction->setEnabled(enable);
    m_viewMessageAction->setEnabled(enable);
    m_beepAction->setEnabled(enable);
    m_mouseAction->setEnabled(enable);

    m_scriptRunAction->setEnabled(enable);
    m_scriptStopAction->setEnabled(enable);

    if (enable)
        mdiconnectTools->show();
    else
        mdiconnectTools->hide();

    return;
}

void Frame::updateKeyToolBar()
{
    key->clear();
    key->addAction(QPixmap(Global::instance()->pathPic() + "pic/keys.png"), tr("Key Setup"), this, SLOT(keySetup()));

    Config * conf = Global::instance()->fileCfg();
    QString strItem, strTmp;
    strTmp = conf->getItemValue("key", "num");
    int num = strTmp.toInt();

    for (int i = 0; i < num; i++) {
        strItem = QString("name%1").arg(i);
        strTmp = conf->getItemValue("key", strItem);
        ToolButton *button = new ToolButton(key, i, strTmp);
//   button->setUsesTextLabel(true);
        button->setText(strTmp);
//   button->setTextPosition(QToolButton::BesideIcon);
        strItem = QString("key%1").arg(i);
        strTmp = (conf->getItemValue("key", strItem));
//   QToolTip::add( button, strTmp.mid(1) );
//   button->addToolTip(strTmp.mid(1));
        connect(button, SIGNAL(buttonClicked(int)), this, SLOT(keyClicked(int)));
        key->addWidget(button);
    }
}

QString Frame::valueToString(bool shown, int dock, int index, bool nl, int extra)
{
    QString str = "";

    str = QString("%1 %2 %3 %4 %5").arg(shown ? 1 : 0).arg(dock).arg(index).arg(nl ? 1 : 0).arg(extra);

    return str;
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

QAction * Frame::insertThemeItem(const QString & themeitem)
{
    QAction * idAction = new QAction(themeitem, this);
    themesMenu->addAction(idAction);
    idAction->setCheckable(true);
    idAction->setChecked(themeitem == theme);
    return idAction;
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


    tray = new QSystemTrayIcon(this);   //pathLib+"pic/qterm_tray.png", "QTerm", trayMenu, this);
    tray->setIcon(QPixmap(Global::instance()->pathPic() + "pic/qterm_tray.png"));
    tray->setContextMenu(trayMenu);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
//  connect(tray, SIGNAL(closed()), this, SLOT(exitQTerm()));

    tray->show();
}

void Frame::buildTrayMenu()
{
    if (!trayMenu)
        return;
    trayMenu->clear();

    if (isHidden())
        trayMenu->addAction(tr("Show"), this, SLOT(trayShow()));
    else
        trayMenu->addAction(tr("Hide"), this, SLOT(trayHide()));
    trayMenu->addSeparator();
    trayMenu->addAction(tr("About"), this, SLOT(aboutQTerm()));
    trayMenu->addAction(tr("Exit"), this, SLOT(exitQTerm()));
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
    raise();
    activateWindow();
}

void Frame::trayHide()
{
    hide();
}

void Frame::buzz()
{
    int xp = x();
    int yp = y();
    QTime t;

    t.start();
    for (int i = 32; i > 0;) {
        //QApplication::processEvents();
        qApp->processEvents();
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
}

#include <qtermframe.moc>
