/****************************************************************************
** Form implementation generated from reading ui file 'quickldialog.ui'
**
** Created: Sat Dec 14 14:25:01 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "quickdialog.h"

#include "qtermconfig.h"
#include "qtermparam.h"
#include "addrdialog.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <stdio.h>

extern QString pathLib;
extern char fileCfg[];
extern char addrCfg[];
extern void saveAddress(QTermConfig*,int,const QTermParam&);


/* 
 *  Constructs a quickDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
quickDialog::quickDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : quickDialogUI( parent, name, modal, fl )
{
	
	addPushButton->setPixmap(QPixmap(pathLib+"pic/addr.png"));
	
	QToolTip::add( addPushButton, tr("Add To AddressBook" ));

	connectPushButton->setDefault(true);
	
	connectSlots();
	
	pConf = new QTermConfig(fileCfg);
	
	loadHistory();
}


/*  
 *  Destroys the object and frees any allocated resources
 */
quickDialog::~quickDialog()
{
    // no need to delete child widgets, Qt does it all for us
	delete pConf;
}

void quickDialog::closeEvent( QCloseEvent *)
{
	pConf->save(fileCfg);
	reject();
}

void quickDialog::loadHistory()
{
	QCString cstrTmp = pConf->getItemValue("quick list", "num" );

	QCString cstrSection;
	for( int i=0; i<cstrTmp.toInt(); i++ )
	{
		cstrSection.sprintf("quick %d", i);
		historyComboBox->insertItem( pConf->getItemValue( cstrSection, "addr" ) );
	}
	
	if(cstrTmp!="0")
	{
		historyComboBox->setCurrentItem(0);
		listChanged(0);
	}
}

void quickDialog::connectSlots()
{
	connect(historyComboBox, SIGNAL(activated(int)), this, SLOT(listChanged(int)) );
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addAddr()) );
	connect(deletePushButton, SIGNAL(clicked()), this, SLOT(deleteAddr()) );
	connect(advPushButton, SIGNAL(clicked()), this, SLOT(advOption()) );
	connect(connectPushButton, SIGNAL(clicked()), this, SLOT(connectIt()) );
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()) );
}

void quickDialog::listChanged( int index )
{
	QCString cstrTmp = pConf->getItemValue("quick list", "num" );
	if(cstrTmp=="0")
		return;
	
	QCString cstrSection;
	cstrSection.sprintf("quick %d", index);

	addrLineEdit->setText( pConf->getItemValue( cstrSection, "addr" ) );
	portLineEdit->setText( pConf->getItemValue( cstrSection, "port" ) );

}
void quickDialog::addAddr()
{
	QTermConfig *pAddrConf = new QTermConfig(addrCfg);
	QString strTmp;
	strTmp = pAddrConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	strTmp.setNum(num+1);
	pAddrConf->setItemValue("bbs list", "num", strTmp);

	param.m_strName = addrLineEdit->text();
	param.m_strAddr = addrLineEdit->text();
	param.m_uPort = portLineEdit->text().toUShort();
	saveAddress(pAddrConf,num,param);
	
	pAddrConf->save(addrCfg);
}
void quickDialog::deleteAddr()
{
	int n = historyComboBox->currentItem();
	
	QCString cstrTmp =  pConf->getItemValue("quick list", "num");
	int num = cstrTmp.toInt();
	
	if( num!=0 && n!=-1 )
	{
		QCString cstrSection;
		cstrSection.sprintf("quick %d", n);
		if(!pConf->deleteSection( cstrSection ))
		{
			printf("Failed to delete %d\n", n);
			return;
		}
		historyComboBox->removeItem(n);

		// change the name after this 
		for( int i=n+1; i<num; i++ )
		{
			cstrTmp.sprintf("quick %d", i);
			cstrSection.sprintf("quick %d", i-1);
			pConf->renameSection(cstrTmp, cstrSection );
		}

		cstrTmp = pConf->getItemValue("quick list", "num" );
		cstrTmp.setNum( QMAX(0,cstrTmp.toInt()-1) );
		pConf->setItemValue("quick list", "num", cstrTmp);
	
		// update
		if(num==1)
		{
			addrLineEdit->setText("");
			portLineEdit->setText("");
		}
		else
		{
			historyComboBox->setCurrentItem(QMIN(n,num-2));
			listChanged(QMIN(n,num-2));
		}
	}
}
void quickDialog::advOption()
{
	addrDialog set(this,true);

	param.m_strName = addrLineEdit->text();
	param.m_strAddr = addrLineEdit->text();
	param.m_uPort = portLineEdit->text().toUShort();
	
	set.param = param;
	set.updateData(false);

	if(set.exec()==1)
	{
		param=set.param;
		addrLineEdit->setText(param.m_strAddr);
		QString strTmp;
		strTmp.setNum(param.m_uPort);
		portLineEdit->setText(strTmp);
	}
}
void quickDialog::connectIt()
{
	if(addrLineEdit->text().isEmpty() || portLineEdit->text().isEmpty() )
	{
		QMessageBox mb( "QTerm",
                  "address or port cant be blank",
                  QMessageBox::Warning,
                  QMessageBox::Ok | QMessageBox::Default,0,
                  0,0,0,true);
        mb.exec();
		return;
	}

	QCString cstrTmp =  pConf->getItemValue("quick list", "num");
	int num = cstrTmp.toInt();

	bool bExist=false;
	int index; 
	// check if it is already there
	QCString cstrSection;
	for(int i=0; i<num; i++)
	{
		cstrSection.sprintf("quick %d",i);
	
		cstrTmp = pConf->getItemValue(cstrSection, "addr");
		if(strcmp(addrLineEdit->text(),cstrTmp)==0 )
		{	bExist=true; index=i; break; }
		cstrTmp = pConf->getItemValue(cstrSection, "port");
		if(portLineEdit->text().toInt()!=cstrTmp.toInt())
		{	bExist=true; index=i; break; }
	}
	// append it 
	if(!bExist)
	{
		cstrSection.sprintf("quick %d",num);
		pConf->setItemValue(cstrSection, "addr", addrLineEdit->text());
		pConf->setItemValue(cstrSection, "port", portLineEdit->text());
		cstrTmp.setNum(num+1);
		pConf->setItemValue("quick list", "num", cstrTmp);
		index=num;
	}
	
	// set another name first to avoid duplicate
	cstrSection.sprintf("quick %d",index);
	cstrTmp.sprintf("quick %d",num+2);
	pConf->renameSection(cstrSection,cstrTmp);
	// shift the current select to  the first
	for(int j=index-1; j>=0; j--)
	{
		cstrSection.sprintf("quick %d",j);
		cstrTmp.sprintf("quick %d",j+1);
		pConf->renameSection(cstrSection,cstrTmp);
	}
	// set it back to 0
	cstrSection.sprintf("quick %d",num+2);
	cstrTmp.sprintf("quick %d",0);
	pConf->renameSection(cstrSection,cstrTmp);

	param.m_strName = addrLineEdit->text();
	param.m_strAddr = addrLineEdit->text();
	param.m_uPort = portLineEdit->text().toUShort();
	
	pConf->save(fileCfg);
	done(1);
}
void quickDialog::close()
{
	pConf->save(fileCfg);
	done(0);
}


