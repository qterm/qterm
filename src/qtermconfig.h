#ifndef QTERMCFG_H
#define QTERMCFG_H

// #include <qmap.h>
//Added by qt3to4:
#include <QTextStream>
#include <QMap>

namespace QTerm
{
typedef QMap<QString, QString> Section;
typedef QMap<QString, Section> StrSecMap;

class Config
{
private:
	StrSecMap data;
	bool loadFromStream(QTextStream&);
	bool saveToStream(QTextStream&);
	
	bool addSection(const QString & szSection);
public:
	Config (const QString & szFileName );
	~Config ();

	bool save (const QString & szFileName);

	bool setItemValue(const QString & szSection, const QString & szItemName,const QString & szItemValue);
	QString getItemValue(const QString & szSection, const QString & szItemName);
   

	bool deleteItem(const QString & szSection, const QString & szItemName);
	bool deleteSection(const QString & szSection);

	bool renameSection(const QString & szSection, const QString & szNewName);
	bool hasSection(const QString & szSection);
};

} // namespace QTerm

#endif	//QTERMCONFIG_H
