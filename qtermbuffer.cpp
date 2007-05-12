#include "qtermbuffer.h"
#include "qtermtextline.h"


#include <QRect>
#include <QRegExp>


QTermBuffer::QTermBuffer( QSize szWin, int limit, QTerm::Mode mod )
{
	column = szWin.width();
	row = szWin.height();
	maxLines = limit;
	mode= mod;
	
	screenY = 0;

	while( listLines.count()< row )
		 listLines.append( new QTermTextLine );

	// initialize variables
	currentAttr = SETCOLOR( NO_COLOR ) | SETATTR( NO_ATTR );

	caretX = 0;	
	caretY = 0;
	oldCaretX = 0;	
	oldCaretY = 0;

	// the whole screen used
	topMargin = 0;
	bottomMargin = row -1;

	Insert_Mode = false;
	NewLine_Mode = false;
	
	ptSelStart = QPoint(-1,-1);
	ptSelEnd = QPoint(-1,-1);

}


QTermBuffer::~QTermBuffer()
{

}

void QTermBuffer::setSize(QSize sz)
{
	if(column==sz.width() && row==sz.height())
		return;

	if(row<sz.height())
		for(int i=0; i<sz.height()-row; i++ )
			listLines.append( new QTermTextLine );
	else if(row>sz.height())
		for(int i=0; i<row-sz.height(); i++ )
			delete listLines.takeAt(screenY+topMargin);
	
	qWarning("QTermBuffer::setSize %d %d", sz.width(),sz.height());
	column = sz.width();
	row = sz.height();
	
	topMargin = 0;
	bottomMargin = row -1;

	caretY = qMin(caretY, row-1);
	oldCaretY = caretY;

	clearSelection();

	emit windowSizeChanged(column,row);

	emit bufferSizeChanged();
}

QSize QTermBuffer :: size()
{
	// for BBS, row is defined as the visble area
	if(mode==QTerm::BBS)
		return QSize(column,row);
	// for others, it is the whole editing area.
	if(mode==QTerm::EDITOR || mode==QTerm::VIEWER)
		return QSize(column,listLines.count());
}

int QTermBuffer::lines()
{
	return listLines.count();
}

// take by the absolute index
QTermTextLine * QTermBuffer::absAt( int index )
{
    if(index<0 || index>=listLines.count())
        return NULL;
    else
	   return listLines.at( index );
}
// take by the relative (to screen) index
QTermTextLine * QTermBuffer::scrAt( int index )
{
    if(screenY+index>listLines.count())
        return NULL;
    else
        return listLines.at( screenY+index );
}

// set current attr when insert new text
void QTermBuffer::setCurrentAttr( short attr )
{
	currentAttr = attr;
}
// change attr of current selected text
// if no text selected, change attr of the char under caret
void QTermBuffer::setAttr( short attr)
{
    
}

void QTermBuffer::setBuffer(const QByteArray& ba)
{
	if((screenY + caretY)>=listLines.count()) // out of current range
	{
		if( mode==QTerm::BBS ) // out of BBS screen
		{
			qWarning("in BBS mode, QTermBuffer::setBuffer: null line");
			return;
		}
		else if( mode==QTerm::EDITOR || mode==QTerm::VIEWER )// insert new line
		{
            qWarning("in EDITOR mode, QTermBuffer::setBuffer: insert new line");
			while(caretY>=listLines.count())
			{
                listLines.append(new QTermTextLine);
                row++;
            }
		}
		else
		  return;
	}
    QTermTextLine* line = listLines.at( screenY+caretY );
	if ( Insert_Mode /*bInsert*/ )
		line->insertText( caretX, ba, currentAttr);
	else // Replace
		line->replaceText( caretX, ba, currentAttr  );
	moveCaret( ba.size(), 0 );
}

// carriage return
void QTermBuffer::cr()
{
    setCaret(0, caretY);
}

// line feed
void QTermBuffer::lf()
{
    if( NewLine_Mode ) //CR also
        cr();

	// for editor and viewer, always insert a newline
	// break from the current line from caret
	if( mode == QTerm::EDITOR || mode== QTerm::VIEWER )
	{
        if( caretY==listLines.count()-1 )// the end
        {
            listLines.append(new QTermTextLine);
            row++; bottomMargin++;
        }
        moveCaret(0, 1);
	    emit bufferSizeChanged();
	}
    else if(mode==QTerm::BBS)
    {
        if( caretY == bottomMargin )
        {
            if( bottomMargin == row-1 )
                addHistoryLine(1);
            else
                scrollLines( topMargin, 1 );
        }
        else if (caretY < bottomMargin )
			moveCaret( 0, 1 );
    }

}

//table
void QTermBuffer::tab()
{
	int x = ( caretX + 8 ) & -8;	// or ( caretX/8 + 1 ) * 8 
	setCaret( x, caretY );

	QByteArray ba;
	ba.fill(' ',x-caretX);
	setBuffer(ba);
}

void QTermBuffer::setMargins( int top, int bottom ) // 1 based
{
	topMargin = qMax(top-1,0);
	bottomMargin = qMin(qMax(bottom - 1,0), row-1);
}

// caret functions
void QTermBuffer::setCaret( int x, int y )
{
    // invalidate old caret
	QTermTextLine * line = listLines.at( caretY+screenY );
	line->setChanged( caretX, caretX );

	// FIXME: column limit is not so important, why not relax it?
//	if ( x >= column )
//		x = column;
	if ( x < 0 )
		x = 0;

	if( mode==QTerm::BBS )
	{
		int stop = (caretY<topMargin) ? 0 : topMargin;
		if ( y < stop )
			y = stop;
		stop  = (caretY>bottomMargin) ? row-1 : bottomMargin;
		if ( y > stop )
			y = stop;
	}
	else
	{
        if(y<0) y=0;
        if(y>=row) y=row-1;
    }
	caretX = x;
	caretY = y;

    // invalidate new caret
	line = listLines.at( caretY+screenY );
	line->setChanged( caretX, caretX );
}

// 
void QTermBuffer::restoreCaret()
{
    setCaret( caretSavedX, caretSavedY );
}

void QTermBuffer::moveCaret( int x, int y )
{
    setCaret( caretX+x, caretY+y );
}

void QTermBuffer::saveCaret()
{
    caretSavedX = caretX; 
    caretSavedY = caretY;
}

// the screen oriented caret
QPoint QTermBuffer::scrCaret()
{
	return QPoint( caretX, caretY );
}

// the buffer oriented caret
QPoint QTermBuffer::absCaret()
{
	return QPoint(caretX, screenY+caretY);
}

// erase functions
void QTermBuffer::eraseStr( int n )
{
    if((caretY+screenY)>listLines.count())
    {
        qDebug("QTermBuffer::eraseStr(%d) Null line",n);
        return;
    }
	QTermTextLine * line = listLines.at( caretY+screenY );
	
	int x = line->length() - caretX;
	
	clearArea( caretX, caretY, qMin(n, x), 1, currentAttr );
}
// delete functions
void QTermBuffer::deleteStr( int n )
{
    if((caretY+screenY)>listLines.count())
    {
		qDebug("QTermBuffer::deleteStr(%d) Null line",n);
        return;
    }
    
	QTermTextLine * line = listLines.at( caretY+screenY );

    line->deleteText(caretX,n);
}
// insert functions
void QTermBuffer::insertStr( int n )
{
    if((caretY+screenY)>listLines.count())
    {
		qDebug("QTermBuffer::insertStr(%d) Null line",n);
        return;
    }

	QTermTextLine * line = listLines.at( caretY+screenY );
    
	int x = line->length() - caretX;

	if ( n >= x )
		clearArea( caretX, caretY, x, caretY, currentAttr );
	else
        line->insertText(caretX, QByteArray(n,' '), currentAttr);
}

// delete n lines including current line
void QTermBuffer::deleteLine( int n )
{
    int y = bottomMargin - caretY;

    if ( n >= y )
    	clearArea( 0, caretY, -1, y, currentAttr );
    else
    	scrollLines( caretY, n );
}

void QTermBuffer::insertLine( int n )
{
	int y = bottomMargin - caretY;
	
	if ( n >= y )
		clearArea( 0, caretY, -1, y, currentAttr );
	else
		scrollLines( caretY, -n );
}

void QTermBuffer::eraseToEndLine()
{
	clearArea( caretX, caretY, -1, 1, currentAttr );
}

void QTermBuffer::eraseToBeginLine()
{
	clearArea( 0, caretY, caretX, 1, currentAttr );
}

void QTermBuffer::eraseEntireLine()
{
	clearArea( 0, caretY, -1, 1, currentAttr );
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
			delete listLines.takeAt( screenY+startY );
			listLines.insert( screenY+bottomMargin, new QTermTextLine );
			num--;
		}
	}

	if ( num < 0 )	// insert
	{
		while ( num )
		{
			delete listLines.takeAt( screenY+bottomMargin );
			listLines.insert( screenY+startY, new QTermTextLine );
			num++;
		}
	}

	for(int i=screenY+startY; i<screenY+bottomMargin; i++)
		listLines.at(i)->setChanged(0,-1);
	
}

void QTermBuffer::eraseToEndScreen()
{
	if( caretX==0 && caretY ==0 )
	{
		eraseEntireScreen();
		return;
	}

	clearArea( caretX, caretY, -1, 1, currentAttr ); // erase to end line

	if ( caretY < bottomMargin - 1 )	// erase 
		clearArea( 0, caretY + 1, -1, bottomMargin - caretY - 1, currentAttr );

}
void QTermBuffer::eraseToBeginScreen()
{
	if( caretX==column-1 && caretY == row-1 )
	{
		eraseEntireScreen();
		return;
	}
	
	clearArea( 0, caretY, caretX, 1, currentAttr );	// erase to begin line
	if ( caretY > 0 )		// erase
		clearArea( 0, 0, -1, caretY - 1, currentAttr );

}
void QTermBuffer::eraseEntireScreen()
{
    if(mode==QTerm::BBS)
	   addHistoryLine( row );

	clearArea( 0, 0, column, bottomMargin, currentAttr );
}
// width = -1 : clear to end
void QTermBuffer::clearArea(int startX, int startY, int width, int height, short attr )
{
	QTermTextLine * line;
    int len;
    	
	if (startY < topMargin)
		startY = topMargin;

	if (height > bottomMargin-startY+1)
		height = bottomMargin-startY+1;

	for (int i=startY; i<height+startY; i++)
	{
		line = listLines.at(i+screenY);
		if(width==-1)
		  len=line->length()-startX;
		else
		  len=width;
/*		
        QByteArray cstr;
		cstr.fill(' ',len);
		line->replaceText( startX, cstr, attr);
*/
        // delete the text instead of fill with blanks
        line->deleteText(startX, len);
	}
}
/*
break one line to two, at the point of caret
*/
void QTermBuffer::splitLine()
{
    // get data from current
	QTermTextLine * line = listLines.at( caretY+screenY );
	if( line==NULL )
	{
		qDebug("QTermBuffer::splitLine() Null line");
		return;
	}
    QByteArray text=line->getPlainText(caretX);
    QByteArray color=line->getColor(caretX);
    QByteArray attr=line->getAttr(caretX);
    // clear until end of line
    int len = line->length()-caretX;
    line->deleteText(caretX,len);

    // insert after current line
    line = new QTermTextLine;
    listLines.insert( screenY+caretY+1, line );
    row++;bottomMargin++;
    // invalidate all line below
	for(int i=screenY+caretY; i<screenY+bottomMargin; i++)
		listLines.at(i)->setChanged(0,-1);
    // move caret down
    setCaret(0,caretY+1);

    // insert it to next line
    // TODO: maybe not always insert in the beginning?
    line->insertText(0,text,color,attr);
}

// merge current to previous
void QTermBuffer::mergeLine()
{
    if(caretY+screenY==topMargin) // no line to merge into
        return;
    // get data from current
	QTermTextLine * line = listLines.at( caretY+screenY );
	if( line==NULL )
	{
		qDebug("QTermBuffer::mergeLine() Null line");
		return;
	}
    QByteArray text=line->getPlainText();
    QByteArray color=line->getColor();
    QByteArray attr=line->getAttr();

    // invalidate all lines below
	for(int i=screenY+caretY+1; i<screenY+bottomMargin; i++)
		listLines.at(i)->setChanged(0,-1);
    // delete current line
    listLines.takeAt(caretY+screenY);
    row--;bottomMargin--;
    // previous line
	line = listLines.at( caretY+screenY-1 );
	if( line==NULL )
	{
		qDebug("QTermBuffer::mergeLine() Null line");
		return;
	}
    setCaret(line->length(),caretY-1);
    line->insertText(caretX,text,color,attr);
}

void QTermBuffer::addHistoryLine( int n )
{
	while( n )
	{
		if( screenY == maxLines-row )
		{
			delete listLines.takeAt(0);
			//m_ptSelStart.setY( m_ptSelStart.y()-1 );
			//m_ptSelEnd.setY( m_ptSelEnd.y()-1 );
			//if(m_ptSelStart.y()<0)
			//	m_ptSelStart=m_ptSelEnd=QPoint(-1,-1);
		}
		
		listLines.append( new QTermTextLine );
		screenY = qMin(screenY+1,maxLines-row);
		n--;
	}
	
	for(int i=screenY+0; i<screenY+bottomMargin; i++)
		listLines.at(i)->setChanged(0,-1);

	emit bufferSizeChanged();
}

// for debug 
void QTermBuffer::startDecode()
{
	oldCaretX = caretX;
	oldCaretY = caretY;
	
}

void QTermBuffer::endDecode()
{
	QTermTextLine * line = listLines.at( oldCaretY+screenY );

	line->setChanged( oldCaretX, oldCaretX );

	line = listLines.at( caretY+screenY );

	line->setChanged( caretX, caretX );

	clearSelection();
	
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
bool QTermBuffer::getMode( int mode )
{
	switch( mode )
	{
		case INSERT_MODE:
			return Insert_Mode;
		case NEWLINE_MODE:
			return NewLine_Mode;
		default:
			return false;
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

// Rectangle selection mode
void QTermBuffer::setSelectionMode(bool rect)
{
    bRect=rect;
}
void QTermBuffer::setSelectionStart( const QPoint& pt )
{
    
}

void QTermBuffer::setSelectionEnd( const QPoint& pt )
{
    
}


void QTermBuffer::clearSelection()
{
}

bool QTermBuffer::isSelected( int index )
{
}

bool QTermBuffer::isSelected( const QPoint& pt, bool rect )
{
}

QByteArray QTermBuffer::getSelection( bool rect, bool color, const QByteArray& escape )
{
}

QRect QTermBuffer::getSelectionRect( int index, bool rect )
{
}
