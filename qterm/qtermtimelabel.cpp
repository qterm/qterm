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

#include <qdatetime.h>


#include <qmessagebox.h>
//constructor
QTermTimeLabel::QTermTimeLabel(QWidget * parent, const char * name, WFlags f)
	:QLabel(parent,name,f)
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
#ifdef HAVE_CONFIG_H
#include "qtermtimelabel.moc"
#endif
