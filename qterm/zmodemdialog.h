#ifndef ZMODEMDIALOG_H
#define ZMODEMDIALOG_H

#include "zmodemdialogui.h"

class zmodemDialog : public zmodemDialogUI
{
	Q_OBJECT

public:
	zmodemDialog( QWidget *parent=0, const char *name=0, bool modal= false, WFlags fl=0 );
	~zmodemDialog();
	
	void setFileInfo(const QString&, int);
	void addErrorLog(const QString&);
	void setProgress(int);

public slots:
	void slotCancel();

signals:
	void canceled();
};

#endif
