#ifndef CHARPANE_H
#define CHARPANE_H

#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtCore/QMap>
#include <QtCore/QString>

class QToolButton;
class QVBoxLayout;

namespace QTerm {

class Header : public QFrame {
    Q_OBJECT
public:
    Header(const QString &title, QWidget *parent = 0);

signals:
    void toggled(bool open);

public slots:
    void toggleOpen();

private:
    QToolButton *m_buttonOpen;
    bool m_bOpen;

};

class CharPane : public QWidget
{
    Q_OBJECT

public:
    CharPane(QWidget *parent = 0);
    ~CharPane();

    void addSubPane(QWidget *child, const QString &title);

protected slots:
    void characterSelected(QString ch);

signals:
    void characterSelectedSignal(QString ch);

private:
    QVBoxLayout *m_layoutVBox;
    QMap<QString,QString> mapSymbol;
    QList<QString> listSymbolName;
};

}
#endif // CHARPANE_H
