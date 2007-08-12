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
QTermWndMgr::QTermWndMgr( QObject * parent, const char * name )
	:QObject(parent)
{
	setObjectName(name);
	pFrame = (QTermFrame *) parent;
	
	nActive = -1;
	
	removed = false;

	//pWin.setAutoDelete( false );
	//pTab.setAutoDelete( false );
	//pIcon.setAutoDelete( false );
}

//destructor
QTermWndMgr::~QTermWndMgr()
{
}

//add window-tab-iconset 
int   QTermWndMgr::addWindow(QTermWindow * mw,const QString& qtab, QIcon * icon)
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
void QTermWndMgr::removeWindow(QTermWindow * mw)
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
		nActive = -1;
		pFrame->enableMenuToolBar( false );
	}
	
	removed = true;
	//remove from the Tabbar
	pFrame->tabBar->removeTab(n);	
}
//avtive the tab when switch the window
void QTermWndMgr::activateTheTab(QTermWindow * mw)
{
	//find where it is
	int n=pWin.indexOf(mw);

	if( n==nActive )
		return;
	
	mw->showNormal();
	mw->showMaximized();
	
	nActive = n;

	QString qtab=pTab.at(n);
	//set it seleted
	
	pFrame->tabBar->setCurrentIndex(n);

	pFrame->updateMenuToolBar();
}
//active the window when switch the tab
void QTermWndMgr::activateTheWindow(const QString& qtab)
{

	//find where it is
	int n=pTab.indexOf(qtab);

	if( n==nActive )
		return;

	nActive = n;

	QTermWindow * mw=pWin.at(n);
	//set focus to it
//	#if (QT_VERSION>=0x030300)
	((QWidget*)pFrame->ws)->setFocus();
	mw->showNormal();
//	#else
//	mw->setFocus();
//	#endif
	
	pFrame->updateMenuToolBar();
}
//blink the tab when message come in
void QTermWndMgr::blinkTheTab(QTermWindow * mw,bool bVisible)
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
int QTermWndMgr::count()
{
	return pWin.count();
}

QTermWindow * QTermWndMgr::activeWindow()
{
	if( nActive == -1 )
		return NULL;
	else
		return pWin.at( nActive );
}

void QTermWndMgr::activeNextPrev(bool next)
{
	int n = nActive;
	
	if(n==-1)
		return;
	if(next)
		n = (n==pWin.count()-1)?0:n+1;
	else
		n = (n==0)?pWin.count()-1:n-1;
	
	nActive = n;

	QTermWindow * mw=pWin.at(n);
	//set focus to it
//	#if (QT_VERSION>=0x030300)
	((QWidget*)pFrame->ws)->setFocus();
	mw->showNormal();
//	#else
//	mw->setFocus();
//	#endif

	QString qtab=pTab.at(n);
	//set it seleted
	pFrame->tabBar->setCurrentIndex(n);
	pFrame->updateMenuToolBar();

}

bool QTermWndMgr::afterRemove()
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
