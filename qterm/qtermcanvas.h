#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

#include <qscrollview.h>
#include <qpixmap.h>

class QLabel;

class QTermCanvas : public QScrollView
{
	Q_OBJECT
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL, 
					WFlags f=WType_Dialog | WStyle_NoBorder);
	~QTermCanvas();

	void loadImage(const QString&);
	
protected:
	void moveImage(float,float);
	void resizeImage(float);
	void rotateImage(float);
	void saveImage();
	void copyImage();
	
	void contentsMousePressEvent(QMouseEvent*);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent *ke);
	void viewportResizeEvent(QResizeEvent *re);
	void adjustSize(const QSize&);
	QPixmap scaleImage(const QSize&);
	void hideWindow();
protected:
	QLabel *label;
	bool bFitWin;
	QSize szImage;
	QString strFileName;
	QPixmap pxm;
};

#endif
