/****************************************************************************
** Form implementation generated from reading ui file 'schemadialog.ui'
**
** Created: Wed Mar 26 18:20:04 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "schemadialogui.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
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
    setMinimumSize( QSize( 580, 390 ) );
    setMaximumSize( QSize( 580, 390 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 20, 50, 30, 20 ) );
    textLabel2->setFrameShape( QLabel::NoFrame );
    textLabel2->setFrameShadow( QLabel::Plain );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 20, 30, 30, 20 ) );

    fg0Frame = new QFrame( this, "fg0Frame" );
    fg0Frame->setGeometry( QRect( 60, 30, 30, 20 ) );
    fg0Frame->setFrameShape( QFrame::StyledPanel );
    fg0Frame->setFrameShadow( QFrame::Raised );

    fg1Frame = new QFrame( this, "fg1Frame" );
    fg1Frame->setGeometry( QRect( 90, 30, 30, 20 ) );
    fg1Frame->setFrameShape( QFrame::StyledPanel );
    fg1Frame->setFrameShadow( QFrame::Raised );

    fg2Frame = new QFrame( this, "fg2Frame" );
    fg2Frame->setGeometry( QRect( 120, 30, 30, 20 ) );
    fg2Frame->setFrameShape( QFrame::StyledPanel );
    fg2Frame->setFrameShadow( QFrame::Raised );

    fg3Frame = new QFrame( this, "fg3Frame" );
    fg3Frame->setGeometry( QRect( 150, 30, 30, 20 ) );
    fg3Frame->setFrameShape( QFrame::StyledPanel );
    fg3Frame->setFrameShadow( QFrame::Raised );

    fg4Frame = new QFrame( this, "fg4Frame" );
    fg4Frame->setGeometry( QRect( 180, 30, 30, 20 ) );
    fg4Frame->setFrameShape( QFrame::StyledPanel );
    fg4Frame->setFrameShadow( QFrame::Raised );

    fg5Frame = new QFrame( this, "fg5Frame" );
    fg5Frame->setGeometry( QRect( 210, 30, 30, 20 ) );
    fg5Frame->setFrameShape( QFrame::StyledPanel );
    fg5Frame->setFrameShadow( QFrame::Raised );

    fg6Frame = new QFrame( this, "fg6Frame" );
    fg6Frame->setGeometry( QRect( 240, 30, 30, 20 ) );
    fg6Frame->setFrameShape( QFrame::StyledPanel );
    fg6Frame->setFrameShadow( QFrame::Raised );

    fg7Frame = new QFrame( this, "fg7Frame" );
    fg7Frame->setGeometry( QRect( 270, 30, 30, 20 ) );
    fg7Frame->setFrameShape( QFrame::StyledPanel );
    fg7Frame->setFrameShadow( QFrame::Raised );

    bg0Frame = new QFrame( this, "bg0Frame" );
    bg0Frame->setGeometry( QRect( 60, 50, 30, 20 ) );
    bg0Frame->setFrameShape( QFrame::StyledPanel );
    bg0Frame->setFrameShadow( QFrame::Raised );

    bg1Frame = new QFrame( this, "bg1Frame" );
    bg1Frame->setGeometry( QRect( 90, 50, 30, 20 ) );
    bg1Frame->setFrameShape( QFrame::StyledPanel );
    bg1Frame->setFrameShadow( QFrame::Raised );

    bg2Frame = new QFrame( this, "bg2Frame" );
    bg2Frame->setGeometry( QRect( 120, 50, 30, 20 ) );
    bg2Frame->setFrameShape( QFrame::StyledPanel );
    bg2Frame->setFrameShadow( QFrame::Raised );

    bg3Frame = new QFrame( this, "bg3Frame" );
    bg3Frame->setGeometry( QRect( 150, 50, 30, 20 ) );
    bg3Frame->setFrameShape( QFrame::StyledPanel );
    bg3Frame->setFrameShadow( QFrame::Raised );

    bg4Frame = new QFrame( this, "bg4Frame" );
    bg4Frame->setGeometry( QRect( 180, 50, 30, 20 ) );
    bg4Frame->setFrameShape( QFrame::StyledPanel );
    bg4Frame->setFrameShadow( QFrame::Raised );

    bg5Frame = new QFrame( this, "bg5Frame" );
    bg5Frame->setGeometry( QRect( 210, 50, 30, 20 ) );
    bg5Frame->setFrameShape( QFrame::StyledPanel );
    bg5Frame->setFrameShadow( QFrame::Raised );

    bg6Frame = new QFrame( this, "bg6Frame" );
    bg6Frame->setGeometry( QRect( 240, 50, 30, 20 ) );
    bg6Frame->setFrameShape( QFrame::StyledPanel );
    bg6Frame->setFrameShadow( QFrame::Raised );

    bg7Frame = new QFrame( this, "bg7Frame" );
    bg7Frame->setGeometry( QRect( 270, 50, 30, 20 ) );
    bg7Frame->setFrameShape( QFrame::StyledPanel );
    bg7Frame->setFrameShadow( QFrame::Raised );

    saveButton = new QPushButton( this, "saveButton" );
    saveButton->setGeometry( QRect( 190, 110, 100, 21 ) );

    removeButton = new QPushButton( this, "removeButton" );
    removeButton->setGeometry( QRect( 190, 150, 100, 21 ) );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 210, 300, 70, 21 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 210, 330, 70, 21 ) );

    bgButtonGroup = new QButtonGroup( this, "bgButtonGroup" );
    bgButtonGroup->setGeometry( QRect( 310, 20, 250, 350 ) );

    TextLabel3 = new QLabel( bgButtonGroup, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 180, 320, 40, 20 ) );

    noneRadioButton = new QRadioButton( bgButtonGroup, "noneRadioButton" );
    noneRadioButton->setGeometry( QRect( 10, 20, 70, 20 ) );

    imageRadioButton = new QRadioButton( bgButtonGroup, "imageRadioButton" );
    imageRadioButton->setGeometry( QRect( 10, 40, 101, 20 ) );

    typeComboBox = new QComboBox( FALSE, bgButtonGroup, "typeComboBox" );
    typeComboBox->setGeometry( QRect( 100, 40, 70, 23 ) );

    imageLineEdit = new QLineEdit( bgButtonGroup, "imageLineEdit" );
    imageLineEdit->setGeometry( QRect( 30, 70, 140, 23 ) );

    chooseButton = new QPushButton( bgButtonGroup, "chooseButton" );
    chooseButton->setGeometry( QRect( 180, 70, 60, 21 ) );

    transpRadioButton = new QRadioButton( bgButtonGroup, "transpRadioButton" );
    transpRadioButton->setGeometry( QRect( 10, 100, 101, 20 ) );

    TextLabel1 = new QLabel( bgButtonGroup, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 290, 66, 20 ) );

    shadeFrame = new QFrame( bgButtonGroup, "shadeFrame" );
    shadeFrame->setGeometry( QRect( 90, 290, 40, 20 ) );
    shadeFrame->setFrameShape( QFrame::StyledPanel );
    shadeFrame->setFrameShadow( QFrame::Raised );

    TextLabel2 = new QLabel( bgButtonGroup, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 320, 40, 20 ) );

    alphaSlider = new QSlider( bgButtonGroup, "alphaSlider" );
    alphaSlider->setGeometry( QRect( 60, 320, 100, 16 ) );
    alphaSlider->setOrientation( QSlider::Horizontal );

    imagePixmapLabel = new QLabel( bgButtonGroup, "imagePixmapLabel" );
    imagePixmapLabel->setGeometry( QRect( 20, 120, 170, 160 ) );
    imagePixmapLabel->setScaledContents( TRUE );

    nameListBox = new QListBox( this, "nameListBox" );
    nameListBox->setGeometry( QRect( 20, 80, 160, 280 ) );
    languageChange();
    resize( QSize(580, 390).expandedTo(minimumSizeHint()) );
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
    textLabel2->setText( tr( "BG" ) );
    textLabel1->setText( tr( "FG" ) );
    saveButton->setText( tr( "Save As" ) );
    removeButton->setText( tr( "Remove " ) );
    okButton->setText( tr( "OK" ) );
    cancelButton->setText( tr( "Cancel" ) );
    bgButtonGroup->setTitle( tr( "Background" ) );
    TextLabel3->setText( tr( "Max" ) );
    noneRadioButton->setText( tr( "None" ) );
    imageRadioButton->setText( tr( "Image" ) );
    typeComboBox->clear();
    typeComboBox->insertItem( tr( "Center" ) );
    typeComboBox->insertItem( tr( "Tile" ) );
    typeComboBox->insertItem( tr( "Scale" ) );
    chooseButton->setText( tr( "Choose" ) );
    transpRadioButton->setText( tr( "Transparent" ) );
    TextLabel1->setText( tr( "Shade to" ) );
    TextLabel2->setText( tr( "Min" ) );
    imagePixmapLabel->setText( QString::null );
}

void schemaDialogUI::Frame4_destroyed(QObject*)
{
    qWarning( "schemaDialogUI::Frame4_destroyed(QObject*): Not implemented yet" );
}

