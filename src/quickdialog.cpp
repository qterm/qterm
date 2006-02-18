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
//Added by qt3to4:
//#include <Q3CString>
#include <QCloseEvent>

#include <QComboBox>
#include <QPixmap>

#include "addrdialog.h"


#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <stdio.h>

extern QString pathLib;
extern QString fileCfg;
extern QString addrCfg;
extern void saveAddress(QTermConfig*,int,const QTermParam&);

/* 
 *  Constructs a quickDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
quickDialog::quickDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	ui.setupUi(this);
	
	ui.addPushButton->setPixmap(QPixmap(pathLib+"pic/addr.png"));
	
	ui.addPushButton->setToolTip(tr("Add To AddressBook" ));

	ui.connectPushButton->setDefault(true);
	
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
	//Q3CString cstrTmp = pConf->getItemValue("quick list", "num" );
	QString strTmp = pConf->getItemValue("quick list", "num" );
	QString strSection;
	for( int i=0; i<strTmp.toInt(); i++ )
	{
		strSection = QString("quick %1").arg(i);
		ui.historyComboBox->insertItem( pConf->getItemValue( strSection.toLatin1(), "addr" ) );
	}
	
	if(strTmp != "0")
	{
		ui.historyComboBox->setCurrentItem(0);
		listChanged(0);
	}
}

void quickDialog::connectSlots()
{
	connect(ui.historyComboBox, SIGNAL(activated(int)), this, SLOT(listChanged(int)) );
	connect(ui.addPushButton, SIGNAL(clicked()), this, SLOT(addAddr()) );
	connect(ui.deletePushButton, SIGNAL(clicked()), this, SLOT(deleteAddr()) );
	connect(ui.advPushButton, SIGNAL(clicked()), this, SLOT(advOption()) );
	connect(ui.connectPushButton, SIGNAL(clicked()), this, SLOT(connectIt()) );
	connect(ui.closePushButton, SIGNAL(clicked()), this, SLOT(close()) );
}

void quickDialog::listChanged( int index )
{
	QString strTmp = pConf->getItemValue("quick list", "num" );
	if(strTmp=="0")
		return;
	
	QString strSection = QString("quick %1").arg(index);
// 	cstrSection.sprintf("quick %d", index);

	ui.addrLineEdit->setText( pConf->getItemValue( strSection, "addr" ) );
	ui.portLineEdit->setText( pConf->getItemValue( strSection, "port" ) );

}
void quickDialog::addAddr()
{
	QTermConfig *pAddrConf = new QTermConfig(addrCfg);
	QString strTmp;
	strTmp = pAddrConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	strTmp.setNum(num+1);
	pAddrConf->setItemValue("bbs list", "num", strTmp);

	param.m_strName = ui.addrLineEdit->text();
	param.m_strAddr = ui.addrLineEdit->text();
	param.m_uPort = ui.portLineEdit->text().toUShort();
	saveAddress(pAddrConf,num,param);
	
	pAddrConf->save(addrCfg);
}
void quickDialog::deleteAddr()
{
	int n = ui.historyComboBox->currentItem();
	
	QString strTmp =  pConf->getItemValue("quick list", "num");
	int num = strTmp.toInt();
	
	if( num!=0 && n!=-1 )
	{
		QString strSection = QString("quick %1").arg(n);
// 		cstrSection.sprintf("quick %d", n);
		if(!pConf->deleteSection( strSection.toLatin1() ))
		{
			qDebug("Failed to delete %d", n);
			return;
		}
		ui.historyComboBox->removeItem(n);

		// change the name after this 
		for( int i=n+1; i<num; i++ )
		{
			strTmp = QString("quick %1").arg(i);
			strSection = QString("quick %1").arg(i-1);
			pConf->renameSection( strTmp.toLatin1(), strSection.toLatin1() );
		}

		strTmp = pConf->getItemValue("quick list", "num" );
		strTmp.setNum( qMax(0,strTmp.toInt()-1) );
		pConf->setItemValue("quick list", "num", strTmp.toLatin1());
	
		// update
		if(num==1)
		{
			ui.addrLineEdit->setText("");
			ui.portLineEdit->setText("");
		}
		else
		{
			ui.historyComboBox->setCurrentItem(qMin(n,num-2));
			listChanged(qMin(n,num-2));
		}
	}
}
void quickDialog::advOption()
{
	addrDialog set(this,true);

	param.m_strName = ui.addrLineEdit->text();
	param.m_strAddr = ui.addrLineEdit->text();
	param.m_uPort = ui.portLineEdit->text().toUShort();
	
	set.param = param;
	set.updateData(false);

	if(set.exec()==1)
	{
		param=set.param;
		ui.addrLineEdit->setText(param.m_strAddr);
		QString strTmp;
		strTmp.setNum(param.m_uPort);
		ui.portLineEdit->setText(strTmp);
	}
}
void quickDialog::connectIt()
{
	if(ui.addrLineEdit->text().isEmpty() || ui.portLineEdit->text().isEmpty() )
	{
		QMessageBox mb( "QTerm",
                  "address or port cant be blank",
                  QMessageBox::Warning,
                  QMessageBox::Ok | QMessageBox::Default,0,
                  0,0,0,true);
        mb.exec();
		return;
	}

	QString strTmp =  pConf->getItemValue("quick list", "num");
	int num = strTmp.toInt();

	bool bExist=false;
	int index; 
	// check if it is already there
	QString strSection;
	for(int i=0; i<num; i++)
	{
		strSection = QString("quick %1").arg(i);
	
		strTmp = pConf->getItemValue(strSection.toLatin1(), "addr");
		if(strTmp == ui.addrLineEdit->text())
		{	bExist=true; index=i; break; }
		strTmp = pConf->getItemValue(strSection.toLatin1(), "port");
		if(ui.portLineEdit->text().toInt()!=strTmp.toInt())
		{	bExist=true; index=i; break; }
	}
	// append it 
	if(!bExist)
	{
		strSection = QString("quick %1").arg(num);
		pConf->setItemValue(strSection.toLatin1(), "addr", ui.addrLineEdit->text());
		pConf->setItemValue(strSection.toLatin1(), "port", ui.portLineEdit->text());
		strTmp.setNum(num+1);
		pConf->setItemValue("quick list", "num", strTmp.toLatin1());
		index=num;
	}
	
	// set another name first to avoid duplicate
	strSection = QString("quick %1").arg(index);
	strTmp = QString("quick %1").arg(num+2);
	pConf->renameSection(strSection.toLatin1(),strTmp.toLatin1());
	// shift the current select to  the first
	for(int j=index-1; j>=0; j--)
	{
		strSection = QString("quick %1").arg(j);
		strTmp = QString("quick %1").arg(j+1);
		pConf->renameSection(strSection.toLatin1(),strTmp.toLatin1());
	}
	// set it back to 0
	strSection = QString("quick %1").arg(num+2);
	strTmp = QString("quick %1").arg(0);
	pConf->renameSection(strSection.toLatin1(),strTmp.toLatin1());

	param.m_strName = ui.addrLineEdit->text();
	param.m_strAddr = ui.addrLineEdit->text();
	param.m_uPort = ui.portLineEdit->text().toUShort();
	
	pConf->save(fileCfg);
	done(1);
}
void quickDialog::close()
{
	pConf->save(fileCfg);
	done(0);
}


#include <quickdialog.moc>
