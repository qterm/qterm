#ifndef QTERMBUFFER_H
#define QTERMBUFFER_H

#include <qglobal.h>

#if (QT_VERSION>=300)
#include <qptrlist.h>
#else
#include <qlist.h>
#endif

#include <qobject.h>

#define INSERT_MODE	0
#define	NEWLINE_MODE	1


class QTermTextLine;
class QCString;

class QTermBuffer : public QObject
{
	Q_OBJECT
	
public:
	QTermBuffer( int, int, int );
	~QTermBuffer();

	QTermTextLine * at( int );
	QTermTextLine * screen( int );

	void setSize( int, int );

	int columns(); 
	int lines();
	int line();
		
	int  caretX();
	int  caretY();

	QPoint caret();

	// string
	void setBuffer( const QCString&, int n );
	
	// attribute
	void setCurAttr( short );
	
	// string functions
	void deleteStr( int );
	void insertStr( int );
	void eraseStr( int );

	void deleteLine( int );
	void insertLine( int );

	void eraseToBeginLine();
	void eraseToEndLine();
	void eraseEntireLine();

	void eraseToBeginScreen();
	void eraseToEndScreen();
	void eraseEntireScreen();


	// cursor functions
	void moveCursorOffset( int, int );
	void moveCursor( int, int );
	void saveCursor();
	void restoreCursor();
	void cr();
	
	// non-printing characters
	void newLine();
	void tab();
	void setMargins( int, int );

	// mode set
	void setMode( int );
	void resetMode( int );

	// for test
	void startDecode();
	void endDecode();

	// selecting function
	void setSelect( const QPoint&, const QPoint&, bool );
	void clearSelect();
	bool isSelected( const QPoint&, bool );
	bool isSelected( int );
	QCString getSelectText( bool, bool, const QCString& );
	QRect getSelectRect( int, bool );

signals:
	
	void bufferSizeChanged();
	
protected:	
	void shiftStr( int, int, int, int );
	void scrollLines( int, int );
	void clearArea( int, int, int, int, short );
	void addHistoryLine( int );
	
	// margins
	int m_top, m_bottom;

	//
	int m_col, m_lin, m_lines, m_limit;

	QTermTextLine * m_pCurrentLine;
#if (QT_VERSION>=300)
	QPtrList<QTermTextLine>  m_lineList;
#else
	QList<QTermTextLine>  m_lineList;
#endif
	
	// caret
	int   m_caretX, m_caretY, m_saveX, m_saveY;
	
	// attribute
	short m_curAttr;

	// Modes
	bool Insert_Mode, NewLine_Mode;

	// for test
	int m_oldCaretX, m_oldCaretY;
	
	// selecting 
	QPoint m_ptSelStart, m_ptSelEnd;

	bool m_bPage;
};
#endif	// QTERMBUFFER_H
