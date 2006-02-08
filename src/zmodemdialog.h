#ifndef ZMODEMDIALOG_H
#define ZMODEMDIALOG_H

#include "ui/ui_zmodemdialog.h"

class zmodemDialog : public QDialog
{
	Q_OBJECT

public:
	zmodemDialog( QWidget *parent=0, Qt::WFlags fl=0 );
	~zmodemDialog();
	
	void setFileInfo(const QString&, int);
	void addErrorLog(const QString&);
	void clearErrorLog();
	void setProgress(int);

public slots:
	void slotCancel();

signals:
	void canceled();
	
private:
	Ui::zmodemDialog ui;
};

#endif
