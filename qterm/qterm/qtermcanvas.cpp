#include "qtermcanvas.h"
#include "qterm.h"

#include <qpixmap.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>

QTermCanvas::QTermCanvas(QWidget *parent, const char *name, WFlags f)
  :QScrollView(parent, name, f)
{
	bFitWin=true;
	label = new QLabel(viewport());
	label->setScaledContents(true);
	label->setAlignment(AlignCenter);
	label->setText("Loading...");
	addChild(label);

	m_pMenu = new QPopupMenu(this);


	m_pMenu->insertItem( tr("zoom 1:1"), this, SLOT(oriSize()), Key_Z );
	m_pMenu->insertItem( tr("zoom in"), this, SLOT(zoomIn()), Key_Equal );
	m_pMenu->insertItem( tr("zoom out"), this, SLOT(zoomOut()), Key_Minus );
	m_pMenu->insertItem( tr("fit window"), this, SLOT(fitWin()), Key_X );
	
	m_pMenu->insertItem( tr("rotate CW 90"), this, SLOT(cwRotate()), Key_BracketRight );
	m_pMenu->insertItem( tr("rotate CCW 90"), this, SLOT(ccwRotate()), Key_BracketLeft );

	m_pMenu->insertItem( tr("fullscreen"), this, SLOT(fullScreen()), Key_F );
	m_pMenu->insertItem( tr("save as"), this, SLOT(saveImage()), Key_S );
	m_pMenu->insertItem( tr("copy to"), this, SLOT(copyImage()), Key_C );


}
QTermCanvas::~QTermCanvas()
{
	delete label;
	delete m_pMenu;
}

void QTermCanvas::oriSize()
{
	bFitWin=false;
	szImage = pxm.size();
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

void QTermCanvas::fullScreen()
{
	if(!isFullScreen())
		showFullScreen();
	else
		showNormal();
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
		szView.scale(640,480,QSize::ScaleMin);
		
		if(szView.width()<pxm.width())
		{
			szImage = szView;
			label->setPixmap(scaleImage(szImage));
			resize(szView*1.1);
		}
		else
		{
			szImage = pxm.size();
			label->setPixmap(pxm);
			resize(szImage+QSize(5,5));
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

void QTermCanvas::contentsMousePressEvent(QMouseEvent *me)
{
	if(me->button()&LeftButton)
	{
		hideWindow();
		return;
	}
	if(me->button()&RightButton)
	{
		m_pMenu->popup(me->globalPos());
	}
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
/*		case Qt::Key_F:
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
*/		case Qt::Key_Left:
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
		return label->resize(visibleWidth(), visibleHeight());

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
		resizeContents(szImage.width()*1.1,szImage.height()*1.1);

	moveChild(label, x>0?x/2:0, y>0?y/2:0); 
	label->resize(szImage);

	label->setPixmap(scaleImage(szImage));

}

void QTermCanvas::hideWindow()
{
	if(isFullScreen())
		showNormal();
	resize(200,100);
	label->setAlignment(AlignCenter);
	label->setText("Loading...");
	moveChild(label,0,0);
//	label->resize(visibleWidth(), visibleHeight());
	hide();
}
