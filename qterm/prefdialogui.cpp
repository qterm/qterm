/****************************************************************************
** Form implementation generated from reading ui file 'prefdialog.ui'
**
** Created: Mon Jan 20 17:29:27 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "prefdialogui.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a prefDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
prefDialogUI::prefDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "prefDialogUI" );
    setMinimumSize( QSize( 520, 320 ) );
    setMaximumSize( QSize( 520, 320 ) );
    QFont f( font() );
    setFont( f ); 

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setGeometry( QRect( 270, 20, 40, 20 ) );
    TextLabel2_2->setFrameShape( QLabel::NoFrame );
    TextLabel2_2->setFrameShadow( QLabel::Plain );

    wordLineEdit3 = new QLineEdit( this, "wordLineEdit3" );
    wordLineEdit3->setGeometry( QRect( 210, 20, 50, 22 ) );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 20, 20, 180, 20 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 290, 180, 80, 20 ) );

    ximComboBox = new QComboBox( FALSE, this, "ximComboBox" );
    ximComboBox->setGeometry( QRect( 400, 180, 85, 22 ) );

    wheelCheckBox = new QCheckBox( this, "wheelCheckBox" );
    wheelCheckBox->setGeometry( QRect( 20, 50, 240, 20 ) );

    urlCheckBox = new QCheckBox( this, "urlCheckBox" );
    urlCheckBox->setGeometry( QRect( 20, 80, 240, 22 ) );

    saveCheckBox = new QCheckBox( this, "saveCheckBox" );
    saveCheckBox->setGeometry( QRect( 20, 110, 190, 22 ) );

    blinkCheckBox = new QCheckBox( this, "blinkCheckBox" );
    blinkCheckBox->setGeometry( QRect( 20, 140, 250, 22 ) );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setGeometry( QRect( 290, 50, 210, 120 ) );

    noneRadioButton = new QRadioButton( ButtonGroup1, "noneRadioButton" );
    noneRadioButton->setGeometry( QRect( 10, 20, 101, 20 ) );

    beepRadioButton = new QRadioButton( ButtonGroup1, "beepRadioButton" );
    beepRadioButton->setGeometry( QRect( 80, 20, 101, 20 ) );

    fileRadioButton = new QRadioButton( ButtonGroup1, "fileRadioButton" );
    fileRadioButton->setGeometry( QRect( 10, 50, 101, 20 ) );

    selectsoundPushButton = new QPushButton( ButtonGroup1, "selectsoundPushButton" );
    selectsoundPushButton->setGeometry( QRect( 120, 50, 70, 21 ) );

    wavefileLineEdit = new QLineEdit( ButtonGroup1, "wavefileLineEdit" );
    wavefileLineEdit->setGeometry( QRect( 10, 80, 180, 22 ) );

    smartCheckBox = new QCheckBox( this, "smartCheckBox" );
    smartCheckBox->setGeometry( QRect( 310, 20, 190, 20 ) );

    okPushButton = new QPushButton( this, "okPushButton" );
    okPushButton->setGeometry( QRect( 150, 290, 70, 21 ) );

    cancelPushButton = new QPushButton( this, "cancelPushButton" );
    cancelPushButton->setGeometry( QRect( 270, 290, 70, 21 ) );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 17, 270, 480, 20 ) );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );

    choosehttpPushButton = new QPushButton( this, "choosehttpPushButton" );
    choosehttpPushButton->setGeometry( QRect( 330, 240, 80, 21 ) );
    QFont choosehttpPushButton_font(  choosehttpPushButton->font() );
    choosehttpPushButton->setFont( choosehttpPushButton_font ); 

    httpLineEdit = new QLineEdit( this, "httpLineEdit" );
    httpLineEdit->setGeometry( QRect( 120, 240, 190, 23 ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 20, 240, 86, 20 ) );

    aacheckBox = new QCheckBox( this, "aacheckBox" );
    aacheckBox->setGeometry( QRect( 20, 199, 110, 20 ) );

    warnCheckBox = new QCheckBox( this, "warnCheckBox" );
    warnCheckBox->setGeometry( QRect( 20, 170, 220, 22 ) );
    languageChange();
    resize( QSize(520, 320).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( wordLineEdit3, smartCheckBox );
    setTabOrder( smartCheckBox, wheelCheckBox );
    setTabOrder( wheelCheckBox, urlCheckBox );
    setTabOrder( urlCheckBox, saveCheckBox );
    setTabOrder( saveCheckBox, blinkCheckBox );
    setTabOrder( blinkCheckBox, noneRadioButton );
    setTabOrder( noneRadioButton, beepRadioButton );
    setTabOrder( beepRadioButton, fileRadioButton );
    setTabOrder( fileRadioButton, selectsoundPushButton );
    setTabOrder( selectsoundPushButton, wavefileLineEdit );
    setTabOrder( wavefileLineEdit, ximComboBox );
    setTabOrder( ximComboBox, httpLineEdit );
    setTabOrder( httpLineEdit, choosehttpPushButton );
    setTabOrder( choosehttpPushButton, okPushButton );
    setTabOrder( okPushButton, cancelPushButton );
    setTabOrder( cancelPushButton, warnCheckBox );
}

/*
 *  Destroys the object and frees any allocated resources
 */
prefDialogUI::~prefDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void prefDialogUI::languageChange()
{
    setCaption( tr( "Preference" ) );
    TextLabel2_2->setText( tr( "chars" ) );
    TextLabel1_2->setText( tr( "Paste auto word-wrap at about" ) );
    TextLabel1->setText( tr( "Input Method" ) );
    ximComboBox->clear();
    ximComboBox->insertItem( tr( "GBK" ) );
    ximComboBox->insertItem( tr( "BIG5" ) );
    wheelCheckBox->setText( tr( "Alternative Mouse Wheel Control" ) );
    urlCheckBox->setText( tr( "Detect URL When Mouse Move" ) );
    saveCheckBox->setText( tr( "Save Message To File" ) );
    blinkCheckBox->setText( tr( "Blink Tab When Receive Message" ) );
    ButtonGroup1->setTitle( tr( "Sound When Receive Messages" ) );
    noneRadioButton->setText( tr( "None" ) );
    beepRadioButton->setText( tr( "Beep" ) );
    fileRadioButton->setText( tr( "Sound File" ) );
    selectsoundPushButton->setText( tr( "Choose" ) );
    smartCheckBox->setText( tr( "Enable Smart Word-Wrap" ) );
    okPushButton->setText( tr( "OK" ) );
    cancelPushButton->setText( tr( "Cancel" ) );
    choosehttpPushButton->setText( tr( "Choose" ) );
    TextLabel3->setText( tr( "HTTP Browser" ) );
    aacheckBox->setText( tr( "AntiAlias" ) );
    warnCheckBox->setText( tr( "Warn When Exit Connected" ) );
}

