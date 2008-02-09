#ifndef QTERMSCREEN_H
#define QTERMSCREEN_H

// #include <qwidget.h>
// #include <qscrollbar.h>
// #include <qlabel.h>
#include "qtermconvert.h"
//Added by qt3to4:
#include <QWheelEvent>
#include <QFocusEvent>
#include <QPaintEvent>
#include <QMoveEvent>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <QShortcut>

class QTextCodec;
// class QShortcut;
class QColor;
class QPainter;
class QInputMethodEvent;

namespace QTerm
{
class Window;
class Buffer;
class BBS;
class Param;
// class Q3Accel;
/*
class QTermInput : public QWidget
{
	Q_OBJECT
public:
	QTermInput(QWidget * parent, int width, int height, int ascent)
		:QWidget(parent),d_text(0),d_pos(0),d_height(height), d_width(width), d_ascent(ascent)
	{
	}
	~QTermInput()
	{
	}
	void drawInput(QString & text, int position);
protected:
	void paintEvent( QPaintEvent * );
private:
	QString d_text;
	int d_pos;
	int d_height, d_width, d_ascent;
};
*/
class Screen : public QWidget
{
	Q_OBJECT
public:
	enum PaintState {NewData, Blink, Cursor, Repaint, Show};
	enum MouseState {Enter, Press, Move, Release, Leave};
	Screen( QWidget *parent, Buffer *buffer, Param *param, BBS *bbs );
 	~Screen();

	void setSchema();

	QFont getDispFont();
	void setDispFont( const QFont& );
	
	void setBgPxm( const QPixmap& pixmap, int nType = 0);

	void refreshScreen();
	void blinkScreen();
	void updateCursor();
	void repaintScreen(QPaintEvent * pe);
	PaintState m_ePaintState;

signals:
	// 0 - enter  1 - press  2 - move  3 - release 4 - leave
	void mouseAction( int, QMouseEvent * );
	void inputEvent(QString *);

public slots:
	
	void bufferSizeChanged();
	void bossColor();
	void updateScrollBar();

protected:	
	void initFontMetrics();

	void moveEvent( QMoveEvent * );
	void resizeEvent( QResizeEvent * );
	void focusInEvent ( QFocusEvent * );	
	void focusOutEvent ( QFocusEvent * );	
	void showEvent( QShowEvent * );
	bool event( QEvent * );

	void paintEvent( QPaintEvent * );

	// mouse
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	void mousePressEvent( QMouseEvent * );
	void mouseMoveEvent(QMouseEvent* );
	void mouseReleaseEvent(QMouseEvent*);
	void wheelEvent( QWheelEvent * );
	
	// display
	int testChar(int, int);
	void eraseRect( QPainter& , int, int, int, int, short );
	void drawStr( QPainter&, const QString&, int, int, int, short, bool );
	void drawLine( QPainter&, int index, int startx = -1, int endx = -1, bool complete = true);
	void drawCaret( QPainter&, bool );
	void drawMenuSelect( QPainter&, int );

	// auxiluary
	int getPosX( int xChar ) { return xChar*m_nCharWidth; }
	int getPosY( int yLine ) { return yLine*m_nCharHeight;}

	QPoint mapToPixel( const QPoint& );
	QPoint mapToChar( const QPoint& );

	QRect mapToRect( int, int, int, int );
	QRect mapToRect( const QRect& );
	
	void updateFont();
	void getFontMetrics(QFontMetrics *);

	QImage& fade( QImage&, float, const QColor&);
/*
	void imStartEvent(QIMEvent * e);
	void imComposeEvent(QIMEvent * e);
	void imEndEvent(QIMEvent * e);
	//void drawInput(QString &, int);
*/
	void inputMethodEvent(QInputMethodEvent *e);
	QVariant inputMethodQuery(Qt::InputMethodQuery property) const;
	
protected slots:
	void blinkEvent();
	void cursorEvent();
	void scrollChanged( int );
	void prevPage();
	void nextPage();
	void prevLine();
	void nextLine();
	void scrollLine(int);
protected:

	QRect  m_rcClient;	// the display area

	int Scrollbar_Width; 
	
	QScrollBar * m_scrollBar;
	
	QTimer * m_blinkTimer;
	QTimer * m_cursorTimer;
	//QTermInput * m_inputContent;

	bool m_blinkScreen;
	bool m_blinkCursor;
	bool m_hasBlink;

	Window * m_pWindow;
	BBS * m_pBBS;
	Buffer * m_pBuffer;
	Param * m_pParam;
// 	QRubberBand * m_pBand;

	QColor m_color[16];
	QFont *m_pFont;
	//QPixmap * m_pCanvas;
	
	int m_nCharAscent,m_nCharDescent,m_nCharWidth,m_nCharHeight;
	int m_nLineSpacing;	// for future

	bool * m_pBlinkLine;
	bool   m_bCursor;
	
	// background
	bool m_hasBg;
	QPixmap m_pxmBg;
	int m_nPxmType;
	
	// the range of the buffer to be displayed
	int m_nStart;
	int m_nEnd;

// 	Q3Accel * m_pAccel;
	QShortcut * m_scPrevPage;
	QShortcut * m_scNextPage;
	QShortcut * m_scPrevLine;
	QShortcut * m_scNextLine;

	Convert m_converter;

	QTextCodec *m_pCodec;
	
	friend class Window;

	// for test only
};

} // namespace QTerm

#endif	//QTERMSCREEN_H

