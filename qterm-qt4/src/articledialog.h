/****************************************************************************
** Form interface generated from reading ui file 'articledialog.ui'
**
** Created: Sun Jan 5 19:46:28 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ARTICLEDIALOG_H
#define ARTICLEDIALOG_H

#include "ui/ui_articledialog.h"

class articleDialog : public QDialog
{
	Q_OBJECT

public:
	articleDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );
	~articleDialog();

	QString strArticle;
	Ui::articleDialog ui;
protected:
	void connectSlots();
protected slots:
	void onClose();
	void onSave();
	void onSelect();
	void onCopy();
};

#endif // ARTICLEDIALOG_H
