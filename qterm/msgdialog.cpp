/****************************************************************************
** Form implementation generated from reading ui file 'msgDialog.ui'
**
** Created: Fri Nov 24 14:49:01 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "msgdialog.h"

#include <qpushbutton.h>
#include <qtextbrowser.h>

#include <stdio.h>

msgDialog::msgDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : msgDialogUI( parent, name, modal, fl )
{
    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
msgDialog::~msgDialog()
{
    // no need to delete child widgets, Qt does it all for us
}
#ifdef HAVE_CONFIG_H
#include "msgdialog.moc"
#endif
