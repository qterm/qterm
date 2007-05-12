#ifndef QTERMBUFFER_H
#define QTERMBUFFER_H

#include "qterm.h"

#include <QObject>
#include <QList>
#include <QPoint>
#include <QSize>


#define INSERT_MODE	0
#define	NEWLINE_MODE	1


class QTermTextLine;
class QString;
class QByteArray;
class QRect;

class QTermBuffer : public QObject
{
	Q_OBJECT
	
public:
	QTermBuffer( QSize, int limit=10240, QTerm::Mode mod=QTerm::BBS );
	~QTermBuffer();

	QTermTextLine * absAt( int );
	QTermTextLine * scrAt( int );

	void setSize( QSize );
    QSize size();
    
   	int lines();

	QPoint absCaret();
	QPoint scrCaret();

	// string
	void setBuffer( const QByteArray& );
	
	// attribute
	void setCurrentAttr( short );
    void setAttr(short);
    	
	// string functions
	void deleteStr( int );
	void insertStr( int );
	void eraseStr( int );

	void deleteLine( int );
	void insertLine( int );

    void splitLine();
    void mergeLine();
    
	void eraseToBeginLine();
	void eraseToEndLine();
	void eraseEntireLine();

	void eraseToBeginScreen();
	void eraseToEndScreen();
	void eraseEntireScreen();


	// caret functions
	void moveCaret( int, int );
	void setCaret( int, int );
	void saveCaret();
	void restoreCaret();
	void cr();
	
	// non-printing characters
	void lf();
	void tab();
	void setMargins( int, int );

	// mode set
	void setMode( int );
	void resetMode( int );
    bool getMode(int);
    
	// for test
	void startDecode();
	void endDecode();

	// selecting function
	void setSelection( const QPoint&, const QPoint&, bool );
	void clearSelection();
	bool isSelected( const QPoint&, bool );
	bool isSelected( int );
	QByteArray getSelection( bool, bool, const QByteArray& );
	QRect getSelectionRect( int, bool );

signals:
	void bufferSizeChanged();
	void windowSizeChanged(int,int);

protected:	
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
	int column, row;
	// absolute position of the first screen line 
	int screenY;
	// the maximum lines holding (BBS and CONSOLE)
	int maxLines;
	// page margins, which define the active area
	int topMargin, bottomMargin;


	// the actual data
	QList<QTermTextLine*>  listLines;

	// caret indicator
	int caretX, caretY, caretSavedX, caretSavedY;
	// attribute
	short currentAttr;

	// ANSI modes
	bool Insert_Mode, NewLine_Mode;

	// data type: bbs, editor, etc
	QTerm::Mode mode;
	
	// selection support
	QPoint ptSelStart;
	QPoint ptSelEnd;
    bool bRect
    
	// for test
	int oldCaretX, oldCaretY;

};
#endif	// QTERMBUFFER_H
