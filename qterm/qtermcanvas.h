#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

#include <qwidget.h>

class QLabel;

class QTermCanvas : public QWidget
{
	Q_OBJECT
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL, 
					WFlags f=WStyle_NoBorder|WX11BypassWM|WType_Popup);
	~QTermCanvas();

	void loadImage(const QString&);
	
protected:
	void resizeImage(float);
	void rotateImage(float);
	void moveImage(float);

	void keyPressEvent(QKeyEvent *ke);
	void resizeEvent(QResizeEvent *re);

protected:
	QLabel *label;
};

#endif
