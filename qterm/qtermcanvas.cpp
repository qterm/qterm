#include "qtermcanvas.h"
#include "qterm.h"

#include <qpixmap.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qdir.h>

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
	QPixmap img;
	img.load(name);
	if(!img.isNull())
	{
		szImage=img.size();
		label->setPixmap(img);
		
		strFileName = name;
		QFileInfo fi(name);
		setCaption(fi.fileName());

		bFitWin=true;
		
		QSize szView(szImage);
		szView.scale(QSize(400,300), QSize::ScaleMin);
		resize(szView*1.1);
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
	
	resizeContents(szImage.width()*1.1,szImage.height()*1.1);
	
	centerImage(QSize(visibleWidth(),visibleHeight()));

	label->resize(szImage);
}

void QTermCanvas::rotateImage(float)
{
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

void QTermCanvas::showEvent(QShowEvent *se)
{
	label->resize(visibleWidth(),visibleHeight());
	label->setAlignment(AlignCenter);
	label->setText("Loading...");
}

void QTermCanvas::viewportResizeEvent(QResizeEvent *re)
{
	adjustSize(re->size());
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
			szImage = label->pixmap()->size();
			adjustSize(QSize(visibleWidth(),visibleHeight()));
			break;
		case Qt::Key_X: // fit window
			if(!bFitWin)
			{
				bFitWin=true;
				adjustSize(QSize(visibleWidth(),visibleHeight()));
			}
			break;
		case Qt::Key_S:
			saveImage();
			break;
	}
}

void QTermCanvas::adjustSize(const QSize& szView)
{
	if(label->pixmap()==NULL)
		return label->resize(szView);

	
	resizeContents(szImage.width(),szImage.height());

	centerImage(szView);

//	if(bFitWin)
		label->resize(szImage);
}

void QTermCanvas::centerImage(const QSize& szView)
{
	QSize szImg=szImage;

	if(bFitWin)
	{
		if(szImg.width()>szView.width() || szImg.height()>szView.height())
		{	
			szImg.scale(szView, QSize::ScaleMin);
		}
	}

	int x=szView.width()-szImg.width();
	int y=szView.height()-szImg.height();

	moveChild(label, x>0?x/2:0, y>0?y/2:0); 

	szImage = szImg;
}

