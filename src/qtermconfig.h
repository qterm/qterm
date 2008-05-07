#ifndef QTERMCFG_H
#define QTERMCFG_H

class QSettings;
class QString;

namespace QTerm
{

class Config
{
public:
    Config(const QString & szFileName );
    ~Config();

    bool save(const QString & szFileName);

    bool setItemValue(const QString & szSection, const QString & szItemName,const QString & szItemValue);
    QString getItemValue(const QString & szSection, const QString & szItemName);


    bool deleteItem(const QString & szSection, const QString & szItemName);
    bool deleteSection(const QString & szSection);

    bool renameSection(const QString & szSection, const QString & szNewName);
    bool hasSection(const QString & szSection);

private:
    bool checkError();
    QSettings * m_settings;
};

} // namespace QTerm

#endif  //QTERMCONFIG_H
