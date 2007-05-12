#ifndef QTERMSCREEN_H
#define QTERMSCREEN_H

#include <QScrollArea>
#include <QMap>
#include <QVariant>
#include <QPixmap>
#include <QEvent>
#include "qterm.h"

class QStandardItemModel;
class QHeaderView;
class QTermBuffer;
class QTermScreen;


class QTermScreenPrivate : public QWidget
{
    Q_OBJECT
public:
    QTermScreenPrivate(QWidget*,QTermBuffer*);
    ~QTermScreenPrivate();
    
    void setColorTable(QList<QColor>);
    void setBackgroundMode(const QPixmap&, QTerm::ImagePosition);

    void updateGeometries();
    void updateFontSize();
    void generateFontMetrics(const QFont&);

signals:
    void inputMethodText(QString);
    
protected slots:
    void caretTimerEvent();
    void blinkTimerEvent();
    
protected:
    void updateScreen();
    void blinkScreen();
    void updateCaret();
    void refreshScreen(QPaintEvent*);

    void drawSingleChar(int,int,QChar, short);
    void resizeEvent(QResizeEvent*);
//    void mouseMoveEvent(QMouseEvent*);
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent*);

	QVariant inputMethodQuery(Qt::InputMethodQuery property);
	void inputMethodEvent(QInputMethodEvent*);
    
    // coordinats conversion
    QPoint mapToPixel(const QPoint&);
    QPoint mapToChar(const QPoint&);
    QRect mapToRect(int,int,int,int);
    QRect mapToRect(const QRect&);
    
    // codecs conversion
    QTerm::CharFlag getCharFlag(const QByteArray& ba, int pos);
    QChar toUnicodeChar(const char*);
    
    // drawing
    void updateBackground();
    void drawGrids(QPainter&);
    void drawTextLine(int, int start=0, int end=-1);
    void drawSingleChar(QPainter*,int,int,QChar,short);
    QImage drawAquaBar(QSize);

protected:
    bool bCaretVisible, bCharVisible;
    QTimer *timerCaret, *timerBlink;
    
    QPixmap bgPxm;
    QTerm::ImagePosition imgPos;
    QList<QColor> listColors;

    QFont font;
    int charWidth, charHeight, charAscent;

    int startLine, endLine;
    QTermBuffer *pBuffer;

    QTerm::DisplayOrientation orientation;
    QTerm::PaintReason paintReason;
    
    friend class QTermScreen;
};

class QTermScreen : public QScrollArea
{
    Q_OBJECT
public:
    QTermScreen(QWidget*, QTermBuffer*, QTerm::Mode);
    ~QTermScreen();

    QPoint mapToChar(QPoint ppt)
    {return pScreen->mapToChar(pScreen->mapFromGlobal(ppt));}

    void update(QTerm::PaintReason);

    void scrollPageUp();
    void scrollPageDn();
    void scrollLineUp();
    void scrollLineDn();

    void ensureCaretVisible();
    void setCaretBlinkable(bool);
    void setRulerVisible(bool);
    void setDisplayOrientation(QTerm::DisplayOrientation ort){pScreen->orientation=ort;}

    void setFont(const QFont&fnt) {pScreen->font=fnt;pScreen->generateFontMetrics(fnt);}
    QFont getFont(){return pScreen->font;}
    
    void setSchema(QString file);

signals:
    void inputMethodText(QString);

public slots:
    void bufferSizeChanged();
    void scrollbarChanged(int);

protected:
    void updateGeometries();
    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    // settings
    bool bAutoFontSize;
    QMap<QString,QVariant> schema;
    QTerm::Mode mode;

    // ruler widgets
    QSize szRuler;
    QStandardItemModel *vModel, *hModel;
    QHeaderView *vRuler, *hRuler;

    QTermBuffer* pBuffer;
    QTermScreenPrivate* pScreen;
};

#endif // QTERMSCREEN_H
