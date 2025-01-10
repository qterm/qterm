#ifndef QTERMTIMELABEL_H
#define QTERMTIMELABEL_H
// #include <qlabel.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QLabel>

class QTime;

class QTermTimeLabel: public QLabel
{
Q_OBJECT
public:
	QTermTimeLabel(QWidget * parent, const char * name=0, Qt::WindowFlags f=Qt::Widget);
	~QTermTimeLabel();
protected:
	QTime * currentTime;
	void timerEvent ( QTimerEvent * );
};

#endif //QTERMTIMELABEL

