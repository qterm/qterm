/****************************************************************************
** Form implementation generated from reading ui file 'msgdialog.ui'
**
** Created: Wed Mar 19 18:39:29 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "msgdialogui.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a msgDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
msgDialogUI::msgDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "msgDialogUI" );
    msgDialogUILayout = new QGridLayout( this, 1, 1, 11, 6, "msgDialogUILayout"); 

    msgBrowser = new QTextBrowser( this, "msgBrowser" );

    msgDialogUILayout->addMultiCellWidget( msgBrowser, 0, 0, 0, 2 );

    okButton = new QPushButton( this, "okButton" );

    msgDialogUILayout->addWidget( okButton, 1, 1 );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    msgDialogUILayout->addItem( spacer, 1, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    msgDialogUILayout->addItem( spacer_2, 1, 0 );
    languageChange();
    resize( QSize(600, 428).expandedTo(minimumSizeHint()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
msgDialogUI::~msgDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void msgDialogUI::languageChange()
{
    setCaption( tr( "Messages Viewer" ) );
    okButton->setText( tr( "OK" ) );
}

