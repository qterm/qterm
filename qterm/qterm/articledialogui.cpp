/****************************************************************************
** Form implementation generated from reading ui file 'articledialog.ui'
**
** Created: Mon Jan 6 21:19:12 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "articledialogui.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a articleDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
articleDialogUI::articleDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "articleDialogUI" );
    articleDialogUILayout = new QGridLayout( this, 1, 1, 11, 6, "articleDialogUILayout"); 

    textBrowser2 = new QTextBrowser( this, "textBrowser2" );

    articleDialogUILayout->addMultiCellWidget( textBrowser2, 0, 0, 0, 4 );

    saveButton = new QPushButton( this, "saveButton" );

    articleDialogUILayout->addWidget( saveButton, 1, 1 );

    closeButton = new QPushButton( this, "closeButton" );

    articleDialogUILayout->addWidget( closeButton, 1, 3 );
    QSpacerItem* spacer = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    articleDialogUILayout->addItem( spacer, 1, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    articleDialogUILayout->addItem( spacer_2, 1, 4 );
    QSpacerItem* spacer_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    articleDialogUILayout->addItem( spacer_3, 1, 0 );
    languageChange();
    resize( QSize(514, 504).expandedTo(minimumSizeHint()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
articleDialogUI::~articleDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void articleDialogUI::languageChange()
{
    setCaption( tr( "Article Viewer" ) );
    textBrowser2->setText( QString::null );
    saveButton->setText( tr( "Save..." ) );
    closeButton->setText( tr( "Close" ) );
}

