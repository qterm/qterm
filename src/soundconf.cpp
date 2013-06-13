#include "soundconf.h"
#include "qtermconfig.h"
#include "qtermsound.h"
#include "qtermglobal.h"
#include "qterm.h"

#include <QMessageBox>
#include <QFileDialog>
namespace QTerm
{
//extern QString getOpenFileName(const QString&, QWidget*);

/* 
 *  Constructs a fSoundConf which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
soundConf::soundConf( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl ),bgMethod(this)
{
	ui.setupUi(this);
	bgMethod.addButton(ui.radioButton1,0);
	bgMethod.addButton(ui.radioButton2,1);
	bgMethod.addButton(ui.radioButton3,2);
	bgMethod.addButton(ui.radioButton4,3);
	m_pSound = NULL;
	loadSetting();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
soundConf::~soundConf()
{
	// no need to delete child widgets, Qt does it all for us
	delete m_pSound;
}

/*
 * public slot
 */
void soundConf::onSelectFile()
{
	QString soundfile = QFileDialog::getOpenFileName( this, "Choose a file", QDir::currentPath(), "*" );
	if ( !soundfile.isNull() ) {
				ui.leFile->setText(soundfile);
	}
}

/*
 * public slot
 */
void soundConf::onSelectProg()
{
	QString progfile = QFileDialog::getOpenFileName( this, "Choose a program", QDir::currentPath(), "*" );
	if ( !progfile.isEmpty() ) {
				ui.leFile->setText(progfile);
	}
}

/*
 * public slot
 */
void soundConf::onPlayMethod( int id )
{
#ifdef _NO_ARTS_COMPILED
	if (id == 1){
	QMessageBox::critical(this, tr("No such output driver"),
		tr("ARTS is not supported by this instance of QTerm,\nCheck whether your ARTS support is enabled in compile time."),
		tr("&OK"));
	bgMethod.button(3)->setChecked(true);
// 	QRadioButton * tmp = static_cast<QRadioButton *>(bgMethod->find(3));
// 	tmp->setChecked(true);
	}
#endif
#ifdef _NO_ESD_COMPILED
	if (id == 2){
	QMessageBox::critical(this, tr("No such output driver"),
		tr("ESD is not supported by this instance of QTerm,\nCheck whether your ESD support is enabled in compile time"),
		tr("&OK"));
// 	QRadioButton * tmp = static_cast<QRadioButton *>(bgMethod->find(3));
// 	tmp->setChecked(true);
	bgMethod.button(3)->setChecked(true);
	}
#endif
	if(id == 3 || bgMethod.checkedId() == 3)
	{
		ui.leProg->setEnabled(true);
		ui.bpSelect->setEnabled(true);
	}
	else if(id==0 || id==1 || id==2)
	{
		ui.leProg->setEnabled(false);
		ui.bpSelect->setEnabled(false);
	}
}
void soundConf::onTestPlay()
{
	if (ui.leFile->text().isEmpty())
		QMessageBox::critical(this, tr("No sound file"),
							  tr("You have to select a file to test the sound"),
							  tr("&Ok"));
	
	switch (bgMethod.checkedId()) {
		case 0:
			m_pSound = new InternalSound(ui.leFile->text());
			break;
		case 1:
	/*
#ifndef _NO_ARTS_COMPILED
	m_pSound = new QTermArtsSound(leFile->text());
#endif
	break;
    case 2:
#ifndef _NO_ESD_COMPILED
	m_pSound = new QTermEsdSound(leFile->text());
#endif*/
			break;
    	case 3:
			if (ui.leProg->text().isEmpty())
				QMessageBox::critical(this, tr("No player"),
									  tr("You have to specify an external player"),
									  tr("&Ok"));
			else
				m_pSound = new ExternalSound(ui.leProg->text(), ui.leFile->text());
			break;
		default:
			m_pSound = NULL;
	}
	if (m_pSound)
		m_pSound->play();
	delete m_pSound;
	m_pSound = NULL;
}

void soundConf::loadSetting()
{
	QString strTmp;

	strTmp = Global::instance()->fileCfg()->getItemValue("preference", "wavefile");
	if (!strTmp.isEmpty())
		ui.leFile->setText( strTmp );

	strTmp = Global::instance()->fileCfg()->getItemValue("preference", "playmethod");
	if (!strTmp.isEmpty()){
		bgMethod.button(strTmp.toInt())->setChecked(true);
		if (strTmp.toInt() != 3) {
			ui.leProg->setEnabled(false);
			ui.bpSelect->setEnabled(false);
		}
		else {
			strTmp = Global::instance()->fileCfg()->getItemValue("preference", "externalplayer");
			if (!strTmp.isEmpty())
				ui.leProg->setText( strTmp );
		}
	}
	else {
		ui.leProg->setEnabled(false);
		ui.bpSelect->setEnabled(false);
	}
}

void soundConf::saveSetting()
{
	QString strTmp;

	Global::instance()->fileCfg()->setItemValue("preference", "beep", "2");

	Global::instance()->fileCfg()->setItemValue("preference", "wavefile", ui.leFile->text());

	strTmp.setNum(bgMethod.checkedId());
	Global::instance()->fileCfg()->setItemValue("preference", "playmethod", strTmp);

	if (strTmp == "3")
			Global::instance()->fileCfg()->setItemValue("preference", "externalplayer", ui.leProg->text());

	Global::instance()->fileCfg()->save();
}

void soundConf::accept()
{
	saveSetting();
	QDialog::accept();
}

} // namespace QTerm

#include <soundconf.moc>
