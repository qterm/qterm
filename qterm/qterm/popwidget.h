#ifndef POPWIDGET_H
#define POPWIDGET_H

#include <qwidget.h>

class QTimer;
class QLabel;
class QTermWindow;

class popWidget : public QWidget
{
	Q_OBJECT

public:
//	#if (QT_VERSION>=310)
//	popWidget(QTermWindow * win, QWidget *parent = 0, const char *name=0, WFlags f=WStyle_Splash);
//	#else
	popWidget(QTermWindow * win, QWidget *parent = 0, const char *name=0, 
					WFlags f= WStyle_StaysOnTop|WX11BypassWM|WStyle_Tool);
//	#endif
	~popWidget();
	
	void popup();
	void setText(const QString&);

public slots:
	void showTimer();

protected:
	QTimer *pTimer;
	int nState; // -1 hide, 0 popup, 1 wait, 2 hiding
	QPoint ptPos;
	QRect rcDesktop;
	int nStep;
	int nInterval;
	QLabel *label;
	QTermWindow *window;
	
	void mousePressEvent( QMouseEvent * );
};


#endif // POPWIDGET_H
