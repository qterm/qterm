/*******************************************************************************
FILENAME:      qtermconfig.cpp
REVISION:      2001.10.10 first created.
               2004.5.3 rewritten by cppgx.bbs@smth.org
               2008.5.6 rewritten by hooey

*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermconfig.h"

#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QString>
#if QT_VERSION > 0x050000
#include <QtWidgets/QMessageBox>
#else
#include <QtGui/QMessageBox>
#endif
#include <QtDebug>

namespace QTerm
{

const QString Config::m_version = "1.11";

Config::Config(const QString & szFileName)
{
    if (QFile::exists(szFileName)) {
        m_settings = new QSettings(szFileName, QSettings::IniFormat);
        checkVersion(szFileName);
    } else {
        m_settings = new QSettings(szFileName, QSettings::IniFormat);
        m_settings->setValue("version", m_version);
    }
}

Config::~Config()
{
    save();
    delete m_settings;
}

void Config::upgrade()
{
    QStringList keys = m_settings->allKeys();

    //QByteArray data;
    //foreach (QString eachKey, keys) {
    //    data = m_settings->value(eachKey).toByteArray();
    //    m_settings->setValue(eachKey,QString::fromUtf8(data.data()));
    //}
    if (!m_settings->contains("version")) {
        m_settings->setValue("version", m_version);
    }
    addShortcuts();
    addToolBars();
    save();
}

void Config::addShortcuts()
{
    if (hasSection("Shortcuts"))
        return;
    m_settings->beginGroup("Shortcuts");
    m_settings->setValue("actionAbout","F1");
    m_settings->setValue("actionAddress","F2");
    m_settings->setValue("actionQuickConnect","F3");
    m_settings->setValue("actionRefresh","F5");
    m_settings->setValue("actionFull","F6");
    m_settings->setValue("actionScriptRun","F7");
    m_settings->setValue("actionScriptStop","F8");
    m_settings->setValue("actionCopyArticle","F9");
    m_settings->setValue("actionViewMessage","F10");
    m_settings->setValue("actionBoss","F12");
    m_settings->setValue("actionCopy","Ctrl+Ins");
    m_settings->setValue("actionPaste","Shift+Ins");
    m_settings->endGroup();
}

void Config::addToolBars()
{
    if (hasSection("ToolBars"))
        return;
    m_settings->beginGroup("ToolBars");
    QStringList listActions;
    listActions << "actionQuickConnect";
    m_settings->setValue("terminalToolBar", listActions);
    listActions.clear();
    listActions << "actionDisconnect" << "Separator" << "actionCopy" << "actionPaste" << "actionRect" << "actionColorCopy" << "Separator" << "actionFont" << "actionColor" << "actionRefresh" << "Separator" << "actionCurrentSession" << "Separator" << "actionCopyArticle" << "actionAntiIdle" << "actionAutoReply" << "actionViewMessage" << "actionMouse" << "actionBeep" << "actionReconnect";
    m_settings->setValue("mainToolBar",listActions);
    m_settings->setValue("terminalToolBarShown", true);
    m_settings->setValue("keyToolBarShown", true);
    m_settings->setValue("mainToolBarShown",true);
    m_settings->endGroup();
}

bool Config::checkVersion(const QString & szFileName)
{
    QString version = m_settings->value("version").toString();

    if (version.isEmpty()) {
        QMessageBox::warning(0, "Old Version","The version of your config file is outdated.\n" "It will be automatically updated, but you should check for errors");
        upgrade();
    } else if (m_version != version) {
        QString bakFile = szFileName+".bak";
        QMessageBox::warning(0, "Version Mismath","The version of your config file is not match the current QTerm version.\n" "The old config file will be backed up as: "+bakFile);

        if (QFile::exists(bakFile))
        {
            QFile::remove(bakFile);
        }

        QFile::copy(szFileName, bakFile);

        m_settings->setValue("version", m_version);
        upgrade();
        return false;
    }
    return true;
}

bool Config::checkError()
{
    QSettings::Status status = m_settings->status();
    if (status == QSettings::NoError) {
        return true;
    }
    return false;
}

bool Config::save(const QString & szFileName)
{
    if (!szFileName.isEmpty()) {
        QFileInfo fi(szFileName);
        Q_ASSERT(QString::compare(fi.absoluteFilePath(), m_settings->fileName()) == 0);
    }
    m_settings->sync();
    return checkError();
}

bool Config::hasSection(const QString & szSection)
{
    QStringList section = m_settings->childGroups();
    return section.contains(szSection);
}

bool Config::hasItem(const QString & szSection, const QString & szItemName)
{
    QString key = szSection+"/"+szItemName;
    return m_settings->contains(key);
}

bool Config::setItemValue(const QString & szSection,
            const QString & szItemName, const QVariant & szItemValue)
{
    QString key = szSection+"/"+szItemName;
    m_settings->setValue(key, szItemValue);
    return checkError();
}

QVariant Config::getItemValue(const QString & szSection, const QString & szItemName)
{
    QString key = szSection+"/"+szItemName;
    QVariant data = m_settings->value(key);
    return data;
}

bool Config::renameSection(const QString & szSection, const QString & szNewName)
{
    if (hasSection(szNewName) || !hasSection(szSection))
    {
        return false;
    }

    m_settings->beginGroup(szSection);
    QStringList keys = m_settings->allKeys();
    m_settings->endGroup();

    QVariant data;
    foreach (QString eachKey, keys) {
        data = m_settings->value(szSection+"/"+eachKey);
        m_settings->setValue(szNewName+"/"+eachKey,data);
    }

    deleteSection(szSection);

    return checkError();
}

bool Config::deleteSection(const QString & szSection)
{
    m_settings->beginGroup(szSection);
    m_settings->remove("");
    m_settings->endGroup();
    return checkError();
}

bool Config::deleteItem(const QString & szSection, const QString & szItemName)
{
    QString key = szSection+"/"+szItemName;
    m_settings->remove(key);
    return checkError();
}

} // namespace QTerm

