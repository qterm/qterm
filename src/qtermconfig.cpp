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
#include <QtDebug>

namespace QTerm
{

Config::Config(const QString & szFileName)
{
    m_settings = new QSettings(szFileName, QSettings::IniFormat);
}


Config::~Config()
{
    m_settings->sync();
    delete m_settings;
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

bool Config::setItemValue(const QString & szSection,
            const QString & szItemName, const QString & szItemValue)
{
    QString key = szSection+"/"+szItemName;
    m_settings->setValue(key, szItemValue);
    return checkError();
}

QString Config::getItemValue(const QString & szSection, const QString & szItemName)
{
    QString key = szSection+"/"+szItemName;
    QVariant data = m_settings->value(key);
    if (data.isValid()) {
        return data.toString();
    } else {
        return "";
    }
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

