/****************************************************************************
** Form implementation generated from reading ui file 'aboutqterm2.ui'
**
** Created: Fri Dec 20 19:01:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "aboutdialog.h"

extern QString pathLib;

#include "qterm.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qlabel.h>
/* 
 *  Constructs a aboutDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutDialog::aboutDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : aboutDialogUI( parent, name, modal, fl )
{
	QFile file(pathLib+"credits");
	if ( file.open( IO_ReadOnly ) ) 
	{
		QTextStream stream( &file );
		QString line;
		while ( !stream.eof() ) 
			line += stream.readLine()+"\n"; // line of text excluding '\n'
		TextBrowser2->setText(line);
		file.close();
	}

	pixmapLabel->setPixmap( QPixmap(pathLib+"pic/qterm_32x32.png") );
	TextLabel->setText("QTerm "+QString(VERSION)+" (Qt 3.1.x+ based)");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
aboutDialog::~aboutDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

