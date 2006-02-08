
#include "zmodemdialog.h"
#include <QMessageBox>

zmodemDialog::zmodemDialog(QWidget *parent, Qt::WFlags fl)
		: QDialog(parent, fl)
{
	ui.setupUi(this);
	connect( ui.buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancel()) );
}

zmodemDialog::~zmodemDialog()
{
}

void zmodemDialog::setProgress(int offset)
{
	ui.pbProgress->setValue(offset);
	QString strTmp;
	strTmp = QString("%1 out of %2 bytes").arg(offset).arg(ui.pbProgress->maximum());
	ui.labelStatus->setText(strTmp);
}

void zmodemDialog::setFileInfo(const QString& name, int size)
{
	ui.labelFileName->setText(name);
	ui.pbProgress->setMaximum(size);
	
}

void zmodemDialog::addErrorLog(const QString& err)
{
	// FIXME:display error message;
	//browserError->append(err);
}
void zmodemDialog::clearErrorLog()
{
	//browserError->clear();
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
#include <zmodemdialog.moc>
