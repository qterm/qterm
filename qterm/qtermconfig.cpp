/*******************************************************************************
FILENAME:      qtermconfig.cpp
REVISION:      2001.10.10 first created.
         
AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermconfig.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>

#define MAXBUFSIZE 256


Section::Section(const char *szSectionName0)
{
	szSectionName = NULL;
	if (szSectionName0 != NULL)
		szSectionName = strdup (szSectionName0);
	pItems = NULL;
	nItems = 0; 
}

Section::~Section ()
{
	if (pItems == NULL)
		return;
	for (int i = 0; i < nItems; i++) 
	{
		if (pItems[i].szName != NULL)
			free (pItems[i].szName);
		if (pItems[i].szValue != NULL)
			free (pItems[i].szValue);
	}
	if (pItems != NULL)
		free (pItems);
	if (szSectionName != NULL)
		free (szSectionName);
}

char * Section::getSectionName ()
{
	return szSectionName;
}

bool Section::setSectionName (const char *szSectionName0)
{
	if (szSectionName != NULL)
		free (szSectionName);
	szSectionName = NULL;
	if (szSectionName0 != NULL)
		szSectionName = strdup (szSectionName0);
	return true;
}

bool Section::addItem (const char *szItemName, const char *szValue)
{
	if (pItems == NULL)
	{
		pItems = (Item *) malloc (sizeof (Item));
		nItems = 1; 
	}
	else
	{
		nItems ++;
		pItems = (Item *) realloc 
			(pItems, nItems * sizeof (Item));
	}
	pItems[nItems - 1].szName = NULL;
	pItems[nItems - 1].szValue = NULL;
	if (szItemName != NULL) {
		pItems[nItems - 1].szName = strdup (szItemName);
		if (szValue != NULL)
			pItems[nItems - 1].szValue = strdup (szValue);
	}
	return true;
}

bool Section::deleteItem(const char *szItemName)
{
	Item *p=findItem (szItemName);
	if ( p==NULL )
		return false;
	
	if(nItems==1)
	{
		nItems=0;
		pItems=NULL;
	}
	else
	{
		int n;
		for(n = 0; n <nItems; n++) 
			if (p==&pItems[n])
				break;
		for(int i=n; i<nItems-1; i++)
		{
			pItems[i].szName = strdup(pItems[i+1].szName);
			pItems[i].szValue = strdup(pItems[i+1].szValue);
		}
		nItems--;
		pItems = (Item *) realloc 
			(pItems, nItems * sizeof (Item));
	}
	if (p!= NULL)
		free(p);

	return true;
}


Item * Section::findItem (const char *szItemName)
{
	if (pItems == NULL)
		return NULL;
	for (int i = 0; i < nItems; i++) 
	if (strcmp (pItems[i].szName, szItemName) == 0)
		return &pItems[i];
	return NULL;
}

bool Section::setItemValue (const char *szItemName, const char *szNewValue)
{
	Item *p;
	if ((p = findItem (szItemName)) == NULL)
		return addItem(szItemName, szNewValue);
	if(szNewValue==NULL)
		return false;
	if (p->szValue != NULL)
		free (p->szValue);
	p->szValue = strdup (szNewValue);
	return true;
}


char *Section::getItemValue (const char *szItemName)
{
	Item *p = findItem (szItemName);
	if (p == NULL)
		return NULL;
	return p->szValue;
}

QTextStream & operator >> (QTextStream &a, Section &b)
{
	char *p = NULL; 
	char szTemp[128];
	char szItemName[128], szValue[128];
	int i;
  	QString str;
	
	if ( a.eof() )
		return a;
	// We should not new buf here.

	// char *buf = new char[MAXBUFSIZE];
	char * buf = NULL;
	do
	{
		str = a.readLine();
		buf = strdup(str.latin1());
	}
	while ( !a.eof() && buf[0] == '\0' );
//	while ( !a.eof())
//	a.getline( buf, MAXBUFSIZE );
		
	if ( a.eof() )
	{
		if (buf != NULL)
			free(buf);
		return a;
	}
	p = buf;
		
	//find the start of section [....]	
	while(*p != '[')
	{
		p++;
	}
	//get the section name
	for (p++, i = 0; *p != ']' && *p; p++, i++)
		szTemp[i] = *p;	
	szTemp[i] = '\0';
	
	b.setSectionName (szTemp);

	if (buf != NULL)
		free(buf);
	p = NULL;
	buf = NULL;
	
	do
	{	str = a.readLine();
		buf = strdup(str.latin1());

		//end of file or end of section
		if ( a.eof() || buf[0] == '\0')
			break;
		//get the item name
		for (i = 0, p = buf; *p != '='; p++, i++)
			szItemName[i] = *p;
		szItemName[i] = '\0';
		//get item value
		for (i = 0, p++; *p != '\0'; p++, i++)
			szValue[i] = *p;
		szValue[i] = '\0';
		//add it
		b.addItem (szItemName, szValue);
	}
	while (! a.eof () && buf[0] != '\0');
	
	if (buf != NULL)
		free(buf);

	return a;
}

QTextStream & operator << (QTextStream &a, Section &b)
{
    
	a << "[" << b.szSectionName << "]" << "\n";
	for (int i = 0; i < b.nItems; i++) 
		a << b.pItems[i].szName << "=" << b.pItems[i].szValue << "\n";
	a << "\n";
	
	return a;
}

QTermConfig::QTermConfig (const char *szFileName0)
{
	ppSections = NULL;
	nSections = 0;
	szFileName = NULL;

	if (szFileName0 != NULL)
	{
		szFileName = strdup (szFileName0);
		if ( !QFileInfo( szFileName ).exists() ) 
		{
			qWarning( "`%s' doesn't exist", szFileName );
			return;
		}
	    QFile f( szFileName );
	    if ( !f.open( IO_ReadOnly ) ) 
		{
			qWarning( "could not open for reading `%s'", szFileName );
			return;
		}

		QTextStream mystream (&f);
		mystream.setEncoding(QTextStream::Latin1);

		Section *pTmpSection;
		do
		{
			pTmpSection = new Section ();
			mystream >> *pTmpSection;
			if (pTmpSection->nItems == 0) 
			{
				delete pTmpSection;
				continue;
			}
			addSection (pTmpSection);
		}
		while (! mystream.eof ());
	
		f.close();
	}
}


QTermConfig::~QTermConfig ()
{
	if (szFileName != NULL)
		free (szFileName);
	for (int i = 0; i < nSections; i++)
		delete ppSections[i];
}

bool QTermConfig::save (const char *szFileName)
{
    QFile f( szFileName );
    if ( !f.open( IO_WriteOnly ) ) 
	{
		qWarning( "could not open for writing `%s'", szFileName );
		return false;
    }

	QTextStream mystream(&f);
	mystream.setEncoding(QTextStream::Latin1);

	for (int i = 0; i < nSections; i++)
		mystream << *ppSections[i];
	f.close();
	return true;
}

bool QTermConfig::deleteSection (const char *szSection)
{
	Section *p = findSection(szSection);
	
	if( p == NULL )
		return false;

	if (nSections == 1)
	{
		nSections = 0;
		if (ppSections != NULL)
			free(ppSections);
		ppSections = NULL;
	}
	else
	{
		// find the index
		int n;
		for (n = 0; n < nSections; n++)
		{
			if (ppSections[n]==p)
				break;
		}
		// shift ahead
		for (int i=n; i< nSections-1; i++)
		{
			ppSections[i]=ppSections[i+1];
		}
		nSections --;
		ppSections = (Section **) realloc
			(ppSections, nSections * sizeof (Section *));
	
	}
	delete p;
	
	return true;
}

Section *QTermConfig::addSection (Section *p)
{
	if (ppSections == NULL)
	{
		nSections = 1;
		ppSections = (Section **) malloc
			(nSections * sizeof (Section *));
	}
	else
	{
		nSections ++;
		ppSections = (Section **) realloc
			(ppSections, nSections * sizeof (Section *));
	}
	
	ppSections[nSections - 1] = p;
	
	return p;
}

Section *QTermConfig::findSection (const char *szSection)
{
	Section *p = NULL;
	for (int i = 0; i < nSections; i++)
	{
		if (strcmp (ppSections[i]->getSectionName(), szSection) == 0)
		{
			p = ppSections[i];
			break;
		}
	}
	return p;
}

bool QTermConfig::setItemValue (const char *szSection, 
			const char *szItemName, const char *szItemValue)
{
	Section *p = findSection (szSection);

	if (p == NULL)
	{
		Section *pTmpSection = new Section(szSection);
		addSection(pTmpSection);
		return pTmpSection->addItem(szItemName, szItemValue);
	}

	return p->setItemValue (szItemName, szItemValue);
}

char * QTermConfig::getItemValue (const char *szSection, const char *szItemName)
{
    Section *p = findSection (szSection);
    if (p == NULL)
        return NULL;
    return p->getItemValue (szItemName);
}

bool QTermConfig::setSectionName (const char *szSection, const char *szNewName)
{
	Section *p = findSection (szSection);

	if (p == NULL)
		return false;

	p->setSectionName( szNewName );

	return true;
}

bool QTermConfig::deleteItem( const char *szSection, const char *szItemName)
{
    Section *p = findSection (szSection);
    if (p == NULL)
        return false;
	
	return p->deleteItem(szItemName);

}


