/****************************************************************************
** Form implementation generated from reading ui file 'aboutqterm2.ui'
**
** Created: Fri Dec 20 19:01:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "aboutdialog.h"
//Added by qt3to4:
#include <QPixmap>

#include "qtermglobal.h"
#include "qterm.h"

#include <QFile>
#include <QTextBrowser>
#include <QTextStream>
/* 
 *  Constructs a aboutDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
namespace QTerm
{

aboutDialog::aboutDialog( QWidget* parent, Qt::WindowFlags fl )
    : QDialog( parent, fl )
{
	QString pathLib = Global::instance()->pathLib();
	ui.setupUi(this);
// 	ui.TextBrowser->setSource(QUrl::fromLocalFile(pathLib+"credits"));
	QFile file(pathLib+"credits");
	if ( file.open( QIODevice::ReadOnly ) ) 
	{
		QTextStream stream( &file );
		QString line;
		while ( !stream.atEnd() ) 
			line += stream.readLine()+"\n"; // line of text excluding '\n'
		ui.TextBrowser->setPlainText(line);
		file.close();
	}

	ui.TextLabel->setText("QTerm "+QString(QTERM_VERSION)+" (Qt "+QT_VERSION_STR+" based)");
	//connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(accept()));
	restoreGeometry(Global::instance()->loadGeometry("About"));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
aboutDialog::~aboutDialog()
{
    // no need to delete child widgets, Qt does it all for us
    Global::instance()->saveGeometry("About",saveGeometry());
}

} // namespace QTerm

#include <moc_aboutdialog.cpp>
