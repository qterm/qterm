/***************************************************************************
 *   Copyright (C) 2004 by Enrico Ros <eros.kde@email.it>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// qt/kde includes
#include <qbitmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qcursor.h>

// local includes
#include "osdmessage.h"

extern QString pathPic;

PageViewMessage::PageViewMessage( QWidget * parent )
    : QWidget( parent, "pageViewMessage" ), m_timer( 0 )
{
    setFocusPolicy( NoFocus );
    setBackgroundMode( NoBackground );
    setPaletteBackgroundColor(qApp->palette().color(QPalette::Active, QColorGroup::Background));
    setCursor(QCursor(ArrowCursor));
    move( 10, 10 );
    resize( 0, 0 );
    hide();
}

void PageViewMessage::display( const QString & message, Icon icon, int durationMs )
// give to Caesar what Caesar owns: code taken from Amarok's osd.h/.cpp
// "redde (reddite, pl.) cesari quae sunt cesaris", just btw. ;)
{
//FIXME: add a option in qterm too.
/*
    if ( !KpdfSettings::showOSD() )
    {
        hide();
        return;
    }
*/
    // determine text rectangle
    QRect textRect = fontMetrics().boundingRect( message );
    textRect.moveBy( -textRect.left(), -textRect.top() );
    textRect.addCoords( 0, 0, 2, 2 );
    int width = textRect.width(),
        height = textRect.height(),
        textXOffset = 0,
        shadowOffset = message.isRightToLeft() ? -1 : 1;

    // load icon (if set) and update geometry
    // [QTerm], we don't have a icon at this time.

    QPixmap symbol;
    if ( icon != None )
    {
        switch ( icon )
        {
            //case Find:
                //symbol = SmallIcon( "viewmag" );
                //break;
            case Error:
                symbol = QPixmap( pathPic + "pic/messagebox_critical.png" );
                break;
            case Warning:
                symbol = QPixmap( pathPic + "pic/messagebox_warning.png" );
                break;
            default:
                symbol = QPixmap( pathPic + "pic/messagebox_info.png" );
                break;
        }
        textXOffset = 2 + symbol.width();
        width += textXOffset;
        height = QMAX( height, symbol.height() );
    }

    QRect geometry( 0, 0, width + 10, height + 8 );

    // resize pixmap, mask and widget
    static QPixmap mask;
    mask.resize( geometry.size() );
    m_pixmap.resize( geometry.size() );
    resize( geometry.size() );

    // create and set transparency mask
    QPainter maskPainter( &mask);
    mask.fill( Qt::black );
    maskPainter.setBrush( Qt::white );
    maskPainter.drawRoundRect( geometry, 1600 / geometry.width(), 1600 / geometry.height() );
    setMask( mask.createHeuristicMask() );

    // draw background
    QPainter bufferPainter( &m_pixmap );
    bufferPainter.setPen( Qt::black );
    bufferPainter.setBrush( paletteBackgroundColor() );
    bufferPainter.drawRoundRect( geometry, 1600 / geometry.width(), 1600 / geometry.height() );

    // draw icon if present
    if ( !symbol.isNull() )
        bufferPainter.drawPixmap( 5, 4, symbol, 0, 0, symbol.width(), symbol.height() );

    // draw shadow and text
    int yText = geometry.height() - height / 2;
    bufferPainter.setPen( paletteBackgroundColor().dark( 115 ) );
    bufferPainter.drawText( 5 + textXOffset + shadowOffset, yText + 1, message );
    bufferPainter.setPen( foregroundColor() );
    bufferPainter.drawText( 5 + textXOffset, yText, message );

    // show widget and schedule a repaint
    show();
    update();

    // close the message window after given mS
    if ( durationMs > 0 )
    {
        if ( !m_timer )
        {
            m_timer = new QTimer( this );
            connect( m_timer, SIGNAL( timeout() ), SLOT( hide() ) );
        }
        m_timer->start( durationMs, true );
    } else if ( m_timer )
        m_timer->stop();
}

void PageViewMessage::paintEvent( QPaintEvent * e )
{
    QPainter p( this );
    p.drawPixmap( e->rect().topLeft(), m_pixmap, e->rect() );
}

void PageViewMessage::mousePressEvent( QMouseEvent * /*e*/ )
{
    if ( m_timer )
        m_timer->stop();
    hide();
}
