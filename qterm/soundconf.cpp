#include "soundconf.h"
#include "qtermconfig.h"
#include "qterm.h"
#include <stdlib.h>
#include <qvariant.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qstring.h>

#include <qmessagebox.h>
#include <qfiledialog.h>
extern QString fileCfg;

/* 
 *  Constructs a fSoundConf which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
fSoundConf::fSoundConf( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : fSoundConfUI( parent, name, modal, fl )
{
	loadSetting();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
fSoundConf::~fSoundConf()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void fSoundConf::onSelectFile()
{
    QString soundfile = QFileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !soundfile.isEmpty() ) {
                leFile->setText(soundfile);
    }
}

/*
 * public slot
 */
void fSoundConf::onSelectProg()
{
    QString progfile = QFileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !progfile.isEmpty() ) {
                leFile->setText(progfile);
    }
}

/*
 * public slot
 */
void fSoundConf::onPlayMethod( int id )
{
    if(id==3)
    {
	leProg->setEnabled(true);
        bpSelect->setEnabled(true);
    }
    else if(id==0 || id==1 || id==2)
    {
	leProg->setEnabled(false);
        bpSelect->setEnabled(false);
    }
}

void fSoundConf::loadSetting()
{
	QTermConfig conf(fileCfg);

	QString strTmp;

	//fprintf(stderr, "we got here\n");

	strTmp = conf.getItemValue("preference", "wavefile");
	if (!strTmp.isEmpty())
		leFile->setText( strTmp );

	strTmp = conf.getItemValue("preference", "playmethod");
	if (!strTmp.isEmpty()){
		((QRadioButton *)bgMethod->find(strTmp.toInt()))->setChecked(true);
	
		if (strTmp.toInt() != 3) {
			leProg->setEnabled(false);
			bpSelect->setEnabled(false);
		}
		else {
			strTmp = conf.getItemValue("preference", "externalplayer");
			if (!strTmp.isEmpty())
				leProg->setText( strTmp );
		}
	}
	else {
		leProg->setEnabled(false);
		bpSelect->setEnabled(false);
	}
}

void fSoundConf::saveSetting()
{
	QTermConfig conf(fileCfg);
	
	QString strTmp;

	conf.setItemValue("preference", "beep", "2");

	conf.setItemValue("preference", "wavefile", leFile->text());

	strTmp.setNum(bgMethod->id(bgMethod->selected()));
#ifdef _NO_ARTS_COMPILED
	if (strTmp == "1"){
		QMessageBox::critical(this, tr("No such output driver"),
			tr("ARTS is not supported by this instance of QTerm,\nCheck whether your ARTS support is enabled in compile time."),
			tr("&OK"));
		return;
	}
#endif
#ifdef _NO_ESD_COMPILED
	if (strTmp == "2"){
		QMessageBox::critical(this, tr("No such output driver"),
			tr("ESD is not supported by this instance of QTerm,\nCheck whether your ESD support is enabled in compile time"),
			tr("&OK"));
		return;
	}
#endif
	conf.setItemValue("preference", "playmethod", strTmp);

	if (strTmp == "3")
			conf.setItemValue("preference", "externalplayer", leProg->text());

	conf.save(fileCfg);
}

void fSoundConf::accept()
{
	saveSetting();
	fSoundConfUI::accept();
}
