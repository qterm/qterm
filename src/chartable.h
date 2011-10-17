#ifndef CHARTABLE_H
#define CHARTABLE_H

#include <QWidget>
#include <QMap>

namespace QTerm
{

class CharTable : public QWidget
{
    Q_OBJECT
public:
    CharTable (QString symbolStr, QWidget *parent = 0);
	QSize sizeHint() const;

signals:
    void characterSelected(QString);

protected:
    void resizeEvent(QResizeEvent *re);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void paintEvent(QPaintEvent *pe);

private:
    QString symbols;
    int maxColumn;
    int square;
    QPoint hovered;
};



}

#endif
