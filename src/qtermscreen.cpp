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

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QTextCodec>
#include <QFontMetrics>
#include <QTimer>
#include <QEvent>
#include <QPoint>
#include <QClipboard>
#include <QFile>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QMoveEvent>
#include <QByteArray>
#include <QWheelEvent>
#include <QInputMethodEvent>
#include <QShortcut>

#include <stdio.h>
#include <stdlib.h>

#include <QtDebug>
namespace QTerm
{
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                            Constructor/Destructor                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

Screen::Screen( QWidget *parent, Buffer *buffer, Param *param, BBS *bbs )
	: QWidget( parent ),Scrollbar_Width(15)
{	
	m_pWindow = (Window *)parent;
	m_pBBS = bbs;
	m_pParam = param;
	//m_pCanvas = NULL;
	m_ePaintState = Repaint;
	m_bCursor = true;

	setFocusPolicy(Qt::ClickFocus);
	setAttribute(Qt::WA_InputMethodEnabled, true);
	setMouseTracking(true);

	setSchema();

	initFontMetrics();

	m_pBuffer = buffer;
	connect( m_pBuffer, SIGNAL(bufferSizeChanged()), 
			this, SLOT(bufferSizeChanged()) );

	m_blinkTimer = new QTimer(this);
	connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkEvent()));

	m_cursorTimer = new QTimer(this);
	connect(m_cursorTimer, SIGNAL(timeout()), this, SLOT(cursorEvent()));

	// the scrollbar
	m_scrollBar = new QScrollBar(this);
	m_scrollBar->setCursor( Qt::ArrowCursor );
	m_nStart = 0;
	m_nEnd = m_pBuffer->line()-1;
	m_pBlinkLine = new bool[m_nEnd - m_nStart + 1];
	m_scrollBar->setRange( 0, 0 );
	m_scrollBar->setSingleStep(1);
	m_scrollBar->setPageStep( m_pBuffer->line() );
	m_scrollBar->setValue(0);
	connect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));
// 	m_pAccel = new Q3Accel(this);

// 	#if (QT_VERSION>=300)
// 	m_pAccel->connectItem(m_pAccel->insertItem(Qt::Key_PageUp+Qt::SHIFT), this, SLOT(prevPage()));
// 	m_pAccel->connectItem(m_pAccel->insertItem(Qt::Key_PageDown+Qt::SHIFT), this, SLOT(nextPage()));
// 	m_pAccel->connectItem(m_pAccel->insertItem(Qt::Key_Up+Qt::SHIFT), this, SLOT(prevLine()));
// 	m_pAccel->connectItem(m_pAccel->insertItem(Qt::Key_Down+Qt::SHIFT), this, SLOT(nextLine()));
// 	#endif
	m_scPrevPage = new QShortcut(Qt::Key_PageUp+Qt::SHIFT, this, "prevPage");
	m_scNextPage = new QShortcut(Qt::Key_PageDown+Qt::SHIFT, this, "nextPage");
	m_scPrevLine = new QShortcut(Qt::Key_Up+Qt::SHIFT, this, "prevLine");
	m_scNextLine = new QShortcut(Qt::Key_Down+Qt::SHIFT, this, "nextLine");
	
	setAttribute(Qt::WA_OpaquePaintEvent, true);

// init variable
	m_blinkScreen = false;
	m_blinkCursor = true;

	if( m_pParam->m_nDispCode == 0 )
		m_pCodec = QTextCodec::codecForName("GBK");
	else
		m_pCodec = QTextCodec::codecForName("Big5");
	
// 	switch(m_pParam->m_nMenuType)
// 	{
// 		case 0:
// 			m_pBand = new QRubberBand(QRubberBand::Line, this);
// 			break;
// 		case 1:
// 		case 2:
// 			m_pBand = new QRubberBand(QRubberBand::Rectangle, this);
// 			break;
// 	}
// 	m_pBand = new QRubberBand(QRubberBand::Rectangle, this);
// 	setAttribute(Qt::WA_InputMethodEnabled);
}

Screen::~Screen()
{
	delete [] m_pBlinkLine;
	delete m_blinkTimer;
	delete m_cursorTimer;
	//delete m_pCanvas;
	//delete m_inputContent;
}

// focus event received
void Screen::focusInEvent( QFocusEvent * )
{

//	#if (QT_VERSION>0x030300) // windows get minimized when closing
//	or switching by ctrl+tab. blame me for this...
	if(m_pWindow->isMaximized()&&m_pWindow->m_pFrame->wndmgr->afterRemove())
	{
		m_pWindow->showNormal();
		m_pWindow->showMaximized();
	}
//	#endif

	m_pWindow->m_pFrame->wndmgr->activateTheTab( m_pWindow );
	
	m_scPrevPage->setEnabled(true);
	m_scNextPage->setEnabled(true);
	m_scPrevLine->setEnabled(true);
	m_scNextLine->setEnabled(true);
	
// #if (QT_VERSION<300)
// 	m_pAccel->connectItem(idPrevPage=m_pAccel->insertItem(Qt::Key_PageUp+Qt::SHIFT),
// 					this, SLOT(prevPage()));
// 	m_pAccel->connectItem(idNextPage=m_pAccel->insertItem(Qt::Key_PageDown+Qt::SHIFT),
// 					this, SLOT(nextPage()));
// 	m_pAccel->connectItem(idPrevLine=m_pAccel->insertItem(Qt::Key_Up+Qt::SHIFT), 
// 					this, SLOT(prevLine()));
// 	m_pAccel->connectItem(idNextLine=m_pAccel->insertItem(Qt::Key_Down+Qt::SHIFT),
// 					this, SLOT(nextLine()));
// #endif
}

// focus out
void Screen::focusOutEvent( QFocusEvent * )
{
	m_scPrevPage->setEnabled(false);
	m_scNextPage->setEnabled(false);
	m_scPrevLine->setEnabled(false);
	m_scNextLine->setEnabled(false);
// #if (QT_VERSION<300)
// 	m_pAccel->removeItem(idPrevPage);
// 	m_pAccel->removeItem(idNextPage);
// 	m_pAccel->removeItem(idPrevLine);
// 	m_pAccel->removeItem(idNextLine);
// #endif
}


void Screen::cursorEvent()
{
	if(m_blinkCursor) {
		m_ePaintState = Cursor;
		m_bCursor = !m_bCursor;
		update();
	}
}

void Screen::updateCursor()
{

	QPainter painter(this);

	if( m_pBuffer->caretY()<=m_nEnd && m_pBuffer->caretY()>=m_nStart )
	{
		QPoint pt=mapToPixel(QPoint(m_pBuffer->caretX(),m_pBuffer->caretY()));
		int startx = testChar(m_pBuffer->caretX(),m_pBuffer->caretY());
		int endx = testChar(m_pBuffer->caretX()+1, m_pBuffer->caretY());
		if (startx == m_pBuffer->caretX())
			if (endx == startx)
				endx += 2;

		switch(m_pParam->m_nCursorType)
		{
			case 0:	// block
				drawLine(painter,m_pBuffer->caretY(),startx,endx,false);
				break;
			case 1:	// underline
				if (m_bCursor)
					painter.fillRect(pt.x(), pt.y()+9*m_nCharHeight/10, m_nCharWidth, m_nCharHeight/10, m_color[7]);
				else
					painter.fillRect(pt.x(), pt.y()+9*m_nCharHeight/10, m_nCharWidth, m_nCharHeight/10, m_color[0]);
				break;
			case 2:	// I type
				if (m_bCursor)
					painter.fillRect(pt.x(), pt.y(), m_nCharWidth/9, m_nCharHeight, m_color[7]);
				else
					painter.fillRect(pt.x(), pt.y(), m_nCharWidth/9, m_nCharHeight, m_color[0]);
				break;
			default:
				drawLine(painter,m_pBuffer->caretY(),startx,endx,false);
		}
	}
}

void Screen::blinkEvent()
{
	if(m_hasBlink)
	{
		m_blinkScreen = !m_blinkScreen;
		m_ePaintState = Blink;
		update();
	}
}

void Screen::moveEvent( QMoveEvent * )
{
//	setBgPxm( m_pxmBg, m_nPxmType );
}
void Screen::resizeEvent( QResizeEvent *  )
{
	updateScrollBar();
	setBgPxm( m_pxmBg, m_nPxmType );

// 	if (m_pCanvas != NULL) {
// 		delete m_pCanvas;
// 		m_pCanvas = NULL;
// 	}
// 	m_pCanvas = new QPixmap(width(), height());
	
	if( m_pParam->m_bAutoFont )
	{
		updateFont();
	}
	else 
	{
		int cx = m_rcClient.width()/m_nCharWidth;
		int cy = m_rcClient.height()/m_nCharHeight;
		if( cx>10 && cy>10 )
		{
			m_pBuffer->setSize( cx, cy );
		}
	}
	m_ePaintState = Show;
	update();
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                         Mouse                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void Screen::enterEvent( QEvent * e )
{
	QApplication::sendEvent(m_pWindow, e);
}

void Screen::leaveEvent( QEvent * e )
{
	QApplication::sendEvent(m_pWindow, e);
}

void Screen::mousePressEvent( QMouseEvent * me )
{
	setFocus();

	m_pWindow->mousePressEvent(me);
	//QApplication::sendEvent(m_pWindow, me);

}
void Screen::mouseMoveEvent( QMouseEvent * me)
{
#ifdef Q_OS_MACX
	m_pWindow->mouseMoveEvent(me);
#else
	m_pWindow->mouseMoveEvent(me);
	//QApplication::sendEvent(m_pWindow, me);
#endif
}

void Screen::mouseReleaseEvent( QMouseEvent * me )
{
	m_pWindow->mouseReleaseEvent(me);
	//QApplication::sendEvent(m_pWindow, me);
}

void Screen::wheelEvent( QWheelEvent * we )
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

void Screen::initFontMetrics()
{
	if(m_pParam->m_bAutoFont)
		m_pFont = new QFont(m_pParam->m_strFontName); 
	else {
		m_pFont = new QFont(m_pParam->m_strFontName, qMax(8,m_pParam->m_nFontSize) ); 
		QFontMetrics *fm = new QFontMetrics( *m_pFont );

		getFontMetrics(fm);
		delete fm;
	}
	m_pFont->setWeight(QFont::Normal);

// 	#if (QT_VERSION >= 300 )
// 	m_pFont->setStyleHint(QFont::System, 
// 		m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
// 	#endif
	
}

void Screen::setDispFont( const QFont& font)
{
	delete m_pFont;
	if(m_pParam->m_bAutoFont)
		m_pFont = new QFont(font.family());
	else {
		m_pFont = new QFont(font);
		
		QFontInfo fi(*m_pFont);
		//int nSize = fi.pixelSize();
		QFontMetrics *fm = new QFontMetrics( *m_pFont );
		getFontMetrics(fm);
		delete fm;
	}

// 	#if (QT_VERSION >= 300 )
// 	m_pFont->setStyleHint(QFont::System, 
// 		m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
// 	#endif

}

QFont Screen::getDispFont( )
{
	return *m_pFont;
}


void Screen::updateFont()
{
	int nPixelSize;
	int nIniSize = qMax(8,qMin(m_rcClient.height()/m_pBuffer->line(),
					m_rcClient.width()*2/m_pBuffer->columns()));

	for( nPixelSize=nIniSize-3; nPixelSize<=nIniSize+3; nPixelSize++)
	{
		m_pFont->setPixelSize( nPixelSize );
		
		QFontMetrics fm( *m_pFont );
		getFontMetrics(&fm);
		if( (m_pBuffer->line()*m_nCharHeight)>m_rcClient.height()
			|| (m_pBuffer->columns()*m_nCharWidth)>m_rcClient.width()  )
		{
			while(nPixelSize>5)
			{
				nPixelSize--;
				m_pFont->setPixelSize( nPixelSize );
		
				QFontMetrics fm2( *m_pFont );
				getFontMetrics(&fm2);
				if( (m_pBuffer->line()*m_nCharHeight)<m_rcClient.height()
				&& (m_pBuffer->columns()*m_nCharWidth)<m_rcClient.width()  )
					break;
			}
			break;	
		}
	}
	m_pFont->setWeight(QFont::Normal);
// 	#if (QT_VERSION >= 300 )
// 	m_pFont->setStyleHint(QFont::System, 
// 		m_pWindow->m_pFrame->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
// 	#endif

}

void Screen::getFontMetrics(QFontMetrics *fm)
{
	float cn=fm->width(QChar(0x4e00));
	float en=fm->width('W');
	if(en/cn<0.7) // almost half
		m_nCharWidth = qMax((cn+1)/2,en);
	else
		m_nCharWidth = (qMax(en,cn)+1)/2;

	m_nCharHeight = fm->height();
	m_nCharAscent = fm->ascent();
	m_nCharDescent = fm->descent();
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                               Colors                                   */
/*	                                                                        */
/* ------------------------------------------------------------------------ */
void Screen::setSchema()
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
		Config *pConf = new Config(m_pParam->m_strSchemaFile);

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
			QImage ima(m_pxmBg.toImage());
			ima = fade(ima, alpha, fadecolor);
			m_pxmBg = QPixmap::fromImage(ima);

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
void Screen::prevPage()
{
	scrollLine(-m_pBuffer->line());
	m_ePaintState = NewData;
	update();
}

void Screen::nextPage()
{
	scrollLine(m_pBuffer->line());
	m_ePaintState = NewData;
	update();
}

void Screen::prevLine()
{
	scrollLine(-1);
	m_ePaintState = NewData;
	update();
}

void Screen::nextLine()
{
	scrollLine(1);
	m_ePaintState = NewData;
	update();
}

void Screen::scrollLine( int delta )
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
void Screen::scrollChanged( int value )
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
	
	m_ePaintState = NewData;
	update();
	
}

void Screen::updateScrollBar()
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
// 	qDebug("second: %d, %d", m_rcClient.width(),m_rcClient.height());
	
// 	if (m_pCanvas != NULL) {
// 		delete m_pCanvas;
// 		m_pCanvas = NULL;
// 	}
// 	m_pCanvas = new QPixmap(m_rcClient.width(), m_rcClient.height());
	//repaint(true);
	update();
}

void Screen::bufferSizeChanged()
{
	disconnect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));

	m_scrollBar->setRange( 0, m_pBuffer->lines()-m_pBuffer->line());
	m_scrollBar->setSingleStep(1);
	m_scrollBar->setPageStep( m_pBuffer->line() );
	m_scrollBar->setValue(m_pBuffer->lines()-m_pBuffer->line());

	m_nStart = m_scrollBar->value();
	m_nEnd =  m_scrollBar->value()+m_pBuffer->line()-1;

	// notify bbs
	m_pBBS->setScreenStart( m_nStart );

	connect(m_scrollBar, SIGNAL(valueChanged(int)), 
			this, SLOT(scrollChanged(int)));

	delete []m_pBlinkLine;
	m_pBlinkLine = new bool[m_nEnd - m_nStart + 1];

	scrollLine(0);
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* 	                          Display	                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */

//set pixmap background

void Screen::setBgPxm( const QPixmap& pixmap, int nType)
{
	m_hasBg = false;
	m_pxmBg = pixmap;
	m_nPxmType = nType;
	QPalette palette;

	if( m_pWindow->m_pFrame->m_bBossColor )
	{
		palette.setColor(backgroundRole(), Qt::white);
		setPalette(palette);
// 		setBackgroundColor( Qt::white );
		return;
	}

	switch( nType )
	{
		case 0:	// none
			palette.setColor(backgroundRole(), m_color[0]);
			setPalette(palette);
		break;
		case 1:	// transparent
		{
		}
		break;
		case 2:	// tile
			if( !pixmap.isNull() )
			{
				palette.setBrush(backgroundRole(), QBrush(pixmap));
				setPalette(palette);
// 				setBackgroundPixmap( pixmap );
				m_hasBg = true;
				break;
			}
		case 3:	// center
			if( !pixmap.isNull() )
			{
				QPixmap pxmBg = QPixmap(size());
				QPainter painter(&pxmBg);
				pxmBg.fill( m_color[0]);
				painter.drawPixmap( ( size().width() - pixmap.width() ) / 2,
						( size().height() - pixmap.height() ) / 2,  pixmap.width(),
						pixmap.height(), pixmap );
				palette.setBrush(backgroundRole(), QBrush(pxmBg));
				setPalette(palette);
// 				setBackgroundPixmap(pxmBg);
				m_hasBg = true;
				break;
			}
		case 4:	// stretch
			if( !pixmap.isNull() )
			{
				float sx = (float)size().width() / pixmap.width();
				float sy = (float)size().height() /pixmap.height();
				QMatrix matrix;
				matrix.scale( sx, sy );
				palette.setBrush(backgroundRole(), QBrush(pixmap.transformed( matrix )));
				setPalette(palette);
// 				setBackgroundPixmap(pixmap.transformed( matrix ));
				m_hasBg = true;
				break;
			}
		default:
			palette.setColor(backgroundRole(), m_color[0]);
			setPalette(palette);
// 			setBackgroundColor( m_color[0] );

	}
}

void Screen::blinkScreen()
{
	QPainter painter;
	painter.begin(this);
	painter.setBackground(QBrush(m_color[0]));
	int startx;
	for (int index=m_nStart; index<=m_nEnd; index++) {
		if (m_pBlinkLine[index - m_nStart]) {
			TextLine *pTextLine = m_pBuffer->at(index);
			uint linelength = pTextLine->getLength();
			QByteArray attr = pTextLine->getAttr();
			for (uint i = 0; i < linelength; ++i) {
				if (GETBLINK(attr.at(i))) {
					startx = i;
					while(i < linelength && GETBLINK(attr.at(i)))
						++i;
					drawLine(painter, index, startx, i, false);
				}
			}
		}
	}
}

// refresh the screen when 
//	1. received new contents form server
//	2. scrolled by user
void Screen::refreshScreen()
{
	if( m_cursorTimer->isActive() ) m_cursorTimer->stop();

	if( m_blinkTimer->isActive() ) m_blinkTimer->stop();

	m_hasBlink = false;
	m_bCursor = true;

	//setUpdatesEnabled(false);
	
	int startx, endx;

	QPainter painter;
	painter.begin(this);
	//qDebug("size: %d, %d", width(),height());
	if(m_ePaintState == Show)
		painter.fillRect(m_rcClient,m_color[0]);
	
	for(int index=m_nStart; index<=m_nEnd; index++ )
	{
		if ( index>=m_pBuffer->lines() )
		{
			printf("Screen::drawLine wrong index %d\n", index);
			painter.end();
			return;
		}

		if(m_ePaintState == Show) {
			drawLine(painter, index, 0, -1);
			continue;
		}

		TextLine *pTextLine = m_pBuffer->at(index);
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
		painter.fillRect(mapToRect(startx, index, -1, 1),m_color[0]);
		drawLine( painter, index, startx);
		pTextLine->clearChange();
	}
	painter.end();

	updateMicroFocus();
	if( m_pWindow->isConnected() ) {
		m_ePaintState = Cursor;
		m_bCursor = true;
		updateCursor();
	}


	if( m_pWindow->isConnected() ) m_cursorTimer->start(1000);

	if(m_hasBlink)	m_blinkTimer->start(1000);
}

int Screen::testChar(int startx, int index)
{
	TextLine *pTextLine = m_pBuffer->at(index);
	QString strDisplay;
	QString strTest;
	QByteArray cstrDisplay;
	QByteArray cstrTest;
	
	if ( startx <= 0 )
		return 0;
	if ( index >= m_pBuffer->lines())
	{
		printf("Screen::drawLine wrong index %d\n", index);
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

	if (strTest.indexOf(strDisplay) == -1)
		return startx - 1;
	else
		return startx;
}

		
void Screen::paintEvent( QPaintEvent * pe )
{
// 	qDebug()<<"Event:"<<m_ePaintState;
	switch(m_ePaintState)
	{
		case NewData:
		case Show:
			refreshScreen();
			m_ePaintState = Repaint;
			break;
		case Blink:
			blinkScreen();
			m_ePaintState = Repaint;
			break;
		case Cursor:
			updateCursor();
			m_ePaintState = Repaint;
			break;
		case Repaint:
			repaintScreen(pe);
			break;
	}
}

void Screen::repaintScreen(QPaintEvent * pe)
{
	QPainter painter;
	painter.begin( this );
	painter.setBackground(QBrush(m_color[0]));
	
// 	qDebug("rcClient: %d, %d", m_pCanvas->width(),m_pCanvas->height());

	QRect rect = pe->rect().intersect(m_rcClient);
	painter.eraseRect(rect);

	QPoint tlPoint = mapToChar(QPoint(rect.left(), rect.top()));
	
	QPoint brPoint = mapToChar(QPoint(rect.right(),rect.bottom()));

	for( int y=tlPoint.y(); y<=brPoint.y(); y++ )
	{
		drawLine(painter,y);
		//if( m_pBBS->isSelected(y)&&m_pParam->m_nMenuType==1 )
		//{
		//	QRect rcMenu = mapToRect(m_pBBS->getSelectRect());
		//	QPixmap pxm(rcMenu.width(), rcMenu.height());
		//	bitBlt(this, rcMenu.x(), rcMenu.y(), &pxm, 0, 0, 
		//			rcMenu.width(), rcMenu.height(), Qt::NotROP );
		//}
	}
	
	painter.end();
}

// draw a line with the specialAtter if given.
// modified by hooey to draw part of the line.
// I know I should not use starx, but I'm lazy ;)
void Screen::drawLine( QPainter& painter, int index, int starx, int endx, bool complete)
{
	if ( index >= m_pBuffer->lines())
	{
		printf("Screen::drawLine wrong index %d\n", index);
		return;
	}

	TextLine *pTextLine = m_pBuffer->at(index);
	QByteArray color = pTextLine->getColor();
	QByteArray attr = pTextLine->getAttr();
	uint linelength = pTextLine->getLength();

	char tempcp, tempea;
	short tempattr;
	bool bSelected;
	int startx;
	QByteArray cstrText;
	QString strShow;
	
	if (starx < 0)
		starx = 0;

	if (endx > linelength || endx < 0)
		endx = linelength;
	
	if (complete == true && m_pBBS->isSelected(index)){
		drawMenuSelect(painter, index);
		starx = 0;
		endx = linelength;
	}

	for( uint i=starx; i < endx;i++)
	{
		int offset = 0;
		startx = i;
		tempcp = color.at(i);
		tempea = attr.at(i);
		bSelected = m_pBuffer->isSelected(QPoint(i,index), m_pWindow->m_bCopyRect);
		// get str of the same attribute
		while ( i < endx && tempcp == color.at(i) && 
				tempea == attr.at(i)  && 
				bSelected == m_pBuffer->isSelected(QPoint(i,index), m_pWindow->m_bCopyRect) )
			++i;

		if(bSelected)	// selected area is text=color(0) background=color(7)
			tempattr = SETCOLOR(SETFG(0)|SETBG(7)) | SETATTR(NO_ATTR);
		else
			tempattr = SETCOLOR(tempcp) | SETATTR(tempea);
		
		cstrText = pTextLine->getText(startx, i - startx);
 		//qDebug()<<"raw: " << cstrText;
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

 		//qDebug() << "string: " << strShow.toLocal8Bit();
		// Draw Charactors one by one to fix the variable font display problem
		for (uint j=0; j < strShow.length(); ++j){
			int length = 2;
			if (strShow.at(j) < 0x7f) {
				drawStr(painter, (QString)strShow.at(j), startx + offset, index, 1, tempattr, bSelected );
				offset++;
			}
			else {
				drawStr(painter, (QString)strShow.at(j), startx + offset, index, length, tempattr, bSelected );
				offset += 2;
			}
		}
		offset = 0;
	
		--i;
	}
}

// draw functions
void Screen::drawStr( QPainter& painter, const QString& str, int x, int y, int length, 
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
	if ( GETREVERSE( ea ) || (m_ePaintState == Cursor && m_bCursor))
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
		painter.setBackground(GETBG(cp)==7?Qt::black:Qt::white);
		painter.drawText( pt.x(), pt.y()+m_nCharAscent, str);
		return;
	}
	
	painter.setPen( m_color[m_pParam->m_bAnsiColor||GETFG(cp)==0?GETFG(cp):7] );

	if( GETBG(cp)!=0 && !transparent )
	{
		painter.setBackgroundMode(Qt::OpaqueMode);
		painter.setBackground( m_color[m_pParam->m_bAnsiColor||GETBG(cp)==7?GETBG(cp):0] );

	}
	else
		painter.setBackgroundMode(Qt::TransparentMode);


	if( m_blinkScreen && GETBLINK(GETATTR(attribute)))
	{
		if(GETBG(cp)!=0)
			painter.fillRect( mapToRect(x,y,length,1), QBrush(m_color[GETBG(cp)]) );
		else
			painter.eraseRect( mapToRect(x, y, length, 1) );
	}
	else {
		if (GETBG(cp)!=0 || m_ePaintState == Cursor)
			painter.fillRect( mapToRect(x,y,length,1), QBrush(m_color[GETBG(cp)]) );
		painter.drawText( pt.x(), pt.y()+m_nCharAscent, str);
	}
}


void Screen::eraseRect( QPainter&, int, int, int, int, short )
{

}

void Screen::bossColor()
{
	setBgPxm(m_pxmBg, m_nPxmType);
		
	//repaint(true);
	update();
}	

void Screen::drawMenuSelect( QPainter& painter, int index )
{
	QRect rcSelect, rcMenu, rcInter;
// 	FIXME: what is this for
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
// 		m_pBand->setGeometry(rcMenu);
// 		m_pBand->show();
		switch(m_pParam->m_nMenuType)
		{
			case 0:	// underline
// 				m_pBand->setGeometry(rcMenu.x(), rcMenu.y()+10*m_nCharHeight/11,
// 									 rcMenu.width(), m_nCharHeight/11);
// 				m_pBand->show();
// 				pxm = new QPixmap( rcMenu.width(), m_nCharHeight/11);
// 				pxm->fill(m_color[7]);
// 				bitBlt(this, rcMenu.x(), rcMenu.y()+10*m_nCharHeight/11, pxm, 0, 0, 
// 									rcMenu.width(), m_nCharHeight/11, Qt::NotROP);
// 				delete pxm;
				painter.fillRect(rcMenu.x(),rcMenu.y()+10*m_nCharHeight/11, rcMenu.width(), m_nCharHeight/11, m_color[7]);
				break;
			case 2:
// 				rcInter = rcSelect.intersect(rcMenu);
// 				if(!rcInter.isEmpty())
// 				{
// 					pxm = new QPixmap(rcInter.width(), rcInter.height());
// 					pxm->fill(m_pParam->m_clrMenu);
// 					bitBlt(this, rcInter.x(), rcInter.y(), pxm, 0, 0, 
// 								rcInter.width(), rcInter.height(), Qt::XorROP );
// 					delete pxm;
// 					QRect rcTmp;
// 					rcTmp.setBottom(rcMenu.bottom());
// 					rcTmp.setTop(rcMenu.top());
// 					if(rcMenu.left()<rcSelect.left())
// 					{
// 						rcTmp.setLeft(rcMenu.left());
// 						rcTmp.setRight(rcInter.left());
// 						painter.fillRect(rcTmp, QBrush(m_pParam->m_clrMenu));
// 					}
// 					if(rcMenu.right()>rcSelect.right())
// 					{
// 						rcTmp.setLeft(rcInter.right());
// 						rcTmp.setRight(rcMenu.right());
// 						painter.fillRect(rcTmp, QBrush(m_pParam->m_clrMenu));
// 					}
// 				}
// 				else
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
QPoint Screen::mapToPixel( const QPoint& point )
{
	QPoint pt = m_rcClient.topLeft();

	QPoint pxlPoint;

	pxlPoint.setX( point.x()*m_nCharWidth + pt.x() );
	pxlPoint.setY( (point.y()-m_nStart)*m_nCharHeight+ pt.y() );
	

	return pxlPoint;
}

QPoint Screen::mapToChar( const QPoint& point )
{
	QPoint pt = m_rcClient.topLeft();

	QPoint chPoint;
	
	chPoint.setX( qMin( qMax(0,(point.x()-pt.x())/m_nCharWidth), m_pBuffer->columns()-1));
	chPoint.setY( qMin( qMax(0,(point.y()-pt.y())/m_nCharHeight+m_nStart), m_nEnd) );

	//FIXME add bound check
	
	return chPoint;
}

QRect Screen::mapToRect( int x, int y, int width, int height )
{
	QPoint pt = mapToPixel( QPoint(x,y) );
	
	if( width == -1 ) // to the end
		return QRect(pt.x(), pt.y(), size().width() , m_nCharHeight*height );
	else
		return QRect(pt.x(), pt.y(), width*m_nCharWidth, m_nCharHeight*height );
}

QRect Screen::mapToRect( const QRect& rect )
{
	return mapToRect( rect.x(), rect.y(), rect.width(), rect.height() );
}

// from KImageEffect::fade
QImage& Screen::fade( QImage& img, float val, const QColor& color)
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

void Screen::inputMethodEvent(QInputMethodEvent * e)
{
	QString text = e->commitString();
	emit inputEvent(&text);
}

QVariant Screen::inputMethodQuery(Qt::InputMethodQuery property) const
{
// 	qDebug()<<"inputMethodQuery";
	switch(property)
	{
		case Qt::ImMicroFocus:
			return QVariant(QRect((m_pBuffer->caret().x()+1)*m_nCharWidth,(m_pBuffer->caret().y()+1)*m_nCharHeight, 
							m_nCharWidth, m_nCharHeight));
		case Qt::ImFont:
			return QVariant(*m_pFont);
		case Qt::ImCursorPosition:
			return m_pBuffer->caret().x();
		default:
			return QVariant();
	}
}
/*
void Screen::imStartEvent(QIMEvent * e)
{
	//m_inputContent = new QTermInput(this, m_nCharWidth, m_nCharHeight, m_nCharAscent);
	//m_inputContent->setFont(*m_pFont);
	//m_inputContent->setTextFormat(Qt::RichText);

	//m_inputContent->show();
}

void Screen::imComposeEvent(QIMEvent * e)
{
	if (m_inputContent == NULL){
		m_inputContent = new QTermInput(this, m_nCharWidth, m_nCharHeight, m_nCharAscent);
		m_inputContent->setFont(*m_pFont);
		m_inputContent->show();
	}
	QString text = QString::null;

	QPoint cursor;
	int x = m_pBuffer->caretX();
	int y = m_pBuffer->caretY();
	int pos = e->cursorPos();

	text += e->text();
// how dirty
#ifdef Q_OS_MACX
ushort code=text[text.length()-1].unicode();
if( (code>0xff00&&code<0xffef) || 
(code>0x3000&&code<0x303f) || 
(code>0x2000&&code<0x206f) ||
code==0x00b7)
{
emit inputEvent(&text);
return;
}
#endif
	m_inputContent->drawInput(text, pos);

	QString text_before = text.left(pos);
	QString text_after = text.mid(pos + 1);
	QString text_select = "<u>" + text.mid(pos,1) + "</u>";

	text = text_before + text_select + text_after;

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

void Screen::imEndEvent(QIMEvent * e)
{
	QString text = QString::null;
	text += e->text();
	//m_inputContent->setText(QString::null);
	delete m_inputContent;
	m_inputContent = NULL;
	emit inputEvent(&text);
}
*/
/*
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
	
	if (d_pos == -1 || d_pos >= d_text.length())
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
	inputPainter.drawText(cursor * d_width ,d_ascent, d_text.mid(d_pos, 1));
#endif
	inputPainter.end();
}*/

} // namespace QTerm

#include <qtermscreen.moc>
