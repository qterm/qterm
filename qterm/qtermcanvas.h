#ifndef QTERMCANVAS_H
#define QTERMCANVAS_H
#include <GL/glu.h>
#include <qgl.h>
#include <qimage.h>

/**
  *@author 
  */

class QTermCanvas : public QGLWidget  
{
public: 
	QTermCanvas(QWidget *parent=NULL, const char *name=NULL);
	~QTermCanvas();

	void loadImage(const QString&);
	void resizeImage(float);
	void rotateImage(float);
	void moveImage(float);
	void fitWindow(bool);
	
protected slots:
	void initializeGL();
	void resizeGL(int, int);
	void paintGL();
	
	void render();
protected:
	void keyPressEvent(QKeyEvent *ke);
	GLuint texture[1];
	QImage image, img;
	QPoint ptTL;
	QSize szDisp;
	float zoom;
	bool bFitWin;
};

#endif
