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

#include "ui/ui_prefdialog.h"
//Added by qt3to4:
//#include <QCloseEvent>
class QCloseEvnt;

class prefDialog : public QDialog
{ 
    Q_OBJECT

public:
    prefDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~prefDialog();

protected slots:
	void onOK();
	void onCancel();
	void onSound();
	void onHttp();
// 	void onBeep(int);
	void onBrowse();
	void onImage();
	void onPool();

protected:
	void closeEvent( QCloseEvent * );
	void connectSlots();
	void loadSetting();
	void saveSetting();

private:
	Ui::prefDialog ui;
	QButtonGroup bgSound;
};

#endif // PREFDIALOG_H
