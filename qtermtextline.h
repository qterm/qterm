#ifndef QTERMTEXTLINE_H
#define QTERMTEXTLINE_H

#include <QByteArray>
#include <QPoint>

class QTermTextLine
{

public:
	QTermTextLine();
	~QTermTextLine();

	void reset();

	void setChanged(int start, int end);
	QPoint getChanged() {return QPoint(m_start,m_end);}
	void clearChange() { m_start=-1; m_end=-1; }
	
	QByteArray getColor(int index=0, int len=-1) { return m_color.mid( index, len ); }
	QByteArray getAttr(int index=0, int len=-1) { return m_attr.mid( index, len );; }

	int length()	{ return m_length; }

	QByteArray getAttrText( int index = 0, int len = -1, const QByteArray & escape="\x1b\x1b" );
	QByteArray getPlainText( int index = 0, int len = -1 );

    void insertText( int index, const QByteArray& text,  const QByteArray& color,  const QByteArray& attr);
	void insertText( int index, const QByteArray& str, short attr = -1);

    void replaceText( int index, const QByteArray& text,  const QByteArray& color,  const QByteArray& attr);
	void replaceText( int index, const QByteArray& str, short attr = -1);

	void deleteText( int, int );	

	bool hasBlink();

protected:
    void leftJustify(int index);
    void appendText( int index, const QByteArray& str, short attribute = -1 );
    
	int m_length;
	QByteArray m_text;
	QByteArray m_color;
	QByteArray m_attr;
	char m_curColor;
	char m_curAttr;
	bool m_bBlink;

	int m_start, m_end;
};

#endif	//QTERMTEXTLINE_H
