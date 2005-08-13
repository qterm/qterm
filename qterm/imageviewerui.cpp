/****************************************************************************
** Form implementation generated from reading ui file 'imageviewerui.ui'
**
** Created: 五  8月 12 14:26:49 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "imageviewerui.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a ImageViewerUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ImageViewerUI::ImageViewerUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ImageViewerUI" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setBaseSize( QSize( 0, 0 ) );
    setSizeGripEnabled( FALSE );
    ImageViewerUILayout = new QVBoxLayout( this, 11, 6, "ImageViewerUILayout"); 

    imageList = new QListView( this, "imageList" );
    imageList->addColumn( tr( "ThumbView" ) );
    imageList->header()->setClickEnabled( FALSE, imageList->header()->count() - 1 );
    imageList->addColumn( tr( "FileName" ) );
    imageList->header()->setClickEnabled( FALSE, imageList->header()->count() - 1 );
    imageList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, imageList->sizePolicy().hasHeightForWidth() ) );
    ImageViewerUILayout->addWidget( imageList );

    layout13 = new QHBoxLayout( 0, 0, 6, "layout13"); 
    spacer3 = new QSpacerItem( 90, 30, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout13->addItem( spacer3 );

    buttonClose = new QPushButton( this, "buttonClose" );
    layout13->addWidget( buttonClose );
    spacer4 = new QSpacerItem( 90, 30, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout13->addItem( spacer4 );
    ImageViewerUILayout->addLayout( layout13 );
    languageChange();
    resize( QSize(525, 356).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(accept()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
ImageViewerUI::~ImageViewerUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ImageViewerUI::languageChange()
{
    setCaption( tr( "ImageViewer" ) );
    imageList->header()->setLabel( 0, tr( "ThumbView" ) );
    imageList->header()->setLabel( 1, tr( "FileName" ) );
    buttonClose->setText( tr( "Close" ) );
}

