#ifndef QTERMTEXTLINE_H
#define QTERMTEXTLINE_H

#include <qstring.h>
class QTermTextLine
{

public:
	QTermTextLine();
	~QTermTextLine();

	void reset();

	void setChanged(int start, int end); 
	
	void clearChange() { m_bChanged = false; m_start=-1; m_end=-1; }

	bool isChanged( int &start, int &end );
	
	QCString getColor() { return m_color; }

	QCString getAttr() { return m_attr; }

	int getLength()	{ return m_length; }

	QCString getAttrText( int index = -1, int len = -1, const QCString& escape="\x1b\x1b" );

	QCString getText( int index = -1, int len = -1 );

	void insertText( const QCString& str, short attr = -1, int index = -1 );

	void deleteText( int index = -1, int len = -1 );
	
	void replaceText( const QCString& str, short attr = -1, int index = -1, int len = -1 );

	bool hasBlink();

protected:


	// we use QString to store any character after decode
	int m_length;
	QCString m_text;
	QCString m_color;
	QCString m_attr;
	char m_curColor;
	char m_curAttr;
	bool m_bChanged;
	bool m_bBlink;

	int m_start, m_end;
};

#endif	//QTERMTEXTLINE_H



