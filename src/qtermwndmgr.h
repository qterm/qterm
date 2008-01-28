#ifndef QTERMWNDMGR_H
#define QTERMWNDMGR_H

// #include <qobject.h>
// 
// #if (QT_VERSION>=300)
// #include <q3ptrlist.h>
// #else
// #include <qlist.h>
// #endif

#include <QString>
#include <QObject>
#include <QList>

//class QTab;
class QIcon;

namespace QTerm
{
class Frame;
class Window;

class WndMgr: public QObject
{
	Q_OBJECT

public:
	WndMgr( QObject * parent=0, const char * name=0 );
	~WndMgr();
	
	int  addWindow(Window *mw);
	void removeWindow(Window *mw);
	void activateTheTab(Window *mw);
	void activateTheWindow(int n);
	void blinkTheTab(Window *mw,bool bVisible);
	int  count();
	
	bool afterRemove();
	
	Window * activeWindow();
	void activeNextPrev(bool);

protected:
	QList<Window*>  pWin;
	Frame * pFrame;
	int nCurrentIndex;

	bool removed;
};

} // namespace QTerm

#endif	//QTERMWNDMGR_H

