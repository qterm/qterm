/****************************************************************************
** Form interface generated from reading ui file 'addrdialog.ui'
**
** Created: Sun Dec 15 20:55:11 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ADDRDIALOG_H
#define ADDRDIALOG_H

#include "qtermparam.h"
#include "addrdialogui.h"


class QTermConfig;

class addrDialog : public addrDialogUI
{ 
    Q_OBJECT

public:
    addrDialog( QWidget* parent = 0, bool partial = false, 
					const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~addrDialog();


	
	QTermParam param;
	QTermConfig * pConf;
	void updateData(bool save);

protected slots:
	void onNamechange(QListBoxItem*);
	void onAdd();
	void onDelete();
	void onApply();
	void onConnect();
	void onClose();
	void onFont();
	void onFgcolor();
	void onBgcolor();
	void onSchema();
	void onProtocol(int);
	void onAutologin(bool);
	void onAuth(bool);
	void onAutoReply(bool);
	void onReconnect(bool);
	void onMenuType(int);
protected:
	void connectSlots();
	bool isChanged();
	void setLabelPixmap();

	bool bPartial;
	QString strFontName;
	int nFontSize;
	QColor clrFg;
	QColor clrBg;
	
	QColor clrMenu;

	int nLastItem;
};

#endif // ADDRDIALOG_H
