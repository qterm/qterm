#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

#include <qscrollview.h>
#include <qpixmap.h>

class QLabel;
class QPopupMenu;

class QTermCanvas : public QScrollView
{
	Q_OBJECT
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL, 
					WFlags f=WType_Dialog);
	~QTermCanvas();

	void loadImage(const QString&);
	void hideWindow();

protected slots:
	void oriSize();
	void zoomIn();
	void zoomOut();
	void fitWin();
	void fullScreen();
	void saveImage();
	void copyImage();
	void cwRotate();
	void ccwRotate();

protected:
	void moveImage(float,float);
	void resizeImage(float);
	void rotateImage(float);
	
	void contentsMousePressEvent(QMouseEvent*);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent *ke);
	void viewportResizeEvent(QResizeEvent *re);
	void adjustSize(const QSize&);
	QPixmap scaleImage(const QSize&);
protected:
	QLabel *label;
	bool bFitWin;
	QSize szImage;
	QString strFileName;
	QPixmap pxm;
	QPopupMenu *m_pMenu;
};

#endif
