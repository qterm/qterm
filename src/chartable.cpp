#include "chartable.h"

#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>

#include <QtDebug>
#include <QMessageBox>

namespace QTerm
{

CharTable :: CharTable(QString symbolStr, QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    maxColumn = 10;
    square = 25;

    symbols = symbolStr;
}

void CharTable ::resizeEvent(QResizeEvent *re)
{
    maxColumn = re->size().width()/square;
    setMinimumHeight((symbols.length()/maxColumn + 1)*square);
}

QSize CharTable :: sizeHint() const
{
    int x = maxColumn*square;
    int y = (symbols.length()/maxColumn + 1)*square;
    return QSize(x, y);
}

void CharTable :: mouseMoveEvent(QMouseEvent *me)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPoint pt = me->pos();
#else
    QPoint pt = me->position().toPoint();
#endif
    int column = pt.x()/square;
    int row = pt.y()/square;
    if (hovered == QPoint(column, row))
        return;
    QRect oldRect(hovered.x()*square, hovered.y()*square, square, square);
    hovered.setX(column);
    hovered.setY(row);
    QRect newRect(hovered.x()*square, hovered.y()*square, square, square);
    update(oldRect.united(newRect));
}

void CharTable :: mouseReleaseEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint pt = me->pos();
#else
        QPoint pt = me->position().toPoint();
#endif
        int n = (pt.y()/square)*maxColumn + pt.x()/square;
        if (n<symbols.length())
            emit characterSelected(symbols.at(n));
        update();
    }
}

void CharTable :: paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    painter.fillRect(pe->rect(), QBrush(Qt::white));
    QRect redrawRect = pe->rect();
    int beginRow = redrawRect.top()/square;
    int endRow = redrawRect.bottom()/square;
    int beginColumn = redrawRect.left()/square;
    int endColumn = redrawRect.right()/square;
    QFont font("SimSun",20);

    for (int row = beginRow; row <= endRow; ++row) {
        for (int column = beginColumn; column <= endColumn && column < maxColumn; ++column) {
            int n = row*maxColumn + column;
            QString text;
            if (n < symbols.length())
                text = symbols.at(n);
            QRect textRect = QRect(column*square, row*square, square, square);
            if (hovered == QPoint(column, row)) {
                painter.fillRect(textRect, palette().highlight());
                painter.setPen(palette().highlightedText().color());
            } else {
                painter.setPen(QPen(Qt::gray));
                painter.drawRect(textRect);
                painter.setPen(QPen(Qt::black));
            }
            if (!text.isEmpty()) {
                painter.setFont(font);
                painter.drawText(textRect, Qt::AlignCenter, text);
            }
        }
    }
}

}

#include "moc_chartable.cpp"
