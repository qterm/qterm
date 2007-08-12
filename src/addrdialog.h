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
#include "ui_addrdialog.h"
#include <QButtonGroup>
namespace QTerm
{
class QTermConfig;

class addrDialog : public QDialog
{ 
    Q_OBJECT

public:
    addrDialog( QWidget* parent = 0, bool partial = false, Qt::WFlags fl = 0 );
    ~addrDialog();
	
	QTermParam param;
	QTermConfig * pConf;
	void updateData(bool save);

protected slots:
	void onNamechange(int);
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
	void onChooseScript();
	void onMenuColor();
	
protected:
	void connectSlots();
	bool isChanged();
	void setLabelPixmap();

	bool bPartial;
	QString strFontName;
	int nFontSize;
	QColor clrFg;
	QColor clrBg;
	QString strSchemaFile;	
	QColor clrMenu;
	QButtonGroup bgMenu;

	int nLastItem;
	
public:
	Ui::addrDialog ui;	
};

} // namespace QTerm

#endif // ADDRDIALOG_H
