/*******************************************************************************
 * FILENAME:      qtermbbs.cpp
 * REVISION:      2002.9.3 first created.
 *
 * AUTHOR:        kingson fiasco
 * *****************************************************************************/
/*******************************************************************************
 *                                     NOTE
 * This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermbbs.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"
#include "qterm.h"
//Added by qt3to4:
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <stdio.h>
#include <ctype.h>
#include <QtDebug>

#ifdef SCRIPT_ENABLED
#include <QtScript>
#endif

namespace QTerm
{
BBS::BBS(Buffer * buffer)
    :m_urlPosList()
{
    m_pBuffer = buffer;
    m_engine = NULL;
}

BBS::~BBS()
{
}

#ifdef SCRIPT_ENABLED
void BBS::setScript(QScriptEngine * engine)
{
    m_engine = engine;
}
#endif

void BBS::setScreenStart(int nStart)
{
    m_nScreenStart = nStart;
}

int BBS::getScreenStart()
{
    return m_nScreenStart;
}

bool BBS::setCursorPos(const QPoint& pt, QRect& rc)
{
    QRect rectOld = getSelectRect();

    m_ptCursor = pt;

    updateSelectRect();

    QRect rectNew = getSelectRect();

    rc = rectOld | rectNew;

    return rectOld != rectNew;
}

QString BBS::getMessage()
{
    TextLine * line;
    QString message;

    if (!isUnicolor(m_pBuffer->screen(0)))
        return message;

    int i = 1;
    message = (m_pBuffer->screen(0)->getText().simplified());
    line = m_pBuffer->screen(i);
    while (isUnicolor(line)) {
        message += "\n" + (line->getText());
        i++;
        line = m_pBuffer->screen(i);
    }
    return message;
}


void BBS::setPageState()
{
    m_nPageState = -1;

    TextLine * line;
//  Q3CString color;

    line = m_pBuffer->screen(0);    // first line
    if (isUnicolor(line)) {
        line = m_pBuffer->screen(2);    // third line
        if (isUnicolor(line))
            m_nPageState = 1; //board and article list
        else
            m_nPageState = 0; //menu
    } else {
        line = m_pBuffer->screen(m_pBuffer->line() - 1);  // last line
        if (isUnicolor(line))
            m_nPageState = 2; // reading
    }

#ifdef SCRIPT_ENABLED
    if (m_engine != NULL) {
        QScriptValue func = m_engine->globalObject().property("QTerm").property("setPageState");
        if (func.isFunction()) {
            int ret = func.call().toInt32();
            if (ret != -1) {
                m_nPageState = ret;
            }
        } else {
            qDebug("setPageState is not a function");
        }
        if (m_engine->hasUncaughtException()) {
            QScriptValue exception = m_engine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif
}

int BBS::getCursorType(const QPoint& pt)
{
    if (m_nScreenStart != (m_pBuffer->lines() - m_pBuffer->line()))
        return 8;

    QRect rc = getSelectRect();

    int nCursorType = 8;
#ifdef SCRIPT_ENABLED
    if (m_engine != NULL) {
        TextLine * line = m_pBuffer->at(pt.y());
        int x = pt.x();
        int y = pt.y() - m_nScreenStart;
        int pos = line->pos(m_ptCursor.x());
        QScriptValue func = m_engine->globalObject().property("QTerm").property("getCursorType");
        if (func.isFunction()) {
            int ret = func.call(QScriptValue(), QScriptValueList() << x << y << pos).toInt32();
            if (ret != -1) {
                nCursorType = ret;
            }
        } else {
            qDebug("getCursorType is not a function");
        }
    }
#endif

    switch (m_nPageState) {
    case -1: // not recognized
        nCursorType = 8;
        break;
    case 0:  // menu
        if (pt.x() < 5) // LEFT
            nCursorType = 6;
        else if (rc.contains(pt))// HAND
            nCursorType = 7;
        else
            nCursorType = 8;
        break;
    case 1:  // list
        if (pt.x() < 12) // LEFT
            nCursorType = 6;
        else if (pt.y() - m_nScreenStart < 3) // HOME
            nCursorType = 0;
        else if (pt.y() == m_pBuffer->lines() - 1) // END
            nCursorType = 1;
        else if (pt.x() > m_pBuffer->columns() - 16
                 && pt.y() - m_nScreenStart <= m_pBuffer->line() / 2) //PAGEUP
            nCursorType = 2;
        else if (pt.x() > m_pBuffer->columns() - 16
                 && pt.y() - m_nScreenStart > m_pBuffer->line() / 2) // PAGEDOWN
            nCursorType = 3;
        else if (rc.contains(pt))
            nCursorType = 7;
        else
            nCursorType = 8;
        break;
    case 2:  // read
        if (pt.x() < 12) // LEFT
            nCursorType = 6;
        else if (pt.x() > (m_pBuffer->columns() - 16)
                 && (pt.y() - m_nScreenStart) <= m_pBuffer->line() / 2) // PAGEUP
            nCursorType = 2;
        else if (pt.x() > (m_pBuffer->columns() - 16)
                 && (pt.y() - m_nScreenStart) > m_pBuffer->line() / 2) // PAGEDOWN
            nCursorType = 3;
        else
            nCursorType = 8;
        break;
    case 3:
        break;
    default:
        break;
    }

    return nCursorType;
}

bool BBS::isSelected(int index)
{
    QRect rect = getSelectRect();
    // nothing selected
    if (rect.isNull())
        return false;

    return (index == rect.y());

}

bool BBS::isSelected(const QPoint& pt)
{
    QRect rect = getSelectRect();

    // nothing selected
    if (rect.isNull())
        return false;

    return (pt.y() == rect.y() &&
            pt.x() >= rect.left() &&
            pt.x() <= rect.right());
}

int BBS::getPageState()
{
    return m_nPageState;
}

char BBS::getMenuChar()
{
    return m_cMenuChar;
}

void BBS::updateSelectRect()
{
    QRect rect(0, 0, 0, 0);

    // current screen scrolled
    if (m_nScreenStart != (m_pBuffer->lines() - m_pBuffer->line())) {
        m_rcSelection = rect;
        return;
    }

    TextLine * line = NULL;

    switch (m_nPageState) {
    case -1:
        break;
    case 0:
        if (m_ptCursor.y() - m_nScreenStart >= 7 &&
                m_ptCursor.x() > 5) {
            line = m_pBuffer->at(m_ptCursor.y());
            QString cstr = line->getText(0, m_ptCursor.x());

            QRegExp reg("[a-zA-Z0-9][).\\]]");
            int indexChar = cstr.indexOf(reg);
            if (indexChar != -1) {
                m_cMenuChar = cstr.at(indexChar).toLatin1();

                int nMenuStart = indexChar;
                if (indexChar > 0 && (cstr[indexChar-1] == '(' || cstr[indexChar-1] == '['))
                    nMenuStart--;

                cstr = line->getText();
                reg = QRegExp("[^ ]");

                int nMenuBaseLength = 20;
                int nMenuEnd = cstr.lastIndexOf(reg, nMenuStart + nMenuBaseLength);
                if (nMenuEnd == -1) {
                    nMenuEnd = cstr.indexOf(" ", nMenuStart + nMenuBaseLength);
                }

                int nMenuLength = nMenuBaseLength + 1;
                if (nMenuEnd != -1) {
                    nMenuLength = line->beginIndex(nMenuEnd) - line->beginIndex(nMenuStart) + line->size(line->beginIndex(nMenuEnd));
                }
                if (m_ptCursor.x() >= nMenuStart && m_ptCursor.x() <= nMenuStart + nMenuLength) {
                    rect.setX(line->beginIndex(nMenuStart));
                    rect.setY(m_ptCursor.y());
                    rect.setWidth(nMenuLength);
                    rect.setHeight(1);
                }
            }
        }
        break;
    case 1:
        if ((m_ptCursor.y() - m_nScreenStart) >= 3 &&
                (m_ptCursor.y() - m_nScreenStart) < m_pBuffer->line() - 1 &&
                m_ptCursor.x() >= 12 && m_ptCursor.x() <= m_pBuffer->columns() - 16) {

            line =  m_pBuffer->at(m_ptCursor.y());
            QString str = line->getText();
            if (str.count(" ") != (int) str.length()) {
                rect.setX(0);
                rect.setY(m_ptCursor.y());
                rect.setWidth(m_pBuffer->columns());
                rect.setHeight(1);
            }
        }
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }

#ifdef SCRIPT_ENABLED
    if (m_engine != NULL) {
        rect.setRect(0,0,0,0);
        int x = m_ptCursor.x();
        int y = m_ptCursor.y() - m_nScreenStart;
        QScriptValue func = m_engine->globalObject().property("QTerm").property("isLineClickable");
        if (func.isFunction()) {
            bool clickable = func.call(QScriptValue(), QScriptValueList() << x << y).toBool();
            if (clickable) {
                rect.setX(0);
                rect.setY(m_ptCursor.y());
                rect.setWidth(m_pBuffer->columns());
                rect.setHeight(1);
                m_rcSelection = rect;
                return;
            }
        } else {
            qDebug("isLineClickable is not a function");
        }
        func = m_engine->globalObject().property("QTerm").property("getClickableString");
        if (func.isFunction()) {
            line = m_pBuffer->at(m_ptCursor.y());
            QString clickableString = func.call(QScriptValue(), QScriptValueList() << x << y).toString();
            if (!clickableString.isEmpty()) {
                int index = line->getText().indexOf(clickableString);
                int rectx = line->beginIndex(index);
                int width = line->beginIndex(index+clickableString.length()) - rectx;
                rect.setX(rectx);
                rect.setY(m_ptCursor.y());
                rect.setWidth(width);
                rect.setHeight(1);
            }

        } else {
            qDebug("getClickableString is not a function");
        }
        if (m_engine->hasUncaughtException()) {
            QScriptValue exception = m_engine->uncaughtException();
            qDebug() << "Exception: " << exception.toString();
        }
    }
#endif

    m_rcSelection = rect;
}

QRect BBS::getSelectRect()
{
    return m_rcSelection;
}

bool BBS::isUnicolor(TextLine *line)
{
    QByteArray color = line->getColor();
    bool bSame = true;
    int clr = GETBG(color[0]);

    for (int i = 0; i < color.length() / 2; i++) {
        if (GETBG(color[i]) != clr ||
                GETBG(color[i]) == GETBG(NO_COLOR)) {
            bSame = false;
            break;
        }
    }
    return bSame;
}

bool BBS::isIllChar(QChar ch)
{
    static QString illChars = ",;'\"()<>^[]";
    return ch > '~' || ch < '#' || illChars.contains(ch);
}

bool BBS::isUrl(QRect& rcUrl, QRect& rcOld)
{
    //return checkUrl(rcUrl, rcOld, false);
    return checkUrl(rcUrl, rcOld);
}
bool BBS::isIP(QRect& rcUrl, QRect& rcOld)
{
    return checkIP(rcUrl, rcOld);
}

bool BBS::checkUrl(QRect & rcUrl, QRect & rcOld)
{
    m_strUrl = "";
    int pt = (m_ptCursor.y()-m_nScreenStart)*m_pBuffer->columns()+m_ptCursor.x();
    if (!m_urlPosList.isEmpty()) {
        QPair<int, int> url;
        foreach (url, m_urlPosList) {
            if (pt > url.first && pt < url.second) {
                //qDebug() << "get text: " << getText(url.first, url.second);
                m_strUrl = getText(url.first, url.second);
                QRegExp urlRe("^(mailto:|(https?|mms|rstp|ftp|gopher|telnet|ed2k|file)://)");
                QRegExp emailRe("^[A-Z0-9._%-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}");
                if (urlRe.indexIn(m_strUrl)==-1) {
                    if (emailRe.indexIn(m_strUrl)==-1) {
                        m_strUrl = "http://"+m_strUrl;
                    } else {
                        m_strUrl = "mailto:"+m_strUrl;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

// TODO: I do not know whether we need to support ip addr in multiple lines
// TODO: Further Simplification
bool BBS::checkIP(QRect& rcUrl, QRect& rcOld)
{
    static const char http[] = "http://";
    static const char https[] = "https://";
    static const char mms[] = "mms://";
    static const char rstp[] = "rstp://";
    static const char ftp[] = "ftp://";
    static const char mailto[] = "mailto:";
    static const char telnet[] = "telnet://";
    int at = m_pBuffer->at(m_ptCursor.y())->pos(m_ptCursor.x());
    if (at == -1) {
        return false;
    }

    if (at > m_rcUrl.left() && at < m_rcUrl.right() && m_rcUrl.y() == m_ptCursor.y()) {
        rcUrl = m_rcUrl;
        rcOld = m_rcUrl;
        return true;
    }
    m_strIP = "";
    rcOld = m_rcUrl;

    QString strText = m_pBuffer->at(m_ptCursor.y())->getText();

    if (at >= strText.length())
        return false;

    int i, index, begin, end, dot, url, host, ata;
    int ip_begin = 0;
    int ip_end = 0;

    for (i = at; i >= 0 && !isIllChar(strText.at(i)); i--);
    url = i + 1;
    for (i = at; i < strText.length() && !isIllChar(strText.at(i)); i++);
    end = i;

    int nNoType = -1;
    QRegExp urlRe("^(mailto:|(https?|mms|rstp|ftp|gopher|telnet|ed2k|file)://)");
    if ((begin = urlRe.indexIn(strText))!=-1) {
        if (urlRe.capturedTexts().contains("mailto:")) {
            if ((ata = strText.indexOf('@', begin + 1)) == -1)
                host = url + (ata - begin) + 1;
            else
                return -1;
        } else {
            host = url+urlRe.matchedLength();
        }
    } else {
        begin = url;
        if ((ata = strText.indexOf('@', begin + 1)) != -1) {
            host = url + (ata - begin) + 1;
            nNoType = 0;
        } else {
            host = url;
            nNoType = 1;
        }
    }

    ip_begin = host;
    ip_end = end;
    for (index = host, dot = host - 1, i = 0; index < end && strText.at(index) != '/'; index++) {
        if (strText.at(index) == '@') {
            ip_begin = index + 1;
        }
        if (strText.at(index) == ':') {
            ip_end = index;
        }
        if (strText.at(index) == '.') {
            if (index <= dot + 1) // xxx..x is illegal
                return false;
            dot = index;
            i++;
        } else {
                if (!strText.at(index).isLetterOrNumber() &&
                        strText.at(index) != '-' &&
                        strText.at(index) != '_' &&
                        strText.at(index) != '~' &&
                        strText.at(index) != ':' &&
                        strText.at(index) != '*' &&  //add by cyber@thuee.org, allow ip like 166.111.1.*
                        strText.at(index) != '@'
                   )
                    return false;
        }
    }
    if (index > 0 && strText.at(index - 1) == '.')
        return false;

    if (i < 1 || ip_end <= ip_begin || end <= url)
        return false;
    m_strIP = strText.mid(ip_begin, ip_end - ip_begin);//get the pure ip address
    if (m_strIP[ m_strIP.length()-1 ] == '*')
        m_strIP.replace(m_strIP.length() - 1 , 1, "1");
    rcUrl = QRect(m_pBuffer->at(m_ptCursor.y())->beginIndex(url), m_ptCursor.y(), end - url, 1);
    return true;
}

QString BBS::getUrl()
{
    return m_strUrl;
}

QString BBS::getIP()
{
    return m_strIP;
}

bool BBS::isPageComplete()
{
    return m_pBuffer->caret().y() == (m_pBuffer->line() - 1)
           && m_pBuffer->caret().x() == (m_pBuffer->columns() - 1);
}

void BBS::updateUrlList()
{
    m_urlPosList.clear();
    for (int i = 0; i < m_pBuffer->line(); i++) {
        TextLine * lineBegin = m_pBuffer->screen(i);
        if (lineBegin == NULL) {
            break;
        }
        for (int j = 0; j < m_pBuffer->columns(); j++) {
            int index = checkUrlBegin(lineBegin->getText(),j);
            if (index != -1) {
                int urlBegin = i*m_pBuffer->columns() + lineBegin->beginIndex(index);
                int index2 = checkUrlEnd(lineBegin->getText(), index);
                bool multiline = false;
                if (index2 == -1) {
                    while (m_pBuffer->screen(i+1) != NULL&&index2 == -1&&i < m_pBuffer->line()) {
                        i++;
                        TextLine * lineCurrent = m_pBuffer->screen(i);
                        index2 = checkUrlEnd(lineCurrent->getText(), 0);
                    }
                    multiline = true;
                }
                int urlEnd = i*m_pBuffer->columns() + m_pBuffer->screen(i)->beginIndex(index2);
                if (verifyUrl(urlBegin, urlEnd))
                    m_urlPosList.append(qMakePair(urlBegin, urlEnd));
                if (multiline)
                    break;
                j = index2;
            }
        }
    }
//    if (!m_urlPosList.isEmpty()) {
//        QPair<int, int> url;
//        foreach (url, m_urlPosList) {
//            qDebug() << getText(url.first, url.second);
//        }
//    }
}

//FIXME: The function use the assumption that the Url is latin only
bool BBS::verifyUrl(int urlBegin, int urlEnd)
{
    int i, index, begin, end, dot, url, host, ata;
    int ip_begin = 0;
    int ip_end = 0;

    QString strText = getText(urlBegin, urlEnd);

    int nNoType = -1;
    url = 0;
    host = 0;
    end = strText.length()-1;
    QRegExp urlRe("^(mailto:|(https?|mms|rstp|ftp|gopher|telnet|ed2k|file)://)");
    if ((begin = urlRe.indexIn(strText))!=-1) {
        if (urlRe.capturedTexts().contains("mailto:")) {
            if ((ata = strText.indexOf('@', begin + 1)) == -1)
                host = url + (ata - begin) + 1;
            else
                return -1;
        } else {
            host = url+urlRe.matchedLength();
        }
    } else {
        begin = url;
        if ((ata = strText.indexOf('@', begin + 1)) != -1) {
            host = url + (ata - begin) + 1;
            nNoType = 0;
        } else {
            host = url;
            nNoType = 1;
        }
    }
    ip_begin = host;
    ip_end = end;
    for (index = host, dot = host - 1, i = 0; index < end && strText.at(index) != '/'; index++) {
        if (strText.at(index) == '.') {
            if (index <= dot + 1) // xxx..x is illegal
                return false;
            dot = index;
            i++;
        } else {
                if (!strText.at(index).isLetterOrNumber() &&
                        strText.at(index) != '-' &&
                        strText.at(index) != '_' &&
                        strText.at(index) != '~' &&
                        strText.at(index) != ':' &&
                        strText.at(index) != '[' &&
                        strText.at(index) != ']' &&
                        strText.at(index) != '%' &&
                        strText.at(index) != '|' &&
                        strText.at(index) != '=' &&
                        strText.at(index) != '@'
                   )
                    return false;
        }
    }
    if (index > 0 && strText.at(index - 1) == '.')
        return false;

    if (i < 1 || end <= url)
        return false;
    return true;
}

QString BBS::getText(int startpt, int endpt)
{
    QString text;
    QPoint start((int)(startpt%m_pBuffer->columns()),(int)(startpt/m_pBuffer->columns()));
    QPoint end((int)(endpt%m_pBuffer->columns()),(int)(endpt/m_pBuffer->columns()));
    TextLine * line;
    if (start.y() == end.y()) {
        line = m_pBuffer->screen(start.y());
        if (line != NULL)
            text = line->getText(start.x(), end.x()-start.x());
    } else if (end.y() > start.y()) {
        line = m_pBuffer->screen(start.y());
        if (line != NULL)
            text = line->getText(start.x(), -1);
        for (int i = start.y()+1; i < end.y(); i++) {
            line = m_pBuffer->screen(i);
            if (line == NULL) {
                return text;
            }
            text += line->getText(0,-1);
        }
        line = m_pBuffer->screen(end.y());
        if (line != NULL && end.x() != 0)
            text += m_pBuffer->screen(end.y())->getText(0,end.x());
    } else {
        qDebug() << start << end;
    }
    return text;
}

int BBS::checkUrlBegin( const QString & lineText, int index)
{
    int i = 0;
    QString urlText = lineText;
    for (i = index; i < urlText.length() && isIllChar(urlText.at(i)); i++);
    if (i < urlText.length()) {
        return i;
    }
    return -1;
}

int BBS::checkUrlEnd( const QString & lineText, int index)
{
    int i = 0;
    QString urlText = lineText;
    if (urlText.isEmpty()) {
        return 0;
    }
    for (i = index; i< urlText.length() && !isIllChar(urlText.at(i)); i++);
    if (i < urlText.length()) {
        return i;
    }
    return -1;
}

} // namespace QTerm

