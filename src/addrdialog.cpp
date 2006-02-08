/****************************************************************************
** Form implementation generated from reading ui file 'addrdialog.ui'
**
** Created: Sun Dec 15 20:55:21 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "addrdialog.h"

#include "qtermparam.h"
#include "qtermconfig.h"
//Added by qt3to4:

#include "schemadialog.h"
#include <QPixmap>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QPainter>
#include <QFileDialog>
#include <QPalette>

extern QString addrCfg;
extern QString pathLib;
extern QString pathCfg;

extern QStringList loadNameList(QTermConfig*);
extern bool loadAddress(QTermConfig*,int,QTermParam&);
extern void saveAddress(QTermConfig*,int,const QTermParam&);

/*
 *  Constructs a addrDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addrDialog::addrDialog( QWidget* parent, bool partial, Qt::WFlags fl )
	: QDialog( parent, fl ),bPartial(partial),bgMenu(this),nLastItem(-1)
{
	ui.setupUi(this);
	bgMenu.addButton(ui.radioButton1,0);
	bgMenu.addButton(ui.radioButton2,1);
	bgMenu.addButton(ui.radioButton3,2);
	if(bPartial)
	{
		ui.nameListWidget->hide();
		ui.Line->hide();
		ui.addPushButton->hide();
		ui.deletePushButton->hide();
		ui.connectPushButton->hide();
		ui.closePushButton->setText( tr("Cancel") );
		ui.applyPushButton->setText( tr("OK") );
		resize( 440, 360 );
	    setMinimumSize( QSize( 440, 360 ) );
	    setMaximumSize( QSize( 440, 360 ) );
		setWindowTitle( tr( "Setting" ) );
	}
	else
	{
	    resize( 650, 360 );
	    setMinimumSize( QSize( 650, 360 ) );
	    setMaximumSize( QSize( 650, 360 ) );
		setWindowTitle( tr( "AddressBook" ) );
		pConf = new QTermConfig(addrCfg.toLocal8Bit());
		ui.nameListWidget->addItems(loadNameList(pConf));
		if(ui.nameListWidget->count()>0)
		{
			loadAddress(pConf,0,param);
			ui.nameListWidget->setCurrentRow(0);
		}
		else	// the default
			if(pConf->hasSection("default"))
				loadAddress(pConf,-1,param);
		updateData(false);
	}
	connectSlots();
}

/*
 *  Destroys the object and frees any allocated resources
 */
addrDialog::~addrDialog()
{
    // no need to delete child widgets, Qt does it all for us
	if(!bPartial)
		delete pConf;
}


void addrDialog::onNamechange(int item)
{
	if(isChanged())
	{
		QMessageBox mb( "QTerm",
			"Setting changed, do you want to save?",
			QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No  | QMessageBox::Escape ,
			0,this,0);
		if ( mb.exec() == QMessageBox::Yes )
		{
			updateData(true);
			if(nLastItem!=-1)
			{
				saveAddress(pConf,nLastItem,param);
				ui.nameListWidget->item(nLastItem)->setText(param.m_strName);
				ui.nameListWidget->setCurrentRow(item);
				return;
			}
		}
	}
	nLastItem = item;
	loadAddress(pConf,item,param);
	qDebug("item changed");
	updateData(false);
}

void addrDialog::onAdd()
{
	QString strTmp;
	strTmp = pConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	int index = ui.nameListWidget->currentRow();

	// change section names after the insert point
	QString strSection;
	for(int i=num-1; i>index; i--)
	{
		strSection = QString("bbs %1").arg(i);
		strTmp = QString("bbs %1").arg(i+1);
		//strSection.sprintf("bbs %d",i);
		//strTmp.sprintf("bbs %d",i+1);
		pConf->renameSection(strSection,strTmp);
	}
	// add list number by one
	strTmp.setNum(num+1);
	pConf->setItemValue("bbs list", "num", strTmp);
	// update the data
	updateData(true);
	saveAddress(pConf,index+1,param);

	// insert it to the listbox
	ui.nameListWidget->insertItem(index+1, param.m_strName);
	ui.nameListWidget->setItemSelected(ui.nameListWidget->item(index+1), true);
}
void addrDialog::onDelete()
{
	QString strTmp;
	strTmp = pConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	if(ui.nameListWidget->count()==0)
		return;
	int index = ui.nameListWidget->currentRow();
	
	// delete the section
	QString strSection = QString("bbs %1").arg(index);
// 	strSection.sprintf("bbs %d",index);
	pConf->deleteSection(strSection);
	// change the number after that
	for(int i=index+1; i<num; i++)
	{
		strSection = QString("bbs %1").arg(i);
		strTmp = QString("bbs %1").arg(i);
// 		strSection.sprintf("bbs %d",i);
// 		strTmp.sprintf("bbs %d",i-1);
		pConf->renameSection(strSection,strTmp);
	}
	// ass list number by one
	strTmp.setNum(qMax(0,num-1));
	pConf->setItemValue("bbs list", "num", strTmp);
	// delete it from name listbox
	loadAddress(pConf,qMin(index,num-2),param);
	updateData(false);
	ui.nameListWidget->takeItem(index);
	ui.nameListWidget->setItemSelected(ui.nameListWidget->item(qMin(index,ui.nameListWidget->count()-1)), true);
}
void addrDialog::onApply()
{
	updateData(true);
	if(!bPartial)
	{
		saveAddress(pConf,ui.nameListWidget->currentRow(),param);
		if(ui.nameListWidget->count()!=0)
			ui.nameListWidget->item(ui.nameListWidget->currentRow())->setText(param.m_strName);
	}
	else
		done(1);
}
void addrDialog::onClose()
{
	if(!bPartial)
		pConf->save(addrCfg);
	done(0);
}
void addrDialog::onConnect()
{
	if(isChanged())
	{
		QMessageBox mb( "QTerm",
			"Setting changed, do you want to save?",
			QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No  | QMessageBox::Escape ,
			0,this,0);
		if ( mb.exec() == QMessageBox::Yes )
			onApply();
	}
	if(!bPartial)
		pConf->save(addrCfg);
	done(1);
}

void addrDialog::onFont()
{
	bool ok;
	QFont now(strFontName,nFontSize);

	QFont font=QFontDialog::getFont( &ok, now );
	if(ok==true)
	{
		strFontName = font.family();
		nFontSize = font.pointSize();
		setLabelPixmap();
	}
}
void addrDialog::onFgcolor()
{
	QColor color=QColorDialog::getColor(clrFg);
	if(color.isValid()==TRUE)
	{
		clrFg=color;
		setLabelPixmap();
	}
}
void addrDialog::onBgcolor()
{
	QColor color=QColorDialog::getColor(clrBg);
	if(color.isValid()==TRUE)
	{
		clrBg=color;
		setLabelPixmap();
	}
}
void addrDialog::onSchema()
{
	schemaDialog schema(this);

	schema.setSchema(strSchemaFile);

	if(schema.exec()==1)
	{
		strSchemaFile = schema.getSchema();
		if(strSchemaFile.isEmpty())
			strSchemaFile="";
	}
}

void addrDialog::onProtocol(int n)
{
	if (n == 0)
	{
		ui.sshuserLineEdit->setEnabled(false);
		ui.sshpasswdLineEdit->setEnabled(false);
	}
	else 
	if (n == 1)
	{
#if defined(_NO_SSH_COMPILED)
		QMessageBox::warning(this, "sorry", "SSH support is not compiled, check your OpenSSL and try to recompile QTerm");
		ui.protocolComboBox->setCurrentItem(0);
		ui.sshuserLineEdit->setEnabled(false);
		ui.sshpasswdLineEdit->setEnabled(false);
#else
		ui.sshuserLineEdit->setEnabled(true);
		ui.sshpasswdLineEdit->setEnabled(true);
#endif
	}
	else if (n == 2)
	{
		QMessageBox::information(this, "sorry", "SSH support is not completed yet, We are trying to bring you this function soon :)");
		ui.protocolComboBox->setCurrentIndex(0);
		ui.sshuserLineEdit->setEnabled(false);
		ui.sshpasswdLineEdit->setEnabled(false);
	}
}

void addrDialog::onChooseScript()
{
	QString path;
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
	path=pathLib+"script";
#else
	path=pathCfg+"script";
#endif
	
	QString strFile = QFileDialog::getOpenFileName(
							this, "choose a script file",
							path, "Python File (*.py)");

	if(strFile.isNull())
		return;

	QFileInfo file(strFile);
	
	ui.scriptLineEdit->setText(file.baseName());
}

void addrDialog::onMenuColor()
{
	QColor color=QColorDialog::getColor(clrMenu);
	if(color.isValid()==TRUE)
	{
		clrMenu=color;
// 		FIXME: How to demostrate the new color?
// 		menuLabel->setBackgroundColor(color);
	}
}

void addrDialog::setLabelPixmap()
{
	QPixmap pxm(ui.displayTextLabel->size());
	QPainter p;
	QFont font(strFontName,nFontSize);
	QString strTmp;
	strTmp.setNum(nFontSize);
	p.begin(&pxm);
	p.setBackgroundMode(Qt::TransparentMode);
	p.setFont(font);
	p.setPen(clrFg);
	p.fillRect( ui.displayTextLabel->rect(), QBrush(clrBg));
	p.drawText(ui.displayTextLabel->rect(), Qt::AlignHCenter|Qt::AlignVCenter,strFontName+" "+strTmp);
	ui.displayTextLabel->setPixmap(pxm);
	p.end();
}

void addrDialog::connectSlots()
{
	connect( ui.nameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onNamechange(int)) );
	connect( ui.nameListWidget, SIGNAL(doubleClicked(QListWidgetItem*)), this, SLOT(onConnect()));
	connect( ui.nameListWidget, SIGNAL(returnPressed(QListWidgetItem*)), this, SLOT(onConnect()));

	connect( ui.addPushButton, SIGNAL(clicked()), this, SLOT(onAdd()) );
	connect( ui.deletePushButton, SIGNAL(clicked()), this, SLOT(onDelete()) );
	connect( ui.applyPushButton, SIGNAL(clicked()), this, SLOT(onApply()) );
	connect( ui.closePushButton, SIGNAL(clicked()), this, SLOT(onClose()) );
	connect( ui.connectPushButton, SIGNAL(clicked()), this, SLOT(onConnect()) );

	connect( ui.fontPushButton, SIGNAL(clicked()), this, SLOT(onFont()) );
	connect( ui.fgcolorPushButton, SIGNAL(clicked()), this, SLOT(onFgcolor()) );
	connect( ui.bgcolorPushButton, SIGNAL(clicked()), this, SLOT(onBgcolor()) );
	connect( ui.schemaPushButton, SIGNAL(clicked()), this, SLOT(onSchema()) );

	connect( ui.protocolComboBox, SIGNAL(activated(int)), this, SLOT(onProtocol(int)) );

	connect( ui.scriptPushButton, SIGNAL(clicked()), this, SLOT(onChooseScript()));

	connect( ui.menuColorButton, SIGNAL(clicked()), this, SLOT(onMenuColor()));
}

bool addrDialog::isChanged()
{
	return( param.m_strName != ui.nameLineEdit->text() ||
		param.m_strAddr != ui.addrLineEdit->text() ||
		param.m_uPort != ui.portLineEdit->text().toUShort() ||
		param.m_nHostType != ui.hostTypeComboBox->currentIndex() ||
		param.m_bAutoLogin != ui.autoLoginCheckBox->isChecked() ||
		param.m_strPreLogin != ui.preloginLineEdit->text() ||
		param.m_strUser != ui.userLineEdit->text() ||
		param.m_strPasswd != ui.passwdLineEdit->text() ||
		param.m_strPostLogin != ui.postloginLineEdit->text() ||
		param.m_nBBSCode != ui.bbscodeComboBox->currentIndex() ||
		param.m_nDispCode != ui.displaycodeComboBox->currentIndex() ||
		param.m_bAutoFont != ui.autofontCheckBox->isChecked() ||
		param.m_bAlwaysHighlight != ui.highlightCheckBox->isChecked() ||
		param.m_bAnsiColor != ui.ansicolorCheckBox->isChecked() ||
		param.m_strFontName != strFontName ||
		param.m_nFontSize != nFontSize ||
		param.m_clrBg != clrBg ||
		param.m_clrFg != clrFg ||
		param.m_strSchemaFile != strSchemaFile ||
		param.m_strTerm != ui.termtypeLineEdit->text() ||
		param.m_nKey != ui.keytypeComboBox->currentIndex() ||
		param.m_nCol != ui.columnLineEdit->text().toInt() ||
		param.m_nRow != ui.rowLineEdit->text().toInt() ||
		param.m_nScrollLines != ui.scrollLineEdit->text().toInt() ||
		param.m_nCursorType != ui.cursorTypeComboBox->currentIndex() ||
		param.m_strEscape != ui.escapeLineEdit->text() ||
		param.m_nProxyType != ui.proxytypeComboBox->currentIndex() ||
		param.m_strProxyHost != ui.proxyaddrLineEdit->text() ||
		param.m_uProxyPort != ui.proxyportLineEdit->text().toUShort() ||
		param.m_bAuth != ui.authCheckBox->isChecked() ||
		param.m_strProxyUser != ui.proxyuserLineEdit->text() ||
		param.m_strProxyPasswd != ui.proxypasswdLineEdit->text() ||
		param.m_nProtocolType != ui.protocolComboBox->currentIndex() ||
		param.m_strSSHUser != ui.sshuserLineEdit->text() ||
		param.m_strSSHPasswd != ui.sshpasswdLineEdit->text() ||
		param.m_nMaxIdle != ui.idletimeLineEdit->text().toInt() ||
		param.m_strAntiString != ui.antiLineEdit->text() ||
		param.m_strReplyKey != ui.replykeyLineEdit->text() ||
		param.m_strAutoReply != ui.replyLineEdit->text()) ||
		param.m_bAutoReply != ui.replyCheckBox->isChecked() ||
		param.m_bReconnect != ui.reconnectCheckBox->isChecked() ||
		param.m_nReconnectInterval != ui.reconnectLineEdit->text().toInt() ||
		param.m_nRetry != ui.retryLineEdit->text().toInt() ||
		param.m_bLoadScript != ui.scriptCheckBox->isChecked() ||
		param.m_strScriptFile != ui.scriptLineEdit->text() ||
		param.m_nMenuType != bgMenu.checkedId() ||
		param.m_clrMenu != clrMenu;
		
}

void addrDialog::updateData(bool save)
{
	if(save) // from display to param
	{
		param.m_strName = ui.nameLineEdit->text();
		param.m_strAddr = ui.addrLineEdit->text();
		param.m_uPort = ui.portLineEdit->text().toUShort();
		param.m_nHostType = ui.hostTypeComboBox->currentIndex();
		param.m_bAutoLogin = ui.autoLoginCheckBox->isChecked();
		param.m_strPreLogin = ui.preloginLineEdit->text();
		param.m_strUser = ui.userLineEdit->text();
		param.m_strPasswd = ui.passwdLineEdit->text();
		param.m_strPostLogin = ui.postloginLineEdit->text();
		param.m_nBBSCode = ui.bbscodeComboBox->currentIndex();
		param.m_nDispCode = ui.displaycodeComboBox->currentIndex();
		param.m_bAutoFont = ui.autofontCheckBox->isChecked();
		param.m_bAlwaysHighlight = ui.highlightCheckBox->isChecked();
		param.m_bAnsiColor = ui.ansicolorCheckBox->isChecked();
		param.m_strFontName = strFontName;
		param.m_nFontSize = nFontSize;
		param.m_clrBg = clrBg;
		param.m_clrFg = clrFg;
		param.m_strSchemaFile = strSchemaFile;
		param.m_strTerm = ui.termtypeLineEdit->text();
		param.m_nKey = ui.keytypeComboBox->currentIndex();
		param.m_nCol = ui.columnLineEdit->text().toInt();
		param.m_nRow = ui.rowLineEdit->text().toInt();
		param.m_nScrollLines = ui.scrollLineEdit->text().toInt();
		param.m_nCursorType = ui.cursorTypeComboBox->currentIndex();
		param.m_strEscape = ui.escapeLineEdit->text();
		param.m_nProxyType = ui.proxytypeComboBox->currentIndex();
		param.m_strProxyHost = ui.proxyaddrLineEdit->text();
		param.m_uProxyPort = ui.proxyportLineEdit->text().toUShort();
		param.m_bAuth = ui.authCheckBox->isChecked();
		param.m_strProxyUser = ui.proxyuserLineEdit->text();
		param.m_strProxyPasswd = ui.proxypasswdLineEdit->text();
		param.m_nProtocolType = ui.protocolComboBox->currentIndex();
		param.m_strSSHUser = ui.sshuserLineEdit->text();
		param.m_strSSHPasswd = ui.sshpasswdLineEdit->text();
		param.m_nMaxIdle = ui.idletimeLineEdit->text().toInt();
		param.m_strReplyKey = ui.replykeyLineEdit->text();
		if(param.m_strReplyKey.isNull())
			printf("saving null\n");
		param.m_strAntiString = ui.antiLineEdit->text();
		param.m_bAutoReply = ui.replyCheckBox->isChecked();
		param.m_strAutoReply = ui.replyLineEdit->text();
		param.m_bReconnect = ui.reconnectCheckBox->isChecked();
		param.m_nReconnectInterval = ui.reconnectLineEdit->text().toInt();
		param.m_nRetry = ui.retryLineEdit->text().toInt();
		param.m_bLoadScript = ui.scriptCheckBox->isChecked();
		param.m_strScriptFile = ui.scriptLineEdit->text();
		param.m_nMenuType = bgMenu.checkedId();
		param.m_clrMenu = clrMenu;
	}
	else	// from param to display
	{
		QString strTmp;
		ui.nameLineEdit->setText(param.m_strName );
		ui.addrLineEdit->setText(param.m_strAddr );
		strTmp.setNum(param.m_uPort);
		ui.portLineEdit->setText(strTmp);
		ui.hostTypeComboBox->setCurrentIndex(param.m_nHostType);
		ui.autoLoginCheckBox->setChecked(param.m_bAutoLogin);
		ui.preloginLineEdit->setEnabled(param.m_bAutoLogin);
		ui.userLineEdit->setEnabled(param.m_bAutoLogin);
		ui.passwdLineEdit->setEnabled(param.m_bAutoLogin);
		ui.postloginLineEdit->setEnabled(param.m_bAutoLogin);
		ui.preloginLineEdit->setText(param.m_strPreLogin);
		ui.userLineEdit->setText(param.m_strUser);
		ui.passwdLineEdit->setText(param.m_strPasswd );
		ui.postloginLineEdit->setText(param.m_strPostLogin);
		ui.bbscodeComboBox->setCurrentIndex(param.m_nBBSCode);
		ui.displaycodeComboBox->setCurrentIndex(param.m_nDispCode);
		ui.autofontCheckBox->setChecked(param.m_bAutoFont );
		ui.highlightCheckBox->setChecked(param.m_bAlwaysHighlight);
		ui.ansicolorCheckBox->setChecked(param.m_bAnsiColor );
		strFontName=param.m_strFontName; 
		nFontSize=param.m_nFontSize ;
		clrBg=param.m_clrBg;
		clrFg=param.m_clrFg;
		strSchemaFile=param.m_strSchemaFile;
		setLabelPixmap();
		ui.termtypeLineEdit->setText(param.m_strTerm);
		ui.keytypeComboBox->setCurrentIndex(param.m_nKey);
		strTmp.setNum(param.m_nCol);
		ui.columnLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nRow);
		ui.rowLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nScrollLines);
		ui.scrollLineEdit->setText(strTmp);
		ui.cursorTypeComboBox->setCurrentIndex(param.m_nCursorType);
		ui.escapeLineEdit->setText(param.m_strEscape);
		ui.proxytypeComboBox->setCurrentIndex(param.m_nProxyType);
		ui.proxyaddrLineEdit->setText(param.m_strProxyHost);
		strTmp.setNum(param.m_uProxyPort);
		ui.proxyportLineEdit->setText(strTmp);
		ui.authCheckBox->setChecked(param.m_bAuth);
		ui.proxyuserLineEdit->setEnabled(param.m_bAuth);
		ui.proxypasswdLineEdit->setEnabled(param.m_bAuth);
		ui.proxyuserLineEdit->setText(param.m_strProxyUser);
		ui.proxypasswdLineEdit->setText(param.m_strProxyPasswd);
		ui.protocolComboBox->setCurrentIndex(param.m_nProtocolType);
		onProtocol(param.m_nProtocolType);
		ui.sshuserLineEdit->setText(param.m_strSSHUser);
		ui.sshpasswdLineEdit->setText(param.m_strSSHPasswd);
		strTmp.setNum(param.m_nMaxIdle);
		ui.idletimeLineEdit->setText(strTmp);
		ui.replykeyLineEdit->setText(param.m_strReplyKey);
		ui.antiLineEdit->setText(param.m_strAntiString);
		ui.replyCheckBox->setChecked(param.m_bAutoReply);
		ui.replyLineEdit->setEnabled(param.m_bAutoReply);
		ui.replyLineEdit->setText(param.m_strAutoReply);
		ui.reconnectCheckBox->setChecked(param.m_bReconnect);
		ui.reconnectLineEdit->setEnabled(param.m_bReconnect);
		ui.retryLineEdit->setEnabled(param.m_bReconnect);
		strTmp.setNum(param.m_nReconnectInterval);
		ui.reconnectLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nRetry);
		ui.retryLineEdit->setText(strTmp);
		ui.scriptCheckBox->setChecked(param.m_bLoadScript);
		ui.scriptLineEdit->setEnabled(param.m_bLoadScript);
		ui.scriptPushButton->setEnabled(param.m_bLoadScript);
		ui.scriptLineEdit->setText(param.m_strScriptFile);
		//ui.menuGroup->setButton(param.m_nMenuType);
		QRadioButton * rbMenu = qobject_cast<QRadioButton*>(bgMenu.button(param.m_nMenuType));
		rbMenu->setChecked(true);
		clrMenu = param.m_clrMenu;
		QPalette palette;
		palette.setColor(ui.menuLabel->backgroundRole(), clrMenu);
		ui.menuLabel->setPalette(palette);
// 		ui.menuLabel->setBackgroundColor(param.m_clrMenu);
	}
}
#include <addrdialog.moc>
