#include "qtermcanvas.h"
#include "qterm.h"

#include <qpixmap.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <qwmatrix.h>

QTermCanvas::QTermCanvas(QWidget *parent, const char *name, WFlags f)
  :QScrollView(parent, name, f)
{
	bFitWin=true;
	label = new QLabel(viewport());
	label->setScaledContents(true);
	label->setAlignment(AlignCenter);
	label->setText("Loading...");
	addChild(label);
}
QTermCanvas::~QTermCanvas()
{
	delete label;
}

void QTermCanvas::loadImage(const QString& name)
{
	pxm.load(name);
	if(!pxm.isNull())
	{

		strFileName = name;
		QFileInfo fi(name);
		setCaption(fi.fileName());

		bFitWin=true;

		QSize szView(pxm.size());
		szView.scale(400,300, 
			QSize::ScaleMin);

		if(szView.width()<pxm.width())
		{
			szImage = szView;
			label->setPixmap(scaleImage(szImage));
			resize(szView*1.2);
		}
		else
		{
			szImage = pxm.size();
			label->setPixmap(pxm);
			resize(szImage*1.2);
		}
		
	}
	else
		qWarning("cant load image "+name);
}

void QTermCanvas::resizeImage(float ratio)
{
	if(label->pixmap()==NULL)
		return;

	QSize szImg=szImage;
	szImg *= (1+ratio);
	//we dont need so big 
	if(szImg.width()>10000 || szImg.height()>10000)
		return;
	szImage = szImg;
	
	adjustSize(QSize(visibleWidth(), visibleHeight()));
}

void QTermCanvas::rotateImage(float ang)
{
	QWMatrix wm;
	
	wm.rotate(ang);

	pxm = pxm.xForm(wm);
	
	szImage = pxm.size();

	adjustSize(QSize(visibleWidth(), visibleHeight()));
}

void QTermCanvas::saveImage()
{
	QFileInfo fi(strFileName);
	QString strSave = 
			QFileDialog::getSaveFileName(QDir::homeDirPath()+"/"+fi.fileName(),
							"*", this);
	if(strSave.isEmpty())
		return;
	QFile file(strFileName);
	if(file.open(IO_ReadOnly))
	{
		QFile save(strSave);
		if(save.open(IO_WriteOnly))
		{
			QByteArray ba = file.readAll();
			QDataStream ds(&save);
			ds.writeRawBytes(ba,ba.size());
			save.close();
		}
		file.close();
	}
}

QPixmap QTermCanvas::scaleImage(const QSize& sz)
{
	QWMatrix wm;
	wm.scale((double)sz.width()/pxm.width(),
			(double)sz.height()/pxm.height());
	return pxm.xForm(wm);
}

void QTermCanvas::moveImage(float dx, float dy)
{
	scrollBy(contentsWidth()*dx, contentsHeight()*dy);
}

void QTermCanvas::copyImage()
{
	QFileInfo fi(strFileName);
	QString strSave = 
			QFileDialog::getSaveFileName(QDir::homeDirPath()+"/"+fi.fileName(),
							"*", this);
	if(strSave.isEmpty())
		return;
	pxm.save(strSave, fi.extension(false));
}

void QTermCanvas::closeEvent(QCloseEvent *ce)
{
	hideWindow();
}

void QTermCanvas::viewportResizeEvent(QResizeEvent *re)
{
	adjustSize(re->size());
}

void QTermCanvas::contentsMousePressEvent(QMouseEvent *)
{
	hideWindow();
}

void QTermCanvas::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key())
	{
		case Qt::Key_Escape:
			if(isFullScreen())
				showNormal();
			else
				hideWindow();
			break;
		case Qt::Key_F:
			if(!isFullScreen())
				showFullScreen();
			else
				showNormal();
			break;
		case Qt::Key_Minus:
			bFitWin=false;
			resizeImage(-0.05);
			break;
		case Qt::Key_Plus:
		case Qt::Key_Equal:
			bFitWin=false;
			resizeImage(0.05);
			break;
		case Qt::Key_Z: // original size
			bFitWin=false;
			szImage = pxm.size();
			adjustSize(QSize(visibleWidth(),visibleHeight()));
			break;
		case Qt::Key_X: // fit window
			bFitWin=true;
			adjustSize(QSize(visibleWidth(),visibleHeight()));
			break;
		case Qt::Key_S:
			saveImage();
			break;
		case Qt::Key_C:
			copyImage();
			break;
		case Qt::Key_BracketLeft: 
			rotateImage(-90);
			break;
		case Qt::Key_BracketRight:
			rotateImage(90);
			break;
		case Qt::Key_Left:
			moveImage(-0.05,0);
			break;
		case Qt::Key_Right:
			moveImage(0.05,0);
			break;
		case Qt::Key_Up:
			moveImage(0,-0.05);
			break;
		case Qt::Key_Down:
			moveImage(0,0.05);
			break;

	}
}

void QTermCanvas::adjustSize(const QSize& szView)
{
	if(label->pixmap()==NULL)
		return;

	QSize szImg=szImage;

	if(bFitWin)
	{
		if(szImg.width()>szView.width() || 
			szImg.height()>szView.height() ||
			szImg.width()<pxm.width())
		{	
			szImg.scale(szView, QSize::ScaleMin);
		}
		szImg = szImg.boundedTo(pxm.size());
	}

	int x=szView.width()-szImg.width();
	int y=szView.height()-szImg.height();	

	szImage = szImg;

	if(bFitWin)
		resizeContents(szView.width(), szView.height());
	else
		resizeContents(szImage.width()*1.2,szImage.height()*1.1);

	moveChild(label, x>0?x/2:0, y>0?y/2:0); 
	label->resize(szImage);

	label->setPixmap(scaleImage(szImage));

}

void QTermCanvas::hideWindow()
{
	label->setAlignment(AlignCenter);
	label->setText("Loading...");
	moveChild(label,0,0);
	label->resize(visibleWidth(), visibleHeight());
	hide();
}
