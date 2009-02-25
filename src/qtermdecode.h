#ifndef QTERMDECODE_H
#define QTERMDECODE_H

#include <QtCore/QObject>

class QTextDecoder;

namespace QTerm
{
class Decode;

class Buffer;

// this for FSM
typedef void (Decode::*StateFunc)();

struct StateOption {
    int byte;  // char value to look for; -1==end/default
    StateFunc action;
    StateOption *nextState;
};

class Decode : public QObject
{
    Q_OBJECT

public:
    Decode(Buffer *, QTextCodec * codec);
    ~Decode();

    // translate data from telnet socket to our own buffer
    void decode(const char *cstr, int length);

    bool bellReceive() {
        return m_bBell;
    }

//signals:
// void decodeFinished();

private:
// escape sequence actions
// you'd better see FSM structure array in Decode.cpp

    void nextLine();
    void getAttr();
    void setMargins();

    // char screen functions
    void deleteStr();
    void deleteLine();
    void insertStr();
    void insertLine();
    void eraseStr();
    void eraseLine();
    void eraseScreen();

    // cursor functions
    void saveCursor();
    void restoreCursor();
    void cursorLeft();
    void cursorDown();
    void cursorRight();
    void cursorUp();
    void cursorPosition();

    /*****************************************************************************/
// other escape sequence actions
    void normalInput();

    // action parameters
    void clearParam();
    void paramDigit();
    void nextParam();

    // non-printing characters
    void cr(), lf(), ff(), bell(), tab(), bs();

    void setMode();
    void resetMode();

    void saveMode();
    void restoreMode();

    void test();

signals:
    void mouseMode(bool);

private:

    bool m_bBell;
    short m_curAttr, m_defAttr;

    // ********** ansi decoder states ****************
    StateOption *currentState;
    static StateOption normalState[], escState[], bracketState[], privateState[];

    // ********** decoder  *****************
    const char *inputData;
    int inputLength, dataIndex;

    int nParam, param[30];
    bool bParam;

    bool bSaveMode[30];
    bool bCurMode[30];

    Buffer * m_pBuffer;

    QTextCodec * m_decoder;
    QTextCodec::ConverterState * m_state;
    //bool CheckESCState(char next);

    bool m_test;
};

} // namespace QTerm

#endif
