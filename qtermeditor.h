#ifndef QTERMEDITOR_H
#define QTERMEDITOR_H

#include "qtermwindowbase.h"
class QTextEdit;

class QTermEditor : public QTermWindowBase
{
	Q_OBJECT
public:	
        	
	QTermEditor(QWidget *parent=0);
	~QTermEditor();
protected:
    void closeEvent(QCloseEvent*e);
    
protected:
	QTextEdit *te;
};

#endif //QTERMEDITOR_H
