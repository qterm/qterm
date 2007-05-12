/*******************************************************************************
FILENAME:      qtermtextline.cpp
REVISION:      2001.8.12 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qterm.h"
#include "qtermtextline.h"

#include <QRegExp>
#include <QString>

QTermTextLine::QTermTextLine()
		:m_text(),m_color(),m_attr()
{
	m_start = -1;
	m_end = -1;
	reset();
}

QTermTextLine::~QTermTextLine()
{

}


// insert cstr at position index of line, 
// if attrib == -1, use current attr,
void QTermTextLine::insertText(  int index, const QByteArray& str, short attribute )
{
	// set attribute
	if ( attribute != -1 )	
	{
		m_curColor = GETCOLOR(attribute);
		if( m_curColor == '\0' )
			m_curColor = NO_COLOR;
		m_curAttr  = GETATTR (attribute);
		if( m_curAttr == '\0' )
			m_curAttr = NO_ATTR;
	}

     // justify
    if(index>m_length)
        leftJustify(index);
	// insert
	m_text.insert( index, str );
	m_length = m_text.length();
	m_color.insert( index,QByteArray(str.length(), m_curColor) );
	m_attr.insert( index, QByteArray(str.length(), m_curAttr));
    // set changes
    m_length = m_text.length();	
    setChanged(index,-1);
}
// insert text/color/attr paired strings
void QTermTextLine::insertText(int index, const QByteArray& text,const QByteArray& color,const QByteArray& attr)
{
    // justify
    if(index>m_length)
        leftJustify(index);
    // insert
    m_text.insert(index, text);
    m_color.insert(index,color);
    m_attr.insert(index,attr);
    // set changes
    m_length = m_text.length();	
    setChanged(index,-1);
}

// replace the cstring with cstr, which start at index and have len chars,
// if attr == -1, use the current attr,
// if index == -1, reset line and insert str.
// if len == -1, replace str's length chars.
void QTermTextLine::replaceText( int index, const QByteArray& str, short attribute)
{
	// set attribute
	if ( attribute != -1 )
	{
		m_curColor = GETCOLOR(attribute);
		if( m_curColor == '\0' )
			m_curColor = NO_COLOR;
		m_curAttr  = GETATTR (attribute);
		if( m_curAttr == '\0' )
			m_curAttr = NO_ATTR;
	}


    // justify
	if( index>m_length )
        leftJustify(index);
   	// replace
	int len = str.length();
	m_text.replace( index, len, str );	
	m_color.replace( index, len, QByteArray(len,m_curColor) );
	m_attr.replace( index, len, QByteArray(len,m_curAttr) );

	m_length = m_text.length();	
	setChanged( index, -1 );
}

// replace text/color/attr paired strings
void QTermTextLine::replaceText(int index, const QByteArray& text,const QByteArray& color,const QByteArray& attr)
{
    // justify
    if(index>m_length)
        leftJustify(index);
    // replace
    int len = text.length();
    m_text.replace(index, len,text);
    m_color.replace(index,len,color);
    m_attr.replace(index,len,attr);
    // set changes
	m_length = m_text.length();
    setChanged(index,-1);
}

void QTermTextLine::leftJustify(int index)
{
    m_text=m_text.leftJustified(index, ' ');
	m_color=m_color.leftJustified( index, NO_COLOR );
	m_attr=m_attr.leftJustified( index, NO_ATTR );
	setChanged(index,-1);
}

void QTermTextLine::appendText(int index, const QByteArray& str, short attribute)
{
	// set attribute
	if ( attribute != -1 )	
	{
		m_curColor = GETCOLOR(attribute);
		if( m_curColor == '\0' )
			m_curColor = NO_COLOR;
		m_curAttr  = GETATTR (attribute);
		if( m_curAttr == '\0' )
			m_curAttr = NO_ATTR;
	}

    int len = str.length();
    
    m_text=m_text.leftJustified(index, ' ');
	m_text.append( str );
	m_length = m_text.length();

	m_color=m_color.leftJustified( index, NO_COLOR );
	m_color.append(QByteArray(len, m_curColor));

	m_attr=m_attr.leftJustified( index, NO_ATTR );
	m_attr.append(QByteArray(len, m_curAttr));
	
    setChanged( index, -1 );
}

// delete len chars starting from position index
void QTermTextLine::deleteText( int index, int len )
{
	m_text.remove( index, len );
	m_color.remove( index, len );
	m_attr.remove( index, len );

	setChanged( index, -1 );

	m_length = m_text.length();
}
	
// get len chars starting from index
// return the whole line by default
QByteArray QTermTextLine::getPlainText( int index, int len )
{
	QByteArray str;
	str = m_text.mid( index, len );
	return str;
}

QByteArray QTermTextLine::getAttrText( int index, int len, const QByteArray& escape )
{
	QByteArray str;
	int startx;
	char tempcp, tempea;
    
    if(len==-1) len=m_length;
    
	for( int i=index; i<index+len && i<m_length; i++)
	{
		startx = i;
		tempcp = m_color.at(i);
		tempea = m_attr.at(i);
		// get str of the same attribute
		while ( tempcp == m_color.at(i) && 
			tempea == m_attr.at(i)  &&
			i < m_length )
			i++;
		
		int fg = GETFG(tempcp)+30;
		int bg = GETBG(tempcp)+40;
		QByteArray cstrAttr=QString("%1;%2").arg(fg).arg(bg).toLatin1();//QByteArray::setNum(fg)+';'+QByteArray::setNum(bg)+';';
		cstrAttr = escape + cstrAttr;

		if (GETBOLD(tempea))
			cstrAttr += "1;";
		if ( GETDIM(tempea ) )
			cstrAttr += "2;";	
		if ( GETUNDERLINE( tempea ) )
			cstrAttr += "4;";		
		if ( GETBLINK( tempea ) )
			cstrAttr += "5;";
		if ( GETRAPIDBLINK( tempea ) )
			cstrAttr += "6;";	
		if ( GETREVERSE( tempea ) )
			cstrAttr += "7;";	
		if ( GETINVISIBLE( tempea ) )
			cstrAttr += "8;";
		cstrAttr.remove(cstrAttr.length()-1,1);
		cstrAttr += "m";
		str += cstrAttr; // set attr
		// the text
		str += getPlainText(startx, i-startx);	
		// reset attr
		cstrAttr = escape+"0m";
		str	+= cstrAttr;
		i--;
	}
	return str;
}

// reset line
inline void QTermTextLine::reset()
{
	m_length = 0;
	
	m_text = "";
	m_color = "";
	m_attr ="";

	m_curColor = NO_COLOR;
	m_curAttr = NO_ATTR;

}
bool QTermTextLine::hasBlink()
{
	bool blink = false;
	
	char tempea;
	for( int i=0; i<m_length; i++)
	{
		tempea = m_attr.at(i);
		if(GETBLINK(tempea))
		{
			blink = true;
			break;
		}
	}

	return blink;
}

void QTermTextLine::setChanged( int start, int end )
{
    if(start==-1) // do nothing
        return;

    if(m_start==-1) // no change ever made, set now
    {
        m_start=start;
        m_end=end;
    }
    else // merge the new change
    {
        m_start=qMin(start, m_start);
        m_end = (end==-1||m_end==-1 ? -1 : qMax(end, m_end));
    }
}
