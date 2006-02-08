#ifndef QTERMBBS_H
#define QTERMBBS_H

#include <qpoint.h>
#include <qrect.h>
//Added by qt3to4:
#include <QString>

class QTermBuffer;
class QRect;
class QTermTextLine;

class QTermBBS
{
public:
	QTermBBS( QTermBuffer * );
	~QTermBBS();

	/* -1 -- undefined
	 *  0 -- menu
	 *  1 -- article or board list
	 *  2 -- read
	 *  3 -- edit
	 */
	void setPageState();
	void setScreenStart(int);
	bool setCursorPos( const QPoint&, QRect& );
			
	bool isSelected( int );
	bool isSelected( const QPoint& );
	bool isUrl(QRect&,QRect&);
	bool isIP(QRect&, QRect&);
	bool isPageComplete();
	bool checkUrl(QRect&, QRect&, bool);

	QString getUrl();
	QString getIP();
	int getPageState();
	char getMenuChar();
	QRect getSelectRect();
	int getCursorType(const QPoint&);
	QString getMessage();
	
protected:
	bool isUnicolor( QTermTextLine * );
	bool isIllChar(char);
	QTermBuffer *m_pBuffer;

	QRect m_rcUrl;
	QString m_strUrl;
	QString m_strIP;
	char m_cMenuChar;
	int m_nPageState;
	QPoint m_ptCursor;
	int m_nScreenStart;
};


#endif // QTERMBBS_H
