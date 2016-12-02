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
#include "qtermparam.h"
#include "qtermtelnet.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "schemedialog.h"
#include "osdmessage.h"
#include "blur.h"

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

Screen::Screen(QWidget *parent, Buffer *buffer, Param *param, BBS *bbs)
        : QWidget(parent), Scrollbar_Width(15)
{
    m_pWindow = (Window *)parent;
    m_pBBS = bbs;
    m_pParam = param;
    //m_pCanvas = NULL;
    m_ePaintState = Repaint;
    m_bCursor = true;
    m_inputContent = NULL;
    m_pASCIIFont = NULL;
    m_pGeneralFont = NULL;
    m_pMessage = new PageViewMessage(this);

    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setMouseTracking(true);

    setScheme();

    initFontMetrics();

    m_pBuffer = buffer;
    connect(m_pBuffer, SIGNAL(bufferSizeChanged()),
            this, SLOT(bufferSizeChanged()));

    m_blinkTimer = new QTimer(this);
    connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkEvent()));

    m_cursorTimer = new QTimer(this);
    connect(m_cursorTimer, SIGNAL(timeout()), this, SLOT(cursorEvent()));

    // the scrollbar
    m_scrollBar = new QScrollBar(this);
    m_scrollBar->setCursor(Qt::ArrowCursor);
    m_nStart = 0;
    m_nEnd = m_pBuffer->line() - 1;
    m_pBlinkLine = new bool[m_nEnd - m_nStart + 1];
    m_scrollBar->setRange(0, 0);
    m_scrollBar->setSingleStep(1);
    m_scrollBar->setPageStep(m_pBuffer->line());
    m_scrollBar->setValue(0);
    connect(m_scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(scrollChanged(int)));

    m_scPrevPage = new QShortcut(Qt::Key_PageUp + Qt::SHIFT, this, SLOT(prevPage()));
    m_scNextPage = new QShortcut(Qt::Key_PageDown + Qt::SHIFT, this, SLOT(nextPage()));
    m_scPrevLine = new QShortcut(Qt::Key_Up + Qt::SHIFT, this, SLOT(prevLine()));
    m_scNextLine = new QShortcut(Qt::Key_Down + Qt::SHIFT, this, SLOT(nextLine()));

    setAttribute(Qt::WA_OpaquePaintEvent, true);

// init variable
    m_blinkScreen = false;
    m_blinkCursor = m_pParam->m_mapParam["blinkcursor"].toBool();

}

PageViewMessage * Screen::osd()
{
    return m_pMessage;
}

Screen::~Screen()
{
    delete [] m_pBlinkLine;
    delete m_blinkTimer;
    delete m_cursorTimer;
    //delete m_pCanvas;
    delete m_inputContent;
}

// focus event received
void Screen::focusInEvent(QFocusEvent *)
{
    // FIXME: Somehow there is an inf loop without this.
    if (Global::instance()->isFullScreen()) {
        return;
    }
    if (m_pWindow->isMaximized()) {
        m_pWindow->showNormal();
        m_pWindow->showMaximized();
    }

    m_scPrevPage->setEnabled(true);
    m_scNextPage->setEnabled(true);
    m_scPrevLine->setEnabled(true);
    m_scNextLine->setEnabled(true);

}

// focus out
void Screen::focusOutEvent(QFocusEvent *)
{
    m_scPrevPage->setEnabled(false);
    m_scNextPage->setEnabled(false);
    m_scPrevLine->setEnabled(false);
    m_scNextLine->setEnabled(false);
}

void Screen::showEvent(QShowEvent *)
{
    m_ePaintState = Show;
    update();
}

void Screen::cursorEvent()
{
    if (m_blinkCursor) {
        m_ePaintState = Cursor;
        m_bCursor = !m_bCursor;
        QRect cursorRect;
        QPoint pt = mapToPixel(QPoint(m_pBuffer->caretX(), m_pBuffer->caretY()));
        switch (m_pParam->m_mapParam["cursor"].toInt()) {
        case 0:
            cursorRect.setRect(pt.x(), pt.y(), m_nCharWidth, m_nCharHeight);
            break;
        case 1:
            cursorRect.setRect(pt.x(), pt.y() + 9*m_nCharHeight / 10, m_nCharWidth, m_nCharHeight / 10);
            break;
        case 2:
            cursorRect.setRect(pt.x(), pt.y(), m_nCharWidth / 9, m_nCharHeight);
            break;
        default:
            qDebug("Unknown cursor type");
            break;
        }
        update(cursorRect);
    }
}

void Screen::updateCursor()
{

    QPainter painter(this);

    if (m_pBuffer->caretY() <= m_nEnd && m_pBuffer->caretY() >= m_nStart) {
        QPoint pt = mapToPixel(QPoint(m_pBuffer->caretX(), m_pBuffer->caretY()));
        int linelength = m_pBuffer->at(m_pBuffer->caretY())->getLength();
        int startx = m_pBuffer->caretX();

		switch (m_pParam->m_mapParam["cursor"].toInt()) {
        case 0: // block
            if (startx < linelength) {
                drawLine(painter, m_pBuffer->caretY(), startx, startx, false);
            } else {
                if (m_bCursor)
                    painter.fillRect(pt.x(), pt.y(), m_nCharWidth, m_nCharHeight, m_color[7]);
                else
                    painter.fillRect(pt.x(), pt.y(), m_nCharWidth, m_nCharHeight, m_color[0]);
            }
            break;
        case 1: // underline
            if (m_bCursor)
                painter.fillRect(pt.x(), pt.y() + 9*m_nCharHeight / 10, m_nCharWidth, m_nCharHeight / 10, m_color[7]);
            else
                painter.fillRect(pt.x(), pt.y() + 9*m_nCharHeight / 10, m_nCharWidth, m_nCharHeight / 10, m_color[0]);
            break;
        case 2: // I type
            if (m_bCursor)
                painter.fillRect(pt.x(), pt.y(), m_nCharWidth / 9, m_nCharHeight, m_color[7]);
            else
                painter.fillRect(pt.x(), pt.y(), m_nCharWidth / 9, m_nCharHeight, m_color[0]);
            break;
        default:
            break;
            //drawLine(painter, m_pBuffer->caretY(), startx, startx, false);
        }
    }
}

void Screen::blinkEvent()
{
    if (m_hasBlink) {
        m_blinkScreen = !m_blinkScreen;
        m_ePaintState = Blink;
        update(m_blinkRegion);
    }
}

void Screen::resizeEvent(QResizeEvent *)
{
    updateScrollBar();
    setBgPxm(m_pxmBg, m_nPxmType);

//  if (m_pCanvas != NULL) {
//   delete m_pCanvas;
//   m_pCanvas = NULL;
//  }
//  m_pCanvas = new QPixmap(width(), height());

	if (m_pParam->m_mapParam["autofont"].toBool()) {
        updateFont();
    } else {
        int cx = m_rcClient.width() / m_nCharWidth;
        int cy = m_rcClient.height() / m_nCharHeight;
        if (cx > 10 && cy > 10) {
            m_pBuffer->setSize(cx, cy);
        }
    }
    m_ePaintState = Show;
    update();
    QTimer::singleShot(50,this,SLOT(blurBackground()));
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                                  Font                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void Screen::initFontMetrics()
{
    if (m_pASCIIFont != NULL) {
        delete m_pASCIIFont;
    }
    if (m_pGeneralFont != NULL) {
        delete m_pGeneralFont;
    }
	m_pASCIIFont = new QFont(m_pParam->m_mapParam["asciifont"].toString(), 
						qMax(8, m_pParam->m_mapParam["fontsize"].toInt()));
    m_pGeneralFont = new QFont(m_pParam->m_mapParam["generalfont"].toString(), 
						qMax(8, m_pParam->m_mapParam["fontsize"].toInt()));

    m_pASCIIFont->setWeight(QFont::Normal);
    m_pGeneralFont->setWeight(QFont::Normal);

    m_pASCIIFont->setStyleStrategy(Global::instance()->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
    m_pGeneralFont->setStyleStrategy(Global::instance()->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
    getFontMetrics();
    m_pMessage->setFont(*m_pGeneralFont);
}

void Screen::updateFont()
{
    int nPointSize;
    int nIniSize = qMax(8, qMin(m_rcClient.height() / m_pBuffer->line(),
                                m_rcClient.width() * 2 / m_pBuffer->columns()));

    for (nPointSize = nIniSize - 3; nPointSize <= nIniSize + 3; nPointSize++) {
        m_pASCIIFont->setPointSize(nPointSize);
        m_pGeneralFont->setPointSize(nPointSize);

        getFontMetrics();
        if ((m_pBuffer->line()*m_nCharHeight) > m_rcClient.height()
                || (m_pBuffer->columns()*m_nCharWidth) > m_rcClient.width()) {
            while (nPointSize > 5) {
                nPointSize--;
                m_pASCIIFont->setPointSize(nPointSize);
                m_pGeneralFont->setPointSize(nPointSize);

                getFontMetrics();
                if ((m_pBuffer->line()*m_nCharHeight) < m_rcClient.height()
                        && (m_pBuffer->columns()*m_nCharWidth) < m_rcClient.width())
                    break;
            }
            break;
        }
    }
    int marginw,marginh;
    marginw=marginh=0;
    if (Frame::instance()->isMaximized()||Frame::instance()->isFullScreen()) {
        marginw = (m_rcClient.width() - (m_pBuffer->columns()*m_nCharWidth))/2;
    }
    QPoint point = m_rcClient.topLeft();
    m_rcClient = QRect(point.x()+marginw, point.y()+marginh, m_pBuffer->columns()*m_nCharWidth, m_pBuffer->line()*m_nCharHeight);
    m_pASCIIFont->setWeight(QFont::Normal);
    m_pGeneralFont->setWeight(QFont::Normal);
    m_pASCIIFont->setStyleStrategy(Global::instance()->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
    m_pGeneralFont->setStyleStrategy(Global::instance()->m_pref.bAA ? QFont::PreferAntialias : QFont::NoAntialias);
    if (m_inputContent != NULL) {
        delete m_inputContent;
        m_inputContent = NULL;
    }
    m_pMessage->setFont(*m_pGeneralFont);
}

void Screen::getFontMetrics()
{
    QFontMetrics ascii_fm(*m_pASCIIFont);
    QFontMetrics general_fm(*m_pGeneralFont);
    float cn = general_fm.width(QChar(0x4e00));
    float en = ascii_fm.width('W');
    if (en / cn < 0.7) // almost half
        m_nCharWidth = qMax((cn + 1) / 2, en);
    else
        m_nCharWidth = (qMax(en, cn) + 1) / 2;

    m_nCharDelta = m_nCharWidth - cn/2;
    m_nCharHeight = qMax(ascii_fm.height(),general_fm.height());
    m_nCharAscent = ascii_fm.ascent();
    m_nCharDescent = ascii_fm.descent();
}

void Screen::asciiFontChanged(const QFont & font)
{
    qDebug() << "ASCII font changed: " << font.family();
    if (m_pASCIIFont != NULL) {
        delete m_pASCIIFont;
    }
    m_pASCIIFont = new QFont(font);
    m_pASCIIFont->setPointSize(qMax(8,m_pParam->m_mapParam["fontsize"].toInt()));
    m_pGeneralFont->setPointSize(qMax(8,m_pParam->m_mapParam["fontsize"].toInt()));
    QResizeEvent* re = new QResizeEvent(size(), size());
    resizeEvent(re);
}

void Screen::generalFontChanged(const QFont & font)
{
    if (m_pGeneralFont != NULL) {
        delete m_pGeneralFont;
    }
    m_pGeneralFont = new QFont(font);
    m_pASCIIFont->setPointSize(qMax(8,m_pParam->m_mapParam["fontsize"].toInt()));
    m_pGeneralFont->setPointSize(qMax(8,m_pParam->m_mapParam["fontsize"].toInt()));
    m_pMessage->setFont(*m_pGeneralFont);
    QResizeEvent* re = new QResizeEvent(size(), size());
    resizeEvent(re);
}

void Screen::fontSizeChanged(int value)
{
    m_pParam->m_mapParam["fontsize"] = value;
    m_pASCIIFont->setPointSize(qMax(8,value));
    m_pGeneralFont->setPointSize(qMax(8,value));
    QResizeEvent* re = new QResizeEvent(size(), size());
    resizeEvent(re);
}

QFont Screen::asciiFont()
{
    return *m_pASCIIFont;
}

QFont Screen::generalFont()
{
    return *m_pGeneralFont;
}

int Screen::fontSize()
{
    return m_pParam->m_mapParam["fontsize"].toInt();
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                                 Colors                                   */
/*                                                                         */
/* ------------------------------------------------------------------------ */
void Screen::setScheme()
{
// the default color table
    m_color[0]  = Qt::black;
    m_color[1]  = Qt::darkRed;
    m_color[2]  = Qt::darkGreen;
    m_color[3]  = Qt::darkYellow;
    m_color[4]  = Qt::darkBlue;
    m_color[5]  = Qt::darkMagenta;
    m_color[6]  = Qt::darkCyan;
    m_color[7]  = Qt::lightGray;
    m_color[8]  = Qt::gray;
    m_color[9]  = Qt::red;
    m_color[10] = Qt::green;
    m_color[11] = Qt::yellow;
    m_color[12] = Qt::blue;
    m_color[13] = Qt::magenta;
    m_color[14] = Qt::cyan;
    m_color[15] = Qt::white;

    m_nPxmType = 0;


	QString strSchemeFile = m_pParam->m_mapParam["schemefile"].toString();
// if we have scheme defined
    if (QFile::exists(strSchemeFile)) {

//  printf("scheme %s loaded sucessfully\n", strSchemeFile);
        Config *pConf = new Config(strSchemeFile);

        m_color[0].setNamedColor(pConf->getItemValue("color", "color0").toString());
        m_color[1].setNamedColor(pConf->getItemValue("color", "color1").toString());
        m_color[2].setNamedColor(pConf->getItemValue("color", "color2").toString());
        m_color[3].setNamedColor(pConf->getItemValue("color", "color3").toString());
        m_color[4].setNamedColor(pConf->getItemValue("color", "color4").toString());
        m_color[5].setNamedColor(pConf->getItemValue("color", "color5").toString());
        m_color[6].setNamedColor(pConf->getItemValue("color", "color6").toString());
        m_color[7].setNamedColor(pConf->getItemValue("color", "color7").toString());
        m_color[8].setNamedColor(pConf->getItemValue("color", "color8").toString());
        m_color[9].setNamedColor(pConf->getItemValue("color", "color9").toString());
        m_color[10].setNamedColor(pConf->getItemValue("color", "color10").toString());
        m_color[11].setNamedColor(pConf->getItemValue("color", "color11").toString());
        m_color[12].setNamedColor(pConf->getItemValue("color", "color12").toString());
        m_color[13].setNamedColor(pConf->getItemValue("color", "color13").toString());
        m_color[14].setNamedColor(pConf->getItemValue("color", "color14").toString());
        m_color[15].setNamedColor(pConf->getItemValue("color", "color15").toString());

        delete pConf;
    }
    m_color[0].setAlphaF(m_pParam->m_mapParam["opacity"].toInt()/100.0);
}

void Screen::schemeChanged(int index)
{
    if (index == -1) {
        return;
    }
    QStringList schemeList = schemeDialog::loadSchemeList();
    m_pParam->m_mapParam["schemefile"] = schemeList[index];
    setScheme();
    QResizeEvent* re = new QResizeEvent(size(), size());
    resizeEvent(re);
}

void Screen::opacityChanged(int val)
{
    m_color[0].setAlphaF(val/100.0);
    QResizeEvent* re = new QResizeEvent(size(), size());
    resizeEvent(re);
}


/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                   Scrollbar                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void Screen::prevPage()
{
    scrollLine(-m_pBuffer->line());
}

void Screen::nextPage()
{
    scrollLine(m_pBuffer->line());

}

void Screen::prevLine()
{
    scrollLine(-1);

}

void Screen::nextLine()
{
    scrollLine(1);

}

void Screen::scrollLine(int delta)
{
    m_nStart += delta;

    if (m_nStart < 0) {
        m_nStart = 0;
        return;
    }
    if (m_nStart > m_pBuffer->lines() - m_pBuffer->line()) {
        m_nStart = m_pBuffer->lines() - m_pBuffer->line();
        return;
    }

    m_scrollBar->setValue(m_nStart);
    m_nEnd = m_nStart + m_pBuffer->line() - 1;

    // notify bbs
    m_pBBS->setScreenStart(m_nStart);

    for (int i = m_nStart; i <= m_nEnd; i++)
        m_pBuffer->at(i)->setChanged(-1, -1);

    updateRegion();
}
void Screen::scrollChanged(int value)
{
    if (m_nStart == value) return;

    if (value < 0)
        value = 0;
    if (value > m_pBuffer->lines() - m_pBuffer->line())
        value = m_pBuffer->lines() - m_pBuffer->line();

    m_nStart = value;
    m_nEnd = value + m_pBuffer->line() - 1;

    // notify bbs
    m_pBBS->setScreenStart(m_nStart);

    for (int i = m_nStart; i <= m_nEnd; i++)
        m_pBuffer->at(i)->setChanged(-1, -1);

    updateRegion();
}

void Screen::updateScrollBar()
{
    switch (Global::instance()->scrollPosition()) {
    case Global::Hide:
        m_scrollBar->hide();
        m_rcClient = QRect(1, 1, rect().width() - 1, rect().height() - 1);
        break;
    case Global::Left:
        m_scrollBar->setGeometry(0, 0, Scrollbar_Width, rect().height());
        m_scrollBar->show();
        m_rcClient = QRect(Scrollbar_Width + 3, 1,
                           rect().width() - Scrollbar_Width - 3, rect().height() - 1);
        break;
    case Global::Right:
        m_scrollBar->setGeometry(rect().width() - Scrollbar_Width, 0,
                                 Scrollbar_Width, rect().height());
        m_scrollBar->show();
        m_rcClient = QRect(3, 1,
                           rect().width() - Scrollbar_Width - 3, rect().height() - 1);
        break;
    }
//  qDebug("second: %d, %d", m_rcClient.width(),m_rcClient.height());

//  if (m_pCanvas != NULL) {
//   delete m_pCanvas;
//   m_pCanvas = NULL;
//  }
//  m_pCanvas = new QPixmap(m_rcClient.width(), m_rcClient.height());
    //repaint(true);
    update();
}

void Screen::bufferSizeChanged()
{
    disconnect(m_scrollBar, SIGNAL(valueChanged(int)),
               this, SLOT(scrollChanged(int)));

    m_scrollBar->setRange(0, m_pBuffer->lines() - m_pBuffer->line());
    m_scrollBar->setSingleStep(1);
    m_scrollBar->setPageStep(m_pBuffer->line());
    m_scrollBar->setValue(m_pBuffer->lines() - m_pBuffer->line());

    m_nStart = m_scrollBar->value();
    m_nEnd =  m_scrollBar->value() + m_pBuffer->line() - 1;

    // notify bbs
    m_pBBS->setScreenStart(m_nStart);

    connect(m_scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(scrollChanged(int)));

    delete []m_pBlinkLine;
    m_pBlinkLine = new bool[m_nEnd - m_nStart + 1];

    scrollLine(0);
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                            Display                                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

//set pixmap background

void Screen::setBgPxm(const QPixmap& pixmap, int nType)
{
    m_hasBg = false;
    m_pxmBg = pixmap;
    m_nPxmType = nType;
    QPalette palette;

    if (Global::instance()->isBossColor()) {
        palette.setColor(backgroundRole(), Qt::white);
        setPalette(palette);
//   setBackgroundColor( Qt::white );
        return;
    }

    switch (nType) {
    case 0: // none
        palette.setColor(backgroundRole(), m_color[0]);
        setPalette(palette);
        break;
    case 1: { // transparent
    }
    break;
    case 2: // tile
        if (!pixmap.isNull()) {
            palette.setBrush(backgroundRole(), QBrush(pixmap));
            setPalette(palette);
//     setBackgroundPixmap( pixmap );
            m_hasBg = true;
            break;
        }
    case 3: // center
        if (!pixmap.isNull()) {
            QPixmap pxmBg = QPixmap(size());
            QPainter painter(&pxmBg);
            pxmBg.fill(m_color[0]);
            painter.drawPixmap((size().width() - pixmap.width()) / 2,
                               (size().height() - pixmap.height()) / 2,  pixmap.width(),
                               pixmap.height(), pixmap);
            palette.setBrush(backgroundRole(), QBrush(pxmBg));
            setPalette(palette);
//     setBackgroundPixmap(pxmBg);
            m_hasBg = true;
            break;
        }
    case 4: // stretch
        if (!pixmap.isNull()) {
            float sx = (float)size().width() / pixmap.width();
            float sy = (float)size().height() / pixmap.height();
            QMatrix matrix;
            matrix.scale(sx, sy);
            palette.setBrush(backgroundRole(), QBrush(pixmap.transformed(matrix)));
            setPalette(palette);
//     setBackgroundPixmap(pixmap.transformed( matrix ));
            m_hasBg = true;
            break;
        }
    default:
        palette.setColor(backgroundRole(), m_color[0]);
        setPalette(palette);
//    setBackgroundColor( m_color[0] );

    }
}

void Screen::blinkScreen()
{
    QPainter painter;
    painter.begin(this);
    painter.setBackground(QBrush(m_color[0]));
    int startx;
    for (int index = m_nStart; index <= m_nEnd; index++) {
        if (m_pBlinkLine[index - m_nStart]) {
            TextLine *pTextLine = m_pBuffer->at(index);
            uint linelength = pTextLine->getLength();
            QByteArray attr = pTextLine->getAttr();
            for (uint i = 0; i < linelength; ++i) {
                if (GETBLINK(attr.at(i))) {
                    startx = i;
                    while (i < linelength && GETBLINK(attr.at(i)))
                        ++i;
                    painter.fillRect(mapToRect(startx, index, i-startx, 1), QBrush(m_color[0]));
                    --i;
                    drawLine(painter, index, startx, i, false);
                }
            }
        }
    }
}

// refresh the screen when
// 1. received new contents form server
// 2. scrolled by user
void Screen::refreshScreen()
{
    if (m_cursorTimer->isActive()) m_cursorTimer->stop();

    if (m_blinkTimer->isActive()) m_blinkTimer->stop();

    m_hasBlink = false;
    m_bCursor = true;

    //setUpdatesEnabled(false);

    int startx, endx;

    QPainter painter;
    QRegion blinkRegion;

    painter.begin(this);
    //qDebug("size: %d, %d", width(),height());
    if (m_ePaintState == Show)
        painter.fillRect(rect(), m_color[0]);

    for (int index = m_nStart; index <= m_nEnd; index++) {
        if (index >= m_pBuffer->lines()) {
            qDebug("Screen::drawLine wrong index %d", index);
            painter.end();
            return;
        }

        TextLine *pTextLine = m_pBuffer->at(index);
        if (pTextLine->hasBlink()) {
            m_hasBlink = true;
            m_pBlinkLine[index - m_nStart] = true;
            uint linelength = pTextLine->getLength();
            QByteArray attr = pTextLine->getAttr();
            for (uint i = 0; i < linelength; ++i) {
                if (GETBLINK(attr.at(i))) {
                    startx = i;
                    while (i < linelength && GETBLINK(attr.at(i)))
                        ++i;
                    blinkRegion += mapToRect(startx, index, i-startx, 1);
                }
            }
        } else
            m_pBlinkLine[index - m_nStart] = false;

        if (m_ePaintState == Show) {
            drawLine(painter, index, 0, -1);
            continue;
        }

        if (!pTextLine->isChanged(startx, endx))
            continue;

        /*
          Finally get around this for pku & ytht, don't know why some weird things
          happened when only erase and draw the changed part.
        */
        //startx = testChar(startx, index);
        painter.fillRect(mapToRect(startx, index, -1, 1), m_color[0]);
        drawLine(painter, index, startx);
        pTextLine->clearChange();
    }
    painter.end();
    m_blinkRegion = blinkRegion;

    updateMicroFocus();
    if (m_pWindow->isConnected()) {
        m_ePaintState = Cursor;
        m_bCursor = true;
        updateCursor();
    }


    if (m_pWindow->isConnected()) m_cursorTimer->start(1000);

    if (m_hasBlink) m_blinkTimer->start(1000);
}

void Screen::blurBackground()
{
    if (m_pParam->m_mapParam["opacity"].toInt() < 100)
        BlurHelper().updateBlurRegion(Frame::instance(), Frame::instance()->rect());
}

void Screen::paintEvent(QPaintEvent * pe)
{
//  qDebug()<<"Event:"<<m_ePaintState;
    switch (m_ePaintState) {
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

bool Screen::event(QEvent * e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent * ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Tab) {
            m_pWindow->keyPressEvent(ke);
            ke->accept();
            return true;
        }
    } else if (e->type() == QEvent::ShortcutOverride) {
        QKeyEvent * ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_W && ke->modifiers() == Qt::ControlModifier) {
            ke->accept();
            return true;
        }
    }
    return QWidget::event(e);
}

void Screen::repaintScreen(QPaintEvent * pe)
{
    QPainter painter;
    painter.begin(this);
    painter.setBackground(QBrush(m_color[0]));

//  qDebug("rcClient: %d, %d", m_pCanvas->width(),m_pCanvas->height());

    painter.eraseRect(pe->rect());

    QRect rect = pe->rect() & m_rcClient;
    if (rect.isEmpty())
        return;

    //painter.eraseRect(rect);

    QPoint tlPoint = mapToChar(QPoint(rect.left(), rect.top()));

    QPoint brPoint = mapToChar(QPoint(rect.right(), rect.bottom()));

    for (int y = tlPoint.y(); y <= brPoint.y(); y++) {
        drawLine(painter, y);
        //if( m_pBBS->isSelected(y)&&m_pParam->m_mapParam["menutype"].toInt()==1 )
        //{
        // QRect rcMenu = mapToRect(m_pBBS->getSelectRect());
        // QPixmap pxm(rcMenu.width(), rcMenu.height());
        // bitBlt(this, rcMenu.x(), rcMenu.y(), &pxm, 0, 0,
        //   rcMenu.width(), rcMenu.height(), Qt::NotROP );
        //}
    }

    painter.end();
}

// draw a line with the specialAtter if given.
// modified by hooey to draw part of the line.
void Screen::drawLine(QPainter& painter, int index, int beginx, int endx, bool complete)
{
    if (index >= m_pBuffer->lines()) {
        qDebug("Screen::drawLine wrong index %d", index);
        return;
    }

    TextLine *pTextLine = m_pBuffer->at(index);
    QByteArray color = pTextLine->getColor();
    QByteArray attr = pTextLine->getAttr();
    int linelength = pTextLine->getLength();
    char tempcp = 0;
    char tempea = 0;
    short tempattr;
    bool bSelected;
    bool bReverse = false;
    int startx;
    //qDebug() << "beginx: " << beginx << ", endx: " << endx << ", linelength: " << linelength;
    QString strShow;

	if (complete == true && m_pBBS->isSelected(index)) {
		drawMenuSelect(painter, index);
		if (m_pParam->m_mapParam["menutype"].toInt() == 1) {
			bReverse = true;
		}
		beginx = 0;
		endx = linelength - 1;
	}

    if (beginx < 0)
        beginx = 0;

    if (endx >= linelength || endx < 0) {
        endx = qMin(m_pBuffer->columns(), linelength)-1;
        //painter.eraseRect(mapToRect(beginx, index, linelength, 1)); // Maybe we should calculate more accurate size;
    }
    if (endx >= qMin(color.size(), attr.size())) {
        endx = qMin(color.size(), attr.size()) -1;
    }

    if (beginx >= linelength) {
        //qDebug("Screen::drawLine: wrong position: %d(%d)", beginx, linelength);
        return;
    }

    if (beginx > 0 && pTextLine->isPartial(beginx)) {
        beginx -= 1;
    }

    if (m_ePaintState == Cursor && m_bCursor) {
        bReverse = true;
    }
    for (int i = beginx; i < endx+1;i++) {
        int len = 0;
        startx = i;
        if (i < color.size())
            tempcp = color.at(i);
        if (i < attr.size())
            tempea = attr.at(i);
        bSelected = m_pBuffer->isSelected(QPoint(i, index), m_pWindow->m_bRectCopy);
        len = pTextLine->size(i);
        if ( (i+1) >= linelength) {
            len = 1;
        }
        if (len <= 0) {
            qDebug("drawLine: non printable char");
            continue;
        }

        if (bSelected) // selected area is text=color(0) background=color(7)
            tempattr = SETCOLOR(SETFG(0) | SETBG(7)) | SETATTR(NO_ATTR);
        else if (bReverse)
            tempattr = SETCOLOR(tempcp) | SETATTR(SETREVERSE(tempea));
        else
            tempattr = SETCOLOR(tempcp) | SETATTR(tempea);

        //qDebug() << "startx: " << startx << " i: " << i << " string: " << strShow;
        // There should be only one.
        // TODO: Rewrite this when we want to do more than char to char convert
        strShow = Global::instance()->convert(pTextLine->getText(startx, len), (Global::Conversion)m_pParam->m_mapParam["displaycode"].toInt());

        if (strShow.isEmpty()) {
            qDebug("drawLine: empty string?");
            continue;
        }
        // Draw Charactors one by one to fix the variable font display problem
        int charWidth = TermString::wcwidth(strShow.at(0));
        if (charWidth <= 0) {
            qDebug("drawLine: non printable char");
            continue;
        }
        CharFlags flags = RenderAll;
        if ( pTextLine->isPartial(startx) ) {
            flags = RenderRight;
            charWidth = 1;
        } else if ( charWidth == 2) {
            if (tempcp != color.at(i+1) || tempea != attr.at(i+1) || bSelected != m_pBuffer->isSelected(QPoint(i+1, index), m_pWindow->m_bRectCopy)) {
                charWidth = 1;
                flags = RenderLeft;
            } else {
                ++i;
            }
        }
        drawStr(painter, (QString)strShow.at(0), startx, index, charWidth, tempattr, bSelected, flags);

        //i += (len-1);
    }
}

// draw functions
void Screen::drawStr(QPainter& painter, const QString& str, int x, int y, int length,
                     short attribute, bool transparent, CharFlags flags)
{
    char cp = GETCOLOR(attribute);
    char ea = GETATTR(attribute);

    // test bold mask or always highlighted
    if (GETBOLD(ea) || m_pParam->m_mapParam["alwayshighlight"].toBool())
        cp = SETHIGHLIGHT(cp);             // use 8-15 color
    // test dim mask
    if (GETDIM(ea)) {
    };
    // test underline mask
    QFont asciiFont = *m_pASCIIFont;
    QFont generalFont = *m_pGeneralFont;
    if (GETUNDERLINE(ea)) {
        asciiFont.setUnderline(true);
        generalFont.setUnderline(true);
    }
    if (length == 1) {
       painter.setFont(asciiFont);
    } else if (length == 2) {
       painter.setFont(generalFont);
    }
    // test blink mask
    if (GETBLINK(ea)) {
    }
    // test rapidblink mask
    if (GETRAPIDBLINK(ea)) {
    };
    // test reverse mask
    if (GETREVERSE(ea))
        cp = REVERSECOLOR(cp);

    // test invisible mask
    if (GETINVISIBLE(ea)) {
    };

    QPoint pt = mapToPixel(QPoint(x, y));

    // black on white without attr
    if (Global::instance()->isBossColor()) {
        painter.setPen(GETFG(cp) == 0 ? Qt::white : Qt::black);
        if (GETBG(cp) != 0 && !transparent)
            painter.setBackgroundMode(Qt::OpaqueMode);
        else
            painter.setBackgroundMode(Qt::TransparentMode);
        painter.setBackground(GETBG(cp) == 7 ? Qt::black : Qt::white);
        painter.drawText(pt.x(), pt.y(), m_nCharWidth*length, m_nCharHeight, flags, str);
        return;
    }

    painter.setPen(m_color[m_pParam->m_mapParam["ansicolor"].toBool()||GETFG(cp)==0?GETFG(cp):7]);

    if (GETBG(cp) != 0 && !transparent) {
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(m_color[m_pParam->m_mapParam["ansicolor"].toBool()||GETBG(cp)==7?GETBG(cp):0]);

    } else
        painter.setBackgroundMode(Qt::TransparentMode);

    if (m_blinkScreen && GETBLINK(GETATTR(attribute))) {
        if (GETBG(cp) != 0)
            painter.fillRect(mapToRect(x, y, length, 1), QBrush(m_color[GETBG(cp)]));
    } else {
        if (GETBG(cp) != 0 || m_ePaintState == Cursor)
            painter.fillRect(mapToRect(x, y, length, 1), QBrush(m_color[GETBG(cp)]));
        if (flags == RenderAll) {
            //painter.fillRect(mapToRect(x, y, length, 1), QBrush(m_color[0]));
            painter.drawText(pt.x(), pt.y(), m_nCharWidth*length, m_nCharHeight, Qt::AlignCenter, str);
        } else {
            QPixmap pm = QPixmap(m_nCharWidth*2, m_nCharHeight);
            QPainter p(&pm);
            p.fillRect(0, 0, pm.width(), pm.height(), m_color[GETBG(cp)]);
            p.setPen(m_color[GETFG(cp)]);
            p.setFont(painter.font());
            p.drawText(0, 0, m_nCharWidth*2, m_nCharHeight, Qt::AlignCenter, str);
            p.end();
            if (flags == RenderLeft) {
                painter.drawPixmap(pt.x(), pt.y(), pm, 0, 0, m_nCharWidth, m_nCharHeight);
            }
            if (flags == RenderRight) {
                painter.drawPixmap(pt.x(), pt.y(), pm, m_nCharWidth, 0, m_nCharWidth, m_nCharHeight);
            }
        }
    }
    painter.setBackground(QBrush(m_color[0]));
}


void Screen::eraseRect(QPainter&, int, int, int, int, short)
{

}

void Screen::bossColor()
{
    setBgPxm(m_pxmBg, m_nPxmType);

    //repaint(true);
    update();
}

void Screen::drawMenuSelect(QPainter& painter, int index)
{
    QRect rcSelect, rcMenu, rcInter;
    if (m_pBuffer->isSelected(index)) {
        rcSelect = mapToRect(m_pBuffer->getSelectRect(index, m_pWindow->m_bRectCopy));
        if (Global::instance()->isBossColor())
            painter.fillRect(rcSelect, Qt::black);
        else
            painter.fillRect(rcSelect, QBrush(m_color[7]));
    }

    if (m_pBBS->isSelected(index)) {
        rcMenu = mapToRect(m_pBBS->getSelectRect().intersected(QRect(0,index,m_pBuffer->columns(), 1)));
        switch (m_pParam->m_mapParam["menutype"].toInt()) {
        case 0: // underline
            painter.fillRect(rcMenu.x(), rcMenu.y() + 10*m_nCharHeight / 11, rcMenu.width(), m_nCharHeight / 11, m_color[7]);
            break;
        case 2:
            painter.fillRect(rcMenu, QBrush(m_pParam->m_mapParam["menucolor"].value<QColor>()));
            break;
        }
    }
}

void Screen::updateRegion()
{
    int startx, endx;

    m_ePaintState = NewData;
    QRegion paintRegion;
    for (int index = m_nStart; index <= m_nEnd; index++) {
        TextLine *pTextLine = m_pBuffer->at(index);
        if (!pTextLine->isChanged(startx, endx))
            continue;
        if (startx > 0 && pTextLine->isPartial(startx)) {
            startx -= 1;
        }

        paintRegion += mapToRect(startx, index, -1, 1);
    }
    update(paintRegion);
}

/* ------------------------------------------------------------------------ */
/*                                                                         */
/*                              Auxilluary                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */
QPoint Screen::mapToPixel(const QPoint& point)
{
    QPoint pt = m_rcClient.topLeft();

    QPoint pxlPoint;

    pxlPoint.setX(point.x()*m_nCharWidth + pt.x());
    pxlPoint.setY((point.y() - m_nStart)*m_nCharHeight + pt.y());


    return pxlPoint;
}

QPoint Screen::mapToChar(const QPoint& point)
{
    QPoint pt = m_rcClient.topLeft();

    QPoint chPoint;

    chPoint.setX(qMin(qMax(0, (point.x() - pt.x()) / m_nCharWidth), m_pBuffer->columns() - 1));
    chPoint.setY(qMin(qMax(0, (point.y() - pt.y()) / m_nCharHeight + m_nStart), m_nEnd));

    //FIXME add bound check

    return chPoint;
}

QRect Screen::mapToRect(int x, int y, int width, int height)
{
    int px = x < 0 ? 0 : x;
    int py = y < 0 ? 0 : y;

    QPoint pt = mapToPixel(QPoint(px, py));

    if (width == -1)  // to the end
        return QRect(pt.x(), pt.y(), size().width() , m_nCharHeight*height);
    else
        return QRect(pt.x(), pt.y(), width*m_nCharWidth, m_nCharHeight*height);
}

QRect Screen::mapToRect(const QRect& rect)
{
    return mapToRect(rect.x(), rect.y(), rect.width(), rect.height());
}

// from KImageEffect::fade
QImage& Screen::fade(QImage& img, float val, const QColor& color)
{
    if (img.width() == 0 || img.height() == 0)
        return img;

    // We don't handle bitmaps
    if (img.depth() == 1)
        return img;

    unsigned char tbl[256];
    for (int i = 0; i < 256; i++)
        tbl[i] = (int)(val * i + 0.5);

    int red = color.red();
    int green = color.green();
    int blue = color.blue();

    QRgb col;
    int r, g, b, cr, cg, cb;

    if (img.depth() <= 8) {
        // pseudo color
        for (int i = 0; i < img.colorCount(); i++) {
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
        for (int y = 0; y < img.height(); y++) {
            QRgb *data = (QRgb *) img.scanLine(y);
            for (int x = 0; x < img.width(); x++) {
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
    QString commitString = e->commitString();
    if (!commitString.isEmpty()) {
        emit inputEvent(commitString);
    }
    QString preeditString = e->preeditString();
    if (m_inputContent == NULL){
        m_inputContent = new Input(this, m_nCharWidth, m_nCharHeight, m_nCharAscent);
        m_inputContent->setFont(*m_pGeneralFont);
    }
    if (preeditString.isEmpty()) {
        m_inputContent->hide();
    } else {
        m_inputContent->drawInput(e);
        QPoint cursor;
        int x = m_pBuffer->caretX();
        int y = m_pBuffer->caretY();
        cursor = mapToPixel(QPoint(x+1,y));

        if (m_inputContent->width() + cursor.x() > m_rcClient.width()){
            cursor = mapToPixel(QPoint(x,y));
            cursor.setX(m_rcClient.right() - m_inputContent->width());
        }else
            cursor = mapToPixel(QPoint(x,y));
        m_inputContent->show();
        m_inputContent->move(cursor);
    }
}

QVariant Screen::inputMethodQuery(Qt::InputMethodQuery property) const
{
//  qDebug()<<"inputMethodQuery";
    switch (property) {
    case Qt::ImMicroFocus:
        return QVariant(QRect((m_pBuffer->caret().x() + 1)*m_nCharWidth, (m_pBuffer->caret().y() + 1)*m_nCharHeight,
                              m_nCharWidth, m_nCharHeight));
    case Qt::ImFont:
        return QVariant(*m_pGeneralFont);
    case Qt::ImCursorPosition:
        return m_pBuffer->caret().x();
    default:
        return QVariant();
    }
}

void Input::drawInput(QInputMethodEvent * e)
{
    int cursorPos = -1;
    foreach (QInputMethodEvent::Attribute attribute, e->attributes()) {
        switch (attribute.type) {
            case QInputMethodEvent::TextFormat:
                //FIXME: text format?
                break;
            case QInputMethodEvent::Cursor:
                if (attribute.length == 0) {
                    cursorPos = -1;
                } else {
                    cursorPos = attribute.start;
                }
                break;
            case QInputMethodEvent::Language:
                break;
            case QInputMethodEvent::Ruby:
                break;
            default:
                qDebug("Unknown attribute");
                break;
        }
    }

    d_text = e->preeditString();
    d_pos = cursorPos;
    repaint();
}

void Input::paintEvent(QPaintEvent * e)
{
    QPainter inputPainter;
    int len = 0;
    int cursor = 0;
    int width, height;

    if (d_pos == -1 || d_pos >= d_text.length())
        d_pos = 0;
    for (int i = 0; i < d_text.length(); ++i) {
        if (i == d_pos)
            cursor = len;
        len += TermString::wcwidth(d_text[i]);
    }

    width = len * d_width;
    height = d_height;

    inputPainter.begin(this);
    setFixedSize(width,height);
    inputPainter.eraseRect(rect());

    len = 0;
    inputPainter.setPen(Qt::black);

    for (int j = 0; j < d_text.length(); ++j) {
        if (j == d_pos) {
            QRect rcCurrent(len * d_width, 0, d_width*TermString::wcwidth(d_text[d_pos]), d_height);
            inputPainter.fillRect(rcCurrent, QBrush(Qt::darkGray));
            inputPainter.setPen(Qt::white);
            inputPainter.drawText(len * d_width ,d_ascent, d_text.mid(d_pos, 1));
        } else {
            inputPainter.drawText(len * d_width, d_ascent, d_text.mid(j, 1));
        }
        len += TermString::wcwidth(d_text[j]);
    }
    inputPainter.end();
}

} // namespace QTerm

#include <moc_qtermscreen.cpp>
