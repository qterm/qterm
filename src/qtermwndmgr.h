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
class QTermFrame;
class QTermWindow;

class QTermWndMgr: public QObject
{
	Q_OBJECT

public:
	QTermWndMgr( QObject * parent=0, const char * name=0 );
	~QTermWndMgr();
	
	int  addWindow(QTermWindow *mw, const QString & qtab, QIcon *icon);
	void removeWindow(QTermWindow *mw);
	void activateTheTab(QTermWindow *mw);
	void activateTheWindow(const QString & qtab);
	void blinkTheTab(QTermWindow *mw,bool bVisible);
	int  count();
	
	bool afterRemove();
	
	QTermWindow * activeWindow();
	void activeNextPrev(bool);

protected:
	QList<QString>  pTab;
	QList<QIcon*> pIcon;
	QList<QTermWindow*>  pWin;
	QTermFrame * pFrame;
	int nActive;

	bool removed;
};

} // namespace QTerm

#endif	//QTERMWNDMGR_H

