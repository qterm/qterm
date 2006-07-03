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

// #include <qfile.h>
// #include <qfileinfo.h>
// #include <qstring.h>
//Added by qt3to4:
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QTextCodec>

QTermConfig::QTermConfig(const QString & szFileName)
{
	QFile file( szFileName );
    if ( !file.open( QIODevice::ReadOnly ) ) 
	{
		//qWarning( "could not open for reading `%s'", szFileName.toLatin1() );
		return;
    }
	QTextStream is;
	is.setDevice(&file);
	loadFromStream(is);
	//is.unsetDevice();
	file.close();
}


QTermConfig::~QTermConfig ()
{

}

bool QTermConfig::save (const QString & szFileName)
{
    QFile file( szFileName );
    if ( !file.open( QIODevice::WriteOnly ) ) 
	{
		//qWarning( "could not open for writing `%s'", szFileName.toLatin1() );
		return false;
    }

	QTextStream os;
	os.setDevice(&file);
	saveToStream(os);
// 	os.unsetDevice();
	file.close();
	return true;
}

bool QTermConfig::loadFromStream(QTextStream& is)
{
	QString strLine, strSection;

    data.clear();
	
	is.setCodec(QTextCodec::codecForName("UTF-8"));

    while(!is.atEnd())
    {
        strLine = is.readLine().trimmed();
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
                strLine.section('=', 0, 0), strValue.isNull()?QString(""):strValue);
        }
    }
    return true;
}

bool QTermConfig::saveToStream(QTextStream& os)
{
    QString strLine, strSection;
    Section::iterator iStr;

	os.setCodec(QTextCodec::codecForName("UTF-8"));

    for(StrSecMap::iterator iSec = data.begin();
         iSec != data.end() ; ++iSec)
    {
        os << '[' << iSec.key() << "]\n";
        for(iStr = iSec.value().begin() ;
             iStr != iSec.value().end() ; ++iStr)
        {
            os << iStr.key() << '=' << iStr.value() << '\n';
        }
        os << '\n';
    }
    return true;	
}


bool QTermConfig::addSection (const QString & szSection)
{
    if(hasSection(szSection))
        return false;
	Section sec;
    data[szSection]=sec;
    return true;	
}

bool QTermConfig::hasSection (const QString & szSection)
{
    return data.find(szSection) != data.end();
}

bool QTermConfig::setItemValue (const QString & szSection, 
			const QString & szItemName, const QString & szItemValue)
{
	if(!hasSection(szSection))
		if(!addSection(szSection))
			return false;

	data[szSection][szItemName] = szItemValue;
	return true;
}

QString QTermConfig::getItemValue (const QString & szSection, const QString & szItemName)
{
	if(hasSection(szSection))
		if(data[szSection].find(szItemName) != data[szSection].end());
			if(!data[szSection][szItemName].isEmpty())
				return  data[szSection][szItemName];
	return "";
}

bool QTermConfig::renameSection (const QString & szSection, const QString & szNewName)
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

bool QTermConfig::deleteSection (const QString & szSection)
{
	if(hasSection(szSection))
    {
        data.remove(szSection);
        return true;
    }
    return false;
}

bool QTermConfig::deleteItem( const QString & szSection, const QString & szItemName)
{
	if(hasSection(szSection))
		if(data[szSection].find(szItemName) != data[szSection].end());
		{
			data[szSection].remove(szItemName);
			return true;
		}
	return false;
}

