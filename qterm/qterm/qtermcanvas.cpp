#include "qtermcanvas.h"

#include <qpixmap.h>
#include <qlabel.h>

QTermCanvas::QTermCanvas(QWidget *parent, const char *name, WFlags f)
  :QWidget(parent, name, f)
{
	label = new QLabel(this);
	label->setScaledContents(true);
}
QTermCanvas::~QTermCanvas()
{
	delete label;
}

void QTermCanvas::loadImage(const QString& name)
{
	QSize sz;
	QPixmap img;
	img.load(name);
	if(!img.isNull())
	{
		sz=img.size();
		if(img.width()>320 || img.height()>240)
			sz.scale(320,240, QSize::ScaleMin);

		resize(sz);
		label->setPixmap(img);
	}
	else
		qWarning("cant load image "+name);
}

void QTermCanvas::resizeImage(float ratio)
{
	if(ratio>0)
		resize(size()*(1+ratio));
}

void QTermCanvas::moveImage(float)
{
}


void QTermCanvas::rotateImage(float)
{
}


void QTermCanvas::resizeEvent(QResizeEvent *re)
{
	label->resize(re->size());
}

void QTermCanvas::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key())
	{
		case Qt::Key_Escape:
			if(isFullScreen())
				showNormal();
			else
				hide();
			break;
		case Qt::Key_F:
			showFullScreen();
			break;
		case Qt::Key_Plus:
			resizeImage(0.05);
			break;
		case Qt::Key_Minus:
			resizeImage(-0.05);
			break;
	}
}
