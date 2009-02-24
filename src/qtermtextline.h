#ifndef QTERMTEXTLINE_H
#define QTERMTEXTLINE_H

#include <QtCore/QByteArray>
#include "termstring.h"

namespace QTerm
{
class TextLine
{

public:
    TextLine();
    ~TextLine();

    void reset();

    void setChanged(int start, int end);

    void clearChange() {
        m_bChanged = false; m_start = -1; m_end = -1;
    }

    bool isChanged(int &start, int &end);

    QByteArray getColor() {
        return m_color;
    }

    QByteArray getAttr() {
        return m_attr;
    }

    int getLength() {
        return m_length;
    }

    QString getAttrText(int index = -1, int len = -1, const QString & escape = "\x1b\x1b");

    QString getText(int index = -1, int len = -1);

    void insertText(const QString & str, short attr = -1, int index = -1);

    void deleteText(int index = -1, int len = -1);

    void replaceText(const QString & str, short attr = -1, int index = -1, int len = -1);

    bool hasBlink();

    int pos(int index);
    bool isPartial(int index);

protected:


    // we use QString to store any character after decode
    int m_length;
    TermString m_text;
    QByteArray m_color;
    QByteArray m_attr;
    char m_curColor;
    char m_curAttr;
    bool m_bChanged;
    bool m_bBlink;

    int m_start, m_end;
};

} // namespace QTerm

#endif //QTERMTEXTLINE_H
