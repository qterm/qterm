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

//Added by qt3to4:
#include <QCloseEvent>
#include <QComboBox>
#include <QFileDialog>

extern QString fileCfg;
extern QString pathCfg;
//extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a prefDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
prefDialog::prefDialog( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl ),bgSound(this)
{
	ui.setupUi(this);
	bgSound.addButton(ui.noneRadioButton, 0);
	bgSound.addButton(ui.beepRadioButton, 1);
	bgSound.addButton(ui.fileRadioButton, 2);

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
	connect(ui.okPushButton, SIGNAL(clicked()), this, SLOT(onOK()) );
	connect(ui.cancelPushButton, SIGNAL(clicked()), this, SLOT(onCancel()) );
	connect(ui.selectsoundPushButton, SIGNAL(clicked()), this, SLOT(onSound()) );
	connect(ui.choosehttpPushButton, SIGNAL(clicked()), this, SLOT(onHttp()) );
    //connect(ButtonGroup1 , SIGNAL(clicked(int)), this, SLOT(onBeep(int)) );
	connect(ui.browsePushButton, SIGNAL(clicked()), this, SLOT(onBrowse()) );
	connect(ui.imagePushButton, SIGNAL(clicked()), this, SLOT(onImage()) );
	connect(ui.poolPushButton, SIGNAL(clicked()), this, SLOT(onPool()) );
}

void prefDialog::loadSetting()
{
	QTermConfig conf(fileCfg);
	
	QString strTmp;
	
	strTmp = conf.getItemValue("preference","xim");
	ui.ximComboBox->setCurrentItem(strTmp.toInt());
	
	strTmp = conf.getItemValue("preference","wordwrap");
	ui.wordLineEdit3->setText(strTmp);
	
	strTmp = conf.getItemValue("preference","smartww");
	ui.smartCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","wheel");
	ui.wheelCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","url");
	ui.urlCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","logmsg");
	ui.saveCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","blinktab");
	ui.blinkCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","warn");
	ui.warnCheckBox->setChecked(strTmp!="0");
	
	strTmp = conf.getItemValue("preference","beep");
	qobject_cast<QRadioButton*>(bgSound.button(strTmp.toInt()))->setChecked(true);
	//ButtonGroup1->find(strTmp.toInt()))->setChecked(true);
	
// 	if(strTmp.toInt()!=2)
// 	{
// 		wavefileLineEdit->setEnabled(false);
// 		selectsoundPushButton->setEnabled(false);
// 	}
	
	strTmp = conf.getItemValue("preference","wavefile");
	ui.wavefileLineEdit->setText( strTmp );
	
	strTmp = conf.getItemValue("preference","antialias");
	ui.aacheckBox->setChecked( strTmp!="0" );
	
	strTmp = conf.getItemValue("preference","tray");
	ui.trayCheckBox->setChecked( strTmp!="0" );
	
	strTmp = conf.getItemValue("preference","clearpool");
	ui.clearCheckBox->setChecked( strTmp!="0" );
	
	strTmp = conf.getItemValue("preference","pool");
	if(strTmp.isEmpty())
		strTmp = pathCfg+"pool/";
	ui.poolLineEdit->setText( strTmp );
	
	strTmp = conf.getItemValue("preference","http");
	ui.httpLineEdit->setText( strTmp );
	
	strTmp = conf.getItemValue("preference","zmodem");
	if(strTmp.isEmpty())
		strTmp = pathCfg+"zmodem/";
	ui.zmodemLineEdit->setText( strTmp );
	
	strTmp = conf.getItemValue("preference","image");
	ui.imageLineEdit->setText( strTmp );
}
void prefDialog::saveSetting()
{
	QTermConfig conf(fileCfg);

	QString strTmp;

	strTmp.setNum(ui.ximComboBox->currentItem());
    conf.setItemValue("preference","xim", strTmp);
	

	conf.setItemValue("preference","wordwrap", ui.wordLineEdit3->text());
	
	strTmp.setNum(ui.smartCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","smartww", strTmp);
	
	strTmp.setNum(ui.wheelCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","wheel", strTmp);

	strTmp.setNum(ui.urlCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","url", strTmp);
  
	strTmp.setNum(ui.saveCheckBox->isChecked()?1:0);
	conf.setItemValue("preference","logmsg", strTmp);
 
	strTmp.setNum(ui.blinkCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","blinktab", strTmp);

	strTmp.setNum(ui.warnCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","warn", strTmp);

	strTmp.setNum(ui.aacheckBox->isChecked()?1:0);
    conf.setItemValue("preference","antialias", strTmp);

	strTmp.setNum(ui.trayCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","tray", strTmp);
   
	strTmp.setNum(bgSound.checkedId());
    conf.setItemValue("preference","beep", strTmp);
	
	if(strTmp=="2")
		conf.setItemValue("preference","wavefile", ui.wavefileLineEdit->text());
	
	strTmp.setNum(ui.clearCheckBox->isChecked()?1:0);
    conf.setItemValue("preference","clearpool", strTmp);

	strTmp=ui.poolLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = pathCfg+"pool/";
	conf.setItemValue("preference","pool",strTmp.toLocal8Bit());

	strTmp=ui.zmodemLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = pathCfg+"zmodem/";
	conf.setItemValue("preference","zmodem",strTmp.toLocal8Bit());
	
	conf.setItemValue("preference","http",ui.httpLineEdit->text());
	conf.setItemValue("preference","image",ui.imageLineEdit->text().toLocal8Bit());
	
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
	soundConf soundconf(this);
	if (soundconf.exec() == 1)
	{
		loadSetting();
	}

}
void prefDialog::onHttp()
{
	QString http = QFileDialog::getOpenFileName( this, "Choose a browser", QDir::currentPath(), "*" );
    if ( !http.isNull() ) 
	{
		ui.httpLineEdit->setText(http+" %L");
	}

}

// void prefDialog::onBeep( int id )
// {
// 	if(id==2)
// 	{
// 		ui.wavefileLineEdit->setEnabled(true);
// 		ui.selectsoundPushButton->setEnabled(true);
// 	}
// 	else if(id==0 || id==1 )
// 	{
// 		ui.wavefileLineEdit->setEnabled(false);
// 		ui.selectsoundPushButton->setEnabled(false);
// 	}
// }

void prefDialog::onBrowse()
{
	qDebug(ui.zmodemLineEdit->text());
	QString dir = QFileDialog::getExistingDirectory(this, "Choose a directory", ui.zmodemLineEdit->text());
	if( !dir.isNull() )
		ui.zmodemLineEdit->setText(dir);
}

void prefDialog::onImage()
{
	QString image = QFileDialog::getOpenFileName( this, "Choose a program", QDir::currentPath(), "*" );
    if ( !image.isNull() ) 
	{
		ui.imageLineEdit->setText(image);
	}
}

void prefDialog::onPool()
{
	QString pool = QFileDialog::getExistingDirectory( this, "Choose a directory", ui.poolLineEdit->text());
    if ( !pool.isEmpty() ) 
	{
		ui.poolLineEdit->setText(pool);
	}
}

#include <prefdialog.moc>
