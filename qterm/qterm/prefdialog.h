/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Sat Dec 14 15:21:34 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include "prefdialogui.h"

class prefDialog : public prefDialogUI
{ 
    Q_OBJECT

public:
    prefDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~prefDialog();

protected slots:
	void onOK();
	void onCancel();
	void onSound();
	void onHttp();
	void onBeep(int);
	void onBrowse();

protected:
	void closeEvent( QCloseEvent * );
	void connectSlots();
	void loadSetting();
	void saveSetting();
};

#endif // PREFDIALOG_H
