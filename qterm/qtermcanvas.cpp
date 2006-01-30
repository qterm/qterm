#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"

#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qstring.h>

extern QString fileCfg;
extern QString getSaveFileName(const QString&, QWidget*);

QTermCanvas::QTermCanvas(QWidget *parent, const char *name, WFlags f)
  :QScrollView(parent, name, f)
{
	bFitWin=true;
	
	label = new QLabel(viewport());
	label->setScaledContents(false);
	label->setAlignment(AlignCenter);
	label->setText("No Preview Available");
	addChild(label);

	setHScrollBarMode(AlwaysOff);
	setVScrollBarMode(AlwaysOff);

}
QTermCanvas::~QTermCanvas()
{
	delete label;
}

void QTermCanvas::setFixed(bool fixed)
{
	bFixed = fixed;
}

void QTermCanvas::oriSize()
{
	bFitWin=false;
	szImage = img.size();
	adjustSize(QSize(visibleWidth(),visibleHeight()));
}

void QTermCanvas::zoomIn()
{
	bFitWin=false;
	resizeImage(0.05);
}

void QTermCanvas::zoomOut()
{
	bFitWin=false;
	resizeImage(-0.05);
}

void QTermCanvas::fitWin()
{
	bFitWin=true;
	adjustSize(QSize(visibleWidth(),visibleHeight()));
}

void QTermCanvas::cwRotate()
{
	rotateImage(90);
}

void QTermCanvas::ccwRotate()
{
	rotateImage(-90);
}


void QTermCanvas::loadImage(const QString& name)
{
	label->setText("Loading...");
	img.load(name);
	if(!img.isNull())
	{

		strFileName = name;
       		
		parentWidget()->setCaption(QFileInfo(name).fileName());

		
		if( bFixed || parentWidget()->isFullScreen() )
		{
			label->setPixmap(QPixmap(img));
			szImage = img.size();
			fitWin();
		}
		else
		{

			QSize szView(img.size());
			szView.scale(640,480,QSize::ScaleMin);
			if(szView.width()<img.width()) // increase window size
			{
				szImage = szView;
				label->setPixmap(scaleImage(szImage));
				emit resizeWindow(szView);
			}
			else	// decrease window size
			{
				szImage = img.size();
				label->setPixmap(QPixmap(img));
				emit resizeWindow(szImage);
			}

		}

	}
	else
	{
		label->setText("Failed to display");
		qWarning("cant load image "+name);
	}
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

	if(!parentWidget()->isFullScreen() && !bFixed)		
		emit resizeWindow(szImage);
	else
		adjustSize(QSize(visibleWidth(), visibleHeight()));
}

void QTermCanvas::rotateImage(float ang)
{

	if(label->pixmap()==NULL)
		return;

	QWMatrix wm;
	
	wm.rotate(ang);

	img = img.xForm(wm);
	
	szImage = img.size();

	adjustSize(QSize(visibleWidth(), visibleHeight()));
}

void QTermCanvas::copyImage()
{
	if(label->pixmap()==NULL)
		return;

	QFileInfo fi(strFileName);
	QString strSave = getSaveFileName(fi.fileName(), this);

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

void QTermCanvas::silentCopy()
{
	if(label->pixmap()==NULL)
		return;

	// save it to $savefiledialog
	QTermConfig conf(fileCfg);
	QString strPath = QString::fromLocal8Bit(
					conf.getItemValue("global","savefiledialog"));
	
	QFileInfo fi(strFileName);
	QString strSave = strPath+"/"+fi.fileName();

	fi.setFile(strSave);

	// add (%d) if exist
	int i=1;
	while(fi.exists())
	{
		strSave = QString("%1/%2(%3).%4")
                            .arg(fi.dirPath())
                            .arg(fi.baseName(true))
                            .arg(i)
                            .arg(fi.extension(false));
		fi.setFile(strSave);
	}

	// copy it
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
	return QPixmap(img.scale(sz));
}

void QTermCanvas::moveImage(float dx, float dy)
{
	scrollBy(contentsWidth()*dx, contentsHeight()*dy);
}

void QTermCanvas::saveImage()
{
	if(label->pixmap()==NULL)
		return;

	QFileInfo fi(strFileName);
	QString strSave = getSaveFileName(fi.fileName(),this);

	if(strSave.isEmpty())
		return;
	QString fmt = fi.extension(false).upper();
	if(!img.save(strSave, fmt=="JPG"?QString("JPEG"):fmt))
		QMessageBox::warning(this, "Failed to save file", "Cant save file, maybe format not supported");
}

void QTermCanvas::deleteImage()
{
	if(label->pixmap()==NULL)
		return;

	QFile::remove(strFileName);
	emit filesChanged();
}

void QTermCanvas::viewportResizeEvent(QResizeEvent *re)
{
	adjustSize(re->size());
}


void QTermCanvas::adjustSize(const QSize& szView)
{
	// occupy whole visible area if nothing
	if(label->pixmap()==NULL)
	{
		label->resize(visibleWidth(), visibleHeight());
		return;
	}

	QSize szImg=szImage;


	if(bFitWin)
	{
		if(szImg.width()>szView.width() || 
			szImg.height()>szView.height() ||
			szImg.width()<img.width())
		{	
			szImg.scale(szView, QSize::ScaleMin);
		}
		szImg = szImg.boundedTo(img.size());
	}

	int x=szView.width()-szImg.width();
	int y=szView.height()-szImg.height();	

	szImage = szImg;


	// I decide to set on/off scrollbar manually because:
	// when ScrollBarMode=Auto, I will receive one more
	// viewportResizeEvent, since it tries to compute
	if(szView.width()<szImg.width())
		setHScrollBarMode(AlwaysOn);
	else
		setHScrollBarMode(AlwaysOff);
	
	if(szView.height()<szImg.height())
		setVScrollBarMode(AlwaysOn);
	else
		setVScrollBarMode(AlwaysOff);

	
	if(bFitWin)
		resizeContents(szView.width(), szView.height());
	else
		resizeContents(szImage.width(),szImage.height());


	moveChild(label, x>0?x/2:0, y>0?y/2:0); 

	label->setPixmap(scaleImage(szImage));

}

