#ifndef QTERMCFG_H
#define QTERMCFG_H

#include <qmap.h>

typedef QMap<QString, QString> Section;
typedef QMap<QString, Section> StrSecMap;

class QTermConfig
{
private:
	StrSecMap data;
	bool loadFromStream(QTextStream&);
	bool saveToStream(QTextStream&);
	
	bool addSection(const char *szSection);
public:
	QTermConfig (const char *szFileName );
	~QTermConfig ();

	bool save (const char *szFileName);

	bool setItemValue(const char *szSection, const char *szItemName,const char *szItemValue);
	const char *getItemValue(const char *szSection, const char *szItemName);
   

	bool deleteItem(const char *szSection, const char *szItemName);
	bool deleteSection(const char *szSection);

	bool renameSection(const char *szSection, const char *szNewName);
	bool hasSection(const char *szSection);
};

#endif	//QTERMCONFIG_H
