#ifndef QTERMTOOLBUTTON_H
#define QTERMTOOLBUTTON_H

#include <qtoolbutton.h>

namespace QTerm
{
class ToolButton: public QToolButton
{
	Q_OBJECT
public:
	 ToolButton( QWidget *parent, int id, QString name="" );
	~ToolButton();

signals:
	void buttonClicked(int);

protected slots:
	void slotClicked();
		
protected:
	int id;
};

} // namespace QTerm

#endif // QTERMTOOLBUTTON_H

