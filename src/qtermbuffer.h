#ifndef QTERMBUFFER_H
#define QTERMBUFFER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPoint>

#define INSERT_MODE 0
#define NEWLINE_MODE 1


class QString;
class QByteArray;
class QRect;

namespace QTerm
{
class TextLine;

class Buffer : public QObject
{
    Q_OBJECT

public:
    Buffer(int, int, int);
    ~Buffer();

    TextLine * at(int);
    TextLine * screen(int);

    void setSize(int, int);

    int columns();
    int lines();
    int line();

    int  caretX();
    int  caretY();

    QPoint caret();

    // string
    void setBuffer(const QString&, int n);
    void saveAttr();
    void restoreAttr();

    // attribute
    void setCurAttr(short);

    // string functions
    void deleteStr(int);
    void insertStr(int);
    void eraseStr(int);

    void deleteLine(int);
    void insertLine(int);

    void eraseToBeginLine();
    void eraseToEndLine();
    void eraseEntireLine();

    void eraseToBeginScreen();
    void eraseToEndScreen();
    void eraseEntireScreen();


    // cursor functions
    void moveCursorOffset(int, int);
    void moveCursor(int, int);
    void saveCursor();
    void restoreCursor();
    void cr();

    // non-printing characters
    void newLine();
    void tab();
    void setMargins(int, int);

    // mode set
    void setMode(int);
    void resetMode(int);

    // for test
    void startDecode();
    void endDecode();

    // selecting function
    void setSelect(const QPoint&, const QPoint&, bool);
    void clearSelect();
    bool isSelected(const QPoint&, bool);
    bool isSelected(int);
    QString getSelectText(bool, bool, const QByteArray&);
    QRect getSelectRect(int, bool);

signals:
    void bufferSizeChanged();
    void windowSizeChanged(int, int);

protected:
    void shiftStr(int, int, int, int);
    void scrollLines(int, int);
    void clearArea(int, int, int, int, short);
    void addHistoryLine(int);

    // margins
    int m_top, m_bottom;

    //
    int m_col, m_lin, m_lines, m_limit;

    TextLine * m_pCurrentLine;

    QList<TextLine*>  m_lineList;


    // caret
    int   m_caretX, m_caretY, m_saveX, m_saveY;
    int   m_tmpX, m_tmpY;

    // attribute
    short m_curAttr;
    short m_tmpAttr;

    // Modes
    bool Insert_Mode, NewLine_Mode;

    // for test
    int m_oldCaretX, m_oldCaretY;

    // selecting
    QPoint m_ptSelStart;
    QPoint m_ptSelEnd;

    bool m_bPage;
};

} // namespace QTerm

#endif // QTERMBUFFER_H
