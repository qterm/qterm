/****************************************************************************
** Form interface generated from reading ui file 'aboutqterm2.ui'
**
** Created: Fri Dec 20 19:01:36 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

//#include <qwidget.h>
//#include "ui/ui_aboutdialog.h"
#include "ui_aboutdialog.h"

class aboutDialog : public QDialog
{ 
    Q_OBJECT

public:
    aboutDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~aboutDialog();
private:
	Ui::aboutDialog ui;
};

#endif // ABOUTDIALOG_H
