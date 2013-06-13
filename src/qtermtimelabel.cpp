/*******************************************************************************
FILENAME:      qtermframe.cpp
REVISION:      2001.10.5 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermtimelabel.h"

// #include <qdatetime.h>
// #include <qmessagebox.h>
//Added by qt3to4:
#include <QLabel>
#include <QTimerEvent>
// #include <QMessageBox>
#include <QDateTime>

//constructor
QTermTimeLabel::QTermTimeLabel(QWidget * parent, const char * name, Qt::WindowFlags f)
	:QLabel(parent,f)
{
	setAlignment(Qt::AlignHCenter);
	currentTime=new QTime();	
	currentTime->start();
	startTimer(1000);
}
//destructor
QTermTimeLabel::~QTermTimeLabel()
{
	delete currentTime;	
}
//timer to display the current time
void QTermTimeLabel::timerEvent(QTimerEvent*)
{
	setText(currentTime->currentTime().toString());	
}
#include <moc_qtermtimelabel.cpp>
