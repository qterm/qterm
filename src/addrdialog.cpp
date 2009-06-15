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
#include "qtermglobal.h"
//Added by qt3to4:

#include "schemedialog.h"
#include <QPixmap>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QPainter>
#include <QFileDialog>
#include <QPalette>
#include <QtCore/QTextCodec>
namespace QTerm
{

/*
 *  Constructs a addrDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addrDialog::addrDialog(QWidget* parent, bool partial, Qt::WFlags fl)
        : QDialog(parent, fl), bPartial(partial), bgMenu(this), nLastItem(-1)
{
    ui.setupUi(this);
    ui.portSpinBox->setRange(0, 65535);
    ui.proxyportSpinBox->setRange(0, 65535);
    QList<QByteArray> codecList = QTextCodec::availableCodecs();
    QByteArray codecName;
    foreach(codecName, codecList) {
        ui.bbscodeComboBox->addItem(QString::fromLatin1(codecName));
    }
    if (bPartial) {
        ui.nameListWidget->hide();
        ui.Line->hide();
        ui.addPushButton->hide();
        ui.deletePushButton->hide();
        ui.connectPushButton->hide();
        ui.closePushButton->setText(tr("Cancel"));
        ui.applyPushButton->setText(tr("OK"));
        ui.closePushButton->move(ui.closePushButton->x() - 210, ui.closePushButton->y());
        ui.applyPushButton->move(ui.applyPushButton->x() - 110, ui.applyPushButton->y());
        ui.resetPushButton->move(ui.resetPushButton->x() - 210, ui.resetPushButton->y());
        ui.tabWidget->move(ui.tabWidget->x() - 210, ui.tabWidget->y());
        resize(600, 600);
        setMinimumSize(QSize(600, 600));
        setMaximumSize(QSize(600, 600));
        setWindowTitle(tr("Setting"));
    } else {
        resize(800, 600);
        setMinimumSize(QSize(800, 600));
        setMaximumSize(QSize(800, 600));
        setWindowTitle(tr("AddressBook"));
        ui.nameListWidget->addItems(Global::instance()->loadNameList());
        if (ui.nameListWidget->count() > 0) {
            Global::instance()->loadAddress(0, param);
            ui.nameListWidget->setCurrentRow(0);
        } else // the default
            if (Global::instance()->addrCfg()->hasSection("default"))
                Global::instance()->loadAddress(-1, param);
        updateData(false);
        ui.nameListWidget->setFocus(Qt::OtherFocusReason);
    }
    connectSlots();
}

/*
 *  Destroys the object and frees any allocated resources
 */
addrDialog::~addrDialog()
{
}


void addrDialog::onNamechange(int item)
{
    if (isChanged()) {
        QMessageBox mb("QTerm",
                       tr("Setting changed, do you want to save?"),
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this, 0);
        if (mb.exec() == QMessageBox::Yes) {
            updateData(true);
            if (nLastItem != -1) {
                Global::instance()->saveAddress(nLastItem, param);
                ui.nameListWidget->item(nLastItem)->setText(param.m_strName);
                ui.nameListWidget->setCurrentRow(item);
                return;
            }
        }
    }
    nLastItem = item;
    Global::instance()->loadAddress(item, param);
    updateData(false);
}

void addrDialog::onAdd()
{
    QString strTmp;
    Config * pConf = Global::instance()->addrCfg();
    strTmp = pConf->getItemValue("bbs list", "num").toString();
    int num = strTmp.toInt();

    int index = ui.nameListWidget->currentRow();

    // change section names after the insert point
    QString strSection;
    for (int i = num - 1; i > index; i--) {
        strSection = QString("bbs %1").arg(i);
        strTmp = QString("bbs %1").arg(i + 1);
        //strSection.sprintf("bbs %d",i);
        //strTmp.sprintf("bbs %d",i+1);
        pConf->renameSection(strSection, strTmp);
    }
    // add list number by one
    strTmp.setNum(num + 1);
    pConf->setItemValue("bbs list", "num", strTmp);
    // update the data
    updateData(true);
    Global::instance()->saveAddress(index + 1, param);

    // insert it to the listbox
    ui.nameListWidget->insertItem(index + 1, param.m_strName);
    ui.nameListWidget->setItemSelected(ui.nameListWidget->item(index + 1), true);
}
void addrDialog::onDelete()
{
    QString strTmp;
    Config * pConf = Global::instance()->addrCfg();
    strTmp = pConf->getItemValue("bbs list", "num").toString();
    int num = strTmp.toInt();

    if (ui.nameListWidget->count() == 0)
        return;
    int index = ui.nameListWidget->currentRow();

    // delete the section
    QString strSection = QString("bbs %1").arg(index);
//  strSection.sprintf("bbs %d",index);
    Global::instance()->removeAddress(index);
    // change the number after that
    for (int i = index + 1; i < num; i++) {
        strSection = QString("bbs %1").arg(i);
        strTmp = QString("bbs %1").arg(i);
//   strSection.sprintf("bbs %d",i);
//   strTmp.sprintf("bbs %d",i-1);
        pConf->renameSection(strSection, strTmp);
    }
    // ass list number by one
    strTmp.setNum(qMax(0, num - 1));
    pConf->setItemValue("bbs list", "num", strTmp);
    // delete it from name listbox
    Global::instance()->loadAddress(qMin(index, num - 2), param);
    updateData(false);
    ui.nameListWidget->takeItem(index);
    ui.nameListWidget->setItemSelected(ui.nameListWidget->item(qMin(index, ui.nameListWidget->count() - 1)), true);
}
void addrDialog::onApply()
{
    updateData(true);
    if (!bPartial) {
        Global::instance()->saveAddress(ui.nameListWidget->currentRow(), param);
        if (ui.nameListWidget->count() != 0)
            ui.nameListWidget->item(ui.nameListWidget->currentRow())->setText(param.m_strName);
    } else
        done(1);
}
void addrDialog::onClose()
{
    if (!bPartial)
        Global::instance()->addrCfg()->save();
    done(0);
}
void addrDialog::onConnect()
{
    if (isChanged()) {
        QMessageBox mb("QTerm",
                       tr("Setting changed, do you want to save?"),
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this, 0);
        if (mb.exec() == QMessageBox::Yes)
            onApply();
    }
    if (!bPartial)
        Global::instance()->addrCfg()->save();
    done(1);
}

void addrDialog::onReset()
{
    updateData(false);
}

void addrDialog::onASCIIFont(const QFont & font)
{
    strASCIIFontName = font.family();
}

void addrDialog::onGeneralFont(const QFont & font)
{
    strGeneralFontName = font.family();
}

void addrDialog::onFontSize(int size)
{
    nFontSize = size;
}

void addrDialog::onScheme()
{
    schemeDialog scheme(this);

    scheme.setScheme(strSchemeFile);

    if (scheme.exec() == 1) {
        strSchemeFile = scheme.getScheme();
        if (strSchemeFile.isEmpty())
            strSchemeFile = "";
    }
}

void addrDialog::onProtocol(int n)
{
#ifndef SSH_ENABLED
    if (n == 1) {
        QMessageBox::warning(this, "QTerm", tr("SSH support is not compiled, check your OpenSSL and try to recompile QTerm"));
        ui.protocolComboBox->setCurrentIndex(0);
    }
#endif
}

void addrDialog::onChooseScript()
{
    QString path;
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    path = Global::instance()->pathLib() + "scripts";
#else
    path = Global::instance()->pathCfg() + "scripts";
#endif

    QString strFile = QFileDialog::getOpenFileName(
                          this, "choose a script file",
                          path, "Script Files (*.js)");

    if (strFile.isNull())
        return;

    QFileInfo file(strFile);

    ui.scriptLineEdit->setText(file.absoluteFilePath());
}

void addrDialog::onMenuColor()
{
    QColor color = QColorDialog::getColor(clrMenu);
    if (color.isValid() == TRUE) {
        clrMenu = color;
    }
}

void addrDialog::connectSlots()
{
    connect(ui.nameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onNamechange(int)));
    connect(ui.nameListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onConnect()));

    connect(ui.addPushButton, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(ui.deletePushButton, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(ui.applyPushButton, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(ui.closePushButton, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(ui.connectPushButton, SIGNAL(clicked()), this, SLOT(onConnect()));
    connect(ui.resetPushButton, SIGNAL(clicked()), this, SLOT(onReset()));

    connect(ui.schemePushButton, SIGNAL(clicked()), this, SLOT(onScheme()));

    connect(ui.protocolComboBox, SIGNAL(activated(int)), this, SLOT(onProtocol(int)));

    connect(ui.scriptPushButton, SIGNAL(clicked()), this, SLOT(onChooseScript()));

    connect(ui.menuColorButton, SIGNAL(clicked()), this, SLOT(onMenuColor()));
    connect(ui.asciiFontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onASCIIFont(const QFont &)));
    connect(ui.generalFontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onGeneralFont(const QFont &)));
    connect(ui.fontSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFontSize(int)));
}

bool addrDialog::isChanged()
{
    return(param.m_strName != ui.nameLineEdit->text() ||
           param.m_strAddr != ui.addrLineEdit->text() ||
           param.m_uPort != ui.portSpinBox->value() ||
           param.m_nHostType != ui.hostTypeComboBox->currentIndex() ||
           param.m_bAutoLogin != ui.autoLoginCheckBox->isChecked() ||
           param.m_strPreLogin != ui.preloginLineEdit->text() ||
           param.m_strUser != ui.userLineEdit->text() ||
           param.m_strPasswd != ui.passwdLineEdit->text() ||
           param.m_strPostLogin != ui.postloginLineEdit->text() ||
           param.m_BBSCode != ui.bbscodeComboBox->currentText() ||
           param.m_nDispCode != ui.displaycodeComboBox->currentIndex() ||
           param.m_bAutoFont != ui.autofontCheckBox->isChecked() ||
           param.m_bAlwaysHighlight != ui.highlightCheckBox->isChecked() ||
           param.m_bAnsiColor != ui.ansicolorCheckBox->isChecked() ||
           param.m_strASCIIFontName != strASCIIFontName ||
           param.m_strGeneralFontName != strGeneralFontName||
           param.m_nFontSize != nFontSize ||
           param.m_strSchemeFile != strSchemeFile ||
           param.m_strTerm != ui.termtypeLineEdit->text() ||
           param.m_nKey != ui.keytypeComboBox->currentIndex() ||
           param.m_nCol != ui.columnLineEdit->text().toInt() ||
           param.m_nRow != ui.rowLineEdit->text().toInt() ||
           param.m_nScrollLines != ui.scrollLineEdit->text().toInt() ||
           param.m_nCursorType != ui.cursorTypeComboBox->currentIndex() ||
           param.m_strEscape != ui.escapeLineEdit->text() ||
           param.m_nProxyType != ui.proxytypeComboBox->currentIndex() ||
           param.m_strProxyHost != ui.proxyaddrLineEdit->text() ||
           param.m_uProxyPort != ui.proxyportSpinBox->value() ||
           param.m_bAuth != ui.authCheckBox->isChecked() ||
           param.m_strProxyUser != ui.proxyuserLineEdit->text() ||
           param.m_strProxyPasswd != ui.proxypasswdLineEdit->text() ||
           param.m_nProtocolType != ui.protocolComboBox->currentIndex() ||
           param.m_nMaxIdle != ui.idletimeLineEdit->text().toInt() ||
           param.m_strAntiString != ui.antiLineEdit->text() ||
           param.m_strReplyKey != ui.replykeyLineEdit->text() ||
           param.m_strAutoReply != ui.replyLineEdit->text()) ||
          param.m_bAutoReply != ui.replyCheckBox->isChecked() ||
          param.m_bReconnect != ui.reconnectCheckBox->isChecked() ||
          param.m_nReconnectInterval != ui.reconnectLineEdit->text().toInt() ||
//  param.m_nRetry != ui.retryLineEdit->text().toInt() ||
          param.m_bLoadScript != ui.scriptCheckBox->isChecked() ||
          param.m_strScriptFile != ui.scriptLineEdit->text() ||
          param.m_nMenuType != ui.menuTypeComboBox->currentIndex() ||
          param.m_clrMenu != clrMenu;

}

void addrDialog::updateData(bool save)
{
    if (save) { // from display to param
        param.m_strName = ui.nameLineEdit->text();
        param.m_strAddr = ui.addrLineEdit->text();
        param.m_uPort = ui.portSpinBox->value();
        param.m_nHostType = ui.hostTypeComboBox->currentIndex();
        param.m_bAutoLogin = ui.autoLoginCheckBox->isChecked();
        param.m_strPreLogin = ui.preloginLineEdit->text();
        param.m_strUser = ui.userLineEdit->text();
        param.m_strPasswd = ui.passwdLineEdit->text();
        param.m_strPostLogin = ui.postloginLineEdit->text();
        param.m_BBSCode = ui.bbscodeComboBox->currentText();
        param.m_nDispCode = ui.displaycodeComboBox->currentIndex();
        param.m_bAutoFont = ui.autofontCheckBox->isChecked();
        param.m_bAlwaysHighlight = ui.highlightCheckBox->isChecked();
        param.m_bAnsiColor = ui.ansicolorCheckBox->isChecked();
        param.m_strASCIIFontName = strASCIIFontName;
        param.m_strGeneralFontName = strGeneralFontName;
        param.m_nFontSize = nFontSize;
        param.m_strSchemeFile = strSchemeFile;
        param.m_strTerm = ui.termtypeLineEdit->text();
        param.m_nKey = ui.keytypeComboBox->currentIndex();
        param.m_nCol = ui.columnLineEdit->text().toInt();
        param.m_nRow = ui.rowLineEdit->text().toInt();
        param.m_nScrollLines = ui.scrollLineEdit->text().toInt();
        param.m_nCursorType = ui.cursorTypeComboBox->currentIndex();
        param.m_strEscape = ui.escapeLineEdit->text();
        param.m_nProxyType = ui.proxytypeComboBox->currentIndex();
        param.m_strProxyHost = ui.proxyaddrLineEdit->text();
        param.m_uProxyPort = ui.proxyportSpinBox->value();
        param.m_bAuth = ui.authCheckBox->isChecked();
        param.m_strProxyUser = ui.proxyuserLineEdit->text();
        param.m_strProxyPasswd = ui.proxypasswdLineEdit->text();
        param.m_nProtocolType = ui.protocolComboBox->currentIndex();
        param.m_nMaxIdle = ui.idletimeLineEdit->text().toInt();
        param.m_strReplyKey = ui.replykeyLineEdit->text();
        if (param.m_strReplyKey.isNull())
            qDebug("saving null");
        param.m_strAntiString = ui.antiLineEdit->text();
        param.m_bAutoReply = ui.replyCheckBox->isChecked();
        param.m_strAutoReply = ui.replyLineEdit->text();
        param.m_bReconnect = ui.reconnectCheckBox->isChecked();
        param.m_nReconnectInterval = ui.reconnectLineEdit->text().toInt();
//  param.m_nRetry = ui.retryLineEdit->text().toInt();
        param.m_bLoadScript = ui.scriptCheckBox->isChecked();
        param.m_strScriptFile = ui.scriptLineEdit->text();
        param.m_nMenuType = ui.menuTypeComboBox->currentIndex();
        param.m_clrMenu = clrMenu;
    } else { // from param to display
        QString strTmp;
        ui.nameLineEdit->setText(param.m_strName);
        ui.addrLineEdit->setText(param.m_strAddr);
        strTmp.setNum(param.m_uPort);
        ui.portSpinBox->setValue(strTmp.toUInt());
        ui.hostTypeComboBox->setCurrentIndex(param.m_nHostType);
        ui.autoLoginCheckBox->setChecked(param.m_bAutoLogin);
        ui.preloginLineEdit->setEnabled(param.m_bAutoLogin);
        ui.userLineEdit->setEnabled(param.m_bAutoLogin);
        ui.passwdLineEdit->setEnabled(param.m_bAutoLogin);
        ui.postloginLineEdit->setEnabled(param.m_bAutoLogin);
        ui.preloginLineEdit->setText(param.m_strPreLogin);
        ui.userLineEdit->setText(param.m_strUser);
        ui.passwdLineEdit->setText(param.m_strPasswd);
        ui.postloginLineEdit->setText(param.m_strPostLogin);
        ui.bbscodeComboBox->setCurrentIndex(ui.bbscodeComboBox->findText(param.m_BBSCode));
        ui.displaycodeComboBox->setCurrentIndex(param.m_nDispCode);
        ui.autofontCheckBox->setChecked(param.m_bAutoFont);
        ui.highlightCheckBox->setChecked(param.m_bAlwaysHighlight);
        ui.ansicolorCheckBox->setChecked(param.m_bAnsiColor);
        strASCIIFontName = param.m_strASCIIFontName;
        ui.asciiFontComboBox->setCurrentFont(QFont(strASCIIFontName));
        strGeneralFontName = param.m_strGeneralFontName;
        ui.generalFontComboBox->setCurrentFont(QFont(strGeneralFontName));
        nFontSize = param.m_nFontSize ;
        ui.fontSizeSpinBox->setValue(nFontSize);
        strSchemeFile = param.m_strSchemeFile;
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
        ui.proxyportSpinBox->setValue(strTmp.toUInt());
        ui.authCheckBox->setChecked(param.m_bAuth);
        ui.proxyuserLineEdit->setEnabled(param.m_bAuth);
        ui.proxypasswdLineEdit->setEnabled(param.m_bAuth);
        ui.proxyuserLineEdit->setText(param.m_strProxyUser);
        ui.proxypasswdLineEdit->setText(param.m_strProxyPasswd);
        ui.protocolComboBox->setCurrentIndex(param.m_nProtocolType);
        onProtocol(param.m_nProtocolType);
        strTmp.setNum(param.m_nMaxIdle);
        ui.idletimeLineEdit->setText(strTmp);
        ui.replykeyLineEdit->setText(param.m_strReplyKey);
        ui.antiLineEdit->setText(param.m_strAntiString);
        ui.replyCheckBox->setChecked(param.m_bAutoReply);
        ui.replyLineEdit->setEnabled(param.m_bAutoReply);
        ui.replyLineEdit->setText(param.m_strAutoReply);
        ui.reconnectCheckBox->setChecked(param.m_bReconnect);
        ui.reconnectLineEdit->setEnabled(param.m_bReconnect);
//  ui.retryLineEdit->setEnabled(param.m_bReconnect);
        strTmp.setNum(param.m_nReconnectInterval);
        ui.reconnectLineEdit->setText(strTmp);
        strTmp.setNum(param.m_nRetry);
//  ui.retryLineEdit->setText(strTmp);
        ui.scriptCheckBox->setChecked(param.m_bLoadScript);
        ui.scriptLineEdit->setEnabled(param.m_bLoadScript);
        ui.scriptPushButton->setEnabled(param.m_bLoadScript);
        ui.scriptLineEdit->setText(param.m_strScriptFile);
        ui.menuTypeComboBox->setCurrentIndex(param.m_nMenuType);
        //ui.menuGroup->setButton(param.m_nMenuType);
        //QRadioButton * rbMenu = qobject_cast<QRadioButton*>(bgMenu.button(param.m_nMenuType));
        //rbMenu->setChecked(true);
        clrMenu = param.m_clrMenu;
    }
}

} // namespace QTerm

#include <addrdialog.moc>

