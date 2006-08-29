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

#include "qtermtextline.h"


//Added by qt3to4:
#include <QString>
#include <QRect>
#include <QRegExp>


QTermBuffer::QTermBuffer( int row, int column, int limit, QTerm_Mode mode )
{
	m_row = row;
	m_col = column;
	m_limit = limit;
	
	m_screenY = 0;

	while(  m_lineList.count()< m_row )
		 m_lineList.append( new QTermTextLine );

// 	m_lineList.setAutoDelete(true);

	// initialize variables
	m_curAttr = SETCOLOR( NO_COLOR ) | SETATTR( NO_ATTR );

	m_caretX = 0;	
	m_caretY = 0;
	m_oldCaretX = 0;	
	m_oldCaretY = 0;

	// the whole screen used
	m_top = 0;
	m_bottom = m_row -1;
	
	Insert_Mode = false;
	NewLine_Mode = false;
	
	m_ptSelStart = QPoint(-1,-1);
	m_ptSelEnd = QPoint(-1,-1);

}


QTermBuffer::~QTermBuffer()
{

}

void QTermBuffer::setSize(int col, int row)
{
	if(m_col==col && m_row==row)
		return;

	if(m_row<row)
		for(int i=0; i<row-m_row; i++ )
			m_lineList.append( new QTermTextLine );
	else if(m_row>row)
		for(int i=0; i<m_row-row; i++ )
			delete m_lineList.takeAt(m_screenY+m_top);
	
	m_col = col;
	m_row = row;
	
	m_top = 0;
	m_bottom = m_row -1;

	m_caretY = qMin(m_caretY, row-1);
	m_oldCaretY = qMin(m_caretY, row-1);

	clearSelect();

	emit windowSizeChanged(m_col,m_row);

	emit bufferSizeChanged();
}

int QTermBuffer::column()
{
	return m_col;
}

int QTermBuffer::lines()
{
	return m_screenY+m_row;
}
int QTermBuffer::row()
{
	// for BBS, row is defined as the visble area
	if(mode==BBS)
		return m_row;
	// for others, it is the whole editing area.
	if(mode==EDITOR || mode==VIEWER)
		return m_lineList.count();
}

// take by the absolute index
QTermTextLine * QTermBuffer::at( int y )
{
	return m_lineList.value( y , NULL);
}
// take by the relative (to screen) index
QTermTextLine * QTermBuffer::screen( int y )
{
	return m_lineList.value( m_screenY+y , NULL);
}

void QTermBuffer::setCurAttr( short attr )
{
	m_curAttr = attr;
}

void QTermBuffer::setBuffer( const QByteArray& cstr, int n )
{
	
	QTermTextLine * line =  m_lineList.value( m_screenY + m_caretY, NULL );
	
	if(line==NULL)
	{
		if( mode==BBS ) // out of BBS screen
		{
			qDebug("setBuffer: null line");
			return;
		}
		if( mode==EDITOR|| mode==VIEWER )// insert new line
		{
			while(m_caretY>=m_lineList.count())
				m_lineList.append(new QTermTextLine);
			line =  m_lineList.value( m_screenY + m_caretY, NULL );
		}
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
	// NewLine_Mode also has effects in EDITOR mode
	if( NewLine_Mode )
	{
		moveCursor( 0, m_caretY );
		return;
	}

	// for editor and viewer, always move to nextline
	// not limited by m_row
	if( mode == EDITOR || mode== VIEWER )
	{
		moveCursorOffset(0, 1);
		if( m_caretY==m_lineList.count() )
			m_lineList.append(new QTermTextLine);
		return;
	}


	if( m_caretY == m_bottom )
	{
		if( m_bottom == m_row -1 )
			addHistoryLine(1);
		else
			scrollLines( m_top, 1 );
	}
	else 
		if (m_caretY < m_row-1 )
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
	m_top = qMax(top-1,0);
	m_bottom = qMin(qMax(bottom - 1,0), m_row-1);
}

// cursor functions
void QTermBuffer::moveCursor( int x, int y )
{
	// FIXME: column limit is not so important, why not relax it?
	if ( x >= m_col )
		x = m_col;
	if ( x < 0 )
		x = 0;
	// row limit, only felt by BBS mode
	if( mode==BBS )
	{
		int stop = m_caretY<m_top?0:m_top;
		if ( y < stop )
			y = stop;
		stop  = m_caretY>m_bottom?m_row-1:m_bottom;
		if ( y > stop )
			y = stop;
	}

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

// carriage return
void QTermBuffer::cr()
{
	m_caretX = 0;
}

// the screen oriented caret
QPoint QTermBuffer::scrCaret()
{
	return QPoint( m_caretX, m_caretY );
}

// the buffer oriented caret
QPoint QTermBuffer::absCaret()
{
	return QPoint(m_caretX, m_screenY+m_caretY);
}

// erase functions
void QTermBuffer::eraseStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_screenY );
	
	if( line==NULL )
	{
		qDebug("QTermBuffer::eraseStr(%d) Null line",n);
		return;
	}

	int x = line->getLength() - m_caretX;
	
	clearArea( m_caretX, m_caretY, qMin(n, x), 1, m_curAttr );
}
// delete functions
void QTermBuffer::deleteStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_screenY );

	if( line==NULL )
	{
		qDebug("QTermBuffer::deleteStr(%d) Null line",n);
		return;
	}

	int x = line->getLength() - m_caretX;

	if ( n >= x )
		clearArea( m_caretX, m_caretY, x, 1, m_curAttr );
	else
		shiftStr( m_caretY, m_caretX, x, -n );
}
// insert functions
void QTermBuffer::insertStr( int n )
{
	QTermTextLine * line = m_lineList.at( m_caretY+m_screenY );

	if( line==NULL )
	{
		qDebug("QTermBuffer::insertStr(%d) Null line",n);
		return;
	}

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
	
	QTermTextLine * line = m_lineList.at( index+m_screenY );

	if( line==NULL )
	{
		qDebug("QTermBuffer::shiftStr() Null line");
		return;
	}

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
			delete m_lineList.takeAt( m_screenY+startY );
			m_lineList.insert( m_screenY+m_bottom, new QTermTextLine );
			num--;
		}
	}

	if ( num < 0 )	// insert
	{
		while ( num )
		{
			delete m_lineList.takeAt( m_screenY+m_bottom );
			m_lineList.insert( m_screenY+startY, new QTermTextLine );
			num++;
		}
	}

	for(int i=m_screenY+startY; i<m_screenY+m_bottom; i++)
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
	if( m_caretX==m_col-1 && m_caretY ==m_row-1 )
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
	addHistoryLine( m_row );

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
		line = m_lineList.at( i + m_screenY );
		
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
		if( m_screenY == m_limit )
		{
			m_lineList.removeAt(uint(0));
			//m_ptSelStart.setY( m_ptSelStart.y()-1 );
			//m_ptSelEnd.setY( m_ptSelEnd.y()-1 );
			//if(m_ptSelStart.y()<0)
			//	m_ptSelStart=m_ptSelEnd=QPoint(-1,-1);
		}
		
		m_lineList.append( new QTermTextLine );
		m_screenY = qMin(m_screenY+1,m_limit);
		n--;
	}
	
	for(int i=m_screenY+0; i<m_screenY+m_bottom; i++)
		m_lineList.at(i)->setChanged(-1,-1);

	emit bufferSizeChanged();
}

// for debug 
void QTermBuffer::startDecode()
{
	m_oldCaretX = m_caretX;
	m_oldCaretY = m_caretY;
	
}

void QTermBuffer::endDecode()
{
	QTermTextLine * line = m_lineList.at( m_oldCaretY+m_screenY );

	line->setChanged( m_oldCaretX, m_oldCaretX+1 );

	line = m_lineList.at( m_caretY+m_screenY );

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
			for( int i=qMin(ptSelStart.y(),m_ptSelStart.y()); i<=qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
				at(i)->setChanged(-1,-1);
		}
		else
		{
			if( ptSelStart==m_ptSelStart )
				for(int i=qMin(ptSelEnd.y(), m_ptSelEnd.y()); i<=qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
					at(i)->setChanged(-1,-1);
			else
				if( ptSelEnd==m_ptSelEnd )
					for(int i=qMin(ptSelStart.y(), m_ptSelStart.y()); i<=qMax(ptSelStart.y(), m_ptSelStart.y()); i++)
						at(i)->setChanged(-1,-1);
				else
					for(int i=qMin(ptSelStart.y(), m_ptSelStart.y()); i<=qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
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
		x1=qMin(m_ptSelStart.x(), m_ptSelEnd.x());
		x2=qMax(m_ptSelStart.x(), m_ptSelEnd.x());
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
		int pos=strTemp.lastIndexOf(QRegExp("[\\S]"));
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
		return QRect( qMin(m_ptSelStart.x(),m_ptSelEnd.x()), index, abs(m_ptSelEnd.x()-m_ptSelStart.x())+1, 1);
	else
		if( m_ptSelStart.y()==m_ptSelEnd.y() )
				return QRect( m_ptSelStart.x(), index, qMin(m_ptSelEnd.x(),m_col)-m_ptSelStart.x()+1, 1);
		else
			if( index==m_ptSelStart.y() )
				return QRect( m_ptSelStart.x(), index, qMax(0,m_col-m_ptSelStart.x()), 1);
			else 
				if( index==m_ptSelEnd.y() )
					return QRect( 0, index, qMin(m_col, m_ptSelEnd.x()+1), 1);
				else
					return QRect( 0, index, m_col, 1);
}
