#ifndef PALLETE_H
#define PALLETE_H

#include "ui_pallete.h"
namespace QTerm
{
class Pallete : public QWidget, Ui::Pallete
{ 
    Q_OBJECT

signals:
	void colorChanged(int index, int role);
public:
    Pallete( QWidget* parent = 0, Qt::WFlags fl = 0 );
protected:
	bool eventFilter(QObject *obj, QEvent *e);

private:
	QList<QFrame*> frameList;
};

} // namespace QTerm

#endif // PALLETE_H
