/****************************************************************************
** Form implementation generated from reading ui file 'schemadialog.ui'
**
** Created: Thu Nov 6 15:52:10 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "schemadialogui.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a schemaDialogUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
schemaDialogUI::schemaDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "schemaDialogUI" );
    setMinimumSize( QSize( 580, 440 ) );
    setMaximumSize( QSize( 580, 440 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 20, 20, 40, 20 ) );

    titleLineEdit = new QLineEdit( this, "titleLineEdit" );
    titleLineEdit->setGeometry( QRect( 70, 20, 230, 23 ) );

    nameListBox = new QListBox( this, "nameListBox" );
    nameListBox->setGeometry( QRect( 70, 120, 230, 250 ) );

    clr0Button = new QPushButton( this, "clr0Button" );
    clr0Button->setGeometry( QRect( 60, 60, 30, 28 ) );

    clr1Button = new QPushButton( this, "clr1Button" );
    clr1Button->setGeometry( QRect( 90, 60, 30, 28 ) );

    clr2Button = new QPushButton( this, "clr2Button" );
    clr2Button->setGeometry( QRect( 120, 60, 30, 28 ) );

    clr3Button = new QPushButton( this, "clr3Button" );
    clr3Button->setGeometry( QRect( 150, 60, 30, 28 ) );

    clr4Button = new QPushButton( this, "clr4Button" );
    clr4Button->setGeometry( QRect( 180, 60, 30, 28 ) );

    clr5Button = new QPushButton( this, "clr5Button" );
    clr5Button->setGeometry( QRect( 210, 60, 30, 28 ) );

    clr6Button = new QPushButton( this, "clr6Button" );
    clr6Button->setGeometry( QRect( 240, 60, 30, 28 ) );

    clr7Button = new QPushButton( this, "clr7Button" );
    clr7Button->setGeometry( QRect( 270, 60, 30, 28 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 20, 70, 30, 20 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 20, 90, 30, 20 ) );
    textLabel2->setFrameShape( QLabel::NoFrame );
    textLabel2->setFrameShadow( QLabel::Plain );

    clr8Button = new QPushButton( this, "clr8Button" );
    clr8Button->setGeometry( QRect( 60, 90, 30, 28 ) );

    clr9Button = new QPushButton( this, "clr9Button" );
    clr9Button->setGeometry( QRect( 90, 90, 30, 28 ) );

    clr10Button = new QPushButton( this, "clr10Button" );
    clr10Button->setGeometry( QRect( 120, 90, 30, 28 ) );

    clr11Button = new QPushButton( this, "clr11Button" );
    clr11Button->setGeometry( QRect( 150, 90, 30, 28 ) );

    clr12Button = new QPushButton( this, "clr12Button" );
    clr12Button->setGeometry( QRect( 180, 90, 30, 28 ) );

    clr13Button = new QPushButton( this, "clr13Button" );
    clr13Button->setGeometry( QRect( 210, 90, 30, 28 ) );

    clr14Button = new QPushButton( this, "clr14Button" );
    clr14Button->setGeometry( QRect( 240, 90, 30, 28 ) );

    clr15Button = new QPushButton( this, "clr15Button" );
    clr15Button->setGeometry( QRect( 270, 90, 30, 28 ) );

    bgButtonGroup = new QButtonGroup( this, "bgButtonGroup" );
    bgButtonGroup->setGeometry( QRect( 310, 20, 250, 350 ) );

    TextLabel3 = new QLabel( bgButtonGroup, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 180, 320, 40, 20 ) );

    chooseButton = new QPushButton( bgButtonGroup, "chooseButton" );
    chooseButton->setGeometry( QRect( 180, 70, 60, 21 ) );

    TextLabel2 = new QLabel( bgButtonGroup, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 320, 40, 20 ) );

    imageLineEdit = new QLineEdit( bgButtonGroup, "imageLineEdit" );
    imageLineEdit->setGeometry( QRect( 30, 70, 140, 23 ) );

    imageRadioButton = new QRadioButton( bgButtonGroup, "imageRadioButton" );
    imageRadioButton->setGeometry( QRect( 10, 40, 101, 20 ) );

    noneRadioButton = new QRadioButton( bgButtonGroup, "noneRadioButton" );
    noneRadioButton->setGeometry( QRect( 10, 20, 70, 20 ) );

    transpRadioButton = new QRadioButton( bgButtonGroup, "transpRadioButton" );
    transpRadioButton->setGeometry( QRect( 10, 100, 101, 20 ) );

    TextLabel1 = new QLabel( bgButtonGroup, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 290, 66, 20 ) );

    fadeButton = new QPushButton( bgButtonGroup, "fadeButton" );
    fadeButton->setGeometry( QRect( 90, 290, 50, 20 ) );

    alphaSlider = new QSlider( bgButtonGroup, "alphaSlider" );
    alphaSlider->setGeometry( QRect( 60, 320, 110, 16 ) );
    alphaSlider->setOrientation( QSlider::Horizontal );

    imagePixmapLabel = new QLabel( bgButtonGroup, "imagePixmapLabel" );
    imagePixmapLabel->setGeometry( QRect( 20, 120, 170, 160 ) );
    imagePixmapLabel->setScaledContents( TRUE );

    typeComboBox = new QComboBox( FALSE, bgButtonGroup, "typeComboBox" );
    typeComboBox->setGeometry( QRect( 110, 40, 100, 23 ) );

    removeButton = new QPushButton( this, "removeButton" );
    removeButton->setGeometry( QRect( 180, 390, 100, 21 ) );

    saveButton = new QPushButton( this, "saveButton" );
    saveButton->setGeometry( QRect( 60, 390, 100, 21 ) );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 350, 400, 70, 21 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 460, 400, 70, 21 ) );
    languageChange();
    resize( QSize(580, 440).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
schemaDialogUI::~schemaDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void schemaDialogUI::languageChange()
{
    setCaption( tr( "Schema Editor" ) );
    textLabel1_2->setText( tr( "Title" ) );
    clr0Button->setText( QString::null );
    clr1Button->setText( QString::null );
    clr2Button->setText( QString::null );
    clr3Button->setText( QString::null );
    clr4Button->setText( QString::null );
    clr5Button->setText( QString::null );
    clr6Button->setText( QString::null );
    clr7Button->setText( QString::null );
    textLabel1->setText( tr( "0-7" ) );
    textLabel2->setText( tr( "8-15" ) );
    clr8Button->setText( QString::null );
    clr9Button->setText( QString::null );
    clr10Button->setText( QString::null );
    clr11Button->setText( QString::null );
    clr12Button->setText( QString::null );
    clr13Button->setText( QString::null );
    clr14Button->setText( QString::null );
    clr15Button->setText( QString::null );
    bgButtonGroup->setTitle( tr( "Background" ) );
    TextLabel3->setText( tr( "Max" ) );
    chooseButton->setText( tr( "Choose" ) );
    TextLabel2->setText( tr( "Min" ) );
    imageRadioButton->setText( tr( "Image" ) );
    noneRadioButton->setText( tr( "None" ) );
    transpRadioButton->setText( tr( "Transparent" ) );
    TextLabel1->setText( tr( "Fade to" ) );
    fadeButton->setText( QString::null );
    imagePixmapLabel->setText( QString::null );
    typeComboBox->clear();
    typeComboBox->insertItem( tr( "Tile" ) );
    typeComboBox->insertItem( tr( "Center" ) );
    typeComboBox->insertItem( tr( "Scale" ) );
    removeButton->setText( tr( "Remove " ) );
    saveButton->setText( tr( "Save" ) );
    okButton->setText( tr( "OK" ) );
    cancelButton->setText( tr( "Cancel" ) );
}

void schemaDialogUI::Frame4_destroyed(QObject*)
{
    qWarning( "schemaDialogUI::Frame4_destroyed(QObject*): Not implemented yet" );
}
#ifdef HAVE_CONFIG_H
#include "schemadialogui.moc"
#endif
