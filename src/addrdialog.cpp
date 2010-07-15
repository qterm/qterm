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
    ui.rowSpinBox->setRange(5,500);
    ui.columnSpinBox->setRange(5,500);
    ui.scrollSpinBox->setRange(0,1000000);
    QList<QByteArray> codecList = QTextCodec::availableCodecs();
    QByteArray codecName;
    foreach(codecName, codecList) {
        ui.bbscodeComboBox->addItem(QString::fromLatin1(codecName));
    }
    updateSchemeList();
    updateKeyboardProfiles();
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
    ui.connectPushButton->setDefault(true);
}

/*
 *  Destroys the object and frees any allocated resources
 */
addrDialog::~addrDialog()
{
}

void addrDialog::updateSchemeList()
{
    ui.schemeComboBox->clear();
    schemeFileList = schemeDialog::loadSchemeList();
    foreach (QString file, schemeFileList) {
        Config *pConf = new Config(file);
        ui.schemeComboBox->addItem(pConf->getItemValue("scheme", "title").toString());
        delete pConf;
    }
}

void addrDialog::updateKeyboardProfiles()
{
    QDir dir;
    QFileInfoList lstFile;

    dir.setNameFilters(QStringList("*.keytab"));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    dir.setPath(Global::instance()->pathCfg() + "/keyboard_profiles");
    lstFile = dir.entryInfoList();
    //if( lstFile.count()!=0 )
    {
        foreach(QFileInfo fi, lstFile) {
            keyboardProfileList.append(fi.absoluteFilePath());
        }
    }
#endif

    dir.setPath(Global::instance()->pathLib() + "/keyboard_profiles");
    lstFile = dir.entryInfoList();
    //if(lstFile != NULL)
    {
        foreach(QFileInfo fi, lstFile) {
            keyboardProfileList.append(fi.absoluteFilePath());
        }
    }
    foreach (QString file, keyboardProfileList) {
        QFileInfo fi(file);
        QString base = fi.baseName();
        ui.keytypeComboBox->addItem(base);
    }
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
				ui.nameListWidget->item(nLastItem)->setText(param.m_mapParam["name"].toString());
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
	ui.nameListWidget->insertItem(index + 1, param.m_mapParam["name"].toString());
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
        strTmp = QString("bbs %1").arg(i-1);
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
            ui.nameListWidget->item(ui.nameListWidget->currentRow())->setText(param.m_mapParam["name"].toString());
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

void addrDialog::onConfigScheme()
{
    schemeDialog scheme(this);

    scheme.setScheme(strSchemeFile);

    if (scheme.exec() == 1) {
        updateSchemeList();
        strSchemeFile = scheme.getScheme();
        if (strSchemeFile.isEmpty())
            strSchemeFile = "";
    }
    ui.schemeComboBox->setCurrentIndex(schemeFileList.indexOf(strSchemeFile));
}

void addrDialog::onScheme(int i)
{
    if (i >=0 && i < schemeFileList.size())
        strSchemeFile = schemeFileList[i];
}

void addrDialog::onKeyboardProfile(int i)
{
    if (i >= 0 && i < keyboardProfileList.size()) {
        strKeyboardProfile = keyboardProfileList[i];
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
    ui.portSpinBox->setValue(23 - n);
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

    connect(ui.schemePushButton, SIGNAL(clicked()), this, SLOT(onConfigScheme()));

    connect(ui.protocolComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onProtocol(int)));

    connect(ui.scriptPushButton, SIGNAL(clicked()), this, SLOT(onChooseScript()));

    connect(ui.menuColorButton, SIGNAL(clicked()), this, SLOT(onMenuColor()));
    connect(ui.asciiFontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onASCIIFont(const QFont &)));
    connect(ui.generalFontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onGeneralFont(const QFont &)));
    connect(ui.fontSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFontSize(int)));
    connect(ui.schemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onScheme(int)));
    connect(ui.keytypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onKeyboardProfile(int)));
}

bool addrDialog::isChanged()
{
    return(param.m_mapParam["name"].toString() != ui.nameLineEdit->text() ||
           param.m_mapParam["addr"].toString() != ui.addrLineEdit->text() ||
		   param.m_mapParam["port"].toInt() != ui.portSpinBox->value() ||
		   param.m_mapParam["hosttype"].toInt() != ui.hostTypeComboBox->currentIndex() ||
           param.m_mapParam["autologin"].toBool() != ui.autoLoginCheckBox->isChecked() ||
           param.m_mapParam["prelogin"].toString() != ui.preloginLineEdit->text() ||
           param.m_mapParam["user"].toString() != ui.userLineEdit->text() ||
           param.m_mapParam["password"].toString() != ui.passwdLineEdit->text() ||
           param.m_mapParam["postlogin"].toString() != ui.postloginLineEdit->text() ||
           param.m_mapParam["bbscode"].toString() != ui.bbscodeComboBox->currentText() ||
           param.m_mapParam["displaycode"].toInt() != ui.displaycodeComboBox->currentIndex() ||
           param.m_mapParam["autofont"].toBool() != ui.autofontCheckBox->isChecked() ||
           param.m_mapParam["alwayshighlight"].toBool() != ui.highlightCheckBox->isChecked() ||
           param.m_mapParam["ansicolor"].toBool() != ui.ansicolorCheckBox->isChecked() ||
           param.m_mapParam["asciifont"].toString() != strASCIIFontName ||
           param.m_mapParam["generalfont"].toString() != strGeneralFontName||
           param.m_mapParam["fontsize"].toInt() != nFontSize ||
           param.m_mapParam["schemefile"].toString() != strSchemeFile ||
           param.m_mapParam["keyboardprofile"].toString() != strKeyboardProfile||
           param.m_mapParam["termtype"].toString() != ui.termtypeLineEdit->text() ||
           param.m_mapParam["column"].toInt() != ui.columnSpinBox->value() ||
           param.m_mapParam["row"].toInt() != ui.rowSpinBox->value() ||
           param.m_mapParam["scroll"].toInt() != ui.scrollSpinBox->value() ||
           param.m_mapParam["cursor"].toInt() != ui.cursorTypeComboBox->currentIndex() ||
           param.m_mapParam["escape"].toString() != ui.escapeLineEdit->text() ||
           param.m_mapParam["proxytype"].toInt() != ui.proxytypeComboBox->currentIndex() ||
           param.m_mapParam["proxyaddr"].toString() != ui.proxyaddrLineEdit->text() ||
           param.m_mapParam["proxyport"].toInt() != ui.proxyportSpinBox->value() ||
           param.m_mapParam["proxyauth"].toBool() != ui.authCheckBox->isChecked() ||
           param.m_mapParam["proxyuser"].toString() != ui.proxyuserLineEdit->text() ||
           param.m_mapParam["proxypassword"].toString() != ui.proxypasswdLineEdit->text() ||
           param.m_mapParam["protocol"].toInt() != ui.protocolComboBox->currentIndex() ||
           param.m_mapParam["maxidle"].toInt() != ui.idletimeLineEdit->text().toInt() ||
           param.m_mapParam["antiidlestring"].toString() != ui.antiLineEdit->text() ||
           param.m_mapParam["replykey"].toString() != ui.replykeyLineEdit->text() ||
           param.m_mapParam["autoreply"].toString() != ui.replyLineEdit->text()) ||
           param.m_mapParam["bautoreply"].toBool() != ui.replyCheckBox->isChecked() ||
           param.m_mapParam["reconnect"].toBool() != ui.reconnectCheckBox->isChecked() ||
           param.m_mapParam["interval"].toInt() != ui.reconnectLineEdit->text().toInt() ||
//  param.m_mapParam["retrytimes"].toInt() != ui.retryLineEdit->text().toInt() ||
           param.m_mapParam["loadscript"].toBool() != ui.scriptCheckBox->isChecked() ||
           param.m_mapParam["scriptfile"].toString() != ui.scriptLineEdit->text() ||
           param.m_mapParam["menutype"].toInt() != ui.menuTypeComboBox->currentIndex() ||
           param.m_mapParam["menucolor"] != clrMenu;

}

void addrDialog::updateData(bool save)
{
    if (save) { // from display to param
        param.m_mapParam["name"] = ui.nameLineEdit->text();
        param.m_mapParam["addr"] = ui.addrLineEdit->text();
        param.m_mapParam["port"] = ui.portSpinBox->value();
        param.m_mapParam["hosttype"] = ui.hostTypeComboBox->currentIndex();
        param.m_mapParam["autologin"] = ui.autoLoginCheckBox->isChecked();
        param.m_mapParam["prelogin"] = ui.preloginLineEdit->text();
        param.m_mapParam["user"] = ui.userLineEdit->text();
        param.m_mapParam["password"] = ui.passwdLineEdit->text();
        param.m_mapParam["postlogin"] = ui.postloginLineEdit->text();
        param.m_mapParam["bbscode"] = ui.bbscodeComboBox->currentText();
        param.m_mapParam["displaycode"] = ui.displaycodeComboBox->currentIndex();
        param.m_mapParam["autofont"] = ui.autofontCheckBox->isChecked();
        param.m_mapParam["alwayshighlight"] = ui.highlightCheckBox->isChecked();
        param.m_mapParam["ansicolor"] = ui.ansicolorCheckBox->isChecked();
        param.m_mapParam["asciifont"] = strASCIIFontName;
        param.m_mapParam["generalfont"] = strGeneralFontName;
        param.m_mapParam["fontsize"] = nFontSize;
        param.m_mapParam["schemefile"] = strSchemeFile;
        param.m_mapParam["keyboardprofile"] = strKeyboardProfile;
        param.m_mapParam["termtype"] = ui.termtypeLineEdit->text();
        param.m_mapParam["column"] = ui.columnSpinBox->value();
        param.m_mapParam["row"] = ui.rowSpinBox->value();
        param.m_mapParam["scroll"] = ui.scrollSpinBox->value();
        param.m_mapParam["cursor"] = ui.cursorTypeComboBox->currentIndex();
        param.m_mapParam["escape"] = ui.escapeLineEdit->text();
        param.m_mapParam["proxytype"] = ui.proxytypeComboBox->currentIndex();
        param.m_mapParam["proxyhost"] = ui.proxyaddrLineEdit->text();
        param.m_mapParam["proxyport"] = ui.proxyportSpinBox->value();
        param.m_mapParam["proxyauth"] = ui.authCheckBox->isChecked();
        param.m_mapParam["proxyuser"] = ui.proxyuserLineEdit->text();
        param.m_mapParam["proxypassword"] = ui.proxypasswdLineEdit->text();
        param.m_mapParam["protocol"] = ui.protocolComboBox->currentIndex();
        param.m_mapParam["maxidle"] = ui.idletimeLineEdit->text().toInt();
        param.m_mapParam["replykey"] = ui.replykeyLineEdit->text();
		if (param.m_mapParam["replykey"].toString().isNull())
            qDebug("saving null");
        param.m_mapParam["antiidlestring"] = ui.antiLineEdit->text();
        param.m_mapParam["bautoreply"] = ui.replyCheckBox->isChecked();
        param.m_mapParam["autoreply"] = ui.replyLineEdit->text();
        param.m_mapParam["reconnect"] = ui.reconnectCheckBox->isChecked();
        param.m_mapParam["interval"] = ui.reconnectLineEdit->text().toInt();
//  param.m_mapParam["retrytimes"] = ui.retryLineEdit->text().toInt();
        param.m_mapParam["loadscript"] = ui.scriptCheckBox->isChecked();
        param.m_mapParam["scriptfile"] = ui.scriptLineEdit->text();
        param.m_mapParam["menutype"] = ui.menuTypeComboBox->currentIndex();
        param.m_mapParam["menucolor"] = clrMenu;
    } else { // from param to display
        disconnect(ui.protocolComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onProtocol(int)));
        QString strTmp;
        ui.nameLineEdit->setText(param.m_mapParam["name"].toString());
        ui.addrLineEdit->setText(param.m_mapParam["addr"].toString());
        ui.portSpinBox->setValue(param.m_mapParam["port"].toInt());
        ui.hostTypeComboBox->setCurrentIndex(param.m_mapParam["hosttype"].toInt());
        ui.autoLoginCheckBox->setChecked(param.m_mapParam["autologin"].toBool());
        ui.preloginLineEdit->setEnabled(param.m_mapParam["autologin"].toBool());
        ui.userLineEdit->setEnabled(param.m_mapParam["autologin"].toBool());
        ui.passwdLineEdit->setEnabled(param.m_mapParam["autologin"].toBool());
        ui.postloginLineEdit->setEnabled(param.m_mapParam["autologin"].toBool());
        ui.preloginLineEdit->setText(param.m_mapParam["prelogin"].toString());
        ui.userLineEdit->setText(param.m_mapParam["user"].toString());
        ui.passwdLineEdit->setText(param.m_mapParam["password"].toString());
        ui.postloginLineEdit->setText(param.m_mapParam["postlogin"].toString());
        ui.bbscodeComboBox->setCurrentIndex(ui.bbscodeComboBox->findText(param.m_mapParam["bbscode"].toString()));
        ui.displaycodeComboBox->setCurrentIndex(param.m_mapParam["displaycode"].toInt());
        ui.autofontCheckBox->setChecked(param.m_mapParam["autofont"].toBool());
        ui.highlightCheckBox->setChecked(param.m_mapParam["alwayshighlight"].toBool());
        ui.ansicolorCheckBox->setChecked(param.m_mapParam["ansicolor"].toBool());
        strASCIIFontName = param.m_mapParam["asciifont"].toString();
        ui.asciiFontComboBox->setCurrentFont(QFont(strASCIIFontName));
        strGeneralFontName = param.m_mapParam["generalfont"].toString();
        ui.generalFontComboBox->setCurrentFont(QFont(strGeneralFontName));
        nFontSize = param.m_mapParam["fontsize"].toInt();
        ui.fontSizeSpinBox->setValue(nFontSize);
        strSchemeFile = param.m_mapParam["schemefile"].toString();
        ui.schemeComboBox->setCurrentIndex(schemeFileList.indexOf(strSchemeFile));
        strKeyboardProfile = param.m_mapParam["keyboardprofile"].toString();
        ui.keytypeComboBox->setCurrentIndex(keyboardProfileList.indexOf(strKeyboardProfile));
        ui.termtypeLineEdit->setText(param.m_mapParam["termtype"].toString());
        ui.columnSpinBox->setValue(param.m_mapParam["column"].toInt());
        ui.rowSpinBox->setValue(param.m_mapParam["row"].toInt());
        ui.scrollSpinBox->setValue(param.m_mapParam["scroll"].toInt());
        ui.cursorTypeComboBox->setCurrentIndex(param.m_mapParam["cursor"].toInt());
        ui.escapeLineEdit->setText(param.m_mapParam["escape"].toString());
        ui.proxytypeComboBox->setCurrentIndex(param.m_mapParam["proxytype"].toInt());
        ui.proxyaddrLineEdit->setText(param.m_mapParam["proxyaddr"].toString());
        ui.proxyportSpinBox->setValue(param.m_mapParam["proxyport"].toInt());
        ui.authCheckBox->setChecked(param.m_mapParam["proxyauth"].toBool());
        ui.proxyuserLineEdit->setEnabled(param.m_mapParam["proxyauth"].toBool());
        ui.proxypasswdLineEdit->setEnabled(param.m_mapParam["proxyauth"].toBool());
        ui.proxyuserLineEdit->setText(param.m_mapParam["proxyuser"].toString());
        ui.proxypasswdLineEdit->setText(param.m_mapParam["proxypassword"].toString());
        ui.protocolComboBox->setCurrentIndex(param.m_mapParam["protocol"].toInt());
        ui.idletimeLineEdit->setText(param.m_mapParam["maxidle"].toString());
        ui.replykeyLineEdit->setText(param.m_mapParam["replykey"].toString());
        ui.antiLineEdit->setText(param.m_mapParam["antiidlestring"].toString());
        ui.replyCheckBox->setChecked(param.m_mapParam["bautoreply"].toBool());
        ui.replyLineEdit->setEnabled(param.m_mapParam["bautoreply"].toBool());
        ui.replyLineEdit->setText(param.m_mapParam["autoreply"].toString());
        ui.reconnectCheckBox->setChecked(param.m_mapParam["reconnect"].toBool());
        ui.reconnectLineEdit->setEnabled(param.m_mapParam["reconnect"].toBool());
//  ui.retryLineEdit->setEnabled(param.m_mapParam["reconnect"].toBool());
        ui.reconnectLineEdit->setText(param.m_mapParam["interval"].toString());
//  ui.retryLineEdit->setText(param.m_mapParam["retrytimes"].toString());
        ui.scriptCheckBox->setChecked(param.m_mapParam["loadscript"].toBool());
        ui.scriptLineEdit->setEnabled(param.m_mapParam["loadscript"].toBool());
        ui.scriptPushButton->setEnabled(param.m_mapParam["loadscript"].toBool());
        ui.scriptLineEdit->setText(param.m_mapParam["scriptfile"].toString());
        ui.menuTypeComboBox->setCurrentIndex(param.m_mapParam["menutype"].toInt());
        //ui.menuGroup->setButton(param.m_nMenuType);
        //QRadioButton * rbMenu = qobject_cast<QRadioButton*>(bgMenu.button(param.m_nMenuType));
        //rbMenu->setChecked(true);
        clrMenu = param.m_mapParam["menucolor"].toString();
        connect(ui.protocolComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onProtocol(int)));
    }
}

} // namespace QTerm

#include <addrdialog.moc>

