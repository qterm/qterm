#ifndef QTERMWNDMGR_H
#define QTERMWNDMGR_H

#include <qobject.h>

#if (QT_VERSION>=300)
#include <qptrlist.h>
#else
#include <qlist.h>
#endif

class QTermWindow;
class QTab;
class QIconSet;

class QTermFrame;

class QTermWndMgr: public QObject
{
	Q_OBJECT

public:
	QTermWndMgr( QObject * parent=0, const char * name=0 );
	~QTermWndMgr();
	
	int  addWindow(QTermWindow *mw,QTab *qtab,QIconSet *icon);
	void removeWindow(QTermWindow *mw);
	void activateTheTab(QTermWindow *mw);
	void activateTheWindow(QTab *qtab);
	void blinkTheTab(QTermWindow *mw,bool bVisible);
	int  count();
	
	bool afterRemove();
	
	QTermWindow * activeWindow();
	void activeNextPrev(bool);

protected:
	QList<QTab>  pTab;
	QList<QIconSet> pIcon;
	QList<QTermWindow>  pWin;
	QTermFrame * pFrame;
	int nActive;

	bool removed;
};

#endif	//QTERMWNDMGR_H



