/****************************************************************************
** Form implementation generated from reading ui file 'prefdialog.ui'
**
** Created: 日  8月 28 14:15:15 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "prefdialogui.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
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
    setMinimumSize( QSize( 540, 460 ) );
    setMaximumSize( QSize( 540, 460 ) );

    wheelCheckBox = new QCheckBox( this, "wheelCheckBox" );
    wheelCheckBox->setGeometry( QRect( 20, 50, 240, 20 ) );

    saveCheckBox = new QCheckBox( this, "saveCheckBox" );
    saveCheckBox->setGeometry( QRect( 20, 110, 190, 22 ) );

    warnCheckBox = new QCheckBox( this, "warnCheckBox" );
    warnCheckBox->setGeometry( QRect( 20, 170, 220, 22 ) );

    urlCheckBox = new QCheckBox( this, "urlCheckBox" );
    urlCheckBox->setGeometry( QRect( 20, 80, 240, 22 ) );

    ximComboBox = new QComboBox( FALSE, this, "ximComboBox" );
    ximComboBox->setGeometry( QRect( 400, 220, 85, 22 ) );

    aacheckBox = new QCheckBox( this, "aacheckBox" );
    aacheckBox->setGeometry( QRect( 20, 199, 110, 20 ) );

    trayCheckBox = new QCheckBox( this, "trayCheckBox" );
    trayCheckBox->setGeometry( QRect( 20, 230, 220, 22 ) );

    smartCheckBox = new QCheckBox( this, "smartCheckBox" );
    smartCheckBox->setGeometry( QRect( 310, 20, 200, 20 ) );

    blinkCheckBox = new QCheckBox( this, "blinkCheckBox" );
    blinkCheckBox->setGeometry( QRect( 20, 140, 260, 22 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 290, 220, 80, 20 ) );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 17, 410, 510, 20 ) );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 30, 360, 100, 20 ) );

    httpLineEdit = new QLineEdit( this, "httpLineEdit" );
    httpLineEdit->setGeometry( QRect( 140, 360, 190, 23 ) );

    imageLineEdit = new QLineEdit( this, "imageLineEdit" );
    imageLineEdit->setGeometry( QRect( 140, 390, 190, 23 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 30, 390, 100, 20 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 30, 290, 140, 20 ) );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 20, 20, 160, 20 ) );

    wordLineEdit3 = new QLineEdit( this, "wordLineEdit3" );
    wordLineEdit3->setGeometry( QRect( 180, 20, 50, 22 ) );

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setGeometry( QRect( 240, 20, 40, 20 ) );
    TextLabel2_2->setFrameShape( QLabel::NoFrame );
    TextLabel2_2->setFrameShadow( QLabel::Plain );

    zmodemLineEdit = new QLineEdit( this, "zmodemLineEdit" );
    zmodemLineEdit->setGeometry( QRect( 170, 290, 210, 23 ) );

    poolLineEdit = new QLineEdit( this, "poolLineEdit" );
    poolLineEdit->setGeometry( QRect( 170, 320, 210, 23 ) );

    clearCheckBox = new QCheckBox( this, "clearCheckBox" );
    clearCheckBox->setGeometry( QRect( 20, 260, 370, 21 ) );

    textLabel1_3 = new QLabel( this, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 30, 320, 120, 20 ) );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setGeometry( QRect( 280, 70, 250, 130 ) );

    noneRadioButton = new QRadioButton( ButtonGroup1, "noneRadioButton" );
    noneRadioButton->setGeometry( QRect( 10, 25, 101, 20 ) );

    beepRadioButton = new QRadioButton( ButtonGroup1, "beepRadioButton" );
    beepRadioButton->setGeometry( QRect( 80, 25, 101, 20 ) );
    ButtonGroup1->insert( beepRadioButton, 1 );

    fileRadioButton = new QRadioButton( ButtonGroup1, "fileRadioButton" );
    fileRadioButton->setGeometry( QRect( 10, 65, 101, 20 ) );
    ButtonGroup1->insert( fileRadioButton, 2 );

    selectsoundPushButton = new QPushButton( ButtonGroup1, "selectsoundPushButton" );
    selectsoundPushButton->setGeometry( QRect( 120, 65, 80, 21 ) );

    wavefileLineEdit = new QLineEdit( ButtonGroup1, "wavefileLineEdit" );
    wavefileLineEdit->setGeometry( QRect( 10, 95, 180, 22 ) );

    cancelPushButton = new QPushButton( this, "cancelPushButton" );
    cancelPushButton->setGeometry( QRect( 270, 430, 70, 21 ) );

    okPushButton = new QPushButton( this, "okPushButton" );
    okPushButton->setGeometry( QRect( 150, 430, 70, 21 ) );

    browsePushButton = new QPushButton( this, "browsePushButton" );
    browsePushButton->setGeometry( QRect( 390, 290, 90, 25 ) );

    poolPushButton = new QPushButton( this, "poolPushButton" );
    poolPushButton->setGeometry( QRect( 390, 320, 90, 25 ) );

    imagePushButton = new QPushButton( this, "imagePushButton" );
    imagePushButton->setGeometry( QRect( 340, 390, 80, 25 ) );

    choosehttpPushButton = new QPushButton( this, "choosehttpPushButton" );
    choosehttpPushButton->setGeometry( QRect( 340, 360, 80, 25 ) );
    languageChange();
    resize( QSize(540, 460).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( wordLineEdit3, smartCheckBox );
    setTabOrder( smartCheckBox, wheelCheckBox );
    setTabOrder( wheelCheckBox, urlCheckBox );
    setTabOrder( urlCheckBox, saveCheckBox );
    setTabOrder( saveCheckBox, blinkCheckBox );
    setTabOrder( blinkCheckBox, warnCheckBox );
    setTabOrder( warnCheckBox, aacheckBox );
    setTabOrder( aacheckBox, trayCheckBox );
    setTabOrder( trayCheckBox, clearCheckBox );
    setTabOrder( clearCheckBox, noneRadioButton );
    setTabOrder( noneRadioButton, beepRadioButton );
    setTabOrder( beepRadioButton, fileRadioButton );
    setTabOrder( fileRadioButton, selectsoundPushButton );
    setTabOrder( selectsoundPushButton, wavefileLineEdit );
    setTabOrder( wavefileLineEdit, ximComboBox );
    setTabOrder( ximComboBox, zmodemLineEdit );
    setTabOrder( zmodemLineEdit, browsePushButton );
    setTabOrder( browsePushButton, poolLineEdit );
    setTabOrder( poolLineEdit, poolPushButton );
    setTabOrder( poolPushButton, httpLineEdit );
    setTabOrder( httpLineEdit, choosehttpPushButton );
    setTabOrder( choosehttpPushButton, imageLineEdit );
    setTabOrder( imageLineEdit, imagePushButton );
    setTabOrder( imagePushButton, okPushButton );
    setTabOrder( okPushButton, cancelPushButton );
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
    wheelCheckBox->setText( tr( "Alternative Mouse Wheel Control" ) );
    saveCheckBox->setText( tr( "Save Message To File" ) );
    warnCheckBox->setText( tr( "Warn When Exit Connected" ) );
    urlCheckBox->setText( tr( "Detect URL When Mouse Move" ) );
    ximComboBox->clear();
    ximComboBox->insertItem( tr( "GBK" ) );
    ximComboBox->insertItem( tr( "BIG5" ) );
    aacheckBox->setText( tr( "AntiAlias" ) );
    trayCheckBox->setText( tr( "Show as tray when close" ) );
    smartCheckBox->setText( tr( "Enable Smart Word-Wrap" ) );
    blinkCheckBox->setText( tr( "Blink Tab When Receive Message" ) );
    TextLabel1->setText( tr( "Input Method" ) );
    TextLabel3->setText( tr( "HTTP Browser" ) );
    textLabel1_2->setText( tr( "Image Viewer" ) );
    textLabel1->setText( tr( "Zmodem directory" ) );
    TextLabel1_2->setText( tr( "Paste word-wrap at" ) );
    TextLabel2_2->setText( tr( "chars" ) );
    clearCheckBox->setText( tr( "Clear ZModem and HTTP directory when exit" ) );
    textLabel1_3->setText( tr( "HTTP directory" ) );
    ButtonGroup1->setTitle( tr( "Sound When Receive Messages" ) );
    noneRadioButton->setText( tr( "None" ) );
    beepRadioButton->setText( tr( "Beep" ) );
    fileRadioButton->setText( tr( "Sound File" ) );
    selectsoundPushButton->setText( tr( "Configure" ) );
    cancelPushButton->setText( tr( "Cancel" ) );
    okPushButton->setText( tr( "OK" ) );
    browsePushButton->setText( tr( "Browse" ) );
    poolPushButton->setText( tr( "Browse" ) );
    imagePushButton->setText( tr( "Choose" ) );
    choosehttpPushButton->setText( tr( "Choose" ) );
}

