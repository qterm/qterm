/****************************************************************************
** Form implementation generated from reading ui file 'prefdialog.ui'
**
** Created: Fri Jun 20 16:28:47 2003
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
    setMinimumSize( QSize( 520, 350 ) );
    setMaximumSize( QSize( 520, 350 ) );
    QFont f( font() );
    setFont( f ); 

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setGeometry( QRect( 270, 20, 40, 20 ) );
    TextLabel2_2->setFrameShape( QLabel::NoFrame );
    TextLabel2_2->setFrameShadow( QLabel::Plain );

    wheelCheckBox = new QCheckBox( this, "wheelCheckBox" );
    wheelCheckBox->setGeometry( QRect( 20, 50, 240, 20 ) );

    saveCheckBox = new QCheckBox( this, "saveCheckBox" );
    saveCheckBox->setGeometry( QRect( 20, 110, 190, 22 ) );

    warnCheckBox = new QCheckBox( this, "warnCheckBox" );
    warnCheckBox->setGeometry( QRect( 20, 170, 220, 22 ) );

    urlCheckBox = new QCheckBox( this, "urlCheckBox" );
    urlCheckBox->setGeometry( QRect( 20, 80, 240, 22 ) );

    wordLineEdit3 = new QLineEdit( this, "wordLineEdit3" );
    wordLineEdit3->setGeometry( QRect( 210, 20, 50, 22 ) );

    smartCheckBox = new QCheckBox( this, "smartCheckBox" );
    smartCheckBox->setGeometry( QRect( 310, 20, 190, 20 ) );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 20, 20, 170, 20 ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 20, 270, 86, 20 ) );

    okPushButton = new QPushButton( this, "okPushButton" );
    okPushButton->setGeometry( QRect( 150, 320, 70, 21 ) );

    httpLineEdit = new QLineEdit( this, "httpLineEdit" );
    httpLineEdit->setGeometry( QRect( 120, 270, 190, 23 ) );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 17, 300, 480, 20 ) );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );

    cancelPushButton = new QPushButton( this, "cancelPushButton" );
    cancelPushButton->setGeometry( QRect( 270, 320, 70, 21 ) );

    choosehttpPushButton = new QPushButton( this, "choosehttpPushButton" );
    choosehttpPushButton->setGeometry( QRect( 330, 270, 80, 21 ) );
    QFont choosehttpPushButton_font(  choosehttpPushButton->font() );
    choosehttpPushButton->setFont( choosehttpPushButton_font ); 

    ximComboBox = new QComboBox( FALSE, this, "ximComboBox" );
    ximComboBox->setGeometry( QRect( 400, 220, 85, 22 ) );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setGeometry( QRect( 270, 70, 230, 130 ) );

    noneRadioButton = new QRadioButton( ButtonGroup1, "noneRadioButton" );
    noneRadioButton->setGeometry( QRect( 10, 20, 101, 20 ) );

    beepRadioButton = new QRadioButton( ButtonGroup1, "beepRadioButton" );
    beepRadioButton->setGeometry( QRect( 80, 20, 101, 20 ) );

    wavefileLineEdit = new QLineEdit( ButtonGroup1, "wavefileLineEdit" );
    wavefileLineEdit->setGeometry( QRect( 10, 90, 180, 22 ) );

    fileRadioButton = new QRadioButton( ButtonGroup1, "fileRadioButton" );
    fileRadioButton->setGeometry( QRect( 10, 60, 101, 20 ) );

    selectsoundPushButton = new QPushButton( ButtonGroup1, "selectsoundPushButton" );
    selectsoundPushButton->setGeometry( QRect( 120, 60, 70, 21 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 270, 220, 80, 20 ) );

    aacheckBox = new QCheckBox( this, "aacheckBox" );
    aacheckBox->setGeometry( QRect( 20, 199, 110, 20 ) );

    blinkCheckBox = new QCheckBox( this, "blinkCheckBox" );
    blinkCheckBox->setGeometry( QRect( 20, 140, 250, 22 ) );

    trayCheckBox = new QCheckBox( this, "trayCheckBox" );
    trayCheckBox->setGeometry( QRect( 20, 230, 220, 22 ) );
    languageChange();
    resize( QSize(520, 350).expandedTo(minimumSizeHint()) );

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
    wheelCheckBox->setText( tr( "Alternative Mouse Wheel Control" ) );
    saveCheckBox->setText( tr( "Save Message To File" ) );
    warnCheckBox->setText( tr( "Warn When Exit Connected" ) );
    urlCheckBox->setText( tr( "Detect URL When Mouse Move" ) );
    smartCheckBox->setText( tr( "Enable Smart Word-Wrap" ) );
    TextLabel1_2->setText( tr( "Paste auto word-wrap at" ) );
    TextLabel3->setText( tr( "HTTP Browser" ) );
    okPushButton->setText( tr( "OK" ) );
    cancelPushButton->setText( tr( "Cancel" ) );
    choosehttpPushButton->setText( tr( "Choose" ) );
    ximComboBox->clear();
    ximComboBox->insertItem( tr( "GBK" ) );
    ximComboBox->insertItem( tr( "BIG5" ) );
    ButtonGroup1->setTitle( tr( "Sound When Receive Messages" ) );
    noneRadioButton->setText( tr( "None" ) );
    beepRadioButton->setText( tr( "Beep" ) );
    fileRadioButton->setText( tr( "Sound File" ) );
    selectsoundPushButton->setText( tr( "Configure" ) );
    TextLabel1->setText( tr( "Input Method" ) );
    aacheckBox->setText( tr( "AntiAlias" ) );
    blinkCheckBox->setText( tr( "Blink Tab When Receive Message" ) );
    trayCheckBox->setText( tr( "Show as tray when close" ) );
}

