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
#include "schemadialog.h"

#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qpainter.h>
#include <qfiledialog.h>

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

extern char addrCfg[];
extern QString pathLib;

extern QStringList loadNameList(QTermConfig*);
extern void loadAddress(QTermConfig*,int,QTermParam&);
extern void saveAddress(QTermConfig*,int,const QTermParam&);

/*
 *  Constructs a addrDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addrDialog::addrDialog( QWidget* parent, bool partial, const char* name, bool modal, WFlags fl )
    : addrDialogUI( parent, name, modal, fl ),bPartial(partial),nLastItem(-1)
{

	if(bPartial)
	{
		nameListBox->hide();
		Line2->hide();
		addPushButton->hide();
		deletePushButton->hide();
		connectPushButton->hide();
		closePushButton->setText( tr("Cancel") );
		applyPushButton->setText( tr("OK") );
		resize( 440, 360 );
	    setMinimumSize( QSize( 440, 360 ) );
	    setMaximumSize( QSize( 440, 360 ) );
	    setCaption( tr( "Setting" ) );
	}
	else
	{
	    resize( 650, 360 );
	    setMinimumSize( QSize( 650, 360 ) );
	    setMaximumSize( QSize( 650, 360 ) );
	    setCaption( tr( "AddressBook" ) );
		pConf = new QTermConfig(addrCfg);
		nameListBox->insertStringList(loadNameList(pConf));
		if(nameListBox->count()>0)
		{
			loadAddress(pConf,0,param);
			nameListBox->setCurrentItem(0);
		}
		else	// the default
			if(pConf->findSection("default")!=NULL)
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

void addrDialog::onNamechange(QListBoxItem* item)
{
	if(isChanged())
	{
		QMessageBox mb( "QTerm",
			"Setting changed, do you want to save?",
			QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No  | QMessageBox::Escape ,
			0,this,0,true);
		if ( mb.exec() == QMessageBox::Yes )
		{
			updateData(true);
			if(nLastItem!=-1)
			{
				saveAddress(pConf,nLastItem,param);
				nameListBox->changeItem(param.m_strName, nLastItem);
				nameListBox->setCurrentItem(item);
				return;
			}
		}
	}
	nLastItem = nameListBox->index(item);
	loadAddress(pConf,nameListBox->index(item),param);
	updateData(false);
}

void addrDialog::onAdd()
{
	QString strTmp;
	strTmp = pConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	int index = nameListBox->currentItem();

	// change section names after the insert point
	QString strSection;
	for(int i=num-1; i>index; i--)
	{
		strSection.sprintf("bbs %d",i);
		strTmp.sprintf("bbs %d",i+1);
		pConf->setSectionName(strSection,strTmp);
	}
	// add list number by one
	strTmp.setNum(num+1);
	pConf->setItemValue("bbs list", "num", strTmp);
	// update the data
	updateData(true);
	saveAddress(pConf,index+1,param);

	// insert it to the listbox
	nameListBox->insertItem(param.m_strName,index+1);
	nameListBox->setSelected(index+1, true);
}
void addrDialog::onDelete()
{
	QString strTmp;
	strTmp = pConf->getItemValue("bbs list", "num");
	int num = strTmp.toInt();

	if(nameListBox->count()==0)
		return;
	int index = nameListBox->currentItem();
	
	// delete the section
	QString strSection;
	strSection.sprintf("bbs %d",index);
	pConf->deleteSection(strSection);
	// change the number after that
	for(int i=index+1; i<num; i++)
	{
		strSection.sprintf("bbs %d",i);
		strTmp.sprintf("bbs %d",i-1);
		pConf->setSectionName(strSection,strTmp);
	}
	// ass list number by one
	strTmp.setNum(QMAX(0,num-1));
	pConf->setItemValue("bbs list", "num", strTmp);
	// delete it from name listbox
	loadAddress(pConf,QMIN(index,num-2),param);
	updateData(false);
	nameListBox->removeItem(index);
	nameListBox->setSelected(QMIN(index,nameListBox->count()-1),true);
}
void addrDialog::onApply()
{
	updateData(true);
	if(!bPartial)
	{
		saveAddress(pConf,nameListBox->currentItem(),param);
		if(nameListBox->count()!=0)
			nameListBox->changeItem(param.m_strName, nameListBox->currentItem());
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
			0,this,0,true);
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
	}
}

void addrDialog::onProtocol(int n)
{
	if(n==0)
	{
		sshuserLineEdit->setEnabled(false);
		sshpasswdLineEdit->setEnabled(false);
	}
	else
	{
		QMessageBox::information(this, "sorry", "We are trying to bring you this function soon :)");
		//sshuserLineEdit->setEnabled(true);
		//sshpasswdLineEdit->setEnabled(true);
		protocolComboBox->setCurrentItem(0);
	}
}
void addrDialog::onAutologin(bool on)
{
	preloginLineEdit->setEnabled(on);
	userLineEdit->setEnabled(on);
	passwdLineEdit->setEnabled(on);
	postloginLineEdit->setEnabled(on);	
}
void addrDialog::onAuth(bool on)
{
	proxyuserLineEdit->setEnabled(on);
	proxypasswdLineEdit->setEnabled(on);
}

void addrDialog::onAutoReply(bool on)
{
	replyLineEdit->setEnabled(on);
}

void addrDialog::onReconnect(bool on)
{
	reconnectLineEdit->setEnabled(on);
	retryLineEdit->setEnabled(on);
}

void addrDialog::onLoadScript(bool on)
{
	scriptLineEdit->setEnabled(on);
	scriptPushButton->setEnabled(on);
}

void addrDialog::onChooseScript()
{
	QString path;
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
	path=pathLib+"script";
#else
	path=QDir::homeDirPath()+"/.qterm/script";
#endif
	
	QString strFile = QFileDialog::getOpenFileName(
							path,
							"Python File (*.py)", this,
							"open file dialog"
							"choose a script file" );

	if(strFile.isEmpty())
		return;

	QFileInfo file(strFile);
	
	scriptLineEdit->setText(file.baseName());
}

void addrDialog::onMenuType(int id)
{
	QColor color;
	switch(id)
	{
		case 0:
			menucolorButton->setEnabled(false);
			break;
		case 1:
			menucolorButton->setEnabled(false);
			break;
		case 2:
			menucolorButton->setEnabled(true);
			break;
		case 3:
			color=QColorDialog::getColor(clrMenu);
			if(color.isValid()==TRUE)
			{
				clrMenu=color;
				menuLabel->setBackgroundColor(color);
			}
			break;
	}
}

void addrDialog::setLabelPixmap()
{
	QPixmap pxm(displayTextLabel->size());
	QPainter p;
	QFont font(strFontName,nFontSize);
	QString strTmp;
	strTmp.setNum(nFontSize);
	p.begin(&pxm);
	p.setBackgroundMode(Qt::TransparentMode);
	p.setFont(font);
	p.setPen(clrFg);
	p.fillRect( displayTextLabel->rect(), QBrush(clrBg));
	p.drawText(displayTextLabel->rect(), Qt::AlignHCenter|Qt::AlignVCenter,strFontName+" "+strTmp);
	displayTextLabel->setPixmap(pxm);
	p.end();
}

void addrDialog::connectSlots()
{
	connect( nameListBox, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(onNamechange(QListBoxItem*)) );
	connect( nameListBox, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(onConnect()));

	connect( addPushButton, SIGNAL(clicked()), this, SLOT(onAdd()) );
	connect( deletePushButton, SIGNAL(clicked()), this, SLOT(onDelete()) );
	connect( applyPushButton, SIGNAL(clicked()), this, SLOT(onApply()) );
	connect( closePushButton, SIGNAL(clicked()), this, SLOT(onClose()) );
	connect( connectPushButton, SIGNAL(clicked()), this, SLOT(onConnect()) );

	connect( fontPushButton, SIGNAL(clicked()), this, SLOT(onFont()) );
	connect( fgcolorPushButton, SIGNAL(clicked()), this, SLOT(onFgcolor()) );
	connect( bgcolorPushButton, SIGNAL(clicked()), this, SLOT(onBgcolor()) );
	connect( schemaPushButton, SIGNAL(clicked()), this, SLOT(onSchema()) );

	connect( protocolComboBox, SIGNAL(activated(int)), this, SLOT(onProtocol(int)) );
	
	connect( autoLoginCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAutologin(bool)) );
	connect( authCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAuth(bool)) );

	connect( replyCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAutoReply(bool)));
	connect( reconnectCheckBox, SIGNAL(toggled(bool)), this, SLOT(onReconnect(bool)));
	
	connect( scriptCheckBox, SIGNAL(toggled(bool)), this, SLOT(onLoadScript(bool)));
	connect( scriptPushButton, SIGNAL(clicked()), this, SLOT(onChooseScript()));
	
	connect( menuGroup, SIGNAL(clicked(int)), this, SLOT(onMenuType(int)));
}

bool addrDialog::isChanged()
{
	return( param.m_strName != nameLineEdit->text() ||
		param.m_strAddr != addrLineEdit->text() ||
		param.m_uPort != portLineEdit->text().toUShort() ||
		param.m_nHostType != hostTypeComboBox->currentItem() ||
		param.m_bAutoLogin != autoLoginCheckBox->isChecked() ||
		param.m_strPreLogin != preloginLineEdit->text() ||
		param.m_strUser != userLineEdit->text() ||
		param.m_strPasswd != passwdLineEdit->text() ||
		param.m_strPostLogin != postloginLineEdit->text() ||
		param.m_nBBSCode != bbscodeComboBox->currentItem() ||
		param.m_nDispCode != displaycodeComboBox->currentItem() ||
		param.m_bAutoFont != autofontCheckBox->isChecked() ||
		param.m_bAlwaysHighlight != highlightCheckBox->isChecked() ||
		param.m_bAnsiColor != ansicolorCheckBox->isChecked() ||
		param.m_strFontName != strFontName ||
		param.m_nFontSize != nFontSize ||
		param.m_clrBg != clrBg ||
		param.m_clrFg != clrFg ||
		param.m_strSchemaFile != strSchemaFile ||
		param.m_strTerm != termtypeLineEdit->text() ||
		param.m_nKey != keytypeComboBox->currentItem() ||
		param.m_nCol != columnLineEdit->text().toInt() ||
		param.m_nRow != rowLineEdit->text().toInt() ||
		param.m_nScrollLines != scrollLineEdit->text().toInt() ||
		param.m_nCursorType != cursorTypeComboBox->currentItem() ||
		param.m_strEscape != escapeLineEdit->text() ||
		param.m_nProxyType != proxytypeComboBox->currentItem() ||
		param.m_strProxyHost != proxyaddrLineEdit->text() ||
		param.m_uProxyPort != proxyportLineEdit->text().toUShort() ||
		param.m_bAuth != authCheckBox->isChecked() ||
		param.m_strProxyUser != proxyuserLineEdit->text() ||
		param.m_strProxyPasswd != proxypasswdLineEdit->text() ||
		param.m_nProtocolType != protocolComboBox->currentItem() ||
		param.m_strSSHUser != sshuserLineEdit->text() ||
		param.m_strSSHPasswd != sshpasswdLineEdit->text() ||
		param.m_nMaxIdle != idletimeLineEdit->text().toInt() ||
		param.m_strAntiString != antiLineEdit->text() ||
		param.m_strAutoReply != replyLineEdit->text()) ||
		param.m_bAutoReply != replyCheckBox->isChecked() ||
		param.m_bReconnect != reconnectCheckBox->isChecked() ||
		param.m_nReconnectInterval != reconnectLineEdit->text().toInt() ||
		param.m_nRetry != retryLineEdit->text().toInt() ||
		param.m_bLoadScript != scriptCheckBox->isChecked() ||
		param.m_strScriptFile != scriptLineEdit->text() ||
		param.m_nMenuType != menuGroup->id(menuGroup->selected()) ||
		param.m_clrMenu != clrMenu;
		
}

void addrDialog::updateData(bool save)
{
	if(save) // from display to param
	{
		param.m_strName = nameLineEdit->text();
		param.m_strAddr = addrLineEdit->text();
		param.m_uPort = portLineEdit->text().toUShort();
		param.m_nHostType = hostTypeComboBox->currentItem();
		param.m_bAutoLogin = autoLoginCheckBox->isChecked();
		param.m_strPreLogin = preloginLineEdit->text();
		param.m_strUser = userLineEdit->text();
		param.m_strPasswd = passwdLineEdit->text();
		param.m_strPostLogin = postloginLineEdit->text();
		param.m_nBBSCode = bbscodeComboBox->currentItem();
		param.m_nDispCode = displaycodeComboBox->currentItem();
		param.m_bAutoFont = autofontCheckBox->isChecked();
		param.m_bAlwaysHighlight = highlightCheckBox->isChecked();
		param.m_bAnsiColor = ansicolorCheckBox->isChecked();
		param.m_strFontName = strFontName;
		param.m_nFontSize = nFontSize;
		param.m_clrBg = clrBg;
		param.m_clrFg = clrFg;
		param.m_strSchemaFile = strSchemaFile;
		param.m_strTerm = termtypeLineEdit->text();
		param.m_nKey = keytypeComboBox->currentItem();
		param.m_nCol = columnLineEdit->text().toInt();
		param.m_nRow = rowLineEdit->text().toInt();
		param.m_nScrollLines = scrollLineEdit->text().toInt();
		param.m_nCursorType = cursorTypeComboBox->currentItem();
		param.m_strEscape = escapeLineEdit->text();
		param.m_nProxyType = proxytypeComboBox->currentItem();
		param.m_strProxyHost = proxyaddrLineEdit->text();
		param.m_uProxyPort = proxyportLineEdit->text().toUShort();
		param.m_bAuth = authCheckBox->isChecked();
		param.m_strProxyUser = proxyuserLineEdit->text();
		param.m_strProxyPasswd = proxypasswdLineEdit->text();
		param.m_nProtocolType = protocolComboBox->currentItem();
		param.m_strSSHUser = sshuserLineEdit->text();
		param.m_strSSHPasswd = sshpasswdLineEdit->text();
		param.m_nMaxIdle = idletimeLineEdit->text().toInt();
		param.m_strAntiString = antiLineEdit->text();
		param.m_bAutoReply = replyCheckBox->isChecked();
		param.m_strAutoReply = replyLineEdit->text();
		param.m_bReconnect = reconnectCheckBox->isChecked();
		param.m_nReconnectInterval = reconnectLineEdit->text().toInt();
		param.m_nRetry = retryLineEdit->text().toInt();
		param.m_bLoadScript = scriptCheckBox->isChecked();
		param.m_strScriptFile = scriptLineEdit->text();
		param.m_nMenuType = menuGroup->id(menuGroup->selected());
		param.m_clrMenu = clrMenu;
	}
	else	// from param to display
	{
		QString strTmp;
		nameLineEdit->setText(param.m_strName );
		addrLineEdit->setText(param.m_strAddr );
		strTmp.setNum(param.m_uPort);
		portLineEdit->setText(strTmp);
		hostTypeComboBox->setCurrentItem(param.m_nHostType);
		autoLoginCheckBox->setChecked(param.m_bAutoLogin);
		onAutologin(param.m_bAutoLogin);
		preloginLineEdit->setText(param.m_strPreLogin);
		userLineEdit->setText(param.m_strUser);
		passwdLineEdit->setText(param.m_strPasswd );
		postloginLineEdit->setText(param.m_strPostLogin);
		bbscodeComboBox->setCurrentItem(param.m_nBBSCode);
		displaycodeComboBox->setCurrentItem(param.m_nDispCode);
		autofontCheckBox->setChecked(param.m_bAutoFont );
		highlightCheckBox->setChecked(param.m_bAlwaysHighlight);
		ansicolorCheckBox->setChecked(param.m_bAnsiColor );
		strFontName=param.m_strFontName; 
		nFontSize=param.m_nFontSize ;
		clrBg=param.m_clrBg;
		clrFg=param.m_clrFg;
		strSchemaFile=param.m_strSchemaFile;
		setLabelPixmap();
		termtypeLineEdit->setText(param.m_strTerm);
		keytypeComboBox->setCurrentItem(param.m_nKey);
		strTmp.setNum(param.m_nCol);
		columnLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nRow);
		rowLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nScrollLines);
		scrollLineEdit->setText(strTmp);
		cursorTypeComboBox->setCurrentItem(param.m_nCursorType);
		escapeLineEdit->setText(param.m_strEscape);
		proxytypeComboBox->setCurrentItem(param.m_nProxyType);
		proxyaddrLineEdit->setText(param.m_strProxyHost);
		strTmp.setNum(param.m_uProxyPort);
		proxyportLineEdit->setText(strTmp);
		authCheckBox->setChecked(param.m_bAuth);
		onAuth(param.m_bAuth);
		proxyuserLineEdit->setText(param.m_strProxyUser);
		proxypasswdLineEdit->setText(param.m_strProxyPasswd);
		protocolComboBox->setCurrentItem(param.m_nProtocolType);
		onProtocol(param.m_nProtocolType);
		sshuserLineEdit->setText(param.m_strSSHUser);
		sshpasswdLineEdit->setText(param.m_strSSHPasswd);
		strTmp.setNum(param.m_nMaxIdle);
		idletimeLineEdit->setText(strTmp);
		antiLineEdit->setText(param.m_strAntiString);
		replyCheckBox->setChecked(param.m_bAutoReply);
		onAutoReply(param.m_bAutoReply);
		replyLineEdit->setText(param.m_strAutoReply);
		reconnectCheckBox->setChecked(param.m_bReconnect);
		onReconnect(param.m_bReconnect);
		strTmp.setNum(param.m_nReconnectInterval);
		reconnectCheckBox->setChecked(param.m_bReconnect);
		reconnectLineEdit->setText(strTmp);
		strTmp.setNum(param.m_nRetry);
		retryLineEdit->setText(strTmp);
		scriptCheckBox->setChecked(param.m_bLoadScript);
		scriptLineEdit->setText(param.m_strScriptFile);
		onLoadScript(param.m_bLoadScript);
		menuGroup->setButton(param.m_nMenuType);
		clrMenu = param.m_clrMenu;
		menuLabel->setBackgroundColor(param.m_clrMenu);
		onMenuType(param.m_nMenuType);
	}
}
