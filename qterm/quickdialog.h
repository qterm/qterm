/****************************************************************************
** Form interface generated from reading ui file 'quicklogin.ui'
**
** Created: Sat Dec 14 14:25:50 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QUICKDIALOG_H
#define QUICKDIALOG_H

#include "qtermparam.h"
#include "quickdialogui.h"

class QTermConfig;

class quickDialog : public quickDialogUI
{ 
    Q_OBJECT

public:
    quickDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~quickDialog();

	QTermParam param;

protected slots:
	void listChanged( int );
	void addAddr();
	void deleteAddr();
	void advOption();
	void connectIt();
	void close();

protected:
	void closeEvent( QCloseEvent *);
	void connectSlots();
	void loadHistory();

	QTermConfig * pConf;
};

#endif // QUICKDIALOG_H
