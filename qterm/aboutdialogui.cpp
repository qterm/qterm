/****************************************************************************
** Form implementation generated from reading ui file 'aboutdialog.ui'
**
** Created: Wed Apr 2 14:53:05 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "aboutdialogui.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a aboutDialogUI as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutDialogUI::aboutDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "aboutDialogUI" );
	#if (QT_VERSION>=300)
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
	#endif
    setMinimumSize( QSize( 400, 290 ) );
    setMaximumSize( QSize( 400, 290 ) );
    setBaseSize( QSize( 0, 0 ) );
    QFont f( font() );
    setFont( f ); 
    setSizeGripEnabled( FALSE );

    TextLabel = new QLabel( this, "TextLabel" );
    TextLabel->setGeometry( QRect( 60, 20, 310, 17 ) );
    TextLabel->setFrameShape( QLabel::NoFrame );
    TextLabel->setFrameShadow( QLabel::Plain );

    closeButton = new QPushButton( this, "closeButton" );
    closeButton->setGeometry( QRect( 310, 250, 80, 30 ) );

    mainTabWidget = new QTabWidget( this, "mainTabWidget" );
    mainTabWidget->setGeometry( QRect( 10, 50, 380, 190 ) );

    tab = new QWidget( mainTabWidget, "tab" );

    TextLabel3 = new QLabel( tab, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 50, 10, 270, 140 ) );
	#if (QT_VERSION>=300)	   
	TextLabel3->setBackgroundOrigin( QLabel::WindowOrigin );
	#endif
    mainTabWidget->insertTab( tab, "" );

    tab_2 = new QWidget( mainTabWidget, "tab_2" );

    TextLabel1 = new QLabel( tab_2, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 40, 10, 280, 140 ) );
    mainTabWidget->insertTab( tab_2, "" );

    tab_3 = new QWidget( mainTabWidget, "tab_3" );

    TextBrowser2 = new QTextBrowser( tab_3, "TextBrowser2" );
    TextBrowser2->setGeometry( QRect( 10, 10, 360, 170 ) );
    mainTabWidget->insertTab( tab_3, "" );

    pixmapLabel = new QLabel( this, "pixmapLabel" );
    pixmapLabel->setGeometry( QRect( 20, 10, 32, 32 ) );
    pixmapLabel->setScaledContents( TRUE );
    languageChange();
    resize( QSize(400, 290).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( closeButton, mainTabWidget );
}

/*
 *  Destroys the object and frees any allocated resources
 */
aboutDialogUI::~aboutDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void aboutDialogUI::languageChange()
{
    setCaption( tr( "About QTerm" ) );
    TextLabel->setText( tr( "QTerm 0.3.1 (Qt 2.3.2 and 3.1.x based)" ) );
    closeButton->setText( tr( "Close" ) );
    TextLabel3->setText( tr( "QTerm --- BBS Client for X Window\n"
"\n"
"2001-2003, The QTerm Developers\n"
"\n"
"http://qterm.sourceforge.net" ) );
    mainTabWidget->changeTab( tab, tr( "About" ) );
    TextLabel1->setText( tr( "kingson     \n"
"	xiaokiangwang@yahoo.com.cn  \n"
"\n"
"fiasco     \n"
"	yinshouyi@peoplemail.com.cn\n"
"\n"
"hooey\n"
"	hephooey@hotmail.com" ) );
    mainTabWidget->changeTab( tab_2, tr( "Authors" ) );
    mainTabWidget->changeTab( tab_3, tr( "Credits" ) );
    pixmapLabel->setText( QString::null );
}

