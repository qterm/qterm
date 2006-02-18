/****************************************************************************
** Form implementation generated from reading ui file 'keydialog.ui'
**
** Created: Thu Jan 2 20:10:25 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "keydialog.h"
#include "qtermconfig.h"
#include <QFileDialog>

extern QString fileCfg;
//extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a keyDialog as a child of 'parent', with the 
 		*  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
keyDialog::keyDialog( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl ),bgKey(this)

{
	ui.setupUi(this);
	
	bgKey.addButton(ui.radioButton1,0);
	bgKey.addButton(ui.radioButton2,1);
	bgKey.addButton(ui.radioButton3,2);
	
	connectSlots();
	
	pConf = new QTermConfig(fileCfg);
	
	loadName();
}

/*
 *  Destroys the object and frees any allocated resources
 */
keyDialog::~keyDialog()
{
    // no need to delete child widgets, Qt does it all for us
	delete pConf;
}


void keyDialog::connectSlots()
{
	connect( ui.nameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onNamechange(int)) );
	connect( ui.addButton, SIGNAL(clicked()), this, SLOT(onAdd()) );
	connect( ui.deleteButton, SIGNAL(clicked()), this, SLOT(onDelete()) );
	connect( ui.updateButton, SIGNAL(clicked()), this, SLOT(onUpdate()) );
	connect( ui.closeButton, SIGNAL(clicked()), this, SLOT(onClose()) );
	connect( ui.upButton, SIGNAL(clicked()), this, SLOT(onUp()) );
	connect( ui.downButton, SIGNAL(clicked()), this, SLOT(onDown()) );
	connect( ui.leftButton, SIGNAL(clicked()), this, SLOT(onLeft()) );
	connect( ui.rightButton, SIGNAL(clicked()), this, SLOT(onRight()) );
	connect( ui.enterButton, SIGNAL(clicked()), this, SLOT(onEnter()) );
	connect( &bgKey, SIGNAL(buttonClicked(int)), this, SLOT(onSelect(int)) );
	connect( ui.scriptButton, SIGNAL(clicked()), this, SLOT(onScript()) );
	connect( ui.programButton, SIGNAL(clicked()), this, SLOT(onProgram()) );

}

void keyDialog::onNamechange( int item )
{
	loadKey(item);
}

void keyDialog::onAdd()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();

	strTmp.setNum(num+1);
	pConf->setItemValue("key","num", strTmp);
	
	QString strValue;
	switch(bgKey.checkedId())
	{
		case 0:
			strValue = "0"+ui.keyEdit->text().toLocal8Bit();
			break;
		case 1:
			strValue = "1"+ui.scriptEdit->text().toLocal8Bit();
			break;
		case 2:
			strValue = "2"+ui.programEdit->text().toLocal8Bit();
			break;
	}
	strTmp = QString("key%1").arg(num);
	pConf->setItemValue("key",strTmp,strValue); 

	strTmp = QString("name%1").arg(num);
	pConf->setItemValue("key", strTmp, ui.nameEdit->text().toLocal8Bit());

	ui.nameListWidget->addItem(ui.nameEdit->text());
	ui.nameListWidget->setCurrentRow(ui.nameListWidget->count()-1);
}

void keyDialog::onDelete()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();
	if(num==0)
		return;
	strTmp.setNum(num-1);
	pConf->setItemValue("key","num", strTmp);

	int index = ui.nameListWidget->currentRow();
	QString strItem1, strItem2;
	for( int i=index; i< num-1; i++)
	{
		strItem1 = QString("key%1").arg(i);
		strItem2 = QString("key%1").arg(i+1);
		pConf->setItemValue("key",strItem1, pConf->getItemValue("key", strItem2));
		strItem1 = QString("name%1").arg(i);
		strItem2 = QString("name%1").arg(i+1);
		pConf->setItemValue("key",strItem1, pConf->getItemValue("key", strItem2));
	}

	ui.nameListWidget->takeItem(index);
	ui.nameListWidget->setCurrentRow( qMin(index,ui.nameListWidget->count()-1) );
}
void keyDialog::onUpdate()
{
	int index = ui.nameListWidget->currentRow();
	if(index<0)
			return;

	QString strValue;
	switch(bgKey.checkedId())
	{
		case 0:
			strValue = "0"+ui.keyEdit->text().toLocal8Bit();
			break;
		case 1:
			strValue = "1"+ui.scriptEdit->text().toLocal8Bit();
			break;
		case 2:
			strValue = "2"+ui.programEdit->text().toLocal8Bit();
			break;
	}
	
	QString strTmp;

	strTmp = QString("key%1").arg(index);
	pConf->setItemValue("key",strTmp,strValue); 

	strTmp = QString("name%1").arg(index);
	pConf->setItemValue("key", strTmp, ui.nameEdit->text().toLocal8Bit());

	ui.nameListWidget->item(index)->setText(ui.nameEdit->text());
}
void keyDialog::onClose()
{
	pConf->save(fileCfg);
	done(1);
}
void keyDialog::onUp()
{
	ui.keyEdit->insert("^[[A");
}
void keyDialog::onDown()
{
	ui.keyEdit->insert("^[[B");
}
void keyDialog::onLeft()
{
	ui.keyEdit->insert("^[[D");
}
void keyDialog::onRight()
{
	ui.keyEdit->insert("^[[C");
}
void keyDialog::onEnter()
{
	ui.keyEdit->insert("^M");
}
// void keyDialog::onSelect(int id)
// {
// 	switch(id)
// 	{
// 		case 0:	// key
// 			ui.keyEdit->setEnabled(true);
// 			ui.scriptEdit->setEnabled(false);
// 			ui.scriptButton->setEnabled(false);
// 			ui.programEdit->setEnabled(false);
// 			ui.programButton->setEnabled(false);
// 			break;
// 		case 1:	// script
// 			ui.keyEdit->setEnabled(false);
// 			ui.scriptEdit->setEnabled(true);
// 			ui.scriptButton->setEnabled(true);
// 			ui.programEdit->setEnabled(false);
// 			ui.programButton->setEnabled(false);
// 		break;
// 		case 2:	// program
// 			ui.scriptEdit->setEnabled(false);
// 			ui.scriptButton->setEnabled(false);
// 			ui.keyEdit->setEnabled(false);
// 			ui.programEdit->setEnabled(true);
// 			ui.programButton->setEnabled(true);
// 			break;
// 	}
// }
void keyDialog::onScript()
{
	QString script = QFileDialog::getOpenFileName( this,"Select a script", QDir::currentPath(), "Python File (*.py *.txt)");
    if ( !script.isNull() ) 
	{
		ui.scriptEdit->setText(script);
	}
}
void keyDialog::onProgram()
{
	QString program = QFileDialog::getOpenFileName( this, "Select a program", QDir::currentPath(),"*");
    if ( !program.isNull() ) 
	{
		ui.programEdit->setText(program);
	}
}

void keyDialog::loadName()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();
	for(int i=0; i<num; i++)
	{
		strTmp = QString("name%1").arg(i);
		ui.nameListWidget->addItem(pConf->getItemValue("key", strTmp));
	}
	if(num>0)
		ui.nameListWidget->setCurrentRow(0);
	else
	{
		ui.radioButton1->setChecked(true);
		//onSelect(0);
	}
}

void keyDialog::loadKey(int n)
{
	QString strTmp = pConf->getItemValue("key", "num");
	if(n>=strTmp.toInt())
		return;
	QString strItem;

	strItem = QString("name%1").arg(n);
	ui.nameEdit->setText(pConf->getItemValue("key", strItem));

	strItem = QString("key%1").arg(n);
	strTmp = pConf->getItemValue("key", strItem);
	if(strTmp[0]=='0')
	{
		ui.keyEdit->setText(strTmp.mid(1));
		ui.radioButton1->setChecked(true);
		//onSelect(0);
	}else if(strTmp[0]=='1')
	{
		ui.scriptEdit->setText(strTmp.mid(1));
		ui.radioButton2->setChecked(true);
		//onSelect(7);
	}
	else if(strTmp[0]=='2')
	{
		ui.programEdit->setText(strTmp.mid(1));
		ui.radioButton3->setChecked(true);
		//onSelect(6);
	}
}
#include <keydialog.moc>
