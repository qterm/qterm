#ifndef QTERMTOOLBUTTON_H
#define QTERMTOOLBUTTON_H

#include <qtoolbutton.h>

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

#endif // QTERMTOOLBUTTON_H



