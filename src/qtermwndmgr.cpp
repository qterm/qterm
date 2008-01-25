/*******************************************************************************
FILENAME:      qtermwndmgr.cpp
REVISION:      2001.8.12 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qtermwindow.h"
#include "qtermwndmgr.h"
#include "qtermframe.h"

// #include <qapplication.h>
// #include <qtabbar.h>
// #include <qicon.h>
// #include <qtimer.h>

#include <QString>
#include <QApplication>
#include <QTabBar>
#include <QIcon>
#include <QTimer>
// #if (QT_VERSION>=300)
// #include <q3ptrlist.h>
// #else
// #include <qlist.h>
// #endif
// 
// #include <qglobal.h>

#include <stdio.h>
namespace QTerm
{
extern QString pathLib;


//constructor
WndMgr::WndMgr( QObject * parent, const char * name )
	:QObject(parent)
{
	setObjectName(name);
	pFrame = (Frame *) parent;
	
	nCurrentIndex = -1;
	
	removed = false;

	//pWin.setAutoDelete( false );
	//pTab.setAutoDelete( false );
	//pIcon.setAutoDelete( false );
}

//destructor
WndMgr::~WndMgr()
{
}

//add window-tab-iconset 
int   WndMgr::addWindow(Window * mw,const QString& qtab, QIcon * icon)
{
  
	Q_ASSERT(mw!=NULL);
	pWin.append(mw);
	Q_ASSERT(qtab!=QString::QString());
	pTab.append(qtab);
	pIcon.append(icon);

	if( pWin.count()==1 )
		pFrame->enableMenuToolBar( true );

	return pTab.count();
}

//remove window-tab-iconset
void WndMgr::removeWindow(Window * mw)
{
	//find where it is
	int n=pWin.indexOf(mw);
	QString qtab=pTab.at(n);
	QIcon * qicon=pIcon.at(n);
	//remove them from list
	pTab.removeAll(qtab);
	pWin.removeAll(mw);
	pIcon.removeAll(qicon);

	if( pWin.count()==0 )
	{
		nCurrentIndex = -1;
		pFrame->enableMenuToolBar( false );
	}
	
	removed = true;
	//remove from the Tabbar
	pFrame->tabBar->removeTab(n);	
}
//avtive the tab when switch the window
void WndMgr::activateTheTab(Window * mw)
{
	//find where it is
	int n=pWin.indexOf(mw);

	if( n==nCurrentIndex )
		return;
	
	mw->setFocus();
	mw->show();
	mw->refresh();
	
	nCurrentIndex = n;

	QString qtab=pTab.at(n);
	//set it seleted
	
	pFrame->tabBar->setCurrentIndex(n);

	pFrame->updateMenuToolBar();
}
//active the window when switch the tab
void WndMgr::activateTheWindow(const QString& qtab)
{

	//find where it is
	int n=pTab.indexOf(qtab);

	if( n==nCurrentIndex )
		return;

	nCurrentIndex = n;

	Window * mw=pWin.at(n);

	//set focus to it
	mw->setFocus();
	mw->show();
	mw->refresh();
	
	pFrame->updateMenuToolBar();
}
//blink the tab when message come in
void WndMgr::blinkTheTab(Window * mw,bool bVisible)
{
	//find where it is
	int n=pWin.indexOf(mw);
	QIcon* icon=pIcon.at(n);
	//FIXME: QIcon::Automatic
	if(bVisible)
		icon->addFile(pathLib+"pic/tabpad.png");//,QIcon::Automatic);
	else
		icon->addFile(pathLib+"pic/transp.png");//,QIcon::Automatic);

	pFrame->tabBar->update();		
}
//return the number of connected window
int WndMgr::count()
{
	return pWin.count();
}

Window * WndMgr::activeWindow()
{
	if( nCurrentIndex == -1 )
		return NULL;
	else
		return pWin.at( nCurrentIndex );
}

void WndMgr::activeNextPrev(bool next)
{
	int n = nCurrentIndex;
	
	if(n==-1)
		return;
	if(next)
		n = (n==pWin.count()-1)?0:n+1;
	else
		n = (n==0)?pWin.count()-1:n-1;
	
	nCurrentIndex = n;

	Window * mw=pWin.at(n);
	mw->setFocus();

	QString qtab=pTab.at(n);
	//set it seleted
	pFrame->tabBar->setCurrentIndex(n);
	pFrame->updateMenuToolBar();

}

bool WndMgr::afterRemove()
{
	if(removed) 
	{
		removed=false;
		return true;
	}
	else
		return false;
}	

} // namespace QTerm

#include <qtermwndmgr.moc>
