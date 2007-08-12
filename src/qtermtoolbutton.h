#ifndef QTERMTOOLBUTTON_H
#define QTERMTOOLBUTTON_H

#include <qtoolbutton.h>

namespace QTerm
{
class QTermToolButton: public QToolButton
{
	Q_OBJECT
public:
	 QTermToolButton( QWidget *parent, int id, QString name="" );
	~QTermToolButton();

signals:
	void buttonClicked(int);

protected slots:
	void slotClicked();
		
protected:
	int id;
};

} // namespace QTerm

#endif // QTERMTOOLBUTTON_H

