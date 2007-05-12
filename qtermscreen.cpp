#include "qtermscreen.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"

#include <QHeaderView>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QVariant>
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QSettings>
#include <QTimer>
#include <QApplication>

QTermScreen :: QTermScreen(QWidget* parent, QTermBuffer* buffer, QTerm::Mode mod)
        : QScrollArea(parent),
        szRuler(40,15),
        mode(mod),
        pBuffer(buffer)
{
    pScreen = new QTermScreenPrivate(this, buffer);
    setWidget(pScreen);
    connect(pBuffer, SIGNAL(bufferSizeChanged()),
            this, SLOT(bufferSizeChanged()));
    connect(pScreen, SIGNAL(fontSizeChanged(QSize)),
            this, SLOT(fontSizeChanged(QSize)));
    connect(pScreen, SIGNAL(inputMethodText(QString)),
            this, SIGNAL(inputMethodText(QString)) );
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrollbarChanged(int)));
    
    // Line/Column Ruler    
    vRuler = new QHeaderView(Qt::Vertical,this);
    vRuler->setResizeMode(QHeaderView::QHeaderView::Stretch);
    vModel = new QStandardItemModel(vRuler);
    vModel->insertRows(0,buffer->size().height());
    vRuler->setModel(vModel);
    
    setFocusProxy(pScreen);

    // for test
    setCaretBlinkable(true);
}

QTermScreen ::~QTermScreen()
{
}

void QTermScreen :: setSchema(QString file)
{
    QSettings setting(file, QSettings::IniFormat);
    foreach(QString key, setting.allKeys())
        schema[key]=setting.value(key);
    // color table
    QList<QColor> listColors;
    listColors.append(setting.value("Black").toString());
    listColors.append(setting.value("Red").toString());
    listColors.append(setting.value("Green").toString());
    listColors.append(setting.value("Yellow").toString());
    listColors.append(setting.value("Blue").toString());
    listColors.append(setting.value("Magenta").toString());
    listColors.append(setting.value("Cyan").toString());
    listColors.append(setting.value("Gray").toString());
    listColors.append(setting.value("LightGray").toString());
    listColors.append(setting.value("LightRed").toString());
    listColors.append(setting.value("LightGreen").toString());
    listColors.append(setting.value("LightYellow").toString());
    listColors.append(setting.value("LightBlue").toString());
    listColors.append(setting.value("LightMagenta").toString());
    listColors.append(setting.value("LightCyan").toString());
    listColors.append(setting.value("White").toString());
    pScreen->setColorTable(listColors);
    QImage iImg(schema["ImageFile"].toString());
    QImage fImg=QImage(iImg.size(),QImage::Format_ARGB32_Premultiplied);
    if(!iImg.isNull())
    {
        // loading image backgound
        QPainter painter(&fImg);
        painter.drawImage(0,0,iImg);
        QColor color(listColors[0]);
        color.setAlpha(schema["Alpha"].toInt());
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.fillRect(0,0,iImg.width(),iImg.height(),color);
        painter.end();
    }
    pScreen->setBackgroundMode(QPixmap::fromImage(fImg),
        QTerm::ImagePosition(schema["ImagePosition"].toInt()));
}

void QTermScreen :: ensureCaretVisible()
{
    QPoint cpt=pBuffer->absCaret();
    QPoint ppt=pScreen->mapToPixel(cpt);
    ensureVisible(ppt.x(),ppt.y());
}
void QTermScreen :: setCaretBlinkable(bool show)
{
    if(show)
        pScreen->timerCaret->start(500);
    else
        pScreen->timerCaret->stop();
}

void QTermScreen :: update(QTerm::PaintReason reason)
{
    pScreen->paintReason = reason;
    if(reason==QTerm::NewData)
    {
        for(int y=0;y<pBuffer->lines();y++)// check for changes line by line
        {
            QTermTextLine *line=pBuffer->absAt(y);
            QPoint cpt=line->getChanged();
            line->clearChange();

            if(cpt.x()==-1) // no change
                continue;

            int start,end;
            start=cpt.x();
            if(cpt.y()==-1) // until end of line
                end=pBuffer->size().width();
            else
                end=cpt.y();

            QRect prc=pScreen->mapToRect(start,y,end-start+1,1);
            pScreen->update(prc);
        }
    }
}

void QTermScreen::scrollPageUp()
{
    QScrollBar *scrollbar;
    switch(pScreen->orientation)
    {
        case QTerm::DisplayHorizontal:
            scrollbar = verticalScrollBar();
        break;
        case QTerm::DisplayVertical:
            scrollbar = horizontalScrollBar();            
        break;
    }    
    scrollbar->triggerAction(QScrollBar::SliderPageStepSub);
}
void QTermScreen::scrollPageDn()
{
    QScrollBar *scrollbar;
    switch(pScreen->orientation)
    {
        case QTerm::DisplayHorizontal:
            scrollbar = verticalScrollBar();
        break;
        case QTerm::DisplayVertical:
            scrollbar = horizontalScrollBar();            
        break;
    }    
    scrollbar->triggerAction(QScrollBar::SliderPageStepAdd);
}
void QTermScreen::scrollLineUp()
{
    QScrollBar *scrollbar;
    switch(pScreen->orientation)
    {
        case QTerm::DisplayHorizontal:
            scrollbar = verticalScrollBar();
        break;
        case QTerm::DisplayVertical:
            scrollbar = horizontalScrollBar();            
        break;
    }    
    scrollbar->triggerAction(QScrollBar::SliderSingleStepSub);
}
void QTermScreen::scrollLineDn()
{
    QScrollBar *scrollbar;
    switch(pScreen->orientation)
    {
        case QTerm::DisplayHorizontal:
            scrollbar = verticalScrollBar();
        break;
        case QTerm::DisplayVertical:
            scrollbar = horizontalScrollBar();            
        break;
    }    
    scrollbar->triggerAction(QScrollBar::SliderSingleStepAdd);
}

/******************
***** SLOTS *******
******************/
void QTermScreen :: bufferSizeChanged()
{
    pScreen->updateGeometries();
    vModel->clear();
    vModel->insertRows(0,pBuffer->lines());
}

void QTermScreen :: scrollbarChanged(int)
{
    vRuler->setOffset(verticalScrollBar()->value());
}

void QTermScreen :: setRulerVisible(bool visible)
{
    vRuler->setVisible(visible);
}

void QTermScreen :: updateGeometries()
{
    int width = !vRuler->isHidden() ? szRuler.width() : 0;
    setViewportMargins(width, 0, 0, 0);

    // propagate size change to QTermBuffer (size)
    // and QTermScreenPrivate (font)
    if(mode==QTerm::EDITOR||mode==QTerm::VIEWER)
    {
        int cx = qMax(viewport()->width()/pScreen->charWidth,
                pBuffer->size().width());
        int cy = qMax(viewport()->height()/pScreen->charHeight,
                pBuffer->lines());
        pBuffer->setSize( QSize(cx, cy) );
    }
    else if(mode==QTerm::BBS)
    {
        if(bAutoFontSize)
        {
            pScreen->updateFontSize();
        }
        else
        {
            int cx = viewport()->width()/pScreen->charWidth;
            int cy = viewport()->height()/pScreen->charHeight;
            if( cx>10 && cy>10 )
                pBuffer->setSize( QSize(cx, cy) );
        }

    }

    // update headers
    QRect vg = viewport()->geometry();
    int verticalLeft = vg.left() - width;
    vRuler->setGeometry(verticalLeft, vg.top(), width, vg.height());
    if (vRuler->isHidden())
        QMetaObject::invokeMethod(vRuler, "updateGeometries");

    vRuler->setOffset(verticalScrollBar()->value());
    
    
  // this is toooo slow, how?
    for(int i=0; i< vRuler->count(); i++)
        vRuler->resizeSection(i, pScreen->charHeight);

}

void QTermScreen :: keyPressEvent(QKeyEvent*e)
{   
    // up propogate to parent
    e->ignore();
}

void QTermScreen :: resizeEvent(QResizeEvent*e)
{
    updateGeometries();

    // get new font settings
    pScreen->updateGeometries();

    QScrollBar* vBar= verticalScrollBar();
    vBar->setSingleStep(pScreen->charHeight);
    vBar->setPageStep(vBar->pageStep()/pScreen->charHeight*pScreen->charHeight);
    QScrollBar* hBar= horizontalScrollBar();
    hBar->setSingleStep(pScreen->charWidth);

    // adjust scrollbar range pagestep etc
    QScrollArea::resizeEvent(e);
}



/*************************************************************
    class QTermScreenPrivate
    the actual display 
*************************************************************/

QTermScreenPrivate :: QTermScreenPrivate(QWidget* parent, QTermBuffer* buffer)
            : QWidget(parent)
//            ,orientation(QTerm::DisplayVertical)
{
    pBuffer=buffer;
    
    setFocusPolicy(Qt::ClickFocus);
	setAttribute(Qt::WA_InputMethodEnabled, true);

    // timers
    timerCaret = new QTimer(this);
	connect(timerCaret, SIGNAL(timeout()), this, SLOT(caretTimerEvent()));
    timerBlink = new QTimer(this);
	connect(timerBlink, SIGNAL(timeout()), this, SLOT(blinkTimerEvent()));
    
    timerBlink->start(1000);
}

QTermScreenPrivate ::~QTermScreenPrivate()
{
}

void QTermScreenPrivate :: blinkTimerEvent()
{
    bCharVisible=!bCharVisible;

    QRegion rg=visibleRegion();    
    QTermTextLine *line;
    for(int y=0; y<pBuffer->lines(); y++)
    {
        line=pBuffer->absAt(y);
        // only update visble lines
        if(line->hasBlink())
        {
            QRect prc=mapToRect(0,y,line->length(),1);
            if(rg.intersects(prc))
                update(prc);
        }
    }
}

void QTermScreenPrivate :: caretTimerEvent()
{
    bCaretVisible=!bCaretVisible;
    paintReason = QTerm::UpdateCaret;
    QPoint pt=pBuffer->absCaret();
    update(mapToRect(pt.x(),pt.y(),1,1));
}

void QTermScreenPrivate :: inputMethodEvent(QInputMethodEvent * e)
{
     emit inputMethodText(e->commitString());
}

QVariant QTermScreenPrivate::inputMethodQuery(Qt::InputMethodQuery property)
{
    QPoint cpt=pBuffer->absCaret();
	switch(property)
	{
		case Qt::ImMicroFocus:
			return QVariant(mapToRect(cpt.x()+1,cpt.y()+1,1,1));
		case Qt::ImFont:
			return QVariant(font);
		case Qt::ImCursorPosition:
			return cpt.x();
		default:
			return QVariant();
	}
}

void QTermScreenPrivate :: keyPressEvent(QKeyEvent*e)
{   
    // up propogate to parent
    e->ignore();
}

void QTermScreenPrivate :: resizeEvent(QResizeEvent*)
{
    updateBackground();
}

void QTermScreenPrivate :: paintEvent(QPaintEvent*pe)
{
   	switch(paintReason)
	{
		case QTerm::UpdateCaret:
//			updateCaret();
            refreshScreen(pe);
			break;
        case QTerm::NewData:
            refreshScreen(pe);
            bCaretVisible=true;
            updateCaret();
            break;
        case QTerm::Refresh:
            refreshScreen(pe);
        default:
            break;
	}
    paintReason = QTerm::Refresh;
}

void QTermScreenPrivate :: updateScreen()
{
    for(int y=0;y<pBuffer->lines();y++)
    {
        QTermTextLine *line=pBuffer->absAt(y);
        QPoint cpt=line->getChanged();
        drawTextLine(y,cpt.x(),cpt.y());
    }
}

void QTermScreenPrivate :: refreshScreen(QPaintEvent*pe)
{
	QRect rc = pe->rect();
	QPoint pntTL = mapToChar(QPoint(rc.topLeft()));	
	QPoint pntBR = mapToChar(QPoint(rc.bottomRight()));
    pntBR+=QPoint(1,1);

/* merge grids into background    
// Draw grids
    QPainter painter(this);
    painter.setPen(listColors[7].dark());
    // Horizontal
    for(int row=pntTL.y(); row<=pntBR.y(); row++)
        painter.drawLine(mapToPixel(QPoint(pntTL.x(),row)),
                mapToPixel(QPoint(pntBR.x(),row)) );
    // Vertical
    for(int col=pntTL.x();col<=pntBR.x();col++)
        painter.drawLine(mapToPixel(QPoint(col,pntTL.y())),
                mapToPixel(QPoint(col,pntBR.y())));
    painter.end();
//    qWarning("(%d,%d),(%d,%d)",pntTL.x(),pntTL.y(),pntBR.x(),pntBR.y());
*/

/* Update text */
    for(int i=pntTL.y()-1;i<=pntBR.y()+1&&i<pBuffer->lines();i++)
    {
        drawTextLine(i,pntTL.x(),pntBR.x());
    }
}

void QTermScreenPrivate :: updateCaret()
{
    QPoint cpt=pBuffer->absCaret(); //position by char
    QPoint ppt=mapToPixel(cpt); //position by pixel
    
    // redraw text because it has been invalidate in caretTimerEvent()
    drawTextLine(cpt.y(),cpt.x(),cpt.x());
    
    // draw the block caret if visible
    if (bCaretVisible)
    {
        QPainter painter(this);
// either draw complex aqua style
/*
        QImage img = drawAquaBar(QSize(charWidth,charHeight));
        QImage alpha(img.size(),QImage::Format_Indexed8);
        alpha.fill(180);
        img.setAlphaChannel(alpha);
        painter.setBrush(QPixmap::fromImage(img));
        painter.drawRect(ppt.x(), ppt.y(), charWidth, charHeight);
*/
// or use pure color
        QColor clr;
        if(pBuffer->getMode(INSERT_MODE))
            clr=listColors[4];
        else
            clr=listColors[1];
        clr.setAlpha(200);
        painter.fillRect(ppt.x(), ppt.y(), charWidth, charHeight,clr);
    }
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                            Drawing	                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
// TODO: add selection & menu attributes
/*
1. if starts with an illed chinese char
     a. if it is not the first one in the selected area, --start index
     b. start index unchanged
2. else start index unchanged
3. do the same with end index
4. define the edge of selected area
5. convert text to unicode
6. draw one by one

define the edge
if illed
    if selected,
        if previous one also selected, --start
        else do nothing
    else if not selected,
        if previous one also not selected, --start
        else do nothing
else do nothing
    
*/
void QTermScreenPrivate :: drawTextLine(int index, int start, int end)
{
    QTermTextLine *line = pBuffer->absAt(index);

    if(line==NULL)
        return;
    if(start>=line->length())
        return;

    QByteArray text = line->getPlainText();
    QByteArray color = line->getColor();
	QByteArray attr = line->getAttr();

    // check (start,end)
    QTerm::CharFlag cf = getCharFlag(text,start);
    if(cf==QTerm::Cn_Second&&start!=0) // avoid breaking wide chars
        start-=1;

    if(end==-1)
        end=line->length()-1;
    
    QPainter painter(this);

    for(int i=start; i<=end && i<line->length(); i++)
    {
        char cp = color.at(i);
		char ea = attr.at(i);
		short tempattr = SETCOLOR(cp) | SETATTR(ea);
        QChar ch;
        QTerm::CharFlag cf = getCharFlag(text,i);    
		switch(cf)
		{
            case QTerm::Latin1:
                drawSingleChar(&painter,i,index,text.at(i),tempattr);
            break;
            case QTerm::Cn_Second:
                i--;    // flow down
            case QTerm::Cn_First:
                // to Unicode
                ch=toUnicodeChar(text.mid(i,2));
                // one chinese character with two colors
                if(color.at(i+1)!=cp || attr.at(i+1)!=ea)
                {
                    painter.setClipRect(mapToRect(i,index,1,1));
                    drawSingleChar(&painter,i,index,ch,tempattr);
                    painter.setClipRect(mapToRect(i+1,index,1,1));
                    drawSingleChar(&painter,i,index,ch,tempattr);
                }
                else // normal
                    drawSingleChar(&painter,i,index,ch,tempattr);
                i++;
            break;
        }
    }
}

// the actual drawing function
void QTermScreenPrivate :: drawSingleChar(QPainter* painter, int x, int y, QChar ch, short attribute)
{
    QPoint pt = mapToPixel(QPoint(x,y));

	char cp = GETCOLOR(attribute);
	char ea = GETATTR(attribute);

	// test bold mask
	if(GETBOLD(ea))
		cp = SETHIGHLIGHT(cp);// use 8-15 color
	// test underline mask
	if( GETUNDERLINE(ea) )
	{font.setUnderline(true);painter->setFont(font);}
	else
	{font.setUnderline(false);painter->setFont(font);}

	// test reverse mask
	if ( GETREVERSE( ea ) )
		cp = REVERSECOLOR( cp );

    char bg = GETBG(cp);
    char fg = GETFG(cp);

 	painter->setPen( listColors[fg] );

   	if(bg!=0)
    {
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBackground( listColors[bg] );
    }
    else
        painter->setBackgroundMode(Qt::TransparentMode);

    int w = ch.decompositionTag()==QChar::Square?2:1;
    if(GETBLINK(ea) && !bCharVisible) // hide text, draw background only
    {
        if(bg!=0)
            painter->fillRect(mapToRect(x,y,w,1), listColors[bg]);
    }
    else // draw text
		painter->drawText( pt.x(), pt.y()+charAscent, ch);
}

void QTermScreenPrivate :: drawGrids(QPainter& painter)
{
    QPen pen(listColors[7].dark());
    pen.setWidth(1);
    painter.setPen(pen);
    //TODO: set pen color to lighter foreground color
    for(int row=0; row<=pBuffer->lines(); row++)
        painter.drawLine(mapToPixel(QPoint(0,row)),
                mapToPixel(QPoint(pBuffer->size().width(),row)) );
    for(int col=0;col<=pBuffer->size().width();col++)
        painter.drawLine(mapToPixel(QPoint(col,0)),
                mapToPixel(QPoint(col,pBuffer->lines())));
}

QImage QTermScreenPrivate :: drawAquaBar(QSize sz)
{
    QImage img(sz,QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&img);
    QLinearGradient lg(0,0,0,sz.height());
    // http://veerle.duoh.com/index.php/blog/comments/5_steps_to_achieve_nice_aqua_effect/
    lg.setColorAt(0,QColor(50,147,255));
    lg.setColorAt(0.8,QColor(76,159,240));
    lg.setColorAt(1, QColor(174,227,225));
    painter.fillRect(0,0,sz.width(),sz.height(),lg);
    
    QSize szi = sz;
    szi.setHeight(sz.height()/6);
    QLinearGradient lp(0,0,0,szi.height());
    // arbitary, I am not a artist
    lp.setColorAt(0,QColor(60,200,255,255));
    lp.setColorAt(0.9,QColor(75,155,255,180));    
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setBrush(lp);
    painter.fillRect(0,sz.height()/7,szi.width(),szi.height(),lp);
    painter.end();    
    
    return img;
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                            Font Related	                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermScreenPrivate :: updateFontSize()
{
    int wBuffer = pBuffer->size().width();
    int hBuffer = pBuffer->size().height();

    int wView = parentWidget()->width();
    int hView = parentWidget()->height();
    
	int nPixelSize;
	int nIniSize = qMax(8,qMin(hView/hBuffer,wView*2/wBuffer));

	for( nPixelSize=nIniSize-3; nPixelSize<=nIniSize+3; nPixelSize++)
	{
		font.setPixelSize( nPixelSize );		
		generateFontMetrics(font);
		
		if( (hBuffer*charHeight)>hView
			|| (wBuffer*charWidth)>wView )
		{
			while(nPixelSize>5)
			{
				nPixelSize--;
				font.setPixelSize( nPixelSize );
				generateFontMetrics(font);
				if( (hBuffer*charHeight)<hView
				&& (wBuffer*charWidth)<wView  )
					break;
			}
			break;
		}
	}
}

void QTermScreenPrivate::generateFontMetrics(const QFont& fnt)
{
    QFontMetrics fm(fnt);
	int cn=fm.width(QChar(0x4e00));
	int en=fm.width('W');
	if(en/cn<0.7) // almost half
		charWidth = qMax((cn+1)/2,en);
	else
		charWidth = (qMax(en,cn)+1)/2;

	charHeight = fm.height();
	charAscent = fm.ascent();
}
/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                            Schema	                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void QTermScreenPrivate :: updateBackground()
{
    if(bgPxm.isNull())
        return;

	QPalette plt(palette());

    QPainter painter;
    QPixmap pxm;
    QSize szWin = parentWidget()->size();
    switch(imgPos)
    {
        case QTerm::Center:
            pxm = QPixmap(size());
            pxm.fill(listColors[0]);
            painter.begin(&pxm);
            painter.drawPixmap( (szWin.width()-bgPxm.width())/2,
				(szWin.height()-bgPxm.height())/2, bgPxm);
			drawGrids(painter);
			painter.end();
			plt.setBrush(backgroundRole(), QBrush(pxm));
			setPalette(plt);
            break;
        case QTerm::Stretch:
            pxm = bgPxm.scaled(szWin,Qt::KeepAspectRatio);
            plt.setBrush(backgroundRole(), QBrush(pxm));
			setPalette(plt);
            break;
    }
}

void QTermScreenPrivate :: setBackgroundMode(const QPixmap& pxm, QTerm::ImagePosition pos)
{
    // set background color anyway
	QPalette plt(palette());
    plt.setColor(backgroundRole(), listColors[0]);
    setPalette(plt);

    if(pxm.isNull())
        return;

    // tiled image is handled by brush
    bgPxm = pxm; imgPos = pos;
    if(imgPos==QTerm::Tile)
    {
        plt.setBrush(backgroundRole(), QBrush(pxm));
        setPalette(plt);
    }
}
void QTermScreenPrivate :: setColorTable(QList<QColor> colors)
{
    listColors = colors;
}

/* ------------------------------------------------------------------------ */
/*	                                                                        */
/* 	                            Auxilluary	                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
// FIXME: This is tooo simplified, not useful
// txt="a \x8a\x4f  example"
// getChar(txt,0) returns QTerm::Latin1
// getChar(txt,2) returns QTerm::Cn_First
// getChar(txt,3) returns QTerm::Cn_Second
QTerm::CharFlag QTermScreenPrivate :: getCharFlag(const QByteArray& ba, int pos)
{
    if(uchar(ba.at(pos))<uchar('\x7f'))
        return QTerm::Latin1;
    int counts=0;
    for(int i=0;i<=pos;i++)
    {
        if(uchar(ba.at(i))>uchar('\x7f'))
            counts++;
    }
    if(counts%2==0)
        return QTerm::Cn_Second;
    else
        return QTerm::Cn_First;
}

// convert a single ascii or chinese characters to QChar
QChar QTermScreenPrivate :: toUnicodeChar(const char* ch)
{
    QTextCodec *pCodec = QTextCodec::codecForName("GBK");
    return pCodec->toUnicode(ch).at(0);
}

void QTermScreenPrivate :: updateGeometries()
{
    switch(orientation)
    {
        case QTerm::DisplayHorizontal:
            resize(charWidth*pBuffer->size().width(),
                charHeight*pBuffer->lines());
        break;
        case QTerm::DisplayVertical:
            resize(charHeight*pBuffer->lines(),
            charWidth*pBuffer->size().width());
        break;
    }
}

QPoint QTermScreenPrivate :: mapToPixel( const QPoint& point )
{
    QPoint pxlPoint;

    switch(orientation)
    {
        case QTerm::DisplayHorizontal:
            pxlPoint.setX( point.x()*charWidth+rect().x() );
            pxlPoint.setY( point.y()*charHeight+rect().y() );
        break;
        case QTerm::DisplayVertical:
            pxlPoint.setX( point.y()*charWidth+rect().x() );
            pxlPoint.setY( point.x()*charHeight+rect().y() );            
        break;    
    }

    return pxlPoint;
}

QPoint QTermScreenPrivate :: mapToChar( const QPoint& point )
{
	QPoint chPoint;
    switch(orientation)
    {
        case QTerm::DisplayHorizontal:
	       chPoint.setX( point.x()/charWidth );
	       chPoint.setY( point.y()/charHeight );
        break;
        case QTerm::DisplayVertical:
	       chPoint.setY( point.x()/charWidth );
	       chPoint.setX( point.y()/charHeight );
        break;
    }
            
	return chPoint;
}

QRect QTermScreenPrivate :: mapToRect( int x, int y, int width, int height )
{
    QPoint pt = mapToPixel( QPoint(x,y) );

    switch(orientation)
    {
        case QTerm::DisplayHorizontal:
            if( width == -1 ) // to the end
                return QRect(pt.x(), pt.y(), 
                    size().width(), height*charHeight );
            else
                return QRect(pt.x(), pt.y(), 
                    width*charWidth, height*charHeight );
        case QTerm::DisplayVertical:
            if( width == -1 ) // to the end
                return QRect(pt.x(), pt.y(), 
                    size().height(), width*charWidth );
            else
                return QRect(pt.x(), pt.y(), 
                    width*charHeight, height*charWidth );
    }                
}

QRect QTermScreenPrivate :: mapToRect( const QRect& rect )
{
	return mapToRect( rect.x(), rect.y(), rect.width(), rect.height() );
}
