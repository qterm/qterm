#ifndef QTERMTIMELABEL_H
#define QTERMTIMELABEL_H
#include <qlabel.h>

class QTime;

class QTermTimeLabel: public QLabel
{
Q_OBJECT
public:
	QTermTimeLabel(QWidget * parent, const char * name=0, WFlags f=0);
	~QTermTimeLabel();
protected:
	QTime * currentTime;
	void timerEvent ( QTimerEvent * );
};

#endif //QTERMTIMELABEL

