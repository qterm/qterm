/*******************************************************************************
FILENAME:      qtermdecode.cpp
REVISION:      2001.8.12 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qterm.h"
#include "qtermdecode.h"
#include "qtermbuffer.h"

#include <QtCore/QTextDecoder>
#include <QtCore/QByteArray>
//#include <QtDebug>

#define MODE_MouseX11 0
namespace QTerm
{
/************************************************************************/
// state for FSM
// please read ANSI decoding
StateOption Decode::normalState[] = {
    { CHAR_CR,   &Decode::cr,   normalState },
    { CHAR_LF,   &Decode::lf,   normalState },
    { CHAR_FF,   &Decode::ff,   normalState },
    { CHAR_TAB,  &Decode::tab,   normalState },
    { CHAR_BS,   &Decode::bs,   normalState },
    { CHAR_BELL,   &Decode::bell,   normalState },
    { CHAR_ESC,  0,       escState    },
    { CHAR_NORMAL,  &Decode::normalInput,  normalState }
};

// state after a ESC_CHAR
// only for BBS, so I reduce a lots
StateOption Decode::escState[] = {
    { '[',   &Decode::clearParam,    bracketState },
    // VT52
    { 'A',   &Decode::cursorUp,   normalState },
    { 'B',  &Decode::cursorDown,  normalState },
    { 'C',   &Decode::cursorRight,  normalState },
    { 'D',   &Decode::cursorLeft,  normalState },
    { 'J',   &Decode::eraseScreen,  normalState },
    { 'K',   &Decode::eraseLine,  normalState },
    { 'Z',  &Decode::test,    normalState },
    { '>',  &Decode::test,    normalState },
    { '<',  &Decode::test,    normalState },
    { CHAR_NORMAL,  0,       normalState }
};

// state after ESC [
StateOption Decode::bracketState[] = {
    { '0',   &Decode::paramDigit,  bracketState },
    { '1',   &Decode::paramDigit,  bracketState },
    { '2',   &Decode::paramDigit,  bracketState },
    { '3',   &Decode::paramDigit,  bracketState },
    { '4',   &Decode::paramDigit,  bracketState },
    { '5',  &Decode::paramDigit,  bracketState },
    { '6',   &Decode::paramDigit,  bracketState },
    { '7',   &Decode::paramDigit,  bracketState },
    { '8',   &Decode::paramDigit,  bracketState },
    { '9',   &Decode::paramDigit,  bracketState },
    { ';',   &Decode::nextParam,  bracketState },

    { '?',  &Decode::clearParam,  privateState},

    { 'A',   &Decode::cursorUp,   normalState },
    { 'B',   &Decode::cursorDown,  normalState },
    { 'C',   &Decode::cursorRight,  normalState },
    { 'D',   &Decode::cursorLeft,  normalState },
    { 'H',   &Decode::cursorPosition,   normalState },
    { 'J',   &Decode::eraseScreen,  normalState },
    { 'K',   &Decode::eraseLine,  normalState },
    { 'L',   &Decode::insertLine,  normalState },
    { 'M',   &Decode::deleteLine,  normalState },
    { 'P',   &Decode::deleteStr,  normalState },
    { 'X',   &Decode::eraseStr,   normalState },
    { 'f',   &Decode::cursorPosition,   normalState },
    { 'h',  &Decode::setMode,   normalState },
    { 'l',  &Decode::resetMode,  normalState },
    { 'm',   &Decode::getAttr,   normalState },
    { 'r',  &Decode::setMargins,  normalState },
    { 's',   &Decode::saveCursor,  normalState },
    { 'u',   &Decode::restoreCursor, normalState },
    { '@',   &Decode::insertStr,  normalState },

    { CHAR_CR,   &Decode::cr,   bracketState },
    { CHAR_LF,   &Decode::lf,   bracketState },
    { CHAR_FF,   &Decode::ff,   bracketState },
    { CHAR_TAB,   &Decode::tab,   bracketState },
    { CHAR_BS,    &Decode::bs,   bracketState },
    { CHAR_BELL,  &Decode::bell,   bracketState },
    { CHAR_NORMAL,  0,       normalState }
};

// state after ESC [ ?

StateOption Decode::privateState[] = {
    { '0',   &Decode::paramDigit,   privateState },
    { '1',   &Decode::paramDigit,   privateState },
    { '2',   &Decode::paramDigit,   privateState },
    { '3',   &Decode::paramDigit,   privateState },
    { '4',   &Decode::paramDigit,   privateState },
    { '5',  &Decode::paramDigit,   privateState },
    { '6',   &Decode::paramDigit,   privateState },
    { '7',   &Decode::paramDigit,   privateState },
    { '8',   &Decode::paramDigit,   privateState },
    { '9',   &Decode::paramDigit,   privateState },
    { ';',   &Decode::nextParam,   privateState },

    { 'h',  &Decode::setMode,    normalState  },
    { 'l',  &Decode::resetMode,   normalState  },
    { 's',  &Decode::saveMode,    normalState  },
    { 'r',  &Decode::restoreMode,   normalState  },

    { CHAR_NORMAL,  0,        normalState  }
};

Decode::Decode(Buffer * buffer, QTextCodec * codec)
{
    m_pBuffer = buffer;

    currentState = /*Decode::*/normalState;

    m_defAttr = SETCOLOR(/*0x4b*/NO_COLOR) | SETATTR(NO_ATTR);

    m_curAttr = m_defAttr;

    m_pBuffer->setCurAttr(m_curAttr);

    bCurMode[MODE_MouseX11] = bSaveMode[MODE_MouseX11] = false;

    m_decoder = codec;
    m_state = new QTextCodec::ConverterState;
}

Decode::~Decode()
{
    //delete m_decoder;
}

// precess input string from telnet socket
//void Decode::ansiDecode( const QCString &cstr, int length )
void Decode::decode(const char *cstr, int length)
{
    inputData = cstr;
    inputLength = length;//inputData.length();

    dataIndex = 0;
    m_bBell = false;

    int i;
    StateOption *lastState;

    m_pBuffer->startDecode();

    // here we use FSM to ANSI decoding
    // use switch case is ok too
    // but i think use function pointer array can make this clear
    // you can see the defination at the beginning
    while (dataIndex < inputLength) {
        // current state always be initialized to point to the deginning of three structures
        // ( normalState, escState, bracketState )
        i = 0;
        while (currentState[i].byte != CHAR_NORMAL
                && currentState[i].byte != inputData[dataIndex])
            i++;

        // action must be allowed to redirect state change
        // get current state with input character i ( hehe, current now become last one )
        lastState = currentState + i; // good !!

        if (lastState->action != 0)
            (this->*(lastState->action))();

        // reinit current state
        currentState = lastState->nextState;

        dataIndex++;
    }
    m_pBuffer->endDecode();
}

// fill letters into char buffer
void Decode::normalInput()
{
    if (m_state->remainingChars == 0 && inputData[dataIndex] < 0x20 && inputData[dataIndex] >= 0x00)   // not print char
        return;
    bool fixAttr = false;
    bool saveAttr = false;
    if (m_state->remainingChars != 0)
        fixAttr = true;
    QString str;
    int n = 0;
    while ((m_state->remainingChars != 0 || inputData[dataIndex + n] >= 0x20 || inputData[dataIndex + n] < 0x00)
            && (dataIndex + n) < inputLength) {
        str += m_decoder->toUnicode(inputData+dataIndex + n, 1, m_state);
        n++;
        if (m_state->remainingChars != 0 && (dataIndex + n + 1) < inputLength && inputData[dataIndex+n] == CHAR_ESC && inputData[dataIndex+n+1] == '[') {
            qDebug("Decode::normalInput: esc sequence in the middle of a char");
            saveAttr = true;
            break;
        }
    }

    //QByteArray cstr(inputData + dataIndex, n);
    //QString str = m_decoder->toUnicode(inputData+dataIndex, n, m_state);
    m_pBuffer->setBuffer(str, n);
    if (fixAttr == true) {
        qDebug("Decode::normalInput: load attr");
        m_pBuffer->restoreAttr();
    }
    if (saveAttr == true) {
        qDebug("Decode::normalInput: save attr");
        m_pBuffer->saveAttr();
    }

    n--;
    dataIndex += n;

}

// non-printing characters functions
void Decode::cr()
{
    // FIXME: dirty
    m_pBuffer->cr();
}

void Decode::lf()
{
    m_pBuffer->newLine();
}

void Decode::ff()
{
    m_pBuffer->eraseEntireScreen();

    m_pBuffer->moveCursor(0, 0);
}

void Decode::tab()
{
    m_pBuffer->tab();
}

void Decode::bs()
{
    m_pBuffer->moveCursorOffset(- 1, 0);
}

void Decode::bell()
{
    m_bBell = true;
}

void Decode::setMargins()
{
    m_pBuffer->setMargins(param[0], param[1]);
}

void Decode::nextLine()
{
    cr();
    lf();
}

// parameters functions
void Decode::clearParam()
{
    nParam = 0;
    memset(param, 0, sizeof(param));
    bParam = false;
}

// for performance, this grabs all digits
void Decode::paramDigit()
{
    bParam = true;

    // make stream into number
    // ( e.g. this input character is '1' and this param is 4
    // after the following sentence this param is changed to 41
    param[nParam] = param[nParam] * 10 + inputData[dataIndex] - '0';
}

void Decode::nextParam()
{
    nParam++;
}

void Decode::saveCursor()
{
    m_pBuffer->saveCursor();
}

void Decode::restoreCursor()
{
    m_pBuffer->restoreCursor();
}

void Decode::cursorLeft()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->moveCursorOffset(-n, 0);
}

void Decode::cursorRight()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->moveCursorOffset(n, 0);
}

void Decode::cursorUp()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->moveCursorOffset(0, -n);
}

void Decode::cursorDown()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->moveCursorOffset(0, n);
}

void Decode::cursorPosition()
{
    int x = param[1];
    int y = param[0];

    if (x == 0) x = 1;
    if (y == 0) y = 1;

    m_pBuffer->moveCursor(x - 1, y - 1);
}

// erase functions
void Decode::eraseStr()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->eraseStr(n);
}

// insert functions
void Decode::insertStr()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->insertStr(n);
}
// delete functions
void Decode::deleteStr()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->deleteStr(n);
}


void Decode::eraseLine()
{
    switch (param[0]) {
    case 0:
        m_pBuffer->eraseToEndLine();
        break;
    case 1:
        m_pBuffer->eraseToBeginLine();
        break;
    case 2:
        m_pBuffer->eraseEntireLine();
        break;
    default:
        break;
    }
}

void Decode::insertLine()
{

    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->insertLine(n);
}

void Decode::deleteLine()
{
    int n = param[0];

    if (n < 1)
        n = 1;

    m_pBuffer->deleteLine(n);
}

void Decode::eraseScreen()
{
    switch (param[0]) {
    case 0:
        m_pBuffer->eraseToEndScreen();
        break;
    case 1:
        m_pBuffer->eraseToBeginScreen();
        break;
    case 2:
        m_pBuffer->eraseEntireScreen();
        break;
    case 3:
        break;
    }
}


void Decode::getAttr()
{
    // get all attributes of character

    if (!nParam && param[0] == 0) {
        m_curAttr = m_defAttr ;
        m_pBuffer->setCurAttr(m_curAttr);
        return;
    }

    char cp = GETCOLOR(m_curAttr);
    char ea = GETATTR(m_curAttr);
    for (int n = 0; n <= nParam; n++) {
        if (param[n] / 10 == 4) { // background color
            cp = cp & ~BGMASK;
            cp += SETBG(param[n] % 10);
        } else if (param[n] / 10 == 3) { // front color
            cp = cp & ~FGMASK;
            cp += SETFG(param[n] % 10);
        } else {
            switch (param[n]) {
            case 0: // attr off
                cp = GETCOLOR(m_defAttr);  //NO_COLOR;
                ea = GETATTR(m_defAttr);  //NO_ATTR;
                break;
            case 1: // bold
                ea = SETBOLD(ea);
                break;
            case 2: // dim
                ea = SETDIM(ea);
                break;
            case 4: // underline
                ea = SETUNDERLINE(ea);
                break;
            case 5: // blink
                ea = SETBLINK(ea);
                break;
            case 7: // reverse
                ea = SETREVERSE(ea);
                break;
            case 8: // invisible
                ea = SETINVISIBLE(ea);
                break;
            default:
                break;
            }
        }
    }

    m_curAttr = SETCOLOR(cp) | SETATTR(ea);

    m_pBuffer->setCurAttr(m_curAttr);
}

void Decode::setMode()
{
    for (int i = 0; i <= nParam; i++) {
        int n = param[i];
        switch (n) {
        case 4:
            m_pBuffer->setMode(INSERT_MODE);
            break;
        case 20:
            m_pBuffer->setMode(INSERT_MODE);
            break;
        case 1000:
        case 1001:
            emit mouseMode(true);
            bCurMode[MODE_MouseX11] = true;
            break;
        default:
            break;
        }
    }
}

void Decode::resetMode()
{
    for (int i = 0; i <= nParam; i++) {
        int n = param[i];
        switch (n) {
        case 4:
            m_pBuffer->resetMode(INSERT_MODE);
            break;
        case 20:
            m_pBuffer->resetMode(NEWLINE_MODE);
            break;
        case 1000:
        case 1001:
            bCurMode[MODE_MouseX11] = false;
            emit mouseMode(false);
            break;
        default:
            break;
        }
    }
}

void Decode::saveMode()
{
    for (int i = 0; i <= nParam; i++) {
        int n = param[i];
        switch (n) {
        case 1000:
        case 1001:
            bSaveMode[MODE_MouseX11] = bCurMode[MODE_MouseX11];
            break;
        default:
            break;
        }
    }
}

void Decode::restoreMode()
{
    for (int i = 0; i <= nParam; i++) {
        int n = param[i];
        switch (n) {
        case 1000:
        case 1001:
            bCurMode[MODE_MouseX11] = bSaveMode[MODE_MouseX11];
            emit mouseMode(bCurMode[MODE_MouseX11]);
            break;
        default:
            break;
        }
    }
}


void Decode::test()
{
}

} // namespace QTerm

#include <qtermdecode.moc>
