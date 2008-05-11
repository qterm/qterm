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
#include "qtermglobal.h"

//Added by qt3to4:
#include <QCloseEvent>
#include <QComboBox>
#include <QFileDialog>
namespace QTerm
{
//extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a prefDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
prefDialog::prefDialog( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl )
{

	ui.setupUi(this);
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
	connect(ui.selectPlayerPushButton, SIGNAL(clicked()), this, SLOT(onPlayer()) );
	connect(ui.choosehttpPushButton, SIGNAL(clicked()), this, SLOT(onHttp()) );
	connect(ui.browsePushButton, SIGNAL(clicked()), this, SLOT(onBrowse()) );
	connect(ui.imagePushButton, SIGNAL(clicked()), this, SLOT(onImage()) );
	connect(ui.poolPushButton, SIGNAL(clicked()), this, SLOT(onPool()) );
	connect(ui.resetPushButton, SIGNAL(clicked()), this, SLOT(onReset()) );
}

void prefDialog::loadSetting()
{
	Config * conf = Global::instance()->fileCfg();

	QString strTmp;

	strTmp = conf->getItemValue("preference","xim");
	ui.ximComboBox->setCurrentIndex(strTmp.toInt());

	strTmp = conf->getItemValue("preference","wordwrap");
	ui.wordSpinBox->setValue(strTmp.toInt());

	strTmp = conf->getItemValue("preference","wheel");
	ui.wheelCheckBox->setChecked(strTmp!="0");

	strTmp = conf->getItemValue("preference","url");
	ui.urlCheckBox->setChecked(strTmp!="0");

	strTmp = conf->getItemValue("preference","blinktab");
	ui.blinkCheckBox->setChecked(strTmp!="0");

	strTmp = conf->getItemValue("preference","warn");
	ui.warnCheckBox->setChecked(strTmp!="0");

	strTmp = conf->getItemValue("preference","wavefile");
	ui.wavefileLineEdit->setText( strTmp );

	strTmp = conf->getItemValue("preference","externalplayer");
	ui.playerLineEdit->setText( strTmp );

	strTmp = conf->getItemValue("preference","antialias");
	ui.aacheckBox->setChecked( strTmp!="0" );

	strTmp = conf->getItemValue("preference","tray");
	ui.trayCheckBox->setChecked( strTmp!="0" );

	strTmp = conf->getItemValue("preference","clearpool");
	ui.clearCheckBox->setChecked( strTmp!="0" );

	strTmp = conf->getItemValue("preference","pool");
	if(strTmp.isEmpty())
		strTmp = Global::instance()->pathCfg()+"pool/";
	ui.poolLineEdit->setText( strTmp );

	strTmp = conf->getItemValue("preference","http");
	ui.httpLineEdit->setText( strTmp );

	strTmp = conf->getItemValue("preference","zmodem");
	if(strTmp.isEmpty())
		strTmp = Global::instance()->pathCfg()+"zmodem/";
	ui.zmodemLineEdit->setText( strTmp );

	strTmp = conf->getItemValue("preference","image");
	ui.imageLineEdit->setText( strTmp );
}

void prefDialog::saveSetting()
{
	Config * conf = Global::instance()->fileCfg();

	QString strTmp;

	strTmp.setNum(ui.ximComboBox->currentIndex());
	conf->setItemValue("preference","xim", strTmp);

	strTmp.setNum(ui.wordSpinBox->value());
	conf->setItemValue("preference","wordwrap", strTmp);

	strTmp.setNum(ui.wheelCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","wheel", strTmp);

	strTmp.setNum(ui.urlCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","url", strTmp);

	strTmp.setNum(ui.blinkCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","blinktab", strTmp);

	strTmp.setNum(ui.warnCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","warn", strTmp);

	strTmp.setNum(ui.aacheckBox->isChecked()?1:0);
	conf->setItemValue("preference","antialias", strTmp);

	strTmp.setNum(ui.trayCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","tray", strTmp);

	conf->setItemValue("preference","wavefile", ui.wavefileLineEdit->text());
	conf->setItemValue("preference","externalplayer", ui.playerLineEdit->text());

	strTmp.setNum(ui.clearCheckBox->isChecked()?1:0);
	conf->setItemValue("preference","clearpool", strTmp);

	strTmp=ui.poolLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = Global::instance()->pathCfg()+"pool/";
	conf->setItemValue("preference","pool",strTmp);

	strTmp=ui.zmodemLineEdit->text();
	if(strTmp.isEmpty())
		strTmp = Global::instance()->pathCfg()+"zmodem/";
	conf->setItemValue("preference","zmodem",strTmp);

	conf->setItemValue("preference","http",ui.httpLineEdit->text());
	conf->setItemValue("preference","image",ui.imageLineEdit->text());

	conf->save();
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

void prefDialog::onReset()
{
	loadSetting();
}

void prefDialog::onSound()
{
	QString sound = QFileDialog::getOpenFileName( this, "Choose a sound file", QDir::currentPath(), "*" );
	if ( !sound.isNull() )
	{
		ui.wavefileLineEdit->setText(sound);
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

void prefDialog::onPlayer()
{
	QString player= QFileDialog::getOpenFileName( this, "Choose a program", QDir::currentPath(), "*" );
	if ( !player.isNull() )
	{
		ui.playerLineEdit->setText(player);
	}
}

void prefDialog::onBrowse()
{
	//qDebug(ui.zmodemLineEdit->text());
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

} // namespace QTerm

#include <prefdialog.moc>
