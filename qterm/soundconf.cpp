#include "soundconf.h"
#include "qtermconfig.h"
#include "qtermsound.h"
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
extern QString getOpenFileName(const QString&, QWidget*);

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
	m_pSound = NULL;
	loadSetting();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
fSoundConf::~fSoundConf()
{
    // no need to delete child widgets, Qt does it all for us
    delete m_pSound;
}

/*
 * public slot
 */
void fSoundConf::onSelectFile()
{
    QString soundfile = getOpenFileName( "*", this );
    if ( !soundfile.isEmpty() ) {
                leFile->setText(soundfile);
    }
}

/*
 * public slot
 */
void fSoundConf::onSelectProg()
{
    QString progfile = getOpenFileName( "*", this );
    if ( !progfile.isEmpty() ) {
                leFile->setText(progfile);
    }
}

/*
 * public slot
 */
void fSoundConf::onPlayMethod( int id )
{
#ifdef _NO_ARTS_COMPILED
    if (id == 1){
	QMessageBox::critical(this, tr("No such output driver"),
	    tr("ARTS is not supported by this instance of QTerm,\nCheck whether your ARTS support is enabled in compile time."),
	    tr("&OK"));
	QRadioButton * tmp = static_cast<QRadioButton *>(bgMethod->find(3));
	tmp->setChecked(true);
    }
#endif
#ifdef _NO_ESD_COMPILED
    if (id == 2){
	QMessageBox::critical(this, tr("No such output driver"),
	    tr("ESD is not supported by this instance of QTerm,\nCheck whether your ESD support is enabled in compile time"),
	    tr("&OK"));
	QRadioButton * tmp = static_cast<QRadioButton *>(bgMethod->find(3));
	tmp->setChecked(true);
    }
#endif
    if(id == 3 || bgMethod->selectedId() == 3)
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
void fSoundConf::onTestPlay()
{
    if (leFile->text().isEmpty())
	QMessageBox::critical(this, tr("No sound file"),
	tr("You have to select a file to test the sound"),
	tr("&Ok"));
    switch (bgMethod->selectedId()) {
    case 0:
	 m_pSound = new QTermInternalSound(leFile->text());
	break;
    case 1:
#ifndef _NO_ARTS_COMPILED
	m_pSound = new QTermArtsSound(leFile->text());
#endif
	break;
    case 2:
#ifndef _NO_ESD_COMPILED
	m_pSound = new QTermEsdSound(leFile->text());
#endif
	break;
    case 3:
	if (leProg->text().isEmpty())
	    QMessageBox::critical(this, tr("No player"),
		tr("You have to specify an external player"),
		tr("&Ok"));
	    else
		m_pSound = new QTermExternalSound(leProg->text(), leFile->text());
	    break;
    default:
	m_pSound = NULL;
    }
    if (m_pSound)
	m_pSound->play();
    delete m_pSound;
    m_pSound = NULL;
}
	    
void fSoundConf::loadSetting()
{
	QTermConfig conf(fileCfg);

	QString strTmp;

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
#include <soundconf.moc>
