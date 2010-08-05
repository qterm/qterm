#ifndef CHARTABLE_H
#define CHARTABLE_H

#include <QWidget>

namespace QTerm
{

class CharTable : public QWidget
{
    Q_OBJECT
public:
    CharTable (QWidget *parent = 0);

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
