/*******************************************************************************
FILENAME:      qtermconfig.cpp
REVISION:      2001.10.10 first created.
        	   2004.5.3 rewritten by cppgx.bbs@smth.org 

*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermconfig.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>


QTermConfig::QTermConfig (const char *szFileName)
{
	QFile file( szFileName );
    if ( !file.open( IO_ReadOnly ) ) 
	{
		qWarning( "could not open for reading `%s'", szFileName );
		return;
    }
	QTextStream is;
	is.setDevice(&file);
	loadFromStream(is);
	is.unsetDevice();
	file.close();
}


QTermConfig::~QTermConfig ()
{

}

bool QTermConfig::save (const char *szFileName)
{
    QFile file( szFileName );
    if ( !file.open( IO_WriteOnly ) ) 
	{
		qWarning( "could not open for writing `%s'", szFileName );
		return false;
    }

	QTextStream os;
	os.setDevice(&file);
	saveToStream(os);
	os.unsetDevice();
	file.close();
	return true;
}

bool QTermConfig::loadFromStream(QTextStream& is)
{
	QString strLine, strSection;

    data.clear();
	
	is.setEncoding(QTextStream::Latin1);

    while(!is.atEnd())
    {
        strLine = is.readLine().stripWhiteSpace();
        if(strLine.isEmpty() || strLine[0] == '#')
            continue;

        if(strLine.left(1) == "[" && strLine.right(1) == "]")
        {
            strSection = strLine.mid(1, strLine.length() - 2);
            addSection(strSection);
        }
        else
        {
			QString strValue = strLine.section('=', 1);
            setItemValue(strSection,
                strLine.section('=', 0, 0), strValue.isNull()?"":strValue);
        }
    }
    return true;
}

bool QTermConfig::saveToStream(QTextStream& os)
{
    QString strLine, strSection;
    Section::iterator iStr;

	os.setEncoding(QTextStream::Latin1);

    for(StrSecMap::iterator iSec = data.begin();
         iSec != data.end() ; ++iSec)
    {
        os << '[' << iSec.key() << "]\n";
        for(iStr = iSec.data().begin() ;
             iStr != iSec.data().end() ; ++iStr)
        {
            os << iStr.key() << '=' << iStr.data() << '\n';
        }
        os << '\n';
    }
    return true;	
}


bool QTermConfig::addSection (const char *szSection)
{
    if(hasSection(szSection))
        return false;
	Section sec;
    data[szSection]=sec;
    return true;	
}

bool QTermConfig::hasSection (const char *szSection)
{
    return data.find(szSection) != data.end();
}

bool QTermConfig::setItemValue (const char *szSection, 
			const char *szItemName, const char *szItemValue)
{
	if(!hasSection(szSection))
		if(!addSection(szSection))
			return false;

	data[szSection][szItemName] = szItemValue;
	return true;
}

const char * QTermConfig::getItemValue (const char *szSection, const char *szItemName)
{
	if(hasSection(szSection))
		if(data[szSection].find(szItemName) != data[szSection].end());
			return data[szSection][szItemName];
	return NULL;
}

bool QTermConfig::renameSection (const char *szSection, const char *szNewName)
{
    if(hasSection(szNewName) || !hasSection(szSection))
    {
        return false;
    }

    if(!addSection(szNewName))
		return false;
    data[szNewName] = data[szSection];

	return deleteSection(szSection);
}

bool QTermConfig::deleteSection (const char *szSection)
{
	if(hasSection(szSection))
    {
        data.erase(szSection);
        return true;
    }
    return false;
}

bool QTermConfig::deleteItem( const char *szSection, const char *szItemName)
{
	if(hasSection(szSection))
		if(data[szSection].find(szItemName) != data[szSection].end());
		{
			data[szSection].erase(szItemName);
			return true;
		}
	return false;
}

