//
// C++ Interface: TermString
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TERMSTRING_H
#define TERMSTRING_H

#include <QtCore/QString>
#include <QList>

namespace QTerm
{
class TermString
{
public:
    TermString();
    TermString(const QString & str);
    ~TermString();
    void insert(int index, const QString & str);
    void append(const QString & str);
    void replace(int index, int length, const QString & str);
    void remove(int index, int length);
    int length();
    QString mid(int index, int len);
    QString string();
    int beginIndex(int pos);
    int pos(int index);
    int size(int index);
    bool isPartial(int index);
    bool isEmpty();
    static int wcwidth(QChar ch);
private:
    struct interval {
        int first;
        int last;
    };
    static int bisearch(QChar ucs, const struct interval *table, int max);
    void updateIndex();
    void dumpIndex();
    QString m_string;
    //TODO: Switch to QVector after we drop Qt4 support
    QList<int> m_index;
};
} // namespace QTerm

#endif // TERMSTRING_H
