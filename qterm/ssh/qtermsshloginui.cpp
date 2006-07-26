/****************************************************************************
** Form implementation generated from reading ui file 'fsshloginui.ui'
**
** Created: 三  2月 18 22:04:50 2004
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "qtermsshloginui.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a fSSHLoginUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
fSSHLoginUI::fSSHLoginUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "fSSHLoginUI" );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 30, 50, 70, 20 ) );

    bOK = new QPushButton( this, "bOK" );
    bOK->setGeometry( QRect( 40, 130, 100, 25 ) );

    bCancel = new QPushButton( this, "bCancel" );
    bCancel->setGeometry( QRect( 200, 130, 100, 25 ) );

    textLabel2_2 = new QLabel( this, "textLabel2_2" );
    textLabel2_2->setGeometry( QRect( 23, 10, 320, 20 ) );
    QFont textLabel2_2_font(  textLabel2_2->font() );
    textLabel2_2_font.setPointSize( 15 );
    textLabel2_2->setFont( textLabel2_2_font ); 

    leUserName = new QLineEdit( this, "leUserName" );
    leUserName->setGeometry( QRect( 110, 50, 210, 23 ) );

    lePassword = new QLineEdit( this, "lePassword" );
    lePassword->setGeometry( QRect( 110, 90, 210, 23 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 30, 90, 70, 20 ) );
    languageChange();
    resize( QSize(359, 173).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    connect(bOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(bCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
fSSHLoginUI::~fSSHLoginUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void fSSHLoginUI::languageChange()
{
    setCaption( tr( "Login" ) );
    textLabel1->setText( tr( "UserName:" ) );
    bOK->setText( tr( "OK" ) );
    bCancel->setText( tr( "Cancel" ) );
    textLabel2_2->setText( tr( "Please enter your username and password" ) );
    textLabel2->setText( tr( "Password:" ) );
}
#ifdef HAVE_CONFIG_H
#include "qtermsshloginui.moc"
#endif
