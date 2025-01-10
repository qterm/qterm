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
#include <Qt>
namespace QTerm
{
class aboutDialog : public QDialog
{ 
    Q_OBJECT

public:
    aboutDialog( QWidget* parent = 0, Qt::WindowFlags fl = Qt::Widget );
    ~aboutDialog();
private:
	Ui::aboutDialog ui;
};

} // namespace QTerm

#endif // ABOUTDIALOG_H
