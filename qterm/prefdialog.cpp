/****************************************************************************
** Form implementation generated from reading ui file 'prefdialog.ui'
**
** Created: Sat Dec 14 15:21:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "prefdialog.h"
#include "soundconf.h"

#include "qtermconfig.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <qfiledialog.h>

extern QString fileCfg;
extern QString pathCfg;
extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a prefDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
prefDialog::prefDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : prefDialogUI( parent, name, modal, fl )
{
	connectSlots();

	loadSetting();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
prefDialog::~prefDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void prefDialog::connectSlots()
{
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(onOK()) );
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(onCancel()) );
	connect(selectsoundPushButton, SIGNAL(clicked()), this, SLOT(onSound()) );
	connect(choosehttpPushButton, SIGNAL(clicked()), this, SLOT(onHttp()) );
    connect(ButtonGroup1 , SIGNAL(clicked(int)), this, SLOT(onBeep(int)) );
	connect(browsePushButton, SIGNAL(clicked()), this, SLOT(onBrowse()) );
	connect(imagePushButton, SIGNAL(clicked()), this, SLOT(onImage()) );
}

void prefDialog::loadSetting()
{
	QTermConfig conf(fileCfg);

	QString strTmp;

    strTmp = conf.getItemValue("preference","xim");
    ximComboBox->setCurrentItem(strTmp.toInt());
		
    strTmp = conf.getItemValue("preference","wordwrap");
	wordLineEdit3->setText(strTmp);

	strTmp = conf.getItemValue("preference","smartww");
    smartCheckBox->setChecked(strTmp!="0");
   
	strTmp = conf.getItemValue("preference","wheel");
    wheelCheckBox->setChecked(strTmp!="0");

    strTmp = conf.getItemValue("preference","url");
    urlCheckBox->setChecked(strTmp!="0");
  
	strTmp = conf.getItemValue("preference","logmsg");
	saveCheckBox->setChecked(strTmp!="0");

    strTmp = conf.getItemValue("preference","blinktab");
	blinkCheckBox->setChecked(strTmp!="0");

    strTmp = conf.getItemValue("preference","warn");
	warnCheckBox->setChecked(strTmp!="0");

    strTmp = conf.getItemValue("preference","beep");
    ((QRadioButton *)ButtonGroup1->find(strTmp.toInt()))->setChecked(true);

	if(strTmp.toInt()!=2)
	{
		wavefileLineEdit->setEnabled(false);
		selectsoundPushButton->setEnabled(false);
	}

	strTmp = conf.getItemValue("preference","wavefile");
 	wavefileLineEdit->setText( strTmp );

	strTmp = conf.getItemValue("preference","antialias");
	aacheckBox->setChecked( strTmp!="0" );

	strTmp = conf.getItemValue("preference","tray");
	trayCheckBox->setChecked( strTmp!="0" );

	strTmp = conf.getItemValue("preference","clearpool");
	clearCheckBox->setChecked( strTmp!="0" );

    strTmp = conf.getItemValue("preference","pool");
	if(strTmp.isEmpty())
		strTmp = pathCfg+"pool/";
	poolLineEdit->setText( strTmp );

    strTmp = conf.getItemValue("preference","http");
 	httpLineEdit->setText( strTmp );

	strTmp = conf.getItemValue("preference","zmodem");
	if(strTmp.isEmpty())
		strTmp = pathCfg+"zmodem/";
	zmodemLineEdit->setText( strTmp );

	strTmp = conf.getItemValue("preference","image");
 	imageLineEdit->setText( strTmp );
}
void prefDialog::saveSetting()
{
	QTermConfig conf(fileCfg);

	QString strTmp;

	strTmp.setNum(ximComboBox->currentItem());
    conf.setItemValue("preference","xim", strTmp);
	

   	conf.setItemValue("preference","wordwrap", wordLineEdit3->text());
	
	strTmp.setNum(smartCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","smartww", strTmp);
	
    strTmp.setNum(wheelCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","wheel", strTmp);

	strTmp.setNum(urlCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","url", strTmp);
  
	strTmp.setNum(saveCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","logmsg", strTmp);
 
	strTmp.setNum(blinkCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","blinktab", strTmp);

	strTmp.setNum(warnCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","warn", strTmp);

	strTmp.setNum(aacheckBox->isChecked()?1:0);
    conf.setItemValue("preference","antialias", strTmp);

	strTmp.setNum(trayCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","tray", strTmp);
   
	strTmp.setNum(ButtonGroup1->id(ButtonGroup1->selected()));
    conf.setItemValue("preference","beep", strTmp);
	
	if(strTmp=="2")
		conf.setItemValue("preference","wavefile", wavefileLineEdit->text());
	
	strTmp.setNum(clearCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","clearpool", strTmp);

	strTmp=poolLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = pathCfg+"pool/";
	conf.setItemValue("preference","pool",strTmp.local8Bit());

	strTmp=zmodemLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = pathCfg+"zmodem/";
	conf.setItemValue("preference","zmodem",strTmp.local8Bit());
	
    conf.setItemValue("preference","http",httpLineEdit->text());
	conf.setItemValue("preference","image",imageLineEdit->text().local8Bit());
	
	conf.save(fileCfg);
}

void prefDialog::closeEvent(QCloseEvent *)
{
	reject();
}

void prefDialog::onOK()
{
	saveSetting();
	done(1);
}
void prefDialog::onCancel()
{
	done(0);
}
void prefDialog::onSound()
{
	fSoundConf soundconf(this);
	if (soundconf.exec() == 1)
	{
		loadSetting();
	}

}
void prefDialog::onHttp()
{
	QString http = getOpenFileName( "*", this );
    if ( !http.isEmpty() ) 
	{
		httpLineEdit->setText(http+" %L");
	}

}
void prefDialog::onBeep( int id )
{
	if(id==2)
	{
		wavefileLineEdit->setEnabled(true);
		selectsoundPushButton->setEnabled(true);
	}
	else if(id==0 || id==1 )
	{
		wavefileLineEdit->setEnabled(false);
		selectsoundPushButton->setEnabled(false);
	}
}

void prefDialog::onBrowse()
{
	QString dir = QFileDialog::getExistingDirectory(zmodemLineEdit->text(), this);
	if( !dir.isEmpty() )
		zmodemLineEdit->setText(dir);
}

void prefDialog::onImage()
{
	QString image = getOpenFileName( "*", this );
    if ( !image.isEmpty() ) 
	{
		imageLineEdit->setText(image);
	}
}

void prefDialog::onPool()
{
	QString pool = QFileDialog::getExistingDirectory( poolLineEdit->text(), this );
    if ( !pool.isEmpty() ) 
	{
		poolLineEdit->setText(pool);
	}
}

