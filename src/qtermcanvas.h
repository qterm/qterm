#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H

//Added by qt3to4:
#include <QScrollArea>
#include <QImage>
// #include <QResizeEvent>
// #include <QCloseEvent>

class QLabel;
class QMenu;
class QCloseEvent;
class QMouseEvent;
class QKeyEvent;
class QResizeEvent;
class QImage;

namespace QTerm
{
class Canvas : public QScrollArea
{
	Q_OBJECT
public: 
	Canvas(QWidget *parent=NULL, Qt::WFlags f=Qt::Window);
	~Canvas();

	void loadImage(QString);

protected slots:
	void oriSize();
	void zoomIn();
	void zoomOut();
	void fitWin();
	void fullScreen();
	void saveImage();
	void copyImage();
	void silentCopy();
	void cwRotate();
	void ccwRotate();
	void deleteImage();
protected:
	void moveImage(float,float);
	void resizeImage(float);
	void rotateImage(float);
	
	void closeEvent(QCloseEvent *);
	void mousePressEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent *ke);
	void viewportResizeEvent(QResizeEvent *re);
	void adjustSize(const QSize&);
	QPixmap scaleImage(const QSize&);
protected:
	QLabel *label;
	bool bFitWin;
	QSize szImage;
	QString strFileName;
	QImage img;
	QMenu *m_pMenu;
	
//Very dirty trick, I hate it
	bool bEmbed;
};

} // namespace QTerm

#endif
