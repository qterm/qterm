#ifndef QTERMBBS_H
#define QTERMBBS_H

#include <qpoint.h>
#include <qrect.h>

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

	QCString getUrl();
	QCString getIP();
	int getPageState();
	char getMenuChar();
	QRect getSelectRect();
	int getCursorType(const QPoint&);
	QCString getMessage();
	
protected:
	bool isUnicolor( QTermTextLine * );
	bool isIllChar(char);
	QTermBuffer *m_pBuffer;

	QRect m_rcUrl;
	QCString m_cstrUrl;
	QCString m_cstrIP;
	char m_cMenuChar;
	int m_nPageState;
	QPoint m_ptCursor;
	int m_nScreenStart;
};


#endif // QTERMBBS_H
