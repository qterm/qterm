#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"

// #include <qimage.h>
// #include <qpixmap.h>
// #include <qlabel.h>
// #include <qfileinfo.h>
// #include <q3filedialog.h>
// #include <qdir.h>
// #include <qmatrix.h>
// #include <q3popupmenu.h>
// #include <qmessagebox.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QMatrix>

namespace QTerm
{
// extern QString getSaveFileName(const QString&, QWidget*);
extern QString fileCfg;

Canvas::Canvas(QWidget *parent, Qt::WFlags f)
  :QScrollArea(parent)
{

	//dirty trick
	if (f == 0)
		bEmbed = true;
	else
		bEmbed = false;

	m_pMenu = new QMenu(this);
	m_pMenu->addAction( tr("zoom 1:1"), this, SLOT(oriSize()), Qt::Key_Z );
	m_pMenu->addAction( tr("fit window"), this, SLOT(fitWin()), Qt::Key_X );
	m_pMenu->addSeparator();	
	m_pMenu->addAction( tr("zoom in"), this, SLOT(zoomIn()), Qt::Key_Equal );
	m_pMenu->addAction( tr("zoom out"), this, SLOT(zoomOut()), Qt::Key_Minus );
	if (!bEmbed)
		m_pMenu->addAction( tr("fullscreen"), this, SLOT(fullScreen()), Qt::Key_F );
	m_pMenu->addSeparator();	
	m_pMenu->addAction( tr("rotate CW 90"), this, SLOT(cwRotate()), Qt::Key_BracketRight );
	m_pMenu->addAction( tr("rotate CCW 90"), this, SLOT(ccwRotate()), Qt::Key_BracketLeft );

	if (!bEmbed) {
		m_pMenu->addSeparator();
		m_pMenu->addAction( tr("save as"), this, SLOT(saveImage()), Qt::Key_S );
		m_pMenu->addAction( tr("copy to"), this, SLOT(copyImage()), Qt::Key_C );
		m_pMenu->addAction( tr("silent copy"), this, SLOT(silentCopy()), Qt::Key_S+Qt::SHIFT );
		m_pMenu->addAction( tr("delete"), this, SLOT(deleteImage()), Qt::Key_D );

		m_pMenu->addSeparator();
		m_pMenu->addAction( tr("exit"), this, SLOT(close()), Qt::Key_Q );
	}

	bFitWin=true;
	
	label = new QLabel(viewport());
	label->setScaledContents(true);
	label->setAlignment(Qt::AlignCenter);
	label->setText("No Preview Available");
	setWidget(label);
	resize(200,100);

}
Canvas::~Canvas()
{
	//delete label;
	//delete m_pMenu;
}

void Canvas::oriSize()
{
	bFitWin=false;
	szImage = img.size();
	adjustSize(QSize(width(),height()));
}

void Canvas::zoomIn()
{
	bFitWin=false;
	resizeImage(0.05);
}

void Canvas::zoomOut()
{
	bFitWin=false;
	resizeImage(-0.05);
}

void Canvas::fitWin()
{
	bFitWin=true;
	adjustSize(QSize(width(),height()));
}

void Canvas::cwRotate()
{
	rotateImage(90);
}

void Canvas::ccwRotate()
{
	rotateImage(-90);
}

void Canvas::fullScreen()
{
	if(!isFullScreen())
		showFullScreen();
	else
		showNormal();
}

void Canvas::loadImage(QString name)
{
	img.load(name);
	if(!img.isNull())
	{

		strFileName = name;
		setWindowTitle(QFileInfo(name).fileName());

		bFitWin=true;

		QSize szView(img.size());
		szView.scale(640,480,Qt::KeepAspectRatio);
		
		if(szView.width()<img.width())
		{
			szImage = szView;
			label->resize(szImage);
			label->setPixmap(scaleImage(szImage));
			if (!bEmbed)
				resize(szView*1.1);
		}
		else
		{
			szImage = img.size();
			label->resize(szImage);
			label->setPixmap(QPixmap::fromImage(img));
			if (!bEmbed)
				resize(szImage+QSize(5,5));
		}
		if (bEmbed)
			fitWin();
		
	}
	else
		qWarning("cant load image");
}

void Canvas::resizeImage(float ratio)
{
	if(label->pixmap()==NULL)
		return;

	QSize szImg=szImage;
	szImg *= (1+ratio);
	//we dont need so big 
	if(szImg.width()>10000 || szImg.height()>10000)
		return;
	szImage = szImg;

	if(!isFullScreen() && !bEmbed)		
		resize(szImage*1.1);
	else
		adjustSize(QSize(width(), height()));
}

void Canvas::rotateImage(float ang)
{
	QMatrix wm;
	
	wm.rotate(ang);

	img = img.transformed(wm);
	
	szImage = img.size();

	adjustSize(QSize(width(), height()));
}

void Canvas::copyImage()
{
	QFileInfo fi(strFileName);
	QString strSave = QFileDialog::getSaveFileName(this,"Choose a filename to save under", QDir::currentPath()+fi.fileName());

	if(strSave.isEmpty())
		return;
	QFile file(strFileName);
	if(file.open(QIODevice::ReadOnly))
	{
		QFile save(strSave);
		if(save.open(QIODevice::WriteOnly))
		{
			QByteArray ba = file.readAll();
			QDataStream ds(&save);
			ds.writeRawData(ba,ba.size());
			save.close();
		}
		file.close();
	}
}

void Canvas::silentCopy()
{
	// save it to $savefiledialog
	Config conf(fileCfg);
	QString strPath = conf.getItemValue("global","savefiledialog");
	
	QFileInfo fi(strFileName);
	QString strSave = strPath+"/"+fi.fileName();

	fi.setFile(strSave);

	// add (%d) if exist
	int i=1;
	while(fi.exists())
	{
		strSave = QString("%1/%2(%3).%4")
                            .arg(fi.dir().path())
                            .arg(fi.completeBaseName())
                            .arg(i)
                            .arg(fi.suffix());
		fi.setFile(strSave);
	}

	// copy it
	QFile file(strFileName);	
	if(file.open(QIODevice::ReadOnly))
	{
		QFile save(strSave);
		if(save.open(QIODevice::WriteOnly))
		{
			QByteArray ba = file.readAll();
			QDataStream ds(&save);
			ds.writeRawData(ba,ba.size());
			save.close();
		}
		file.close();
	}
}


QPixmap Canvas::scaleImage(const QSize& sz)
{
	return QPixmap::fromImage(img.scaled(sz));
}

void Canvas::moveImage(float dx, float dy)
{
	scrollContentsBy(widget()->width()*dx, widget()->height()*dy);
}

void Canvas::saveImage()
{
	QFileInfo fi(strFileName);
	QString strSave = QFileDialog::getSaveFileName(this,"Choose a filename to save under", QDir::currentPath()+fi.fileName());

	if(strSave.isEmpty())
		return;
	QString fmt = fi.suffix().toUpper();
	if(!img.save(strSave, fmt.toLatin1()))
		QMessageBox::warning(this, "Failed to save file", "Cant save file, maybe format not supported");
}

void Canvas::deleteImage()
{
	QFile::remove(strFileName);
	close();
}

void Canvas::closeEvent(QCloseEvent *ce)
{
	if (!bEmbed)
		delete this;
}

void Canvas::viewportResizeEvent(QResizeEvent *re)
{
	adjustSize(re->size());
}

void Canvas::mousePressEvent(QMouseEvent *me)
{
/* remove this to avoid click by mistake
	if(me->button()&LeftButton)
	{
		close();
		return;
	}
*/
	if(me->button()&Qt::RightButton)
	{
		m_pMenu->popup(me->globalPos());
	}
}

void Canvas::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key())
	{
		case Qt::Key_Escape:
			if(isFullScreen())
				showNormal();
			else
				close();
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

void Canvas::adjustSize(const QSize& szView)
{
	if(label->pixmap()==NULL)
	{
		label->resize(width(), height());
		return;
	}

	QSize szImg=szImage;

	if(bFitWin)
	{
		if(szImg.width()>szView.width() || 
			szImg.height()>szView.height() ||
			szImg.width()<img.width())
		{	
			szImg.scale(szView, Qt::KeepAspectRatio);
		}
		szImg = szImg.boundedTo(img.size());
	}

	int x=szView.width()-szImg.width();
	int y=szView.height()-szImg.height();	

	szImage = szImg;

// 	if(bFitWin)
// 		resizeContents(szView.width(), szView.height());
// 	else
// 		resizeContents(szImage.width()*1.1,szImage.height()*1.1);
// 
// 	moveChild(label, x>0?x/2:0, y>0?y/2:0); 
	label->resize(szImage);

	label->setPixmap(scaleImage(szImage));

}

} // namespace QTerm

#include <qtermcanvas.moc>
