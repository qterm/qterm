
#include "zmodemdialog.h"

#include <qlabel.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>

zmodemDialog::zmodemDialog(QWidget *parent, const char *name, bool modal, WFlags fl)
		: zmodemDialogUI(parent, name, modal, fl)
{
	connect( buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancel()) );
}

zmodemDialog::~zmodemDialog()
{
}

void zmodemDialog::setProgress(int offset)
{
	pbProgress->setProgress(offset);
	QString strTmp;
	strTmp.sprintf("%d out of %d bytes", offset, pbProgress->totalSteps());
	labelStatus->setText(strTmp);
}

void zmodemDialog::setFileInfo(const QString& name, int size)
{
	labelFileName->setText(name);
	pbProgress->setTotalSteps(size);
	
}

void zmodemDialog::addErrorLog(const QString& err)
{
	browserError->append(err);
}
void zmodemDialog::clearErrorLog()
{
	browserError->clear();
}

void zmodemDialog::slotCancel()
{
	QMessageBox mb( "QTerm",
			"We dont support cancel operation yet. "
			"But you can try, it will crash when downloading.\n"
			"Do you want to continue?",
            QMessageBox::Warning,
            QMessageBox::Yes,
            QMessageBox::No  | QMessageBox::Escape | QMessageBox::Default,
            0,this,0,true);
    if ( mb.exec() == QMessageBox::Yes )
    {
		emit canceled();
		hide();
	}
}
#ifdef HAVE_CONFIG_H
#include "zmodemdialog.moc"
#endif
