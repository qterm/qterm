/*******************************************************************************
FILENAME: qtermbuffer.cpp
REVISION: 2002.6.15 first created.

AUTHOR:  kingson
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 ******************************************************************************/

#include "qterm.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"
#include "termstring.h"


#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QRect>

//#include <QtDebug>

namespace QTerm
{

Buffer::Buffer(int line, int column, int limit)
{
    m_lin = line;
    m_col = column;
    m_limit = limit;

    m_lines = 0;

    while (m_lineList.count() < m_lin)
        m_lineList.append(new TextLine);

    m_curAttr = SETCOLOR(NO_COLOR) | SETATTR(NO_ATTR);


    m_caretX = 0;
    m_caretY = 0;

    m_oldCaretX = 0;
    m_oldCaretY = 0;


    m_top = 0;
    m_bottom = m_lin - 1;

    Insert_Mode = false;
    NewLine_Mode = false;

    m_ptSelStart = QPoint(-1, -1);
    m_ptSelEnd = QPoint(-1, -1);

}


Buffer::~Buffer()
{

}

void Buffer::setSize(int col, int lin)
{
    if (m_col == col && m_lin == lin)
        return;

    if (m_lin < lin)
        for (int i = 0; i < lin - m_lin; i++)
            m_lineList.append(new TextLine);
    else if (m_lin > lin)
        for (int i = 0; i < m_lin - lin; i++)
            delete m_lineList.takeAt(m_lines + m_top);

    m_col = col;
    m_lin = lin;

    m_top = 0;
    m_bottom = m_lin - 1;

    m_caretY = qMin(m_caretY, lin - 1);
    m_oldCaretY = qMin(m_caretY, lin - 1);

    clearSelect();

    emit windowSizeChanged(m_col, m_lin);

    emit bufferSizeChanged();
}

int Buffer::columns()
{
    return m_col;
}

int Buffer::lines()
{
    return m_lin + m_lines;
}
int Buffer::line()
{
    return m_lin;
}

TextLine * Buffer::at(int y)
{
    return m_lineList.value(y , NULL);
}

TextLine * Buffer::screen(int y)
{
    return m_lineList.value(y + m_lines , NULL);
}

void Buffer::setCurAttr(short attr)
{
    m_curAttr = attr;
}

void Buffer::setBuffer(const QString & str, int n)
{

    TextLine * line =  m_lineList.value(m_lines + m_caretY, NULL);

    if (line == NULL) {
        qWarning("setBuffer null line");
        return;
    }

    if (Insert_Mode /*bInsert*/)
        line->insertText(str, m_curAttr, m_caretX);
    else
        line->replaceText(str, m_curAttr, m_caretX);
// FIXME: optimization
    moveCursorOffset(TermString(str).length(), 0);
}

//nextline
void Buffer::newLine()
{
    if (NewLine_Mode) {
        moveCursor(0, m_caretY);
        return;
    }

    if (m_caretY == m_bottom) {
        if (m_bottom == m_lin - 1)
            addHistoryLine(1);
        else
            scrollLines(m_top, 1);
    } else
        if (m_caretY < m_lin - 1)
            moveCursorOffset(0, 1);

}

//table
void Buffer::tab()
{
    int x = (m_caretX + 8) & -8;   // or ( caretX/8 + 1 ) * 8
    moveCursor(x, m_caretY);

    QByteArray cstr;
    cstr.fill(' ', x - m_caretX);
    setBuffer(cstr, x - m_caretX);
}

void Buffer::setMargins(int top, int bottom)
{
    m_top = qMax(top - 1, 0);
    m_bottom = qMin(qMax(bottom - 1, 0), m_lin - 1);
}

// cursor functions
void Buffer::moveCursor(int x, int y)
{
    // detect index boundary
    if (x >= m_col)
        x = m_col;
    if (x < 0)
        x = 0;

    int stop = m_caretY < m_top ? 0 : m_top;
    if (y < stop)
        y = stop;

    stop  = m_caretY > m_bottom ? m_lin - 1 : m_bottom;
    if (y > stop)
        y = stop;


    m_caretX = x;
    m_caretY = y;

}

//
void Buffer::restoreCursor()
{
    moveCursor(m_saveX, m_saveY);
}

void Buffer::moveCursorOffset(int x, int y)
{
    moveCursor(m_caretX + x, m_caretY + y);
}

void Buffer::saveCursor()
{
    m_saveX = m_caretX;
    m_saveY = m_caretY;
}

void Buffer::cr()
{
    m_caretX = 0;
}

// the screen oriented caret
QPoint Buffer::caret()
{
    return QPoint(m_caretX, m_caretY);
}

// the buffer oriented caret
int Buffer::caretX()
{
    return m_caretX;
}
int Buffer::caretY()
{
    return m_caretY + m_lines;
}

// erase functions
void Buffer::eraseStr(int n)
{
    TextLine * line = m_lineList.at(m_caretY + m_lines);

    int x = line->getLength() - m_caretX;

    clearArea(m_caretX, m_caretY, qMin(n, x), 1, m_curAttr);
}
// delete functions
void Buffer::deleteStr(int n)
{
    TextLine * line = m_lineList.at(m_caretY + m_lines);

    int x = line->getLength() - m_caretX;

    if (n >= x)
        clearArea(m_caretX, m_caretY, x, 1, m_curAttr);
    else
        shiftStr(m_caretY, m_caretX, x, -n);
}
// insert functions
void Buffer::insertStr(int n)
{
    TextLine * line = m_lineList.at(m_caretY + m_lines);

    int x = line->getLength() - m_caretX;

    if (n >= x)
        clearArea(m_caretX, m_caretY, x, m_caretY, m_curAttr);
    else
        shiftStr(m_caretY, m_caretX, x, n);
}

// shift str in one line
// num > 0 shift right
void Buffer::shiftStr(int index, int startX, int len, int num)
{
    if (!num)
        return;

    QByteArray cstr;
    cstr.fill(' ', qAbs(num));

    TextLine * line = m_lineList.at(index + m_lines);

    if (num > 0) { // insert
        line->insertText(cstr, m_curAttr, startX);
        return;
    }

    if (len + startX > line->getLength())
        len = line->getLength() - startX;

    if (num < 0) { // delete
        line->deleteText(startX + num, -num);
        line->insertText(cstr, m_curAttr, startX + len + num);
        return;
    }
}


void Buffer::deleteLine(int n)
{
    int y = m_bottom - m_caretY;

    if (n >= y)
        clearArea(0, m_caretY, -1, y, m_curAttr);
    else
        scrollLines(m_caretY, n);
}

void Buffer::insertLine(int n)
{
    int y = m_bottom - m_caretY;

    if (n >= y)
        clearArea(0, m_caretY, -1, y, m_curAttr);
    else
        scrollLines(m_caretY, -n);
}

void Buffer::eraseToEndLine()
{
    clearArea(m_caretX, m_caretY, -1, 1, m_curAttr);
}

void Buffer::eraseToBeginLine()
{
    clearArea(0, m_caretY, m_caretX, 1, m_curAttr);
}

void Buffer::eraseEntireLine()
{
    clearArea(0, m_caretY, -1, 1, m_curAttr);
}
//scroll line
//num > 0 scroll up
void Buffer::scrollLines(int startY, int num)
{
    if (!num)
        return;

    if (num > 0) { // delete
        while (num) {
            delete m_lineList.takeAt(m_lines + startY);
            m_lineList.insert(m_lines + m_bottom, new TextLine);
            num--;
        }
    }

    if (num < 0) { // insert
        while (num) {
            delete m_lineList.takeAt(m_lines + m_bottom);
            m_lineList.insert(m_lines + startY, new TextLine);
            num++;
        }
    }

    for (int i = m_lines + startY; i < m_lines + m_bottom; i++)
        m_lineList.at(i)->setChanged(-1, -1);

}

void Buffer::eraseToEndScreen()
{
    if (m_caretX == 0 && m_caretY == 0) {
        eraseEntireScreen();
        return;
    }

    clearArea(m_caretX, m_caretY, -1, 1, m_curAttr);   // erase to end line

    if (m_caretY < m_bottom - 1)   // erase
        clearArea(0, m_caretY + 1, -1, m_bottom - m_caretY - 1, m_curAttr);

}
void Buffer::eraseToBeginScreen()
{
    if (m_caretX == m_col - 1 && m_caretY == m_lin - 1) {
        eraseEntireScreen();
        return;
    }

    clearArea(0, m_caretY, m_caretX, 1, m_curAttr);   // erase to begin line
    if (m_caretY > 0)    // erase
        clearArea(0, 0, -1, m_caretY - 1, m_curAttr);

}
void Buffer::eraseEntireScreen()
{
    addHistoryLine(m_lin);

    clearArea(0, 0, m_col, m_bottom, m_curAttr);
}

// width = -1 : clear to end
void Buffer::clearArea(int startX, int startY, int width, int height, short attr)
{
    QByteArray cstr;

    TextLine * line;

    if (startY < m_top)
        startY = m_top;

    if (height > (int)(m_bottom - startY + 1))
        height = m_bottom - startY;

    for (int i = startY; i < height + startY; i++) {
        line = m_lineList.at(i + m_lines);

        if (width == -1) {
            cstr.fill(' ', line->getLength() - startX);
        } else
            cstr.fill(' ', width);
        line->replaceText(cstr, attr, startX);
    }

}

void Buffer::addHistoryLine(int n)
{
    while (n) {
        if (m_lines == m_limit) {
            m_lineList.removeFirst();
            //m_ptSelStart.setY( m_ptSelStart.y()-1 );
            //m_ptSelEnd.setY( m_ptSelEnd.y()-1 );
            //if(m_ptSelStart.y()<0)
            // m_ptSelStart=m_ptSelEnd=QPoint(-1,-1);
        }

        m_lineList.append(new TextLine);
        m_lines = qMin(m_lines + 1, m_limit);
        n--;
    }

    for (int i = m_lines + 0; i < m_lines + m_bottom; i++)
        m_lineList.at(i)->setChanged(-1, -1);

    emit bufferSizeChanged();
}

void Buffer::startDecode()
{
    m_oldCaretX = m_caretX;
    m_oldCaretY = m_caretY;

}

void Buffer::endDecode()
{
    TextLine * line = m_lineList.at(m_oldCaretY + m_lines);

    line->setChanged(m_oldCaretX, m_oldCaretX + 1);

    line = m_lineList.at(m_caretY + m_lines);

    line->setChanged(m_caretX, m_caretX + 1);

    clearSelect();

}

void Buffer::setMode(int mode)
{
    switch (mode) {
    case INSERT_MODE:
        Insert_Mode = true;
        break;
    case NEWLINE_MODE:
        NewLine_Mode = true;
        break;
    default:
        break;
    }

}
void Buffer::resetMode(int mode)
{
    switch (mode) {
    case INSERT_MODE:
        Insert_Mode = false;
        break;
    case NEWLINE_MODE:
        NewLine_Mode = false;
        break;
    default:
        break;
    }
}

void Buffer::setSelect(const QPoint& pt1, const QPoint& pt2, bool rect)
{
    QPoint ptSelStart, ptSelEnd;

    if (pt1.y() == pt2.y()) {
        ptSelStart = pt1.x() < pt2.x() ? pt1 : pt2;
        ptSelEnd = pt1.x() > pt2.x() ? pt1 : pt2;
    } else {
        ptSelStart = pt1.y() < pt2.y() ? pt1 : pt2;
        ptSelEnd = pt1.y() > pt2.y() ? pt1 : pt2;
    }

    if (m_ptSelStart == m_ptSelEnd) { // the first
        for (int i = ptSelStart.y(); i <= m_ptSelEnd.y(); i++)
            at(i)->setChanged(-1, -1);
    } else { // find the changed area
        if (rect) {
            for (int i = qMin(ptSelStart.y(), m_ptSelStart.y()); i <= qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
                at(i)->setChanged(-1, -1);
        } else {
            if (ptSelStart == m_ptSelStart)
                for (int i = qMin(ptSelEnd.y(), m_ptSelEnd.y()); i <= qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
                    at(i)->setChanged(-1, -1);
            else
                if (ptSelEnd == m_ptSelEnd)
                    for (int i = qMin(ptSelStart.y(), m_ptSelStart.y()); i <= qMax(ptSelStart.y(), m_ptSelStart.y()); i++)
                        at(i)->setChanged(-1, -1);
                else
                    for (int i = qMin(ptSelStart.y(), m_ptSelStart.y()); i <= qMax(ptSelEnd.y(), m_ptSelEnd.y()); i++)
                        at(i)->setChanged(-1, -1);

        }
    }

    m_ptSelStart = ptSelStart;
    m_ptSelEnd = ptSelEnd;
}

void Buffer::clearSelect()
{
    if (m_ptSelStart == m_ptSelEnd)
        return;

    for (int i = m_ptSelStart.y(); i <= m_ptSelEnd.y(); i++)
        at(i)->setChanged(-1, -1);

    m_ptSelStart = m_ptSelEnd = QPoint(-1, -1);
}

bool Buffer::isSelected(int index)
{
    if (m_ptSelStart == m_ptSelEnd)
        return false;
    else
        return index >= m_ptSelStart.y() && index <= m_ptSelEnd.y();
}

bool Buffer::isSelected(const QPoint& pt, bool rect)
{
    if (m_ptSelStart == m_ptSelEnd)
        return false;

    if (rect) {
        int x1, x2;
        x1 = qMin(m_ptSelStart.x(), m_ptSelEnd.x());
        x2 = qMax(m_ptSelStart.x(), m_ptSelEnd.x());
        return pt.x() < x2 && pt.x() >= x1 && pt.y() >= m_ptSelStart.y() && pt.y() <= m_ptSelEnd.y();
    }

    if (pt.y() == m_ptSelStart.y() && m_ptSelStart.y() == m_ptSelEnd.y())
        return pt.x() >= m_ptSelStart.x() && pt.x() < m_ptSelEnd.x();
    else
        if (pt.y() == m_ptSelStart.y())
            return pt.x() >= m_ptSelStart.x();
        else
            if (pt.y() == m_ptSelEnd.y())
                return pt.x() < m_ptSelEnd.x();
            else
                return pt.y() > m_ptSelStart.y() && pt.y() < m_ptSelEnd.y();

}

QString Buffer::getSelectText(bool rect, bool color, const QByteArray& escape)
{
    QString strSelect = "";

    if (m_ptSelStart == m_ptSelEnd)
        return strSelect;

    QRect rc;
    QString strTemp;

    for (int i = m_ptSelStart.y(); i <= m_ptSelEnd.y(); i++) {
        rc = getSelectRect(i, rect);

        if (color)
            strTemp = m_lineList.at(i)->getAttrText(rc.left(), rc.width(), escape);
        else
            strTemp = m_lineList.at(i)->getText(rc.left(), rc.width());

        //qDebug() << strTemp;
        //FIXME: potential problem?
        int pos = strTemp.lastIndexOf(QRegExp("[\\S]"));
        strTemp.truncate(pos + 1);
        strSelect += strTemp;

        // add newline except the last line
        if (i != m_ptSelEnd.y()) {
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
            strSelect += '\r';
#endif
            strSelect += '\n';
        }
    }

    return strSelect;
}

QRect Buffer::getSelectRect(int index, bool rect)
{
    if (rect)
        return QRect(qMin(m_ptSelStart.x(), m_ptSelEnd.x()), index, qAbs(m_ptSelEnd.x() - m_ptSelStart.x()) + 1, 1);
    else
        if (m_ptSelStart.y() == m_ptSelEnd.y())
            return QRect(m_ptSelStart.x(), index, qMin(m_ptSelEnd.x()-1, m_col) - m_ptSelStart.x() + 1, 1);
        else
            if (index == m_ptSelStart.y())
                return QRect(m_ptSelStart.x(), index, qMax(0, m_col - m_ptSelStart.x()), 1);
            else
                if (index == m_ptSelEnd.y())
                    return QRect(0, index, qMin(m_col, m_ptSelEnd.x()), 1);
                else
                    return QRect(0, index, m_col, 1);
}

} // namespace QTerm

#include <qtermbuffer.moc>
