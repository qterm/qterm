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
#include "qtermglobal.h"

#include <QString>
#include <QApplication>
#include <QTabBar>
#include <QIcon>
#include <QTimer>

#include <stdio.h>

namespace QTerm
{

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
int WndMgr::addWindow(Window * mw)
{
	Q_ASSERT(mw!=NULL);
	pWin.append(mw);

	if( pWin.count()==1 )
		pFrame->enableMenuToolBar( true );

	return pWin.count();
}

//remove window-tab-iconset
void WndMgr::removeWindow(Window * mw)
{
	//find where it is
	int n=pWin.indexOf(mw);
	//remove them from list
	pWin.removeAll(mw);

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
	
	nCurrentIndex = n;

	//set it seleted
	
	pFrame->tabBar->setCurrentIndex(n);

	pFrame->updateMenuToolBar();
}

//active the window when switch the tab
void WndMgr::activateTheWindow(int n)
{
	if( n==nCurrentIndex )
		return;

	nCurrentIndex = n;

	Window * mw=pWin.at(n);

	//set focus to it
	mw->setFocus();
	mw->show();

	pFrame->updateMenuToolBar();
}

//FIXME: how does this suppose to work?
//blink the tab when message come in
void WndMgr::blinkTheTab(Window * mw,bool bVisible)
{
//	//find where it is
//	int n=pWin.indexOf(mw);
//	QIcon* icon=pIcon.at(n);
//	//FIXME: QIcon::Automatic
//	if(bVisible)
//		icon->addFile(pathLib+"pic/tabpad.png");//,QIcon::Automatic);
//	else
//		icon->addFile(pathLib+"pic/transp.png");//,QIcon::Automatic);
//
//	pFrame->tabBar->update();
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
