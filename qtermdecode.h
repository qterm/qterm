#ifndef QTERMDECODE_H
#define QTERMDECODE_H


#include <QObject>

class QTermDecode;
class QTermBuffer;

typedef void ( QTermDecode::*StateFunc )();

struct StateOption
{
	int byte;		// char value to look for; -1==end/default
	StateFunc action;
	StateOption *nextState;
};



class QTermDecode : public QObject
{
	Q_OBJECT
	
public:
	QTermDecode( QTermBuffer * );
	~QTermDecode();

	void decode( const char *cstr, int length );
	
	bool bellReceive()	{ return m_bBell; }
	
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
	
	void saveMode();
	void restoreMode();

	void test();

signals:
	void mouseMode(bool);

private:
	
	bool m_bBell;
	short m_curAttr, m_defAttr;

	StateOption *currentState;	// FSM related
	static StateOption normalState[], escState[], 
		bracketState[], privateState[];
	
	const char *m_pData;
	int m_nIndex, m_nLength;

	int nParam, param[30];	// to store digital params like *[m,nPn;
	
	QTermBuffer * m_pBuffer; // the buffer operated on


	bool bSaveMode[30];
	bool bCurMode[30];

	bool m_test;
};
	
#endif
