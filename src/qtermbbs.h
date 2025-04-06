#ifndef QTERMBBS_H
#define QTERMBBS_H

#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QList>

class QRect;
#ifdef SCRIPT_ENABLED
class QQmlEngine;
#endif

namespace QTerm
{
class TextLine;
class Buffer;
#ifdef SCRIPT_ENABLED
class ScriptHelper;
#endif

class BBS
{
public:
    BBS(Buffer *);
    ~BBS();

#ifdef SCRIPT_ENABLED
    void setScript(QQmlEngine *, ScriptHelper *);
#endif
    /* -1 -- undefined
     *  0 -- menu
     *  1 -- article or board list
     *  2 -- read
     *  3 -- edit
     */
    void setPageState();
    void setScreenStart(int);
    int getScreenStart();
    bool setCursorPos(const QPoint&, QRect&, QRect&);

    bool isSelected(int);
    bool isSelected(const QPoint&);
    bool isUrl(QRect&, QRect&);
    bool isIP(QRect&, QRect&);
    bool isPageComplete();
    bool checkUrl(QRect&, QRect&);
    bool checkIP(QRect&, QRect&);

    QString getUrl();
    QString getIP();
    int getPageState();
    char getMenuChar();
    QRect getSelectRect();
    void updateSelectRect();
    int getCursorType(const QPoint&);
    QString getMessage();
    QString getText(int startpt, int endpt);
    void updateUrlList();

    int checkUrlBegin(const QString & lineText, int index);
    int checkUrlEnd(const QString & lineText, int index);
    bool verifyUrl(int urlBegin, int urlEnd);

protected:
    bool isUnicolor(TextLine *);
    bool isIllChar(QChar);
    Buffer *m_pBuffer;

    QRect m_rcUrl;
    QRect m_rcSelection;
    QString m_strUrl;
    QString m_strIP;
    char m_cMenuChar;
    int m_nPageState;
    QPoint m_ptCursor;
    int m_nScreenStart;
    QList< QPair<int,int> > m_urlPosList;
    QString m_url;
#ifdef SCRIPT_ENABLED
    QQmlEngine * m_scriptEngine;
    ScriptHelper * m_scriptHelper;
#endif
};

} // namespace QTerm


#endif // QTERMBBS_H
