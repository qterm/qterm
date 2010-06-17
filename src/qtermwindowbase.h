#ifndef QTERMWINDOWBASE_H
#define QTERMWINDOWBASE_H

#include <QWidget>
class QTermWindowBase;

class QTermWindowBase : public QWidget
{
    Q_OBJECT

public:
	QTermWindowBase(QWidget *parent=0):QWidget(parent,Qt::Window) {}
	virtual ~QTermWindowBase() {}

    bool hasAction(const QString& act){return listActions.contains(act);}
    
signals:
    void windowClosed(QTermWindowBase*);
    

protected:
    void closeEvent(QCloseEvent*e)
    {
        emit windowClosed(this);
    }
    
    QStringList listActions;
};


#endif // QTERMWINDOWBASE_H
