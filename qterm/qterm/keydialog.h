/****************************************************************************
** Form interface generated from reading ui file 'keydialog.ui'
**
** Created: Thu Jan 2 20:10:15 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include "keydialogui.h"

class QTermConfig;

class keyDialog : public keyDialogUI
{
    Q_OBJECT

public:
    keyDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~keyDialog();

protected:
	void connectSlots();
	void loadKey(int);
	void loadName();
	QTermConfig *pConf;

protected slots:
	void onNamechange(QListBoxItem*);
	void onClose();
	void onAdd();
	void onDelete();
	void onUpdate();
	void onScript();
	void onProgram();
	void onUp();
	void onDown();
	void onLeft();
	void onRight();
	void onEnter();
	void onSelect(int);
};

#endif // KEYDIALOG_H
