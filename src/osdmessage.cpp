/***************************************************************************
 *   Copyright (C) 2004 by Enrico Ros <eros.kde@email.it>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// qt/kde includes
// #include <qbitmap.h>
// #include <qpainter.h>
// #include <qimage.h>
// #include <qtimer.h>
// #include <qapplication.h>
// #include <qcursor.h>
//Added by qt3to4:
// #include <QPixmap>
// #include <QMouseEvent>
#include "osdmessage.h"
#include "qtermglobal.h"

#include <QPaintEvent>
#include <QCursor>
#include <QTimer>
#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include <QPalette>

// local includes

namespace QTerm
{

PageViewMessage::PageViewMessage(QWidget * parent)
        : QWidget(parent), m_timer(0), m_message()
{
    setFocusPolicy(Qt::NoFocus);
    //setBackgroundMode( Qt::NoBackground );
    QPalette palette;
    palette.setColor(backgroundRole(), qApp->palette().color(QPalette::Active, QPalette::Background));
    setPalette(palette);
//     setPaletteBackgroundColor(qApp->palette().color(QPalette::Active, QPalette::Background));
    setCursor(QCursor(Qt::ArrowCursor));
    move(10, 10);
    resize(0, 0);
    hide();
}

//slot warp for display, ugly.
void PageViewMessage::showText(const QString & message)
{
    display(message);
}

void PageViewMessage::display(const QString & message, Icon icon, int durationMs, Type type)
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
    if (!isHidden() && message.trimmed() == m_message) {
        m_timer->setSingleShot(true);
        m_timer->start(durationMs);
        return;
    }
    m_message = message.trimmed();
    m_type = type;
    QRect textRect = fontMetrics().boundingRect(m_message);
    textRect.translate(-textRect.left(), -textRect.top());
    textRect.adjust(0, 0, 2, 2);
    int width = textRect.width(),
                height = fontMetrics().ascent()+fontMetrics().descent(),
                         textXOffset = 0,
                                       shadowOffset = m_message.isRightToLeft() ? -1 : 1;

    // load icon (if set) and update geometry
    // [QTerm], we don't have a icon at this time.

    QPixmap symbol;
    if (icon != None) {
        switch (icon) {
            //case Find:
            //symbol = SmallIcon( "viewmag" );
            //break;
        case Error:
            symbol = QPixmap(":/pic/messagebox_critical.png");
            break;
        case Warning:
            symbol = QPixmap(":/pic/messagebox_warning.png");
            break;
        default:
            symbol = QPixmap(":/pic/messagebox_info.png");
            break;
        }
        textXOffset = 2 + symbol.width();
        width += textXOffset;
        height = qMax(height, symbol.height());
    }

    QRect geometry(0, 0, width + 10, height + 8);

    // resize pixmap, mask and widget
    static QPixmap mask;
    mask = QPixmap(geometry.size());
    m_pixmap = QPixmap(geometry.size());
    resize(geometry.size());

    // create and set transparency mask
    QPainter maskPainter(&mask);
    mask.fill(Qt::black);
    maskPainter.setBrush(Qt::white);
    maskPainter.drawRoundRect(geometry, 1600 / geometry.width(), 1600 / geometry.height());
    setMask(mask.createHeuristicMask());

    // draw background
    QPainter bufferPainter(&m_pixmap);
    bufferPainter.setPen(Qt::black);
    bufferPainter.setFont(font());
    bufferPainter.setBrush(palette().brush(QPalette::Window));
    bufferPainter.drawRoundRect(geometry, 1600 / geometry.width(), 1600 / geometry.height());

    // draw icon if present
    if (!symbol.isNull())
        bufferPainter.drawPixmap(5, (geometry.height() - symbol.height())/2, symbol, 0, 0, symbol.width(), symbol.height());

    // draw shadow and text
    int yText = (geometry.height() - height)/2 + fontMetrics().ascent();
    bufferPainter.setPen(palette().color(QPalette::Window).dark(115));
    bufferPainter.drawText(5 + textXOffset + shadowOffset, yText + 1, m_message);
    bufferPainter.setPen(palette().color(QPalette::WindowText));
    bufferPainter.drawText(5 + textXOffset, yText, m_message);

    // show widget and schedule a repaint
    show();
    update();

    // close the message window after given mS
    if (durationMs > 0) {
        if (!m_timer) {
            m_timer = new QTimer(this);
            m_timer->setSingleShot(true);
            connect(m_timer, SIGNAL(timeout()), SLOT(hide()));
        }
        m_timer->start(durationMs);
    } else if (m_timer)
        m_timer->stop();
}

PageViewMessage::Type PageViewMessage::type()
{
    return m_type;
}

void PageViewMessage::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    p.drawPixmap(e->rect().topLeft(), m_pixmap, e->rect());
}

void PageViewMessage::mousePressEvent(QMouseEvent * /*e*/)
{
    if (m_timer)
        m_timer->stop();
    hide();
}

} // namespace QTerm

//#include "osdmessage.moc"
#include <moc_osdmessage.cpp>
