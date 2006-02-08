/*******************************************************************************
FILENAME:	qtermbuffer.cpp
REVISION:	2002.6.15 first created.
         
AUTHOR:		kingson		
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 ******************************************************************************/
#include "qtermbuffer.h"

#include "qterm.h"
#include "qtermtextline.h"


//Added by qt3to4:
#include <QString>
#include <QRect>
#include <QRegExp>
// #include <stdlib.h>
// #include <stdio.h>
// #include <math.h>


QTermBuffer::QTermBuffer( int line, int column, int limit )
{
	m_lin = line;
	m_col = column;
	m_limit = limit;
	
	m_lines = 0;

	while(  m_lineList.count()< m_lin )
		 m_lineList.append( new QTermTextLine );

// 	m_lineList.setAutoDelete(true);

	m_curAttr = SETCOLOR( NO_COLOR ) | SETATTR( NO_ATTR );


	m_caretX = 0;	
	m_caretY = 0;

	m_oldCaretX = 0;	
	m_oldCaretY = 0;


	m_top = 0;
	m_bottom = m_lin -1;
	
	Insert_Mode = false;
	NewLine_Mode = false;
	
	m_ptSelStart = QPoint(-1,-1);
	m_ptSelEnd = QPoint(-1,-1);

}


QTermBuffer::~QTermBuffer()
{

}

void QTermBuffer::setSize(int col, int lin)
{
	if(m_col==col && m_lin==lin)
		return;

	if(m_lin<lin)
		for(int i=0; i<lin-m_lin; i++ )
			m_lineList.append( new QTermTextLine );
	else if(m_lin>lin)
		for(int i=0; i<m_lin-lin; i++ )
			delete m_lineList.takeAt(m_lines+m_top);
	
	m_col = col;
	m_lin = lin;
	
	m_top = 0;
	m_bottom = m_lin -1;

	m_caretY = QMIN(m_caretY, lin-1);
	m_oldCaretY = QMIN(m_caretY, lin-1);

	clearSelect();

	emit windowSizeChanged(m_col,m_lin);

	emit bufferSizeChanged();
}

int QTermBuffer::columns()
{
	return m_col;
}

int QTermBuffer::lines()
{
	return m_lin+m_lines;
}
int QTermBuffer::line()
{
	return m_lin;
}

QTermTextLine * QTermBuffer::at( int y )
{
	return m_lineList.value( y , NULL);
}

QTermTextLine * QTermBuffer::screen( int y )
{
	return m_lineList.value( y+m_lines , NULL);
}

void QTermBuffer::setCurAttr( short attr )
{
	m_curAttr = attr;
}

void QTermBuffer::setBuffer( const QByteArray& cstr, int n )
{
	
	QTermTextLine * line =  m_lineList.value( m_lines + m_caretY, NULL );
	
	if(line==NULL)
	{	
		qWarning("setBuffer null line");
		return;
	}

	if ( Insert_Mode /*bInsert*/ )
		line->insertText( cstr, m_curAttr, m_caretX );
	else
		line->replaceText( cstr, m_curAttr, m_caretX );

	moveCursorOffset( n, 0 );
}
//nextline
void QTermBuffer::newLine()
{
	if( NewLine_Mode )
	{
		moveCursor( 0, m_caretY );
		return;
	}
	
	if( m_caretY == m_bottom )
	{
		if( m_bottom == m_lin -1 )
			addHistoryLine(1);
		else
			scrollLines( m_top, 1 );
	}
	else 
		if (m_caretY < m_lin-1 )
			moveCursorOffset( 0, 1 );

}

//table
void QTermBuffer::tab()
{
	int x = ( m_caretX + 8 ) & -8;	// or ( caretX/8 + 1 ) * 8 
	moveCursor( x, m_caretY );

	QByteArray cstr;
	cstr.fill(' ',x-m_caretX);
	setBuffer(cstr,x-m_caretX);
}

void QTermBuffer::setMargins( int top, int bottom )
{
	m_top = QMAX(top-1,0);
	m_bottom = QMIN(QMAX(bottom - 1,0), m_lin-1);
}

// cursor functions
void QTermBuffer::moveCursor( int x, int y )
{
	// detect index boundary	
	if ( x >= m_col )
		x = m_col;
	if ( x < 0 )
		x = 0;

	int stop = m_caretY<m_top?0:m_top;
	if ( y < stop )
		y = stop;

	stop  = m_caretY>m_bottom?m_lin-1:m_bottom;
	if ( y > stop )
		y = stop;


	m_caretX = x;
	m_caretY = y;

}

// 
void QTermBuffer::restoreCursor()
{
	moveCursor( m_saveX, m_saveY );
}

void QTermBuffer::moveCursorOffset( int x, int y )
{
	moveCursor( m_caretX+x, m_caretY+y );
}

void QTermBuffer::saveCursor()
{
	m_saveX = m_caretX; 
	m_saveY = m_caretY;
}

void QTermBuffer::cr()
{
	m_caretX = 0;
}

// the screen oriented caret
QPoint QTermBuffer::caret()
{
	return QPoint( m_caretX, m_caretY );
}

// the buffer oriented caret
int QTermBuffer::caretX()
{
	return m_caretX;
}
int QTermBuffer::caretY()
{
	return m_caretY + m_lines;
}

// erase functions
void QTermBuffer::eraseStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_lines );

	int x = line->getLength() - m_caretX;
	
	clearArea( m_caretX, m_caretY, QMIN(n, x), 1, m_curAttr );
}
// delete functions
void QTermBuffer::deleteStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_lines );

	int x = line->getLength() - m_caretX;

	if ( n >= x )
		clearArea( m_caretX, m_caretY, x, 1, m_curAttr );
	else
		shiftStr( m_caretY, m_caretX, x, -n );
}
// insert functions
void QTermBuffer::insertStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_lines );

	int x = line->getLength() - m_caretX;

	if ( n >= x )
		clearArea( m_caretX, m_caretY, x, m_caretY, m_curAttr );
	else
		shiftStr( m_caretY, m_caretX, x, n );
}

// shift str in one line
// num > 0 shift right
void QTermBuffer::shiftStr( int index, int startX, int len, int num )
{
	if ( !num )
		return;
	
	QByteArray cstr;
	cstr.fill( ' ', abs( num ) );
	
	QTermTextLine * line = m_lineList.at( index+m_lines );

	if ( num > 0 )	// insert
	{
		line->insertText( cstr, m_curAttr, startX );
		return;
	}

	if ( len + startX > line->getLength() )
		len = line->getLength() - startX;

	if ( num < 0 )	// delete
	{
		line->deleteText( startX + num, -num );
		line->insertText( cstr, m_curAttr, startX + len + num );	
		return;
	} 							
}


void QTermBuffer::deleteLine( int n )
{
	int y = m_bottom - m_caretY;

	if ( n >= y )
		clearArea( 0, m_caretY, -1, y, m_curAttr );
	else
		scrollLines( m_caretY, n );
}

void QTermBuffer::insertLine( int n )
{
	int y = m_bottom - m_caretY;
	
	if ( n >= y )
		clearArea( 0, m_caretY, -1, y, m_curAttr );
	else
		scrollLines( m_caretY, -n );
}

void QTermBuffer::eraseToEndLine()
{
	clearArea( m_caretX, m_caretY, -1, 1, m_curAttr );
}

void QTermBuffer::eraseToBeginLine()
{
	clearArea( 0, m_caretY, m_caretX, 1, m_curAttr );
}

void QTermBuffer::eraseEntireLine()
{
	clearArea( 0, m_caretY, -1, 1, m_curAttr );
}
//scroll line
//num > 0 scroll up
void QTermBuffer::scrollLines( int startY, int num )
{
	if ( !num )
		return;
	
	if ( num > 0 )	// delete
	{
		while ( num )
		{
			delete m_lineList.takeAt( m_lines+startY );
			m_lineList.insert( m_lines+m_bottom, new QTermTextLine );
			num--;
		}
	}

	if ( num < 0 )	// insert
	{
		while ( num )
		{
			delete m_lineList.takeAt( m_lines+m_bottom );
			m_lineList.insert( m_lines+startY, new QTermTextLine );
			num++;
		}
	}

	for(int i=m_lines+startY; i<m_lines+m_bottom; i++)
		m_lineList.at(i)->setChanged(-1,-1);
	
}

void QTermBuffer::eraseToEndScreen()
{
	if( m_caretX==0 && m_caretY ==0 )
	{
		eraseEntireScreen();
		return;
	}

	clearArea( m_caretX, m_caretY, -1, 1, m_curAttr ); // erase to end line

	if ( m_caretY < m_bottom - 1 )	// erase 
		clearArea( 0, m_caretY + 1, -1, m_bottom - m_caretY - 1, m_curAttr );

}
void QTermBuffer::eraseToBeginScreen()
{
	if( m_caretX==m_col-1 && m_caretY ==m_lin-1 )
	{
		eraseEntireScreen();
		return;
	}
	
	clearArea( 0, m_caretY, m_caretX, 1, m_curAttr );	// erase to begin line
	if ( m_caretY > 0 )		// erase
		clearArea( 0, 0, -1, m_caretY - 1, m_curAttr );

}
void QTermBuffer::eraseEntireScreen()
{
	addHistoryLine( m_lin );

	clearArea( 0, 0, m_col, m_bottom, m_curAttr );
}
// width = -1 : clear to end
void QTermBuffer::clearArea(int startX, int startY, int width, int height, short attr )
{
	QByteArray cstr;

	QTermTextLine * line;
	
	if ( startY < m_top )
		startY = m_top;

	if ( height > (int)( m_bottom - startY +1 ) )
		height = m_bottom - startY;

	for ( int i = startY; i < height + startY; i++ )
	{
		line = m_lineList.at( i + m_lines );
		
		if(width == -1)
		{
			cstr.fill(' ',line->getLength() - startX );
		}
		else
			cstr.fill(' ',width);
		line->replaceText( cstr, attr, startX );
	}

}

void QTermBuffer::addHistoryLine( int n )
{
	while( n )
	{
		if( m_lines == m_limit )
		{
			m_lineList.remove(uint(0));
			//m_ptSelStart.setY( m_ptSelStart.y()-1 );
			//m_ptSelEnd.setY( m_ptSelEnd.y()-1 );
			//if(m_ptSelStart.y()<0)
			//	m_ptSelStart=m_ptSelEnd=QPoint(-1,-1);
		}
		
		m_lineList.append( new QTermTextLine );
		m_lines = QMIN(m_lines+1,m_limit);
		n--;
	}
	
	for(int i=m_lines+0; i<m_lines+m_bottom; i++)
		m_lineList.at(i)->setChanged(-1,-1);

	emit bufferSizeChanged();
}

void QTermBuffer::startDecode()
{
	m_oldCaretX = m_caretX;
	m_oldCaretY = m_caretY;
	
}

void QTermBuffer::endDecode()
{
	QTermTextLine * line = m_lineList.at( m_oldCaretY+m_lines );

	line->setChanged( m_oldCaretX, m_oldCaretX+1 );

	line = m_lineList.at( m_caretY+m_lines );

	line->setChanged( m_caretX, m_caretX+1 );

	clearSelect();
	
}

void QTermBuffer::setMode( int mode )
{
	switch( mode )
	{
		case INSERT_MODE:
			Insert_Mode = true;
			break;
		case NEWLINE_MODE:
			NewLine_Mode = true;
			break;
		default:
			break;
	}

}
void QTermBuffer::resetMode( int mode )
{
	switch( mode )
	{
		case INSERT_MODE:
			Insert_Mode = false;
			break;
		case NEWLINE_MODE:
			NewLine_Mode = false;
			break;
		default:
			break;
	}
}

void QTermBuffer::setSelect( const QPoint& pt1, const QPoint& pt2, bool rect )
{
	QPoint ptSelStart, ptSelEnd;
	
	if(pt1.y()==pt2.y())
	{
		ptSelStart = pt1.x()<pt2.x() ? pt1 : pt2;
		ptSelEnd = pt1.x()>pt2.x() ? pt1 : pt2;
	}
	else
	{
		ptSelStart = pt1.y()<pt2.y() ? pt1 : pt2;
		ptSelEnd = pt1.y()>pt2.y() ? pt1 : pt2;
	}

	if( m_ptSelStart == m_ptSelEnd )	// the first
	{
		for(int i=ptSelStart.y(); i<=m_ptSelEnd.y(); i++)
			at(i)->setChanged(-1,-1);
	}
	else	// find the changed area
	{
		if(rect)
		{
			for( int i=QMIN(ptSelStart.y(),m_ptSelStart.y()); i<=QMAX(ptSelEnd.y(), m_ptSelEnd.y()); i++)
				at(i)->setChanged(-1,-1);
		}
		else
		{
			if( ptSelStart==m_ptSelStart )
				for(int i=QMIN(ptSelEnd.y(), m_ptSelEnd.y()); i<=QMAX(ptSelEnd.y(), m_ptSelEnd.y()); i++)
					at(i)->setChanged(-1,-1);
			else
				if( ptSelEnd==m_ptSelEnd )
					for(int i=QMIN(ptSelStart.y(), m_ptSelStart.y()); i<=QMAX(ptSelStart.y(), m_ptSelStart.y()); i++)
						at(i)->setChanged(-1,-1);
				else
					for(int i=QMIN(ptSelStart.y(), m_ptSelStart.y()); i<=QMAX(ptSelEnd.y(), m_ptSelEnd.y()); i++)
						at(i)->setChanged(-1,-1);

		}
	}
	
	m_ptSelStart = ptSelStart;
	m_ptSelEnd = ptSelEnd;
}

void QTermBuffer::clearSelect()
{
	if( m_ptSelStart == m_ptSelEnd )
		return;

	for(int i=m_ptSelStart.y(); i<=m_ptSelEnd.y(); i++ )
		at(i)->setChanged(-1,-1);

	m_ptSelStart=m_ptSelEnd=QPoint(-1,-1);
}

bool QTermBuffer::isSelected( int index )
{
	if( m_ptSelStart == m_ptSelEnd )
		return false;
	else
		return index>=m_ptSelStart.y() && index<=m_ptSelEnd.y();
}

bool QTermBuffer::isSelected( const QPoint& pt, bool rect )
{
	if( m_ptSelStart == m_ptSelEnd )
		return false;

	if(rect)
	{
		int x1, x2;
		x1=QMIN(m_ptSelStart.x(), m_ptSelEnd.x());
		x2=QMAX(m_ptSelStart.x(), m_ptSelEnd.x());
		return pt.x()<=x2 && pt.x()>=x1 && pt.y()>=m_ptSelStart.y() && pt.y()<=m_ptSelEnd.y();
	}

	if( pt.y()==m_ptSelStart.y() && m_ptSelStart.y()==m_ptSelEnd.y() )
		return pt.x()>=m_ptSelStart.x() && pt.x()<=m_ptSelEnd.x();
	else
		if( pt.y()==m_ptSelStart.y() )
			return pt.x()>=m_ptSelStart.x();
		else 
			if( pt.y()==m_ptSelEnd.y() )
				return pt.x()<=m_ptSelEnd.x();
			else
				return pt.y()>m_ptSelStart.y() && pt.y()<m_ptSelEnd.y();
	
}

QByteArray QTermBuffer::getSelectText( bool rect, bool color, const QByteArray& escape )
{
	QByteArray cstrSelect="";
	
	if(m_ptSelStart == m_ptSelEnd )
		return cstrSelect;

	QRect rc;
	QString strTemp;
	
	for( int i=m_ptSelStart.y(); i<=m_ptSelEnd.y(); i++ )
	{
		rc = getSelectRect( i, rect );

		if(color)
			strTemp = QString::fromLatin1(m_lineList.at(i)->getAttrText( rc.left(), rc.width(), escape ));
		else
			strTemp = QString::fromLatin1(m_lineList.at(i)->getText( rc.left(), rc.width() ));
		
		//FIXME: potential problem?
		int pos=strTemp.findRev(QRegExp("[\\S]"));
		strTemp.truncate(pos+1);
		cstrSelect += strTemp.toLatin1();
		
		// add newline except the last line
		if(i != m_ptSelEnd.y())
		{
			#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
			cstrSelect += '\r';
			#endif
			cstrSelect += '\n';
		}
	}

	return cstrSelect;
}

QRect QTermBuffer::getSelectRect( int index, bool rect )
{
	if(rect)
		return QRect( QMIN(m_ptSelStart.x(),m_ptSelEnd.x()), index, abs(m_ptSelEnd.x()-m_ptSelStart.x())+1, 1);
	else
		if( m_ptSelStart.y()==m_ptSelEnd.y() )
				return QRect( m_ptSelStart.x(), index, QMIN(m_ptSelEnd.x(),m_col)-m_ptSelStart.x()+1, 1);
		else
			if( index==m_ptSelStart.y() )
				return QRect( m_ptSelStart.x(), index, QMAX(0,m_col-m_ptSelStart.x()), 1);
			else 
				if( index==m_ptSelEnd.y() )
					return QRect( 0, index, QMIN(m_col, m_ptSelEnd.x()+1), 1);
				else
					return QRect( 0, index, m_col, 1);
}
#include <qtermbuffer.moc>
