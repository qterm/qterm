#include "qtermcanvas.h"

#include <qimage.h>

QTermCanvas::QTermCanvas(QWidget *parent, const char *name)
  :QGLWidget(parent, name)
{
	setWFlags(WStyle_Tool);
	ptTL=QPoint(0,0);
	zoom =1;
	bFitWin = false;
}
QTermCanvas::~QTermCanvas()
{
}

void QTermCanvas::initializeGL()
{
	glClearColor(0.0,0.0,0.0,1);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);

	// set texturing parameters
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures( 1, &texture[0]);
	glBindTexture( GL_TEXTURE_2D, texture[0]);

	glTexImage2D(GL_TEXTURE_2D,0,3,image.width(), image.height(),
                0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
	
}

void QTermCanvas::resizeGL(int width, int height)
{
	QSize szImg(img.size());
	
	if(bFitWin)
		szImg.scale(width, height, QSize::ScaleMin);
	else
		szImg *= zoom;

	glViewport(0,0, (GLsizei)szImg.width(), (GLsizei)szImg.height());
	glMatrixMode(GL_MODELVIEW);
}

void QTermCanvas::paintGL()
{
	render();
}

void QTermCanvas::render()
{
	glEnable(GL_TEXTURE_2D);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glTexImage2D(GL_TEXTURE_2D,0,3,image.width(), image.height(),
                0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	glBindTexture( GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-1,-1,0);
        glTexCoord2f(0,1); glVertex3f(-1,1,0);
        glTexCoord2f(1,1); glVertex3f(1,1,0);
        glTexCoord2f(1,0); glVertex3f(1,-1,0);
    glEnd();
}

void QTermCanvas::loadImage(const QString& name)
{
//	QImage img;
	img.load(name);
	if(!img.isNull())
	{
		image = this->convertToGLFormat(img);
		szDisp = img.size();
	


	}
	else
		qWarning("cant load image "+name);

	updateGL();
}

void QTermCanvas::resizeImage(float ratio)
{
	if(ratio<-1)
		return;
	// original size
	if(ratio==0)
		zoom = 1;
 
	zoom *= (1+ratio);
	
	bFitWin = false;

	updateGL();
}

void QTermCanvas::moveImage(float)
{
/*
	ptTL.rx() += x;
	ptTL.ry() += y;
*/
	updateGL();
}

void QTermCanvas::fitWindow(bool fit)
{
	bFitWin = fit;
	updateGL();
}

void QTermCanvas::rotateImage(float)
{
	updateGL();
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
		case Qt::Key_Left:
			moveImage(-0.05);
			break;
		case Qt::Key_Right:
			moveImage(0.05);
			break;
		case Qt::Key_Up:
			moveImage(0.05);
			break;
		case Qt::Key_Down:
			moveImage(-0.05);
			break;
	}
}
