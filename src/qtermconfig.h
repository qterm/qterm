#ifndef QTERMCFG_H
#define QTERMCFG_H

#include <QtCore/QString>

class QSettings;
class QVariant;

namespace QTerm
{

class Config
{
public:
    Config(const QString & szFileName );
    ~Config();

    bool save(const QString & szFileName = "");

    bool setItemValue(const QString & szSection, const QString & szItemName,const QVariant & szItemValue);
    QVariant getItemValue(const QString & szSection, const QString & szItemName);

    bool deleteItem(const QString & szSection, const QString & szItemName);
    bool deleteSection(const QString & szSection);

    bool renameSection(const QString & szSection, const QString & szNewName);
    bool hasSection(const QString & szSection);
    bool hasItem(const QString & szSection, const QString & szItemName);

private:
    bool checkError();
    bool checkVersion();
    void upgrade();
    void addShortcuts();
    void addToolBars();
    static const QString m_version;
    QSettings * m_settings;
};

} // namespace QTerm

#endif  //QTERMCONFIG_H
