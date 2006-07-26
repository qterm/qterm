/****************************************************************************
** Form implementation generated from reading ui file 'quickdialog.ui'
**
** Created: Mon Apr 7 18:54:02 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "quickdialogui.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>


/* 
 *  Constructs a quickDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
quickDialogUI::quickDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "quickDialogUI" );
    setMinimumSize( QSize( 350, 150 ) );
    setMaximumSize( QSize( 350, 150 ) );
    QFont f( font() );
    setFont( f ); 

    deletePushButton = new QPushButton( this, "deletePushButton" );
    deletePushButton->setGeometry( QRect( 230, 20, 60, 26 ) );

    advPushButton = new QPushButton( this, "advPushButton" );
    advPushButton->setGeometry( QRect( 20, 100, 70, 26 ) );

    connectPushButton = new QPushButton( this, "connectPushButton" );
    connectPushButton->setGeometry( QRect( 150, 100, 70, 26 ) );

    closePushButton = new QPushButton( this, "closePushButton" );
    closePushButton->setGeometry( QRect( 250, 100, 60, 26 ) );

    historyComboBox = new QComboBox( FALSE, this, "historyComboBox" );
    historyComboBox->setGeometry( QRect( 70, 20, 150, 22 ) );

    TextLabel6 = new QLabel( this, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 230, 60, 40, 20 ) );

    portLineEdit = new QLineEdit( this, "portLineEdit" );
    portLineEdit->setGeometry( QRect( 270, 60, 60, 22 ) );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 20, 20, 50, 20 ) );
    TextLabel4->setFrameShape( QLabel::NoFrame );
    TextLabel4->setFrameShadow( QLabel::Plain );

    TextLabel5 = new QLabel( this, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 10, 60, 50, 20 ) );

    addrLineEdit = new QLineEdit( this, "addrLineEdit" );
    addrLineEdit->setGeometry( QRect( 70, 60, 140, 23 ) );

    addPushButton = new QPushButton( this, "addPushButton" );
    addPushButton->setGeometry( QRect( 300, 20, 30, 26 ) );
    languageChange();
    resize( QSize(350, 150).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( historyComboBox, deletePushButton );
    setTabOrder( deletePushButton, addPushButton );
    setTabOrder( addPushButton, addrLineEdit );
    setTabOrder( addrLineEdit, portLineEdit );
    setTabOrder( portLineEdit, advPushButton );
    setTabOrder( advPushButton, connectPushButton );
    setTabOrder( connectPushButton, closePushButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
quickDialogUI::~quickDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void quickDialogUI::languageChange()
{
    setCaption( tr( "Quick Login" ) );
    deletePushButton->setText( tr( "Delete" ) );
    advPushButton->setText( tr( "Advanced" ) );
    connectPushButton->setText( tr( "Connect" ) );
    closePushButton->setText( tr( "Close" ) );
    TextLabel6->setText( tr( "Port" ) );
    TextLabel4->setText( tr( "History" ) );
    TextLabel5->setText( tr( "Address" ) );
    addPushButton->setText( QString::null );
}
#ifdef HAVE_CONFIG_H
#include "quickdialogui.moc"
#endif
