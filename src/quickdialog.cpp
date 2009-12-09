/****************************************************************************
** Form implementation generated from reading ui file 'quickldialog.ui'
**
** Created: Sat Dec 14 14:25:01 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "quickdialog.h"
#include "qtermconfig.h"
#include "qtermparam.h"
#include "addrdialog.h"
#include "qtermglobal.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QPixmap>
#include <QMessageBox>

namespace QTerm
{

/*
 *  Constructs a quickDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
quickDialog::quickDialog(QWidget* parent, Qt::WFlags fl)
        : QDialog(parent, fl)
{
    ui.setupUi(this);

    //ui.addPushButton->setIcon(QPixmap(Global::instance()->pathLib()+"pic/addr.png"));

    //ui.addPushButton->setToolTip(tr("Add To AddressBook" ));

    ui.connectPushButton->setDefault(true);

    connectSlots();

    pConf = Global::instance()->fileCfg();

    loadHistory();
}


/*
 *  Destroys the object and frees any allocated resources
 */
quickDialog::~quickDialog()
{
}

void quickDialog::closeEvent(QCloseEvent *)
{
    pConf->save();
    reject();
}

void quickDialog::loadHistory()
{
    QString strTmp = pConf->getItemValue("quick list", "num").toString();
    QString strSection;
    for (int i = 0; i < strTmp.toInt(); i++) {
        strSection = QString("quick %1").arg(i);
        ui.historyComboBox->addItem(pConf->getItemValue(strSection, "addr").toString());
    }

    if (strTmp != "0") {
        ui.historyComboBox->setCurrentIndex(0);
        listChanged(0);
    }
}

void quickDialog::connectSlots()
{
    connect(ui.historyComboBox, SIGNAL(activated(int)), this, SLOT(listChanged(int)));
    connect(ui.addPushButton, SIGNAL(clicked()), this, SLOT(addAddr()));
    connect(ui.deletePushButton, SIGNAL(clicked()), this, SLOT(deleteAddr()));
    connect(ui.advPushButton, SIGNAL(clicked()), this, SLOT(advOption()));
    connect(ui.connectPushButton, SIGNAL(clicked()), this, SLOT(connectIt()));
    connect(ui.closePushButton, SIGNAL(clicked()), this, SLOT(close()));
}

void quickDialog::listChanged(int index)
{
    QString strTmp = pConf->getItemValue("quick list", "num").toString();
    if (strTmp == "0")
        return;

    QString strSection = QString("quick %1").arg(index);
//  cstrSection.sprintf("quick %d", index);

    ui.addrLineEdit->setText(pConf->getItemValue(strSection, "addr").toString());
    ui.portSpinBox->setValue(pConf->getItemValue(strSection, "port").toInt());
    ui.protocolComboBox->setCurrentIndex(1);
    int protocolType = pConf->getItemValue(strSection, "protocol").toInt();
    ui.protocolComboBox->setCurrentIndex(protocolType);

}
void quickDialog::addAddr()
{
    QString strTmp;
    strTmp = Global::instance()->addrCfg()->getItemValue("bbs list", "num").toString();
    int num = strTmp.toInt();

    strTmp.setNum(num + 1);
    Global::instance()->addrCfg()->setItemValue("bbs list", "num", strTmp);

    param.m_strName = ui.addrLineEdit->text();
    param.m_strAddr = ui.addrLineEdit->text();
    param.m_uPort = ui.portSpinBox->value();
    param.m_nProtocolType = ui.protocolComboBox->currentIndex();
    Global::instance()->saveAddress(num, param);
}

void quickDialog::deleteAddr()
{
    int n = ui.historyComboBox->currentIndex();

    QString strTmp =  pConf->getItemValue("quick list", "num").toString();
    int num = strTmp.toInt();

    if (num != 0 && n != -1) {
        QString strSection = QString("quick %1").arg(n);
//      cstrSection.sprintf("quick %d", n);
        if (!pConf->deleteSection(strSection)) {
            qDebug("Failed to delete %d", n);
            return;
        }
        ui.historyComboBox->removeItem(n);

        // change the name after this
        for (int i = n + 1; i < num; i++) {
            strTmp = QString("quick %1").arg(i);
            strSection = QString("quick %1").arg(i - 1);
            pConf->renameSection(strTmp, strSection);
        }

        strTmp = pConf->getItemValue("quick list", "num").toString();
        strTmp.setNum(qMax(0, strTmp.toInt() - 1));
        pConf->setItemValue("quick list", "num", strTmp);

        // update
        if (num == 1) {
            ui.addrLineEdit->setText("");
            ui.portSpinBox->setValue(0);
        } else {
            ui.historyComboBox->setCurrentIndex(qMin(n, num - 2));
            listChanged(qMin(n, num - 2));
        }
    }
}
void quickDialog::advOption()
{
    addrDialog set(this, true);

    param.m_strName = ui.addrLineEdit->text();
    param.m_strAddr = ui.addrLineEdit->text();
    param.m_uPort = ui.portSpinBox->value();
    param.m_nProtocolType = ui.protocolComboBox->currentIndex();

    set.param = param;
    set.updateData(false);

    if (set.exec() == 1) {
        param = set.param;
        ui.addrLineEdit->setText(param.m_strAddr);
        ui.portSpinBox->setValue(param.m_uPort);
        ui.protocolComboBox->setCurrentIndex(param.m_nProtocolType);
    }
}
void quickDialog::connectIt()
{
    if (ui.addrLineEdit->text().isEmpty()) {
        QMessageBox mb("QTerm",
                       tr("Address can not be blank."),
                       QMessageBox::Warning,
                       QMessageBox::Ok | QMessageBox::Default, 0,
                       0);
        mb.exec();
        return;
    }

    QString strTmp =  pConf->getItemValue("quick list", "num").toString();
    int num = strTmp.toInt();

    bool bExist = false;
    int index;
    // check if it is already there
    QString strSection;
    for (int i = 0; i < num; i++) {
        strSection = QString("quick %1").arg(i);

        strTmp = pConf->getItemValue(strSection, "addr").toString();
        if (strTmp == ui.addrLineEdit->text()) {
            bExist = true; index = i; break;
        }
        strTmp = pConf->getItemValue(strSection, "port").toString();
        if (ui.portSpinBox->value() != strTmp.toInt()) {
            bExist = true; index = i; break;
        }
        strTmp = pConf->getItemValue(strSection, "protocol").toString();
        if (ui.protocolComboBox->currentIndex() != strTmp.toInt()) {
            bExist = true; index = i; break;
        }
    }
    // append it
    if (!bExist) {
        strSection = QString("quick %1").arg(num);
        pConf->setItemValue(strSection, "addr", ui.addrLineEdit->text());
        pConf->setItemValue(strSection, "port", ui.portSpinBox->value());
        pConf->setItemValue(strSection, "protocol", ui.protocolComboBox->currentIndex());
        strTmp.setNum(num + 1);
        pConf->setItemValue("quick list", "num", strTmp);
        index = num;
    }

    // set another name first to avoid duplicate
    strSection = QString("quick %1").arg(index);
    strTmp = QString("quick %1").arg(num + 2);
    pConf->renameSection(strSection, strTmp);
    // shift the current select to  the first
    for (int j = index - 1; j >= 0; j--) {
        strSection = QString("quick %1").arg(j);
        strTmp = QString("quick %1").arg(j + 1);
        pConf->renameSection(strSection, strTmp);
    }
    // set it back to 0
    strSection = QString("quick %1").arg(num + 2);
    strTmp = QString("quick %1").arg(0);
    pConf->renameSection(strSection, strTmp);

    param.m_strName = ui.addrLineEdit->text();
    param.m_strAddr = ui.addrLineEdit->text();
    param.m_uPort = ui.portSpinBox->value();
    param.m_nProtocolType = ui.protocolComboBox->currentIndex();

    pConf->save();
    done(1);
}
void quickDialog::close()
{
    pConf->save();
    done(0);
}

} // namespace QTerm


#include <quickdialog.moc>
