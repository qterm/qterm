/****************************************************************************
** Form implementation generated from reading ui file 'zmodemdialogui.ui'
**
** Created: Tue Jul 20 23:00:10 2004
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

    pbProgress = new QProgressBar( this, "pbProgress" );
    pbProgress->setGeometry( QRect( 11, 35, 309, 24 ) );

    labelStatus = new QLabel( this, "labelStatus" );
    labelStatus->setGeometry( QRect( 11, 65, 309, 18 ) );
    labelStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, labelStatus->sizePolicy().hasHeightForWidth() ) );

    labelFileName = new QLabel( this, "labelFileName" );
    labelFileName->setGeometry( QRect( 11, 11, 309, 18 ) );
    labelFileName->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, labelFileName->sizePolicy().hasHeightForWidth() ) );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setGeometry( QRect( 117, 229, 77, 25 ) );
    buttonCancel->setAutoMask( FALSE );

    browserError = new QTextBrowser( this, "browserError" );
    browserError->setGeometry( QRect( 10, 90, 310, 128 ) );
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
    labelFileName->setText( tr( "textLabel1" ) );
    buttonCancel->setText( tr( "Cancel" ) );
}

