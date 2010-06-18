#ifndef QTERMWINDOWBASE_H
#define QTERMWINDOWBASE_H

#include <QMdiSubWindow>

namespace QTerm
{
class WindowBase : public QMdiSubWindow
{
    Q_OBJECT

public:
	WindowBase(QWidget *parent=0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags) {
       		setAttribute(Qt::WA_DeleteOnClose); 
    }
	virtual ~WindowBase() {}

    bool hasAction(const QString& act){return listActions.contains(act);}
    
protected:
    QStringList listActions;
};

} // namespace QTerm

#endif // QTERMWINDOWBASE_H
