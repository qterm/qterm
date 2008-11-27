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
#include <QtGui/QMessageBox>
#include <QtDebug>

namespace QTerm
{

const QString Config::m_version = "1.5";

Config::Config(const QString & szFileName)
{
    if (QFile::exists(szFileName)) {
        m_settings = new QSettings(szFileName, QSettings::IniFormat);
        checkVersion();
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

    QByteArray data;
    foreach (QString eachKey, keys) {
        data = m_settings->value(eachKey).toByteArray();
        m_settings->setValue(eachKey,QString::fromUtf8(data.data()));
    }
    if (!m_settings->contains("version")) {
        m_settings->setValue("version", m_version);
    }
    save();
}

bool Config::checkVersion()
{
    QString version = m_settings->value("version").toString();

    if (version.isEmpty()) {
        QMessageBox::warning(0, "Old Version","The version of your config file is outdated.\n" "It will be automatically updated, but you should check for errors");
        upgrade();
    } else if (m_version != version) {
        QMessageBox::warning(0, "Version Mismath","The version of your config file is not match the current QTerm version.\n" "It will be automatically updated, but you should check for errors");
        m_settings->setValue("version", m_version);
        save();
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

