#ifndef POPWIDGET_H
#define POPWIDGET_H

#include <qwidget.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QLabel>

class QTimer;
class QLabel;

namespace QTerm
{
class Window;

class popWidget : public QWidget
{
	Q_OBJECT

public:
	popWidget(Window * win, QWidget *parent = 0, const char *name=0, 
					Qt::WFlags f= Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Tool);
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
	Window *window;
	
	void mousePressEvent( QMouseEvent * );
};

} // namespace QTerm

#endif // POPWIDGET_H
