//
// C++ Implementation: TermString
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "termstring.h"
//#include <QtDebug>
//FIXME: check the range of index!!!
namespace QTerm
{

TermString::TermString()
    :m_string(),m_index()
{
}

TermString::TermString(const QString & str)
    :m_string(str),m_index()
{
    updateIndex();
}

TermString::~TermString()
{
}

void TermString::replace(int index, int length, const QString & str)
{
    if (index >= m_index.size()) {
        m_string += QString(index - m_index.size(), ' ');
        append(str);
        return;
    }
    int startpos = m_index.at(index);
    int index2 = index+length < m_index.size() ? index+length - 1 : m_index.size() - 1;
    int endpos = m_index.at(index2);
    if (startpos == -1 && index > 0) {
        //qDebug("TermString::replace: start pos is in the middle of a char");
        startpos = m_index.at(index-1);
    }
    if (index2 < (m_index.size() - 1) && m_index.at(index2+1) == -1) {
        //qDebug("TermString::mid: end pos is in the middle of a char");
    }
    if (endpos == -1 && index2 > 0) {
        endpos = m_index.at(index2 - 1); //Could this happen at the endof a string?
    }
    //qDebug() << "startpos: " << startpos << " endpos: " << endpos;
    m_string.replace(startpos, endpos-startpos + 1, str);
    updateIndex();
}

void TermString::append(const QString & str)
{
    m_string.append(str);
    updateIndex();
}

void TermString::insert(int index, const QString & str)
{
    if (index > m_index.size()) {
        m_string += QString(index - m_index.size(), ' ');
    }
    m_string.insert(index, str);
    updateIndex();
}

void TermString::updateIndex()
{
    m_index.clear();
    int cellCounter = 0;
    for (int i = 0; i < m_string.length(); i++) {
        m_index.append(i);
        if (m_string.at(i) > 0x7f) {
            m_index.append(-1);
            cellCounter++;
        }
        cellCounter++;
        //qDebug() << "Char: " << m_string.at(i) << " index " << i << " counter " << cellCounter;
    }
}

void TermString::remove(int index, int len)
{
    if (index >= m_index.size()) {
        qDebug("TermString::remove: pos larger than the size of the array");
        return;
    }
    int pos = m_index.at(index);
    if (pos == -1) {
        qDebug("TermString::remove: pos is in the middle of a char");
        pos = m_index.at(index-1);
    }
    m_string.remove(index, len);
}

QString TermString::string()
{
    return m_string;
}

QString TermString::mid(int index, int len)
{
    if (len <= 0 || index < 0) {
        return QString();
    }
    if (index >= m_index.size()) {
        qDebug("TermString::mid: pos larger than the size of the array");
        return QString();
    }
    int startpos= m_index.at(index);
    int index2 = index+len < m_index.size() ? index+len - 1 : m_index.size() - 1;
    int endpos = m_index.at(index2);
    if (startpos == -1) {
        qDebug("TermString::mid: start pos is in the middle of a char");
        startpos = m_index.at(index-1);
    }
    if (index2 < (m_index.size() - 1) && m_index.at(index2+1) == -1) {
        qDebug("TermString::mid: end pos is in the middle of a char");
        return mid(index, len + 1);
    }
    if (endpos == -1) {
        endpos = m_index.at(index2-1);
    }
    return m_string.mid(startpos, endpos-startpos + 1);
}

int TermString::length()
{
    return m_index.size();
}

int TermString::pos(int index)
{
    int result = m_index.indexOf(index);
    if (m_string.at(index) > 0x7f) {
        result += 1;
    }
    return result;
}

bool TermString::isPartial(int index)
{
    //qDebug() << "length " << m_index.size() << ", index: " << index;
    if (m_index.at(index) == -1) {
        return true;
    }
    return false;
}

} // namespace QTerm
