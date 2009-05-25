/*******************************************************************************
FILENAME:      qtermtextline.cpp
REVISION:      2001.8.12 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qterm.h"
#include "qtermtextline.h"

#include <QtCore/QRegExp>
#include <QtCore/QString>

//#include <QtDebug>

namespace QTerm
{
TextLine::TextLine()
        : m_text(), m_color(), m_attr()
{
    m_bChanged = true;
    m_start = -1;
    m_end = -1;
    reset();
}

TextLine::~TextLine()
{

}


// insert cstr at position index of line,
// if attrib == -1, use current attr,
// if index == -1, append line
void TextLine::insertText(const QString & str, short attribute, int index)
{
    // set attribute
    if (attribute != -1) {
        m_curColor = GETCOLOR(attribute);
        m_curAttr  = GETATTR(attribute);
        if (m_curAttr == '\0')
            m_curAttr = NO_ATTR;
    }


    int len = TermString(str).length();

    QByteArray tmp;

    int start;

    if (index == -1) { // append
        m_text.append(str);

        tmp.fill(m_curColor, len);
        m_color += tmp;
        tmp.fill(m_curAttr, len);
        m_attr  += tmp;

        start = m_length;
        m_length += len;
    } else { // insert

        if (index >= m_length) {
            m_text.insert(index, str);
            m_length = m_text.length();

            tmp.fill(NO_COLOR, index - m_length);
            m_color.append(tmp);
            tmp.fill(m_curColor, len);
            m_color.insert(index, tmp.data());

            tmp.fill(NO_ATTR, index - m_length);
            m_attr.append(tmp);
            tmp.fill(m_curAttr, len);
            m_attr.insert(index, tmp.data());
        } else {
            m_text.insert(index, str);
            m_length = m_text.length();
            tmp.fill(m_curColor, len);
            m_color.insert(index, tmp.data());
            tmp.fill(m_curAttr, len);
            m_attr.insert(index, tmp.data());

        }
        start = index;
    }

    setChanged(start, m_length);

}

// replace the cstring with cstr, which start at index and have len chars,
// if attr == -1, use the current attr,
// if index == -1, reset line and insert str.
// if len == -1, replace str's length chars.
void TextLine::replaceText(const QString & str, short attribute, int index, int len)
{
    // set attribute
    if (str.isEmpty()) { // Return if the string is empty
        return;
    }
    if (attribute != -1) {
        m_curColor = GETCOLOR(attribute);
        m_curAttr  = GETATTR(attribute);
        if (m_curAttr == '\0')
            m_curAttr = NO_ATTR;
    }

    int newlen = TermString(str).length();

    QByteArray tmp;

    if (index == -1) { // replace whole line
        m_text = TermString(str);

        m_color.fill(m_curColor, newlen);
        m_attr.fill(m_curAttr, newlen);

        setChanged(0, qMax(newlen, m_length));

        m_length = newlen;

        return;
    }

    if (len == -1)   // replace with  str
        len = newlen;

    if (index + len > m_length) {
        //qDebug() << "index: " << index << " len: " << len << " string: " << str;
        m_text.replace(index, len, str);

        setChanged(index, qMax(m_length, m_text.length()));

        tmp.fill(NO_COLOR, index + len - m_length);
        m_color.append(tmp);
        tmp.fill(m_curColor, len);
        m_color.replace(index, len, tmp);

        tmp.fill(NO_ATTR, index + len - m_length);
        m_attr.append(tmp);
        tmp.fill(m_curAttr, len);
        m_attr.replace(index, len, tmp);

        m_length = m_text.length();

        //if ( m_color.length() != m_text.length()) {
        //    qDebug() << "=================================color length: " << m_color.length() << ", tmp: " << tmp.length() << ", old length: " << tmplen;
        //}

    } else {
        //qDebug() << "string : " << m_text.string() << " old length: " << m_text.length();
        //qDebug() << "index: " << index << " len: " << len << " string: " << str;
        m_text.replace(index, len, str);
        //qDebug() << "new length: " << m_text.length() << "," << newlen;
        int delta = m_length - m_text.length();

        setChanged(index, qMax(m_length, m_text.length()));

        tmp.fill(m_curColor, len);
        m_color.replace(index, len + delta, tmp);
        //if ( m_color.length() != m_text.length()) {
        //    qDebug() << "=======================color length: " << m_color.length() << ", tmp: " << tmp.length();
        //}
        tmp.fill(m_curAttr, len);
        m_attr.replace(index, len + delta, tmp);
        m_length = m_text.length();
    }
}

// delete cstr from position index of line,
// it will delete len chars,
// if index == -1, delete the whole line
// if len ==-1, delete the rest from index
void TextLine::deleteText(int index, int len)
{
    if (index == -1) { // delete the line
        setChanged(0, m_length);
        reset();
        return;
    }

    if (len == -1)   // only make len larger so it will delete
        len = m_length; // the right from index

    m_text.remove(index, len);
    m_color.remove(index, len);
    m_attr.remove(index, len);

    setChanged(index, m_length);

    m_length = m_text.length();


}

// return str in text for show
// if index == -1, get the whole line
// if len == -1, get the rest from index
QString TextLine::getText(int index, int len)
{
    if (m_text.isEmpty()||len == 0) {
        return QString();
    }
    QString str;

    if (index == -1)
        str = m_text.string();
    else
        str = m_text.mid(index, len);
    return str;
}

QString TextLine::getAttrText(int index, int len, const QString & escape)
{
    QString str;
    int startx;
    char tempcp, tempea;

    if (index == -1) {
        index = 0;
        len = m_length;
    } else if (len == -1)
        len = m_length - index;


    if (index >= m_length)
        return (char *)NULL;

    //qDebug("index=%d len=%d m_length=%d", index, len, m_length);

    for (int i = index; i < index + len && i < m_length; i++) {
        startx = i;
        tempcp = m_color.at(i);
        tempea = m_attr.at(i);
        // get str of the same attribute
        while (tempcp == m_color.at(i) &&
                tempea == m_attr.at(i)) {
            i++;
            if (i >= m_length) break;
        }

        int fg = GETFG(tempcp) + 30;
        int bg = GETBG(tempcp) + 40;
        QString strAttr = QString("%1;%2").arg(fg).arg(bg).toLatin1();//QByteArray::setNum(fg)+';'+QByteArray::setNum(bg)+';';
//   cstrAttr.sprintf("%d;%d;", fg, bg );
        strAttr = escape + strAttr;

        if (GETBOLD(tempea))
            strAttr += "1;";
        if (GETDIM(tempea))
            strAttr += "2;";
        if (GETUNDERLINE(tempea))
            strAttr += "4;";
        if (GETBLINK(tempea))
            strAttr += "5;";
        if (GETRAPIDBLINK(tempea))
            strAttr += "6;";
        if (GETREVERSE(tempea))
            strAttr += "7;";
        if (GETINVISIBLE(tempea))
            strAttr += "8;";
        strAttr.remove(strAttr.length() - 1, 1);
        strAttr += "m";
        str += strAttr; // set attr
        // the text
        str += getText(startx, i - startx);
        // reset attr
        strAttr = escape + "0m";
        str += strAttr;
        i--;
    }

    return str;
}

// reset line
inline void TextLine::reset()
{
    m_length = 0;

    m_text = TermString();
    m_color = "";
    m_attr = "";

    m_curColor = NO_COLOR;
    m_curAttr = NO_ATTR;


}
bool TextLine::hasBlink()
{
    bool blink = false;

    //qDebug() << "m_length: " << m_length << " m_attr " << m_attr.length();
    char tempea;
    for (int i = 0; i < m_length && i < m_attr.length(); i++) {
        tempea = m_attr.at(i);
        if (GETBLINK(tempea)) {
            blink = true;
            break;
        }
    }

    return blink;
}


bool TextLine::isChanged(int &start, int &end)
{
    start = m_start;
    end = m_end;

    return m_bChanged;
}

void TextLine::setChanged(int start, int end)
{
    if (start == -1 && end == -1) {
        m_bChanged = true;
        m_start = start;
        m_end = end;
        return;
    }

    if (m_bChanged) {
        if (!(m_start == -1 && m_end == -1)) {
            m_start = qMin(start, m_start);
            m_end = qMax(end, m_end);
        }
    } else {
        m_start = start;
        m_end = end;
    }

    m_bChanged = true;
}

int TextLine::beginIndex(int pos)
{
    return m_text.beginIndex(pos);
}

int TextLine::size(int index)
{
    return m_text.size(index);
}

int TextLine::pos(int index)
{
    return m_text.pos(index);
}

bool TextLine::isPartial(int index)
{
    return m_text.isPartial(index);
}

void TextLine::setAttr(short attr, int index)
{
    char tmpColor = GETCOLOR(attr);
    char tmpAttr  = GETATTR(attr);
    if (tmpAttr == '\0')
        tmpAttr = NO_ATTR;
    m_color[index] = tmpColor;
    m_attr[index] = tmpAttr;
}

} // namespace QTerm
#include <qtermtextline.moc>
