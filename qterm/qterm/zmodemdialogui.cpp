/****************************************************************************
** Form implementation generated from reading ui file 'zmodemdialogui.ui'
**
** Created: Wed Aug 18 19:57:58 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "zmodemdialogui.h"

#include <qvariant.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a zmodemDialogUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
zmodemDialogUI::zmodemDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "zmodemDialogUI" );
    zmodemDialogUILayout = new QGridLayout( this, 1, 1, 11, 6, "zmodemDialogUILayout"); 

    pbProgress = new QProgressBar( this, "pbProgress" );

    zmodemDialogUILayout->addMultiCellWidget( pbProgress, 1, 1, 0, 2 );

    labelStatus = new QLabel( this, "labelStatus" );
    labelStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, labelStatus->sizePolicy().hasHeightForWidth() ) );

    zmodemDialogUILayout->addMultiCellWidget( labelStatus, 2, 2, 0, 2 );
    spacer2 = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    zmodemDialogUILayout->addItem( spacer2, 4, 2 );
    spacer1 = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    zmodemDialogUILayout->addItem( spacer1, 4, 0 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoMask( FALSE );

    zmodemDialogUILayout->addWidget( buttonCancel, 4, 1 );

    browserError = new QTextBrowser( this, "browserError" );

    zmodemDialogUILayout->addMultiCellWidget( browserError, 3, 3, 0, 2 );

    labelFileName = new QLabel( this, "labelFileName" );
    labelFileName->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, labelFileName->sizePolicy().hasHeightForWidth() ) );

    zmodemDialogUILayout->addMultiCellWidget( labelFileName, 0, 0, 0, 2 );
    languageChange();
    resize( QSize(333, 259).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
zmodemDialogUI::~zmodemDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void zmodemDialogUI::languageChange()
{
    setCaption( tr( "Zmodem Status" ) );
    labelStatus->setText( tr( "textLabel2" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    labelFileName->setText( tr( "textLabel1" ) );
}

