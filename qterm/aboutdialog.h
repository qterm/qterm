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

#include <qwidget.h>
#include "aboutdialogui.h"

class aboutDialog : public aboutDialogUI
{ 
    Q_OBJECT

public:
    aboutDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~aboutDialog();

};

#endif // ABOUTDIALOG_H
