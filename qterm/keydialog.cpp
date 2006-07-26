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

#include <qfiledialog.h>

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

extern QString fileCfg;
extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a keyDialog as a child of 'parent', with the 
 		*  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
keyDialog::keyDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : keyDialogUI( parent, name, modal, fl )

{
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
	connect( nameListBox, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(onNamechange(QListBoxItem*)) );
	connect( addButton, SIGNAL(clicked()), this, SLOT(onAdd()) );
	connect( deleteButton, SIGNAL(clicked()), this, SLOT(onDelete()) );
	connect( updateButton, SIGNAL(clicked()), this, SLOT(onUpdate()) );
	connect( closeButton, SIGNAL(clicked()), this, SLOT(onClose()) );
	connect( upButton, SIGNAL(clicked()), this, SLOT(onUp()) );
	connect( downButton, SIGNAL(clicked()), this, SLOT(onDown()) );
	connect( leftButton, SIGNAL(clicked()), this, SLOT(onLeft()) );
	connect( rightButton, SIGNAL(clicked()), this, SLOT(onRight()) );
	connect( enterButton, SIGNAL(clicked()), this, SLOT(onEnter()) );
	connect( buttonGroup2, SIGNAL(clicked(int)), this, SLOT(onSelect(int)) );
	connect( scriptButton, SIGNAL(clicked()), this, SLOT(onScript()) );
	connect( programButton, SIGNAL(clicked()), this, SLOT(onProgram()) );

}

void keyDialog::onNamechange( QListBoxItem *item )
{
	loadKey(nameListBox->index(item));
}

void keyDialog::onAdd()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();

	strTmp.setNum(num+1);
	pConf->setItemValue("key","num", strTmp);
	
	QString strValue;
	switch(buttonGroup2->id(buttonGroup2->selected()))
	{
		case 0:
			strValue = "0"+keyEdit->text().local8Bit();
			break;
		case 7:
			strValue = "1"+scriptEdit->text().local8Bit();
			break;
		case 6:
			strValue = "2"+programEdit->text().local8Bit();
			break;
	}
	strTmp.sprintf("key%d",num);
	pConf->setItemValue("key",strTmp,strValue); 

	strTmp.sprintf("name%d",num);
	pConf->setItemValue("key", strTmp, nameEdit->text().local8Bit());

	nameListBox->insertItem(nameEdit->text());
	nameListBox->setSelected(nameListBox->count()-1, true);
}
void keyDialog::onDelete()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();
	if(num==0)
		return;
	strTmp.setNum(num-1);
	pConf->setItemValue("key","num", strTmp);

	int index = nameListBox->currentItem();
	QString strItem1, strItem2;
	for( int i=index; i< num-1; i++)
	{
		strItem1.sprintf("key%d",i);
		strItem2.sprintf("key%d",i+1);
		pConf->setItemValue("key",strItem1, pConf->getItemValue("key", strItem2));
		strItem1.sprintf("name%d",i);
		strItem2.sprintf("name%d",i+1);
		pConf->setItemValue("key",strItem1, pConf->getItemValue("key", strItem2));
	}

	nameListBox->removeItem(index);
	nameListBox->setSelected( QMIN(index,nameListBox->count()-1),true );
}
void keyDialog::onUpdate()
{
	int index = nameListBox->currentItem();
	if(index<0)
			return;

	QString strValue;
	switch(buttonGroup2->id(buttonGroup2->selected()))
	{
		case 0:
			strValue = "0"+keyEdit->text().local8Bit();
			break;
		case 7:
			strValue = "1"+scriptEdit->text().local8Bit();
			break;
		case 6:
			strValue = "2"+programEdit->text().local8Bit();
			break;
	}
	
	QString strTmp;

	strTmp.sprintf("key%d",index);
	pConf->setItemValue("key",strTmp,strValue); 

	strTmp.sprintf("name%d",index);
	pConf->setItemValue("key", strTmp, nameEdit->text().local8Bit());

	nameListBox->changeItem(nameEdit->text(), index);
}
void keyDialog::onClose()
{
	pConf->save(fileCfg);
	done(1);
}
void keyDialog::onUp()
{
	keyEdit->insert("^[[A");
}
void keyDialog::onDown()
{
	keyEdit->insert("^[[B");
}
void keyDialog::onLeft()
{
	keyEdit->insert("^[[D");
}
void keyDialog::onRight()
{
	keyEdit->insert("^[[C");

}
void keyDialog::onEnter()
{
	keyEdit->insert("^M");
}
void keyDialog::onSelect(int id)
{
	switch(id)
	{
		case 0:	// key
			keyEdit->setEnabled(true);
			scriptEdit->setEnabled(false);
			scriptButton->setEnabled(false);
			programEdit->setEnabled(false);
			programButton->setEnabled(false);
			break;
		case 7:	// script
			keyEdit->setEnabled(false);
			scriptEdit->setEnabled(true);
			scriptButton->setEnabled(true);
			programEdit->setEnabled(false);
			programButton->setEnabled(false);
		break;
		case 6:	// program
			scriptEdit->setEnabled(false);
			scriptButton->setEnabled(false);
			keyEdit->setEnabled(false);
			programEdit->setEnabled(true);
			programButton->setEnabled(true);
			break;
	}
}
void keyDialog::onScript()
{
	QString script = getOpenFileName( "Python File (*.py *.txt)",this );
    if ( !script.isEmpty() ) 
	{
		scriptEdit->setText(script);
	}
}
void keyDialog::onProgram()
{
	QString program = getOpenFileName( "*", this );
    if ( !program.isEmpty() ) 
	{
		programEdit->setText(program);
	}
}

void keyDialog::loadName()
{
	QString strTmp = pConf->getItemValue("key", "num");
	int num = strTmp.toInt();
	for(int i=0; i<num; i++)
	{
		strTmp.sprintf("name%d",i);
		nameListBox->insertItem(QString::fromLocal8Bit(pConf->getItemValue("key", strTmp)));
	}
	if(num>0)
		nameListBox->setSelected(0, true);
	else
	{
		radioButton1->setChecked(true);
		onSelect(0);
	}
}

void keyDialog::loadKey(int n)
{
	QString strTmp = pConf->getItemValue("key", "num");
	if(n>=strTmp.toInt())
		return;
	QString strItem;

	strItem.sprintf("name%d",n);
	nameEdit->setText(QString::fromLocal8Bit(pConf->getItemValue("key", strItem)));

	strItem.sprintf("key%d",n);
	strTmp = pConf->getItemValue("key", strItem);
	if(strTmp[0]=='0')
	{
		keyEdit->setText(QString::fromLocal8Bit(strTmp.mid(1)));
		radioButton1->setChecked(true);
		onSelect(0);
	}else if(strTmp[0]=='1')
	{
		scriptEdit->setText(QString::fromLocal8Bit(strTmp.mid(1)));
		radioButton2->setChecked(true);
		onSelect(7);
	}
	else if(strTmp[0]=='2')
	{
		programEdit->setText(QString::fromLocal8Bit(strTmp.mid(1)));
		radioButton3->setChecked(true);
		onSelect(6);
	}
}
#ifdef HAVE_CONFIG_H
#include "keydialog.moc"
#endif
