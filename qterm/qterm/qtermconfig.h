#ifndef QTERMCFG_H
#define QTERMCFG_H


class QTextStream;


struct Item
{
	char *szName;
	char *szValue;
};

class Section
{

public:
	Section (const char *szSectionName0 = 0);
	~Section ();

	char *getSectionName ();
	bool setSectionName (const char *szSectionName);
	
	bool addItem (const char *szItemName, const char *szValue);
	Item *findItem (const char *szItemName);
	bool setItemValue (const char *szItemName, const char *szNewValue);
	char *getItemValue (const char *szItemName);

	bool deleteItem(const char *szItemName);
	
	char *szSectionName;
	Item *pItems;
	int  nItems; 

	friend QTextStream & operator >> (QTextStream &a, Section &b);
	friend QTextStream & operator << (QTextStream &a, Section &b);
};

class QTermConfig
{
private:
	char *szFileName;
	Section ** ppSections;
	int nSections;
	
	Section *addSection (Section *p);

public:
	QTermConfig (const char *szFileName );
	~QTermConfig ();

	bool save (const char *szFileName);
	bool setItemValue(const char *szSection, const char *szItemName,const char *szItemValue);
	char *getItemValue(const char *szSection, const char *szItemName);
   
	bool setSectionName(const char *szSection, const char *szNewName);
	bool deleteItem(const char *szSection, const char *szItemName);

	Section *findSection (const char *szSection);
	bool deleteSection(const char *szSection);
};

#endif	//QTERMCONFIG_H
