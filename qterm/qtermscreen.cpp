/*******************************************************************************
FILENAME:      qtermscreen.cpp
REVISION:      2001.10.5 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qterm.h"

#include "qtermwindow.h"
#include "qtermscreen.h"
#include "qtermtextline.h"
#include "qtermconvert.h"
#include "qtermbuffer.h"
#include "qtermbbs.h"
#include "qtermframe.h"
#include "qtermwndmgr.h"
#include "qtermparam.h"
#include "qtermtelnet.h"
#include "qtermconfig.h"

#include <qaccel.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtextcodec.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <qevent.h>
#include <qpoint.h>
#include <qclipboard.h>
#include <qfile.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                            Constructor/Destructor                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

QTermScreen::QTermScreen( QWidget *parent, QTermBuffer *buffer, QTermParam *param, QTermBBS *bbs )
	: QWidget( parent ),Scrollbar_Width(15)
{	
	m_pWindow = (QTermWindow *)parent;
	m_pBBS = bbs;
	m_pParam = param;

	setFocusPolicy(ClickFocus);	

	#if (QT_VERSION>=0x030200)
	setInputMethodEnabled(true);
	#endif

	setSchema();

	initFontMetrics();

	#if (QT_VERSION>=200 && QT_VERSION<=300)
	setMicroFocusHint(650,350,0,0,true);
	#endif

	m_pBuffer = buffer;
	connect( m_pBuffer, SIGNAL(bufferSizeChanged()), 
			this, SLOT(bufferSizeChanged()) );

	m_blinkTimer = new QTimer(this);
	connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkEvent()));

	m_cursorTimer = new QTimer(this);
	connect(m_cursorTimer, SIGNAL(timeout()), this, SLOT(cursorEvent()));

	m_inputContent = NULL;

	// the scrollbar
	m_scrollBar = new QScrollBar(this);
	m_scrollBar->setCursor( arrowCursor );
	m_nStart = 0;
	m_nEnd = m_pBuffer->line()-1;
	m_pBlinkLine = new bool[m_nEnd - m_nStart + 1];
	m_scrollBar->setRange( 0, 0 );
	m_scrollBar->setLineStep(1);
	m_scrollBar->setPageStep( m_pBuffer->line() );
	m_scrollBar->setValue(0);
	connect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));
	m_pAccel = new QAccel(this);

	#if (QT_VERSION>=300)
	m_pAccel->connectItem(m_pAccel->insertItem(Key_PageUp+SHIFT), this, SLOT(prevPage()));
	m_pAccel->connectItem(m_pAccel->insertItem(Key_PageDown+SHIFT), this, SLOT(nextPage()));
	m_pAccel->connectItem(m_pAccel->insertItem(Key_Up+SHIFT), this, SLOT(prevLine()));
	m_pAccel->connectItem(m_pAccel->insertItem(Key_Down+SHIFT), this, SLOT(nextLine()));
	#endif
	
	// to avoid flickering
	setBackgroundMode( NoBackground );

	setMouseTracking( true );

// init variable
	m_blinkScreen = false;
	m_blinkCursor = true;

	if( m_pParam->m_nDispCode == 0 )
		m_pCodec = QTextCodec::codecForName("GBK");
	else
		m_pCodec = QTextCodec::codecForName("Big5");
	
}

QTermScreen::~QTermScreen()
{
	delete [] m_pBlinkLine;
	delete m_blinkTimer;
	delete m_cursorTimer;
	delete m_inputContent;
}


// focus event received
void QTermScreen::focusInEvent( QFocusEvent * )
{

//	#if (QT_VERSION>0x030300) // blame me for this...
	if(m_pWindow->isMaximized()&&m_pWindow->m_pFrame->wndmgr->afterRemove())
	{
		m_pWindow->showNormal();
		m_pWindow->showMaximized();
	}
//	#endif

	m_pWindow->m_pFrame->wndmgr->activateTheTab( m_pWindow );
	
#if (QT_VERSION<300)
	m_pAccel->connectItem(idPrevPage=m_pAccel->insertItem(Key_PageUp+SHIFT), this, SLOT(prevPage()));
	m_pAccel->connectItem(idNextPage=m_pAccel->insertItem(Key_PageDown+SHIFT), this, SLOT(nextPage()));
	m_pAccel->connectItem(idPrevLine=m_pAccel->insertItem(Key_Up+SHIFT), this, SLOT(prevLine()));
	m_pAccel->connectItem(idNextLine=m_pAccel->insertItem(Key_Down+SHIFT), this, SLOT(nextLine()));
#endif
}

// focus out
void QTermScreen::focusOutEvent( QFocusEvent * )
{
#if (QT_VERSION<300)
	m_pAccel->removeItem(idPrevPage);
	m_pAccel->removeItem(idNextPage);
	m_pAccel->removeItem(idPrevLine);
	m_pAccel->removeItem(idNextLine);
#endif
}


void QTermScreen::cursorEvent()
{
	if(!m_blinkCursor) return;

	if( m_pBuffer->caretY()<=m_nEnd && m_pBuffer->caretY()>=m_nStart )
	{
		QPoint pt=mapToPixel(QPoint(m_pBuffer->caretX(),m_pBuffer->caretY()));
		QPixmap *tempPxm;
		
		switch(m_pParam->m_nCursorType)
		{
			case 0:	// block
				tempPxm = new QPixmap(m_nCharWidth,m_nCharHeight);
				bitBlt(this, pt.x(), pt.y(), tempPxm, 0, 0, 
								m_nCharWidth, m_nCharHeight, Qt::NotROP, false);
				break;
			case 1:	// underline
				tempPxm = new QPixmap(m_nCharWidth,m_nCharHeight/10);
				bitBlt(this, pt.x(), pt.y()+9*m_nCharHeight/10, tempPxm, 0, 0, 
								m_nCharWidth, m_nCharHeight/10, Qt::NotROP, false);
				break;
			case 2:	// I type
				tempPxm = new QPixmap(m_nCharWidth/9,m_nCharHeight);
				bitBlt(this, pt.x(), pt.y(), tempPxm, 0, 0, 
								m_nCharWidth/9, m_nCharHeight, Qt::NotROP, false);
				break;
			default:
				tempPxm = new QPixmap(m_nCharWidth,m_nCharHeight);
				bitBlt(this, pt.x(), pt.y(), tempPxm, 0, 0, 
								m_nCharWidth, m_nCharHeight, Qt::NotROP, false);

		}
		if(tempPxm!=NULL)
			delete tempPxm;
	}
}
void QTermScreen::blinkEvent()
{
	if(m_hasBlink)
	{
		m_blinkScreen = !m_blinkScreen;
		blinkScreen();
	}
}

void QTermScreen::moveEvent( QMoveEvent * )
{
//	setBgPxm( m_pxmBg, m_nPxmType );
}
void QTermScreen::resizeEvent( QResizeEvent *  )
{
	updateScrollBar();
	setBgPxm( m_pxmBg, m_nPxmType );
	
	if( m_pParam->m_bAutoFont )
	{
		updateFont();
	}
	else
	{
		/*
		// FIXME this still not work
		int cx = m_rcClient.width()/m_nCharWidth;
		int cy = m_rcClient.height()/m_nCharHeight;
		if( cx>0 && cy>0 )
		{
			m_pBuffer->setSize( cx, cy );
			m_pWindow->m_pTelnet->setWindowSize( cx, cy );
		}
		*/
	}
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Mouse                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermScreen::enterEvent( QEvent * e )
{
	QApplication::sendEvent(m_pWindow, e);
}

void QTermScreen::leaveEvent( QEvent * e )
{
	QApplication::sendEvent(m_pWindow, e);
}

void QTermScreen::mousePressEvent( QMouseEvent * me )
{
	setFocus();

	QApplication::sendEvent(m_pWindow, me);

}
void QTermScreen::mouseMoveEvent( QMouseEvent * me)
{
#ifdef Q_OS_MACX
	m_pWindow->mouseMoveEvent(me);
#else
	QApplication::sendEvent(m_pWindow, me);
#endif
}

void QTermScreen::mouseReleaseEvent( QMouseEvent * me )
{
	QApplication::sendEvent(m_pWindow, me);
}

void QTermScreen::wheelEvent( QWheelEvent * we )
{
	if(m_pWindow->m_pFrame->m_pref.bWheel)
		QApplication::sendEvent(m_pWindow, we);
	else
		QApplication::sendEvent(m_scrollBar,we);
}


/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                                Font                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void QTermScreen::initFontMetrics()
{
	if(m_pParam->m_bAutoFont)
		m_pFont = new QFont(m_pParam->m_strFontName); 
	else {
		m_pFont = new QFont(m_pParam->m_strFontName, QMAX(8,m_pParam->m_nFontSize) ); 
		QFontMetrics *fm = new QFontMetrics( *m_pFont );

		if (abs(m_pParam->m_nFontSize - fm->width(0x4e00)) < abs(m_pParam->m_nFontSize - fm->width('W') * 2))
			m_nCharWidth  = (fm->width(0x4e00) + 1)/2;
		else
			m_nCharWidth  = fm->width('W');
		m_nCharHeight = fm->height();
		m_nCharAscent = fm->ascent();
		delete fm;
	}

	#if (QT_VERSION >= 300 )
	m_pFont->setStyleHint(QFont::System, 
				m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
	#endif
	
}

void QTermScreen::setDispFont( const QFont& font)
{
	delete m_pFont;
	if(m_pParam->m_bAutoFont)
		m_pFont = new QFont(font.family());
	else {
		int nSize = font.pixelSize();
		m_pFont = new QFont(font);

		QFontMetrics *fm = new QFontMetrics( *m_pFont );
		if (abs(nSize - fm->width(0x4e00)) < abs(nSize - fm->width('W') * 2))
			m_nCharWidth  = (fm->width(0x4e00) + 1)/2;
		else
			m_nCharWidth  = fm->width('W');
		m_nCharHeight = fm->height();
		m_nCharAscent = fm->ascent();
		delete fm;
	}

	#if (QT_VERSION >= 300 )
	m_pFont->setStyleHint(QFont::System, 
				m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
	#endif

}

QFont QTermScreen::getDispFont( )
{
	return *m_pFont;
}

void QTermScreen::updateFont()
{
	QString strFamily = m_pFont->family();
	delete m_pFont;

	int nPixelSize;

	int nIniSize = QMAX(8,QMIN(m_rcClient.height()/m_pBuffer->line(),
						m_rcClient.width()*2/m_pBuffer->columns()));

	for( nPixelSize=nIniSize-3; nPixelSize<=nIniSize+3; nPixelSize++)
	{
		m_pFont = new QFont(strFamily);
		m_pFont->setPixelSize( nPixelSize );
		
		QFontMetrics fm( *m_pFont );
/* 
   Use the width of char 'W' or the width of Chinese charactor to calculate
   the display. use nPixelSize as a reference size. 0x4e00 stand for Chinese
   one ;)
*/
		if (abs(nPixelSize - fm.width(0x4e00)) < abs(nPixelSize - fm.width('W') * 2))
			m_nCharWidth  = (fm.width(0x4e00) + 1)/2;
		else
			m_nCharWidth  = fm.width('W');
		m_nCharHeight = fm.height();
		m_nCharAscent = fm.ascent();
		m_nCharDescent = fm.descent();

		if( (m_pBuffer->line()*m_nCharHeight)>m_rcClient.height()
			|| (m_pBuffer->columns()*m_nCharWidth)>m_rcClient.width()  )
		{
			delete m_pFont;
			break;
		}
		delete m_pFont;
	}

	m_pFont = new QFont(strFamily);
	m_pFont->setPixelSize( nPixelSize-1 );
	#if (QT_VERSION >= 300 )
	m_pFont->setStyleHint(QFont::System, 
									m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
	#endif
	QFontMetrics fm( *m_pFont );
	if (abs(nPixelSize - 1 - fm.width(0x4e00)) < abs(nPixelSize - 1 - fm.width('W') * 2))
		m_nCharWidth  = (fm.width(0x4e00) + 1)/2;
	else
		m_nCharWidth  = fm.width('W');
/*
   Don't trust the font width if it's nonsense, we just guess it from the 
   pixelsize, mostly because some system(redhat and some debian) return the 
   wrong width of Chinese charactors, for using of variable fonts, we can
   only use nPixelsize
*/
	if (abs(nPixelSize - m_nCharWidth * 2) > (nPixelSize > 16 ? 3 : 2))
		m_nCharWidth = (nPixelSize + 1)/2;
	m_nCharHeight = fm.height();
	m_nCharAscent = fm.ascent();
	m_nCharDescent = fm.descent();

}


/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                               Colors                                   */
/*	                                                                        */
/* ------------------------------------------------------------------------ */
void QTermScreen::setSchema()
{
// the default color table
	m_color[0]  = Qt::black;
	m_color[1]  = Qt::darkRed;
	m_color[2]  = Qt::darkGreen;
	m_color[3]  = Qt::darkYellow;
	m_color[4]  = Qt::darkBlue;
	m_color[5]  = Qt::darkMagenta;
	m_color[6]  = Qt::darkCyan;
	m_color[7]  = Qt::darkGray; 
	m_color[8]  = Qt::gray;
	m_color[9]  = Qt::red;
	m_color[10] = Qt::green;
	m_color[11] = Qt::yellow;
	m_color[12] = Qt::blue;
	m_color[13] = Qt::magenta;
	m_color[14] = Qt::cyan;
	m_color[15] = Qt::white;

	m_nPxmType = 0;



// if we have schema defined
	if(QFile::exists(m_pParam->m_strSchemaFile))
	{

//		printf("schema %s loaded sucessfully\n", m_pParam->m_strSchemaFile);
		QTermConfig *pConf = new QTermConfig(m_pParam->m_strSchemaFile);

		m_color[0].setNamedColor(pConf->getItemValue("color","color0"));
		m_color[1].setNamedColor(pConf->getItemValue("color","color1"));
		m_color[2].setNamedColor(pConf->getItemValue("color","color2"));
		m_color[3].setNamedColor(pConf->getItemValue("color","color3"));
		m_color[4].setNamedColor(pConf->getItemValue("color","color4"));
		m_color[5].setNamedColor(pConf->getItemValue("color","color5"));
		m_color[6].setNamedColor(pConf->getItemValue("color","color6"));
		m_color[7].setNamedColor(pConf->getItemValue("color","color7"));
		m_color[8].setNamedColor(pConf->getItemValue("color","color8"));
		m_color[9].setNamedColor(pConf->getItemValue("color","color9"));
		m_color[10].setNamedColor(pConf->getItemValue("color","color10"));
		m_color[11].setNamedColor(pConf->getItemValue("color","color11"));
		m_color[12].setNamedColor(pConf->getItemValue("color","color12"));
		m_color[13].setNamedColor(pConf->getItemValue("color","color13"));
		m_color[14].setNamedColor(pConf->getItemValue("color","color14"));
		m_color[15].setNamedColor(pConf->getItemValue("color","color15"));

		// bg type
		QString strTmp = pConf->getItemValue("image", "type");
		m_nPxmType = strTmp.toInt();
		
		// fade effect
		QColor fadecolor;
		fadecolor.setNamedColor(pConf->getItemValue("image","fade"));
		strTmp = pConf->getItemValue("image", "alpha");
		float alpha = strTmp.toFloat();

		// get the image name
		if(QFile::exists(pConf->getItemValue("image","name")) && m_nPxmType>1) // valid image name and type
		{
			m_pxmBg = QPixmap(pConf->getItemValue("image","name"));
			QImage ima(m_pxmBg.convertToImage());
			ima = fade(ima, alpha, fadecolor);
			m_pxmBg.convertFromImage(ima);

		}
		delete pConf;
	}

// override schema using user defined Fg/Bg color	
	m_color[0]  = m_pParam->m_clrBg;
	m_color[7]  = m_pParam->m_clrFg;
}


/* ------------------------------------------------------------------------ */
/*	                                                                        */
/*                  	Scrollbar                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermScreen::prevPage()
{
	scrollLine(-m_pBuffer->line());
	refreshScreen();
}

void QTermScreen::nextPage()
{
	scrollLine(m_pBuffer->line());
	refreshScreen();
}

void QTermScreen::prevLine()
{
	scrollLine(-1);
	refreshScreen();
}

void QTermScreen::nextLine()
{
	scrollLine(1);
	refreshScreen();
}

void QTermScreen::scrollLine( int delta )
{
	m_nStart += delta;

	if( m_nStart<0 )	
	{
		m_nStart = 0;
		return;
	}
	if( m_nStart>m_pBuffer->lines()-m_pBuffer->line()) 
	{
		m_nStart = m_pBuffer->lines()-m_pBuffer->line();
		return;
	}

	m_scrollBar->setValue( m_nStart );
	m_nEnd = m_nStart+m_pBuffer->line()-1;

	// notify bbs
	m_pBBS->setScreenStart( m_nStart );
	
	for( int i=m_nStart; i<=m_nEnd; i++ )
		m_pBuffer->at(i)->setChanged(-1,-1);

}
void QTermScreen::scrollChanged( int value )
{
	if(m_nStart == value) return;

	if( value<0 )	
		value = 0;
	if( value>m_pBuffer->lines()-m_pBuffer->line()) 
		value = m_pBuffer->lines()-m_pBuffer->line();
	
	m_nStart=value;
	m_nEnd=value+m_pBuffer->line()-1;

	// notify bbs
	m_pBBS->setScreenStart( m_nStart );

	for( int i=m_nStart; i<=m_nEnd; i++ )
		m_pBuffer->at(i)->setChanged(-1,-1);
	
	refreshScreen();
	
}

void QTermScreen::updateScrollBar()
{
	switch(m_pWindow->m_pFrame->m_nScrollPos)
	{
		case 0:
			m_scrollBar->hide();
			m_rcClient = QRect( 3,1, rect().width()-3, rect().height()-1 );
			break;
		case 1:	// LEFT
			m_scrollBar->setGeometry( 0, 0, Scrollbar_Width, rect().height() );
			m_scrollBar->show();
			m_rcClient = QRect( Scrollbar_Width+3, 1, 
							rect().width()-Scrollbar_Width-3, rect().height()-1 );
			break;
		case 2:	// RIGHT
			m_scrollBar->setGeometry( rect().width()-Scrollbar_Width, 0, 
							Scrollbar_Width, rect().height() );
			m_scrollBar->show();
			m_rcClient = QRect( 3, 1, 
							rect().width()-Scrollbar_Width-3, rect().height()-1 );
			break;
	}
	repaint(true);
}

void QTermScreen::bufferSizeChanged()
{
	disconnect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));

	m_scrollBar->setRange( 0, m_pBuffer->lines()-m_pBuffer->line());
	m_scrollBar->setLineStep(1);
	m_scrollBar->setPageStep( m_pBuffer->line() );
	m_scrollBar->setValue(m_pBuffer->lines()-m_pBuffer->line());

	m_nStart = m_scrollBar->value();
	m_nEnd =  m_scrollBar->value()+m_pBuffer->line()-1;

	// notify bbs
	m_pBBS->setScreenStart( m_nStart );

	connect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* 	                          Display	                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */

//set pixmap background

void QTermScreen::setBgPxm( const QPixmap& pixmap, int nType)
{
	m_hasBg = false;
	m_pxmBg = pixmap;
	m_nPxmType = nType;

	if( m_pWindow->m_pFrame->m_bBossColor )
	{
		setBackgroundColor( Qt::white );
		return;
	}

	switch( nType )
	{
		case 0:	// none
			setBackgroundColor( m_color[0] );
		break;
		case 1:	// transparent
		{
		}
		break;
		case 2:	// tile
			if( !pixmap.isNull() )
			{
				setBackgroundPixmap( pixmap );
				m_hasBg = true;
				break;
			}
		case 3:	// center
			if( !pixmap.isNull() )
			{
				QPixmap pxmBg;
				pxmBg.resize(size());
				pxmBg.fill( m_color[0]);
				bitBlt( &pxmBg, 
				( size().width() - pixmap.width() ) / 2,
				( size().height() - pixmap.height() ) / 2,
				 &pixmap, 0, 0,
				 pixmap.width(), pixmap.height() );
				setBackgroundPixmap(pxmBg);
				m_hasBg = true;
				break;
			}
		case 4:	// stretch
			if( !pixmap.isNull() )
			{
				float sx = (float)size().width() / pixmap.width();
				float sy = (float)size().height() /pixmap.height();
				QWMatrix matrix;
				matrix.scale( sx, sy );
				setBackgroundPixmap(pixmap.xForm( matrix ));
				m_hasBg = true;
				break;
			}
		default:
			setBackgroundColor( m_color[0] );

	}
}

void QTermScreen::blinkScreen()
{
	QPainter painter;
	painter.begin(this);
	int startx;
	for (int index=m_nStart; index<=m_nEnd; index++)
		if (m_pBlinkLine[index - m_nStart]) {
			QTermTextLine *pTextLine = m_pBuffer->at(index);
			//erase(mapToRect(0, index, pTextLine->getLength(), 1));
		        //drawLine(painter, index);
			uint linelength = pTextLine->getLength();
			QCString attr = pTextLine->getAttr();
			for (uint i = 0; i < linelength; ++i)
				if (GETBLINK(attr.at(i))) {
					startx = i;
					while(GETBLINK(attr.at(i))&& i < linelength)
						++i;
					drawLine(painter, index, startx, i, false);
				}
		}
}

// refresh the screen when 
//	1. received new contents form server
//	2. scrolled by user
void QTermScreen::refreshScreen( )
{
	if( m_cursorTimer->isActive() ) m_cursorTimer->stop();

	if( m_blinkTimer->isActive() ) m_blinkTimer->stop();

	m_hasBlink = false;

	setUpdatesEnabled(FALSE);
	
	int startx, endx;

	QPainter painter;
	painter.begin(this);
	
	
	for(int index=m_nStart; index<=m_nEnd; index++ )
	{
		if ( index>=m_pBuffer->lines() )
		{
			printf("QTermScreen::drawLine wrong index %d\n", index);
			painter.end();
			return;
		}
		QTermTextLine *pTextLine = m_pBuffer->at(index);
		
		if(pTextLine->hasBlink()){
			m_hasBlink = true;
			m_pBlinkLine[index - m_nStart] = true;
		}
		else
			m_pBlinkLine[index - m_nStart] = false;
		
		if(!pTextLine->isChanged(startx, endx))
			continue;
			
/* 
  Finally get around this for pku & ytht, don't know why some weird things
  happened when only erase and draw the changed part. 
*/
		startx = testChar(startx, index);

		erase(mapToRect(startx, index, -1, 1));

		drawLine( painter, index, startx);

		pTextLine->clearChange();
	}
	painter.end();

	setUpdatesEnabled(TRUE);

	setMicroFocusHint((m_pBuffer->caret().x()+2)*m_nCharWidth,(m_pBuffer->caret().y()+1)*m_nCharHeight, 
					m_nCharWidth, m_nCharHeight, true);

	if( m_pWindow->isConnected() )
		cursorEvent();

	if( m_pWindow->isConnected() ) m_cursorTimer->start(1000);

	if(m_hasBlink)	m_blinkTimer->start(1000);

}

int QTermScreen::testChar(int startx, int index)
{
	QTermTextLine *pTextLine = m_pBuffer->at(index);
	QString strDisplay;
	QString strTest;
	QCString cstrDisplay;
	QCString cstrTest;
	
	if ( index >= m_pBuffer->lines())
	{
		printf("QTermScreen::drawLine wrong index %d\n", index);
		return startx; 
	}

	cstrDisplay = pTextLine->getText(startx);
	cstrTest = pTextLine->getText();

	if(m_pParam->m_nDispCode != m_pParam->m_nBBSCode)
	{
		char * chDisplay;
		char * chTest;
		if(m_pParam->m_nBBSCode==0) {
			chDisplay = m_converter.G2B( cstrDisplay, cstrDisplay.length() );
			chTest = m_converter.G2B( cstrTest, cstrTest.length() );
		}

		else {
			chDisplay = m_converter.G2B( cstrDisplay, cstrDisplay.length() );
			chTest = m_converter.G2B( cstrTest, cstrTest.length() );
		}
		
		strDisplay = m_pCodec->toUnicode(chDisplay);
		strTest = m_pCodec->toUnicode(chTest);

		delete chTest;
		delete chDisplay;
	}
	else {
		strDisplay = m_pCodec->toUnicode(cstrDisplay);
		strTest = m_pCodec->toUnicode(cstrTest);
	}

	if (strTest.find(strDisplay) == -1)
		return startx - 1;
	else
		return startx;
}

		
void QTermScreen::paintEvent( QPaintEvent * pe )
{
	
	QPainter painter;

	setUpdatesEnabled( false );
	
	painter.begin( this );
	
	QRect rect = pe->rect().intersect(m_rcClient);

	QPoint tlPoint = mapToChar(QPoint(rect.left(), rect.top()));
	
	QPoint brPoint = mapToChar(QPoint(rect.right(),rect.bottom()));

	for( int y=tlPoint.y(); y<=brPoint.y(); y++ )
	{
		drawLine(painter,y);
		if( m_pBBS->isSelected(y)&&m_pParam->m_nMenuType==1 )
		{
			QRect rcMenu = mapToRect(m_pBBS->getSelectRect());
			QPixmap pxm(rcMenu.width(), rcMenu.height());
			bitBlt(this, rcMenu.x(), rcMenu.y(), &pxm, 0, 0, 
					rcMenu.width(), rcMenu.height(), Qt::NotROP );
		}
	}
	
	painter.end();

	setUpdatesEnabled( true );

	setMicroFocusHint((m_pBuffer->caret().x()+2)*m_nCharWidth,(m_pBuffer->caret().y()+1)*m_nCharHeight, 
					m_nCharWidth, m_nCharHeight, true);
}

// draw a line with the specialAtter if given.
// modified by hooey to draw part of the line.
// I know I should not use starx, but I'm lazy ;)
void QTermScreen::drawLine( QPainter& painter, int index, int starx, int endx, bool complete)
{
	if ( index >= m_pBuffer->lines())
	{
		printf("QTermScreen::drawLine wrong index %d\n", index);
		return;
	}

	QTermTextLine *pTextLine = m_pBuffer->at(index);
	QCString color = pTextLine->getColor();
	QCString attr = pTextLine->getAttr();
	uint linelength = pTextLine->getLength();

	char tempcp, tempea;
	short tempattr;
	bool bSelected;
	int startx;
	QCString cstrText;
	QString strShow;
	
	if (starx < 0)
		starx = 0;

	if (endx > linelength || endx < 0)
		endx = linelength;

	if (complete = true)
		drawMenuSelect(painter, index);

	for( uint i=starx; i < endx;i++)
	{
		int offset = 0;
		startx = i;
		tempcp = color.at(i);
		tempea = attr.at(i);
		bSelected = m_pBuffer->isSelected(QPoint(i,index), m_pWindow->m_bCopyRect);
		// get str of the same attribute
		while ( tempcp == color.at(i) && 
				tempea == attr.at(i)  && 
				bSelected == m_pBuffer->isSelected(QPoint(i,index), m_pWindow->m_bCopyRect) && 
				i < endx )
			++i;

		if(bSelected)	// selected area is text=color(0) background=color(7)
			tempattr = SETCOLOR(SETFG(0)|SETBG(7)) | SETATTR(NO_ATTR);
		else
			tempattr = SETCOLOR(tempcp) | SETATTR(tempea);
		
		QCString cstrText = pTextLine->getText(startx, i - startx);
		QString strShow;
		if(m_pParam->m_nDispCode != m_pParam->m_nBBSCode)
		{
			char * chText;
			if(m_pParam->m_nBBSCode==0)
				chText = m_converter.G2B( cstrText, cstrText.length() );
			else
				chText = m_converter.B2G( cstrText, cstrText.length() );
		
			strShow = m_pCodec->toUnicode(chText);

			delete chText;
		}
		else
			strShow = m_pCodec->toUnicode(cstrText);

		// Draw Charactors one by one to fix the variable font display problem
		for (uint j=0; j < strShow.length(); ++j){
			int length = 2;
			if (strShow.at(j) < 0x7f) {
				int en=0;
				QString en_str(strShow.at(j));
				// Pick up ascii char to draw them together
				while(strShow.at(j+en+1) < 0x7f && !(strShow.at(j+en+1)).isSpace() && j+en+1 < strShow.length()){
					++en;
					en_str += strShow.at(j+en);
				}
				length = en + 1;
				drawStr(painter, en_str, startx + offset, index, length, tempattr, bSelected );
				j += en;
			}

			else
				drawStr(painter, (QString)strShow.at(j), startx + offset, index, length, tempattr, bSelected );
			offset += length;
		}
		offset = 0;
	
		--i;
	}
}

// draw functions
void QTermScreen::drawStr( QPainter& painter, const QString& str, int x, int y, int length, 
								short attribute, bool transparent )
{
	char cp = GETCOLOR( attribute );
	char ea = GETATTR( attribute );

	// test bold mask or always highlighted
	if ( GETBOLD( ea ) || m_pParam->m_bAlwaysHighlight )
		cp = SETHIGHLIGHT( cp );           // use 8-15 color
	// test dim mask
	if ( GETDIM( ea ) )
	{
	};
	// test underline mask
	if ( GETUNDERLINE( ea ) )
	{
		m_pFont->setUnderline( true );
		painter.setFont( *m_pFont );
	}
	else
	{
		m_pFont->setUnderline( false );
		painter.setFont( *m_pFont );
	}
	 // test blink mask         
	if ( GETBLINK( ea ) )
	{
	}
	// test rapidblink mask
	if ( GETRAPIDBLINK( ea ) )
	{
	};
	// test reverse mask
	if ( GETREVERSE( ea ) )
		cp = REVERSECOLOR( cp );
	
	// test invisible mask
	if ( GETINVISIBLE( ea ) )
	{
	};
	
	QPoint pt = mapToPixel(QPoint(x,y));

	// black on white without attr
	if(m_pWindow->m_pFrame->m_bBossColor)
	{
		painter.setPen(GETFG(cp)==0?Qt::white:Qt::black);
		if( GETBG(cp)!=0 && !transparent )
			painter.setBackgroundMode(Qt::OpaqueMode);
		else
			painter.setBackgroundMode(Qt::TransparentMode);
		painter.setBackgroundColor(GETBG(cp)==7?Qt::black:Qt::white);
		painter.drawText( pt.x(), pt.y()+m_nCharAscent, str);
		return;
	}
	
	painter.setPen( m_color[m_pParam->m_bAnsiColor||GETFG(cp)==0?GETFG(cp):7] );

	if( GETBG(cp)!=0 && !transparent )
	{
		painter.setBackgroundMode(Qt::OpaqueMode);
		painter.setBackgroundColor( m_color[m_pParam->m_bAnsiColor||GETBG(cp)==7?GETBG(cp):0] );

	}
	else
		painter.setBackgroundMode(Qt::TransparentMode);


	if( m_blinkScreen && GETBLINK(GETATTR(attribute)))
	{
		if(GETBG(cp)!=0)
			painter.fillRect( mapToRect(x,y,length,1), QBrush(m_color[GETBG(cp)]) );
		else
			erase( mapToRect(x, y, length, 1) );
	}
	else {
		if(GETBG(cp)!=0)
			painter.fillRect( mapToRect(x,y,length,1), QBrush(m_color[GETBG(cp)]) );
		painter.drawText( pt.x(), pt.y()+m_nCharAscent, str);
	}
}


void QTermScreen::eraseRect( QPainter&, int, int, int, int, short )
{

}

void QTermScreen::bossColor()
{
	setBgPxm(m_pxmBg, m_nPxmType);
		
	repaint(true);
}	

void QTermScreen::drawMenuSelect( QPainter& painter, int index )
{
	QRect rcSelect, rcMenu, rcInter;
	if( m_pBuffer->isSelected(index) )
	{
		rcSelect = mapToRect(m_pBuffer->getSelectRect(index, m_pWindow->m_bCopyRect));
		if(m_pWindow->m_pFrame->m_bBossColor)
			painter.fillRect(rcSelect, Qt::black);
		else
			painter.fillRect(rcSelect, QBrush(m_color[7]));
	}
	if( m_pBBS->isSelected(index) )
	{
		rcMenu = mapToRect(m_pBBS->getSelectRect());
		QPixmap *pxm;
		switch(m_pParam->m_nMenuType)
		{
			case 0:	// underline
				pxm = new QPixmap( rcMenu.width(), m_nCharHeight/11);
				pxm->fill(m_color[7]);
				bitBlt(this, rcMenu.x(), rcMenu.y()+10*m_nCharHeight/11, pxm, 0, 0, 
									rcMenu.width(), m_nCharHeight/11, Qt::NotROP);
				delete pxm;
			break;
			case 2:
				rcInter = rcSelect.intersect(rcMenu);
				if(!rcInter.isEmpty())
				{
					pxm = new QPixmap(rcInter.width(), rcInter.height());
					pxm->fill(m_pParam->m_clrMenu);
					bitBlt(this, rcInter.x(), rcInter.y(), pxm, 0, 0, 
								rcInter.width(), rcInter.height(), Qt::XorROP );
					delete pxm;
					QRect rcTmp;
					rcTmp.setBottom(rcMenu.bottom());
					rcTmp.setTop(rcMenu.top());
					if(rcMenu.left()<rcSelect.left())
					{
						rcTmp.setLeft(rcMenu.left());
						rcTmp.setRight(rcInter.left());
						painter.fillRect(rcTmp, QBrush(m_pParam->m_clrMenu));
					}
					if(rcMenu.right()>rcSelect.right())
					{
						rcTmp.setLeft(rcInter.right());
						rcTmp.setRight(rcMenu.right());
						painter.fillRect(rcTmp, QBrush(m_pParam->m_clrMenu));
					}
				}
				else
					painter.fillRect(rcMenu, QBrush(m_pParam->m_clrMenu));
			break;
		}
	}
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                            Auxilluary	                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
QPoint QTermScreen::mapToPixel( const QPoint& point )
{
	QPoint pt = m_rcClient.topLeft();

	QPoint pxlPoint;

	pxlPoint.setX( point.x()*m_nCharWidth + pt.x() );
	pxlPoint.setY( (point.y()-m_nStart)*m_nCharHeight+ pt.y() );
	

	return pxlPoint;
}

QPoint QTermScreen::mapToChar( const QPoint& point )
{
	QPoint pt = m_rcClient.topLeft();

	QPoint chPoint;
	
	chPoint.setX( (point.x()-pt.x())/m_nCharWidth  );
	chPoint.setY( QMIN( QMAX(0,(point.y()-pt.y())/m_nCharHeight+m_nStart), m_nEnd) );

	//FIXME add bound check
	
	return chPoint;
}

QRect QTermScreen::mapToRect( int x, int y, int width, int height )
{
	QPoint pt = mapToPixel( QPoint(x,y) );
	
	if( width == -1 ) // to the end
		return QRect(pt.x(), pt.y(), size().width() , m_nCharHeight*height );
	else
		return QRect(pt.x(), pt.y(), width*m_nCharWidth, m_nCharHeight*height );
}

QRect QTermScreen::mapToRect( const QRect& rect )
{
	return mapToRect( rect.x(), rect.y(), rect.width(), rect.height() );
}

// from KImageEffect::fade
QImage& QTermScreen::fade( QImage& img, float val, const QColor& color)
{
    if (img.width() == 0 || img.height() == 0)
      return img;

    // We don't handle bitmaps
    if (img.depth() == 1)
	return img;

    unsigned char tbl[256];
    for (int i=0; i<256; i++)
	tbl[i] = (int) (val * i + 0.5);

    int red = color.red();
    int green = color.green();
    int blue = color.blue();

    QRgb col;
    int r, g, b, cr, cg, cb;

    if (img.depth() <= 8) {
	// pseudo color
	for (int i=0; i<img.numColors(); i++) {
	    col = img.color(i);
	    cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
	    if (cr > red)
		r = cr - tbl[cr - red];
	    else
		r = cr + tbl[red - cr];
	    if (cg > green)
		g = cg - tbl[cg - green];
	    else
		g = cg + tbl[green - cg];
	    if (cb > blue)
		b = cb - tbl[cb - blue];
	    else
		b = cb + tbl[blue - cb];
	    img.setColor(i, qRgba(r, g, b, qAlpha(col)));
	}

    } else {
	// truecolor
        for (int y=0; y<img.height(); y++) {
            QRgb *data = (QRgb *) img.scanLine(y);
            for (int x=0; x<img.width(); x++) {
                col = *data;
                cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
                if (cr > red)
                    r = cr - tbl[cr - red];
                else
                    r = cr + tbl[red - cr];
                if (cg > green)
                    g = cg - tbl[cg - green];
                else
                    g = cg + tbl[green - cg];
                if (cb > blue)
                    b = cb - tbl[cb - blue];
                else
                    b = cb + tbl[blue - cb];
                *data++ = qRgba(r, g, b, qAlpha(col));
            }
        }
    }

    return img;
}

void QTermScreen::imStartEvent(QIMEvent * e)
{
	m_inputContent = new QTermInput(this, m_nCharWidth, m_nCharHeight, m_nCharAscent);
	m_inputContent->setFont(*m_pFont);
	//m_inputContent->setTextFormat(Qt::RichText);

	m_inputContent->show();
}

void QTermScreen::imComposeEvent(QIMEvent * e)
{
	QString text = QString::null;

	QPoint cursor;
	int x = m_pBuffer->caretX();
	int y = m_pBuffer->caretY();
	int pos = e->cursorPos();

	text += e->text();

	m_inputContent->drawInput(text, pos);
/*
	QString text_before = text.left(pos);
	QString text_after = text.mid(pos + 1);
	QString text_select = "<u>" + text.mid(pos,1) + "</u>";

	text = text_before + text_select + text_after;
*/
	cursor = mapToPixel(QPoint(x+1,y));
	
	//m_inputContent->setText(text);
	//m_inputContent->adjustSize();
	
	if (m_inputContent->width() + cursor.x() > m_rcClient.width()){
		cursor = mapToPixel(QPoint(x,y));
		cursor.setX(m_rcClient.right() - m_inputContent->width());
	}else
		cursor = mapToPixel(QPoint(x,y));
	m_inputContent->move(cursor);
}

void QTermScreen::imEndEvent(QIMEvent * e)
{
	QString text = QString::null;
	text += e->text();
	//m_inputContent->setText(QString::null);
	delete m_inputContent;
	m_inputContent = NULL;
	emit inputEvent(&text);
}

void QTermInput::drawInput(QString & inputText, int position)
{
	d_text = inputText;
	d_pos = position;
	repaint();
}

void QTermInput::paintEvent(QPaintEvent * e)
{
	QPainter inputPainter;
	int len = 0;
	int cursor = 0;
	int width, height;
	
	if (d_pos == -1)
		d_pos = 0;
	for (int i = 0; i < d_text.length(); ++i) {
		if (d_text[i] <= 0x7f) {
			++len;
			if (i == d_pos)
				cursor = len - 1;
		}
		else {
			len += 2;
			if (i == d_pos)
				cursor = len - 2;
		}
	}
	
	width = len * d_width;
	height = d_height;

	inputPainter.begin(this);
	setFixedSize(width,height);
	erase();
	
	len = 0;
	inputPainter.setPen(Qt::black);
	//inputPainter.drawText(0 ,m_nCharAscent, text);

	for (int j = 0; j < d_text.length(); ++j) {
		inputPainter.drawText(len * d_width, d_ascent, d_text.mid(j, 1));
		if (d_text[j] <= 0x7f)
			++len;
		else
			len += 2;
	}
#ifndef Q_OS_MACX
	QRect rcCurrent(cursor * d_width, 0, d_width*2, d_height);
	if (d_text[d_pos] <= 0x7f)
		rcCurrent.setRect(cursor * d_width, 0, d_width, d_height);
	erase(rcCurrent);
	inputPainter.fillRect(rcCurrent, QBrush(Qt::darkGray));
	inputPainter.setPen(Qt::white);
	inputPainter.drawText(cursor * d_width ,d_ascent, d_text.mid(cursor, 1));
#endif
	inputPainter.end();
}
