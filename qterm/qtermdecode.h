#ifndef QTERMDECODE_H
#define QTERMDECODE_H

class QTermDecode;

class QTermBuffer;

// this for FSM
typedef void ( QTermDecode::*StateFunc )();

struct StateOption
{
	int byte;		// char value to look for; -1==end/default
	StateFunc action;
	StateOption *nextState;
};

// i think not use signal and slot in this class
class QTermDecode	// : QObject
{
//	Q_OBJECT
	
public:
	QTermDecode( QTermBuffer * );
	~QTermDecode();

	// translate data from telnet socket to our own buffer
	void decode( const char *cstr, int length );
	
	bool bellReceive()	{ return m_bBell; }
	
//signals:
//	void decodeFinished();
		
private:		
// escape sequence actions
// you'd better see FSM structure array in QTermDecode.cpp

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

	void test();

private:
	
	bool m_bBell;
	short m_curAttr, m_defAttr;

	// ********** ansi decoder states ****************	
	StateOption *currentState;
	static StateOption normalState[], escState[], bracketState[];

	// ********** decoder		*****************
	const char *inputData;
	int inputLength, dataIndex;

	int nParam, param[30];
	bool bParam;

	QTermBuffer * m_pBuffer;

	bool m_test;
};
	
#endif
