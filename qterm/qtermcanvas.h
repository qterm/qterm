#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

#include <qscrollview.h>

class QLabel;

class QTermCanvas : public QScrollView
{
	Q_OBJECT
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL, 
					WFlags f=WType_Dialog /*WStyle_NoBorder|WX11BypassWM|WType_Popup*/);
	~QTermCanvas();

	void loadImage(const QString&);
	
protected:
	void resizeImage(float);
	void rotateImage(float);
	void saveImage();
	
	void showEvent(QShowEvent *);
	void keyPressEvent(QKeyEvent *ke);
	void viewportResizeEvent(QResizeEvent *re);
	void adjustSize(const QSize&);
	void centerImage(const QSize&);
protected:
	QLabel *label;
	bool bFitWin;
	QSize szImage;
	QString strFileName;
};

#endif
