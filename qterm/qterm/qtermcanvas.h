#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

#include <qscrollview.h>
#include <qimage.h>

class QLabel;

class QTermCanvas : public QScrollView
{
	Q_OBJECT
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL, 
					WFlags f=0);
	~QTermCanvas();

	QLabel* picLabel(){return label;}
	bool isScrollbarVisible(){return horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible();}
	
public slots:
	void loadImage(const QString&);
	void oriSize();
	void zoomIn();
	void zoomOut();
	void fitWin();
	void saveImage();
	void copyImage();
	void silentCopy();
	void cwRotate();
	void ccwRotate();
	void deleteImage();
	void moveImage(float,float);
	void resizeImage(float);
	void rotateImage(float);
	void setFixed(bool);

signals:
	void resizeWindow(const QSize&);
	void filesChanged();
	
protected:
	void viewportResizeEvent(QResizeEvent *re);
	void adjustSize(const QSize&);
	QPixmap scaleImage(const QSize&);
protected:
	QLabel *label;
	bool bFitWin, bFixed;
	QSize szImage;
	QString strFileName;
	QImage img;
};

#endif
