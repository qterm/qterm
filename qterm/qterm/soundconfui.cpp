/****************************************************************************
** Form implementation generated from reading ui file 'fconfigure.ui'
**
** Created: 三  9月 24 09:45:00 2003
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "soundconfui.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a fSoundConfUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
fSoundConfUI::fSoundConfUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "fSoundConfUI" );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setGeometry( QRect( 11, 11, 430, 60 ) );

    leFile = new QLineEdit( groupBox2, "leFile" );
    leFile->setGeometry( QRect( 10, 20, 260, 25 ) );

    bfSelect = new QPushButton( groupBox2, "bfSelect" );
    bfSelect->setGeometry( QRect( 310, 20, 110, 25 ) );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    groupBox3->setGeometry( QRect( 10, 190, 431, 70 ) );

    bpSelect = new QPushButton( groupBox3, "bpSelect" );
    bpSelect->setGeometry( QRect( 310, 30, 110, 25 ) );

    leProg = new QLineEdit( groupBox3, "leProg" );
    leProg->setGeometry( QRect( 10, 30, 261, 25 ) );

    bgMethod = new QButtonGroup( this, "bgMethod" );
    bgMethod->setGeometry( QRect( 10, 90, 431, 90 ) );

    radioButton16 = new QRadioButton( bgMethod, "radioButton16" );
    radioButton16->setGeometry( QRect( 322, 41, 97, 19 ) );
    bgMethod->insert( radioButton16, 3 );

    radioButton14 = new QRadioButton( bgMethod, "radioButton14" );
    radioButton14->setGeometry( QRect( 115, 41, 97, 19 ) );
    bgMethod->insert( radioButton14, 1 );

    radioButton15 = new QRadioButton( bgMethod, "radioButton15" );
    radioButton15->setGeometry( QRect( 218, 41, 98, 19 ) );
    bgMethod->insert( radioButton15, 2 );

    radioButton13 = new QRadioButton( bgMethod, "radioButton13" );
    radioButton13->setGeometry( QRect( 11, 41, 98, 19 ) );
    radioButton13->setChecked( TRUE );
    bgMethod->insert( radioButton13, 0 );

    bOK = new QPushButton( this, "bOK" );
    bOK->setGeometry( QRect( 170, 280, 111, 25 ) );

    bCancel = new QPushButton( this, "bCancel" );
    bCancel->setGeometry( QRect( 320, 280, 111, 25 ) );
    languageChange();
    resize( QSize(453, 332).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( bOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( bCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( bfSelect, SIGNAL( clicked() ), this, SLOT( onSelectFile() ) );
    connect( bpSelect, SIGNAL( clicked() ), this, SLOT( onSelectProg() ) );
    connect( bgMethod, SIGNAL( clicked(int) ), this, SLOT( onPlayMethod(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
fSoundConfUI::~fSoundConfUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void fSoundConfUI::languageChange()
{
    setCaption( tr( "Configure" ) );
    groupBox2->setTitle( tr( "Sound File:" ) );
    bfSelect->setText( tr( "Select..." ) );
    groupBox3->setTitle( tr( "External Program:" ) );
    bpSelect->setText( tr( "Select..." ) );
    bgMethod->setTitle( tr( "Play With:" ) );
    radioButton16->setText( tr( "External" ) );
    radioButton14->setText( tr( "ARTS" ) );
    radioButton15->setText( tr( "ESD" ) );
    radioButton13->setText( tr( "Internal" ) );
    bOK->setText( tr( "OK" ) );
    bCancel->setText( tr( "Cancel" ) );
}

