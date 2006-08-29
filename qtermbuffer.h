#ifndef QTERMBUFFER_H
#define QTERMBUFFER_H

#include <QList>
#include <QObject>

#include <QPoint>

#include "../frame/qterm.h"

#define INSERT_MODE	0
#define	NEWLINE_MODE	1

enum QTerm_Mode {BBS, EDITOR, VIEWER};

class QTermTextLine;
class QString;
class QByteArray;
class QRect;

class QTermBuffer : public QObject
{
	Q_OBJECT
	
public:
	QTermBuffer( int, int, int, QTerm_Mode=BBS );
	~QTermBuffer();

	QTermTextLine * at( int );
	QTermTextLine * screen( int );

	void setSize( int, int );

	int column(); 
	int row();
	int lines();

	QPoint absCaret();
	QPoint scrCaret();

	// string
	void setBuffer( const QByteArray&, int n );
	
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
	QByteArray getSelectText( bool, bool, const QByteArray& );
	QRect getSelectRect( int, bool );

signals:
	void bufferSizeChanged();
	void windowSizeChanged(int,int);

protected:	
	void shiftStr( int, int, int, int );
	void scrollLines( int, int );
	void clearArea( int, int, int, int, short );
	void addHistoryLine( int );
/*
 * simple illustration of buffer structure
 * 
 * 
 *   |       ......................
 *   |       ......................
 * l |       ......................
 * l |    [  ######################  --->m_screenY
 * i |    [  ###################### 
 * m |  r [  ###################### }--->m_top (relative to screen)
 * i |  o [  ###################### }
 * t |  w [  ###################### }
 *   |    [  ###################### }--->m_bottom (relative to screen)
 *   |    [  ######################
 *   |    [  ######################
 *           ======================
 *                   column
 */
	
	// buffer size
	int m_col, m_row;
	// absolute position of the first screen line 
	int m_screenY;
	// the maximum lines holding (BBS and CONSOLE)
	int m_limit;
	// page margins, which define the active area
	int m_top, m_bottom;


	// the actual data
	QList<QTermTextLine*>  m_lineList;
	QTermTextLine * m_pCurrentLine;

	// caret indicator
	int m_caretX, m_caretY, m_saveX, m_saveY;
	// attribute
	short m_curAttr;

	// ANSI modes
	bool Insert_Mode, NewLine_Mode;
	// data type: bbs, editor, etc
	QTerm_Mode mode;
	
	// selection support
	QPoint m_ptSelStart;
	QPoint m_ptSelEnd;

	// for test
	int m_oldCaretX, m_oldCaretY;

};
#endif	// QTERMBUFFER_H
