/****************************************************************************
** Form implementation generated from reading ui file 'articledialog.ui'
**
** Created: Sun Jan 5 19:46:42 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "articledialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qapplication.h>

/* 
 *  Constructs a articleDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
articleDialog::articleDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : articleDialogUI( parent, name, modal, fl )

{
	connectSlots();
}

/*
 *  Destroys the object and frees any allocated resources
 */
articleDialog::~articleDialog()
{
    // no need to delete child widgets, Qt does it all for us
}


void articleDialog::connectSlots()
{
	connect(selectButton, SIGNAL(clicked()), this, SLOT(onSelect()));
	connect(copyButton, SIGNAL(clicked()), this, SLOT(onCopy()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(onSave()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
}

void articleDialog::onSelect()
{
	textBrowser2->selectAll(true);
}

void articleDialog::onCopy()
{
	QString strText = textBrowser2->selectedText();

	QClipboard *clipboard = QApplication::clipboard();
	
	#if (QT_VERSION>=0x030100)
		clipboard->setText(strText, QClipboard::Selection );
	#else
		clipboard->setText(strText);
	#endif


}

void articleDialog::onSave()
{
	QFileDialog fileDlg;
	QString filename  = fileDlg.getSaveFileName();

    if ( filename != NULL )
	{
        QFile f(filename);
        if ( (f.open(IO_WriteOnly)) ) 
		{
            f.writeBlock(cstrArticle, cstrArticle.length());
            f.close();
        }
        else 
		{
            QMessageBox mb( "Access file error:",
                    filename,
                    QMessageBox::Warning,
                    0, 0,
                    0, this, 0, true);
            mb.exec();
        }
    }
}

void articleDialog::onClose()
{
	done(0);
}


