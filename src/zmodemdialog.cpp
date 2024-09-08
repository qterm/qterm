#include "zmodemdialog.h"
#include "qtermglobal.h"
#include <QMessageBox>
namespace QTerm
{
zmodemDialog::zmodemDialog(QWidget *parent, Qt::WindowFlags fl)
		: QDialog(parent, fl)
{
	ui.setupUi(this);
	connect( ui.buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancel()) );
    restoreGeometry(Global::instance()->loadGeometry("ZModem"));
}

zmodemDialog::~zmodemDialog()
{
    Global::instance()->saveGeometry("ZModem",saveGeometry());
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
    QMessageBox mb(QMessageBox::Warning, "QTerm",
			"We don't support cancel operation yet. "
			"But you can try, it will crash when downloading.\n"
            "Do you want to continue?",
            QMessageBox::Yes | QMessageBox::No,
            this);
    mb.setDefaultButton(QMessageBox::No);
    mb.setEscapeButton(QMessageBox::No);
    if ( mb.exec() == QMessageBox::Yes )
    {
		emit canceled();
		hide();
	}
}

} // namespace QTerm

#include <moc_zmodemdialog.cpp>

