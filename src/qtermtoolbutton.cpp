/*******************************************************************************
FILENAME:      qtermtoolbutton.cpp
REVISION:      2003.1.2 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qtermtoolbutton.h"
namespace QTerm
{
QTermToolButton::QTermToolButton( QWidget *parent, int id, QString name )
		:QToolButton(parent)
{
	setObjectName(name);
	this->id = id;
	connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

QTermToolButton::~QTermToolButton()
{
}

void QTermToolButton::slotClicked()
{
	emit(buttonClicked(id));
}

} // namespace QTerm

#include <qtermtoolbutton.moc>
