/*******************************************************************************
FILENAME:      qtermencode.cpp
REVISION:      2005.7.19 first created.
         
AUTHOR:        kingson
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermencode.h"

#include "qtermbuffer.h"

QTermEncode::QTermEncode( QTermBuffer * buffer )
{
	m_pBuffer = buffer;
	
}

QTermEncode::~QTermEncode()
{
}

QByteArray QTermEncode::encode( const QByteArray& escape )
{
	QByteArray text;

	int n=0;
	while( n<m_pBuffer->lines() )
	{
		QTermTextLine *pLine  = m_pBuffer->at(n);

		n++;
	}
}
