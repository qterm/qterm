/****************************************************************************
** Form implementation generated from reading ui file 'keydialog.ui'
**
** Created: Mon Jan 6 21:19:45 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "keydialogui.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a keyDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
keyDialogUI::keyDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "keyDialogUI" );
    setMinimumSize( QSize( 440, 380 ) );
    setMaximumSize( QSize( 440, 380 ) );

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setGeometry( QRect( 10, 200, 420, 170 ) );
    buttonGroup2->setMinimumSize( QSize( 0, 0 ) );

    radioButton1 = new QRadioButton( buttonGroup2, "radioButton1" );
    radioButton1->setGeometry( QRect( 10, 30, 50, 20 ) );

    keyEdit = new QLineEdit( buttonGroup2, "keyEdit" );
    keyEdit->setGeometry( QRect( 80, 30, 320, 20 ) );

    downButton = new QPushButton( buttonGroup2, "downButton" );
    downButton->setGeometry( QRect( 180, 60, 50, 25 ) );

    enterButton = new QPushButton( buttonGroup2, "enterButton" );
    enterButton->setGeometry( QRect( 360, 60, 50, 25 ) );

    rightButton = new QPushButton( buttonGroup2, "rightButton" );
    rightButton->setGeometry( QRect( 300, 60, 50, 25 ) );

    leftButton = new QPushButton( buttonGroup2, "leftButton" );
    leftButton->setGeometry( QRect( 240, 60, 50, 25 ) );

    upButton = new QPushButton( buttonGroup2, "upButton" );
    upButton->setGeometry( QRect( 120, 60, 50, 25 ) );

    scriptEdit = new QLineEdit( buttonGroup2, "scriptEdit" );
    scriptEdit->setGeometry( QRect( 100, 90, 220, 20 ) );

    radioButton3 = new QRadioButton( buttonGroup2, "radioButton3" );
    radioButton3->setGeometry( QRect( 10, 130, 80, 20 ) );

    radioButton2 = new QRadioButton( buttonGroup2, "radioButton2" );
    radioButton2->setGeometry( QRect( 10, 90, 70, 20 ) );

    programEdit = new QLineEdit( buttonGroup2, "programEdit" );
    programEdit->setGeometry( QRect( 100, 130, 220, 20 ) );

    textLabel3 = new QLabel( buttonGroup2, "textLabel3" );
    textLabel3->setGeometry( QRect( 20, 60, 100, 20 ) );

    scriptButton = new QPushButton( buttonGroup2, "scriptButton" );
    scriptButton->setGeometry( QRect( 340, 90, 40, 25 ) );

    programButton = new QPushButton( buttonGroup2, "programButton" );
    programButton->setGeometry( QRect( 340, 130, 40, 25 ) );

    nameEdit = new QLineEdit( this, "nameEdit" );
    nameEdit->setGeometry( QRect( 100, 170, 110, 20 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 170, 50, 20 ) );

    nameListBox = new QListBox( this, "nameListBox" );
    nameListBox->setGeometry( QRect( 10, 40, 420, 120 ) );

    addButton = new QPushButton( this, "addButton" );
    addButton->setGeometry( QRect( 10, 10, 60, 25 ) );

    deleteButton = new QPushButton( this, "deleteButton" );
    deleteButton->setGeometry( QRect( 80, 10, 60, 25 ) );

    updateButton = new QPushButton( this, "updateButton" );
    updateButton->setGeometry( QRect( 150, 10, 60, 25 ) );

    closeButton = new QPushButton( this, "closeButton" );
    closeButton->setGeometry( QRect( 370, 10, 60, 25 ) );
    languageChange();
    resize( QSize(440, 380).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( addButton, deleteButton );
    setTabOrder( deleteButton, updateButton );
    setTabOrder( updateButton, closeButton );
    setTabOrder( closeButton, nameListBox );
    setTabOrder( nameListBox, nameEdit );
    setTabOrder( nameEdit, radioButton1 );
    setTabOrder( radioButton1, radioButton2 );
    setTabOrder( radioButton2, radioButton3 );
    setTabOrder( radioButton3, keyEdit );
    setTabOrder( keyEdit, upButton );
    setTabOrder( upButton, downButton );
    setTabOrder( downButton, leftButton );
    setTabOrder( leftButton, rightButton );
    setTabOrder( rightButton, enterButton );
    setTabOrder( enterButton, scriptButton );
    setTabOrder( scriptButton, programEdit );
    setTabOrder( programEdit, programButton );
    setTabOrder( programButton, scriptEdit );
}

/*
 *  Destroys the object and frees any allocated resources
 */
keyDialogUI::~keyDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void keyDialogUI::languageChange()
{
    setCaption( tr( "Key Setup" ) );
    buttonGroup2->setTitle( tr( "Define" ) );
    radioButton1->setText( tr( "Key" ) );
    downButton->setText( tr( "Down" ) );
    enterButton->setText( tr( "Enter" ) );
    rightButton->setText( tr( "Right" ) );
    leftButton->setText( tr( "Left" ) );
    upButton->setText( tr( "Up" ) );
    radioButton3->setText( tr( "Program" ) );
    radioButton2->setText( tr( "Script" ) );
    textLabel3->setText( tr( "General Keys" ) );
    scriptButton->setText( tr( "..." ) );
    programButton->setText( tr( "..." ) );
    textLabel1->setText( tr( "Name" ) );
    addButton->setText( tr( "Add" ) );
    deleteButton->setText( tr( "Delete" ) );
    updateButton->setText( tr( "Update" ) );
    closeButton->setText( tr( "Close" ) );
}

