/*******************************************************************************
 * FILENAME:      qtermbbs.cpp
 * REVISION:      2002.9.3 first created.
 *
 * AUTHOR:        kingson fiasco
 * *****************************************************************************/
/*******************************************************************************
 *                                     NOTE
 * This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermbbs.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"
#include "qterm.h"

#include <stdio.h>
#include <ctype.h>
#include <qregexp.h>

QTermBBS::QTermBBS( QTermBuffer * buffer )
{
	m_pBuffer=buffer;
}

QTermBBS::~QTermBBS()
{
}

void QTermBBS::setScreenStart(int nStart )
{
	m_nScreenStart = nStart;
}

bool QTermBBS::setCursorPos(const QPoint& pt, QRect& rc)
{
	QRect rectOld = getSelectRect();
	
	m_ptCursor = pt;

	QRect rectNew = getSelectRect();

	rc = rectOld | rectNew;

	return rectOld!=rectNew;
}

QCString QTermBBS::getMessage()
{
	QTermTextLine * line;
	QCString message="";

	if(!isUnicolor(m_pBuffer->screen(0)))
		return message;

	int i = 1;
	message = m_pBuffer->screen(0)->getText().simplifyWhiteSpace();
	line = m_pBuffer->screen(i);
	while (isUnicolor(line))
	{
		message += "\n" + line->getText();
		i++;
		line = m_pBuffer->screen(i);
	}
	return message;
}	
						

void QTermBBS::setPageState()
{
	m_nPageState = -1;
	
	QTermTextLine * line;
	QCString color;

	line = m_pBuffer->screen(0);    // first line
	if(isUnicolor(line))
	{
		line = m_pBuffer->screen(2);    // third line
		if(isUnicolor(line))
			m_nPageState = 1;	//board and article list 
		else
			m_nPageState = 0;	//menu
	}
	else
	{
		line = m_pBuffer->screen(m_pBuffer->line()-1);  // last line 
		if(isUnicolor(line))
			m_nPageState = 2;	// reading
	}

}

int QTermBBS::getCursorType(const QPoint& pt)
{
	if( m_nScreenStart != (m_pBuffer->lines()-m_pBuffer->line()) )
		return 8;

	QRect rc = getSelectRect();

	int nCursorType = 8;
	switch( m_nPageState )
	{
	case -1:	// not recognized
		nCursorType = 8;
		break;
	case 0:		// menu
		if( pt.x()<5 )		// LEFT
			nCursorType=6;
		else if(rc.contains(pt))// HAND
			nCursorType=7;
		else
			nCursorType=8;
		break;
	case 1:		// list
		if( pt.x()<12 )		// LEFT
			nCursorType=6;
        else if( pt.y()-m_nScreenStart<3 )	// HOME
            nCursorType=0;	
        else if( pt.y()==m_pBuffer->lines()-1 )	// END
            nCursorType=1;				
        else if( pt.x()>m_pBuffer->columns()-16 
				&& pt.y()-m_nScreenStart<=m_pBuffer->line()/2 ) //PAGEUP
            nCursorType=2;			
        else if( pt.x()>m_pBuffer->columns()-16 
				&& pt.y()-m_nScreenStart>m_pBuffer->line()/2 ) // PAGEDOWN
            nCursorType=3;
        else if( rc.contains(pt) )
            nCursorType=7;
		else
			nCursorType=8;
		break;
	case 2:		// read
		if( pt.x()<12 )		// LEFT
			nCursorType=6;
        else if( pt.x()>(m_pBuffer->columns()-16) 
			&& ( pt.y()-m_nScreenStart)<=m_pBuffer->line()/2 )	// PAGEUP
            nCursorType=2; 
        else if( pt.x()>(m_pBuffer->columns()-16) 
			&& ( pt.y()-m_nScreenStart)>m_pBuffer->line()/2 )	// PAGEDOWN
            nCursorType=3;
		else
			nCursorType=8;
		break;
	case 3:
		break;
	default:
		break;
	}

	return nCursorType;
}

bool QTermBBS::isSelected( int index )
{
	QRect rect = getSelectRect();
	// nothing selected
	if(rect.isNull())
		return false;

	return (index==rect.y());

}

bool QTermBBS::isSelected( const QPoint& pt )
{
	QRect rect = getSelectRect();
	
	// nothing selected
	if(rect.isNull())
		return false;

	return ( pt.y()==rect.y() && 
		pt.x()>=rect.left() && 
		pt.x()<=rect.right() );
}

int QTermBBS::getPageState()
{
	return m_nPageState;
}

char QTermBBS::getMenuChar()
{
	return m_cMenuChar;
}

QRect QTermBBS::getSelectRect()
{
	QRect rect(0,0,0,0);

	// current screen scrolled
	if(m_nScreenStart != (m_pBuffer->lines()-m_pBuffer->line()))
	{
		return rect;
	}

	QTermTextLine * line;
	
	switch( m_nPageState )
    {
    case -1:
		break;
    case 0:
        if( m_ptCursor.y()-m_nScreenStart>=7 &&
			m_ptCursor.x()>5)
        {
            line = m_pBuffer->at(m_ptCursor.y());
            QCString cstr = line->getText( 0, m_ptCursor.x() );

            QRegExp reg( "[a-zA-Z0-9][).\\]]" );
            char indexChar = cstr.findRev( reg );
            if (indexChar != -1)
            {
				m_cMenuChar = cstr.at(indexChar);

                int nMenuStart = indexChar;
                if ( cstr[indexChar-1] == '(' || cstr[indexChar-1] == '[' )
                    nMenuStart--;

                cstr = line->getText();
                reg = QRegExp( "[^ ]" );

                int nMenuBaseLength = 20;
                int nMenuLength = cstr.findRev( reg, nMenuStart + nMenuBaseLength ) - nMenuStart + 1;
                if ( nMenuLength == nMenuBaseLength + 1 )
                    nMenuLength = cstr.find( " ", nMenuStart + nMenuBaseLength ) - nMenuStart;
				if( m_ptCursor.x()>=nMenuStart && m_ptCursor.x()<=nMenuStart+nMenuLength )
				{
					rect.setX(nMenuStart);
					rect.setY(m_ptCursor.y());
					rect.setWidth(nMenuLength);
					rect.setHeight(1);
				}
            }
        }
        break;
    case 1:
        if ( (m_ptCursor.y()-m_nScreenStart)>=3 && 
			(m_ptCursor.y()-m_nScreenStart)<m_pBuffer->line()-1 &&
			m_ptCursor.x()>=12 && m_ptCursor.x()<=m_pBuffer->columns()-16 )
        {
            line =  m_pBuffer->at(m_ptCursor.y());
            QString str = line->getText();

            if ( str.contains( " " ) != ( int ) str.length() )
			{
				rect.setX(0);
				rect.setY(m_ptCursor.y());
				rect.setWidth(m_pBuffer->columns());
				rect.setHeight(1);
			}
		}
        break;
	case 2:
		break;
	case 3:
		break;
	default:
        break;
    }

	return rect;

}

bool QTermBBS::isUnicolor( QTermTextLine *line )
{
	QCString color = line->getColor();
	bool bSame=true;
	int clr = GETBG(color[0]);

	for( int i=0; i<color.length()/2; i++ )
	{
		if ( GETBG(color[i]) != clr ||
				GETBG(color[i]) == GETBG(NO_COLOR) )
		{
			bSame = false;
			break;
		}
	}
	return bSame;
}

bool QTermBBS::isIllChar(char ch)
{
	static char illChars[] = ",;'\"()[]<>^";	
	return ch>'~' || ch<'#' || strchr(illChars, ch) !=NULL;
}

bool QTermBBS::isUrl(QRect& rcUrl, QRect& rcOld)
{
	static const char http[] = "http://";
	static const char https[] = "https://";
	static const char mms[] = "mms://";
	static const char rstp[] = "rstp://";
	static const char ftp[] = "ftp://";
	static const char mailto[] = "mailto:";
	static const char telnet[] = "telnet://";
	
	int at = m_ptCursor.x();

	if(at>m_rcUrl.left() && at<m_rcUrl.right() && m_rcUrl.y()==m_ptCursor.y())
	{	
		rcUrl = m_rcUrl;
		rcOld = m_rcUrl;
		return true;
	}
	m_cstrUrl = "";
	rcOld = m_rcUrl;
	m_rcUrl = QRect(0,0,0,0);

	QCString cstrText = m_pBuffer->at(m_ptCursor.y())->getText();


	if(at>=cstrText.length())
		return false;
	
	int i,index,begin,end,dot,url,host,ata;

	for (i=at; i>=0 && !isIllChar(cstrText.at(i)); i--);
	url = i+1;
	for (i=at; i<cstrText.length() && !isIllChar(cstrText.at(i)); i++);
	end = i;

	int nNoType = -1;
	if ((begin = cstrText.find(http, url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 7;
	}else
	if ((begin = cstrText.find(https, url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 8;
	}else
	if ((begin = cstrText.find(mms, url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 6;
	}else
	if ((begin = cstrText.find(rstp, url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 7;
	}else
	if ((begin = cstrText.find(ftp,url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 6;
	}else
	if ((begin = cstrText.find(mailto,url, false)) != -1)
	{		
		if (begin > at)
			return false;
		if ((ata = cstrText.find('@',begin+9)) == -1)
			return false;
		host = ata + 1;
	}else
	if ((begin = cstrText.find(telnet,url, false)) != -1)
	{
		if (begin > at)
			return false;
		host = url + 9;
	}else
	{
		begin = url;
		if ((ata = cstrText.find('@',begin+1)) != -1)
		{
			host = url+(ata-begin)+1;
			nNoType=0;
		}
		else
		{
			host = url;
			nNoType=1;
		}
	}
	
//	if (end - begin < 7) // too short
//		return false;

	for(index=host, dot=host-1, i=0; index<end && cstrText.at(index)!='/'; index++)
	{
		if (cstrText.at(index) == '.')
		{
			if (index <= dot + 1) // xxx..x is illegal
				return false;
			dot = index;
			i++;
		}
		else 
		{
			if (!isalnum(cstrText.at(index)) && 
				cstrText.at(index)!='-' && 
				cstrText.at(index)!='_' &&
				cstrText.at(index)!='~' &&
				cstrText.at(index)!=':' &&
				cstrText.at(index)!='@')
				return false;
		}
	}
	
	if (cstrText.at(index-1) == '.')
		return false;

	if(i<1)
		return false;

	m_cstrUrl = cstrText.mid(url, end-url);
	
	if(nNoType==0)
		m_cstrUrl = "mailto:"+m_cstrUrl;
	else if(nNoType==1)
		m_cstrUrl = "http://"+m_cstrUrl;
	
	rcUrl = QRect(url, m_ptCursor.y(), end-url, 1);
	m_rcUrl = rcUrl;

	return true;;
}

QCString QTermBBS::getUrl()
{
	return m_cstrUrl;
}

bool QTermBBS::isPageComplete()
{
	return m_pBuffer->caret().y()==(m_pBuffer->line()-1) 
			&& m_pBuffer->caret().x()==(m_pBuffer->columns()-1);
}
