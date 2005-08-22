/****************************************************************************
** Form implementation generated from reading ui file 'browser.ui'
**
** Created: 一  8月 22 19:33:34 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "imageviewerui.h"
#include "qtermcanvas.h"

#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QTermImageUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QTermImageUI::QTermImageUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "QTermImageUI" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    QTermImageUILayout = new QGridLayout( this, 1, 1, 11, 6, "QTermImageUILayout"); 

    d_canvas = new QTermCanvas( this, "d_canvas",0 );
    //d_canvas->setFrameShape( QFrame::StyledPanel );
    //d_canvas->setFrameShadow( QFrame::Raised );

    QTermImageUILayout->addWidget( d_canvas, 0, 0 );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 
    spacer3_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer3_3 );

    d_previous = new QPushButton( this, "d_previous" );
    d_previous->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, d_previous->sizePolicy().hasHeightForWidth() ) );
    layout2->addWidget( d_previous );
    spacer3 = new QSpacerItem( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer3 );

    d_browser = new QPushButton( this, "d_browser" );
    d_browser->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, d_browser->sizePolicy().hasHeightForWidth() ) );
    layout2->addWidget( d_browser );
    spacer3_2 = new QSpacerItem( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer3_2 );

    d_next = new QPushButton( this, "d_next" );
    d_next->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, d_next->sizePolicy().hasHeightForWidth() ) );
    layout2->addWidget( d_next );
    spacer4 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer4 );

    QTermImageUILayout->addLayout( layout2, 1, 0 );
    languageChange();
    resize( QSize(705, 527).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QTermImageUI::~QTermImageUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QTermImageUI::languageChange()
{
    setCaption( tr( "Form1" ) );
    d_previous->setText( tr( "&Previous" ) );
    d_previous->setAccel( QKeySequence( tr( "Alt+P" ) ) );
    d_browser->setText( tr( "&Browser" ) );
    d_browser->setAccel( QKeySequence( tr( "Alt+B" ) ) );
    d_next->setText( tr( "&Next" ) );
    d_next->setAccel( QKeySequence( tr( "Alt+N" ) ) );
}

