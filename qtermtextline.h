#ifndef QTERMTEXTLINE_H
#define QTERMTEXTLINE_H

#include <QByteArray>

class QTermTextLine
{

public:
	QTermTextLine();
	~QTermTextLine();

	void reset();

	void setChanged(int start, int end); 
	
	void clearChange() { m_bChanged = false; m_start=-1; m_end=-1; }

	bool isChanged( int &start, int &end );
	
	QByteArray getColor() { return m_color; }

	QByteArray getAttr() { return m_attr; }

	int getLength()	{ return m_length; }

	QByteArray getAttrText( int index = -1, int len = -1, const QByteArray & escape="\x1b\x1b" );

	QByteArray getText( int index = -1, int len = -1 );

	void insertText( const QByteArray& str, short attr = -1, int index = -1 );

	void deleteText( int index = -1, int len = -1 );
	
	void replaceText( const QByteArray& str, short attr = -1, int index = -1, int len = -1 );

	bool hasBlink();

protected:


	// we use QString to store any character after decode
	int m_length;
	QByteArray m_text;
	QByteArray m_color;
	QByteArray m_attr;
	char m_curColor;
	char m_curAttr;
	bool m_bChanged;
	bool m_bBlink;

	int m_start, m_end;
};

#endif	//QTERMTEXTLINE_H
