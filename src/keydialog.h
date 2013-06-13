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

#include "ui_keydialog.h"
namespace QTerm
{
class Config;

class keyDialog : public QDialog
{
    Q_OBJECT

public:
    keyDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~keyDialog();

protected:
	void connectSlots();
	void loadKey(int);
	void loadName();
	Config *pConf;

protected slots:
	void onNamechange(int);
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
// 	void onSelect(int);
private:
	Ui::keyDialog ui;
	QButtonGroup bgKey;
};

} // namespace QTerm

#endif // KEYDIALOG_H
