#include "imageviewer.h"

#include <qdir.h>
#include <qlayout.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qheader.h>
#include <qtooltip.h>
#include <qrect.h>
#include <qpoint.h>
#include <qfileinfo.h>

const QString ImageViewer::thumbDir = "shadow-cache/";
extern QString pathPic;
ImageViewer::ImageViewer(const QString & path, QWidget * parent)
	:QListBox(parent)
{
	d_path = path;
	sort = QDir::Time;
	lastOne = NULL;

	// add items
	buildList();
	connect(this, SIGNAL(selectionChanged(QListBoxItem *)), this, SLOT(viewImage(QListBoxItem *)));
	connect(this, SIGNAL(onItem(QListBoxItem *)), this, SLOT(hoverOnItem(QListBoxItem *)));
}

void ImageViewer::genThumb(const QString & path, const QString & filename)
{
	const QString file = QString( "shadow_cache_%1_%2.png" ).arg(filename).arg( thumbSize );
	const QString folder = path+thumbDir;
	QImage original(path+filename);
	original = original.scale(thumbSize, thumbSize, QImage::ScaleMin);
	QImage shadow;
	uint shadowSize;
	if (original.width() > original.height())
		shadowSize = static_cast<uint>(original.width()/100.0*6.0);
	else
		shadowSize = static_cast<uint>(original.height()/100.0*6.0);
	
	shadow.load(pathPic+"pic/shadow.png");
	shadow = shadow.scale( original.width() + shadowSize, original.height() + shadowSize);

	QImage target(shadow);
	bitBlt (&target, 0, 0, &original);

	target.save( folder + file, "PNG" );
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::setTimeFilter(const QDateTime& dt)
{
	dtFilter = dt;
}

void ImageViewer::setCurrentItemS(const QString& filename)
{
	const QFileInfo *fi;
	QFileInfoListIterator it(fileList);
	int i=0;
	while( (fi=it.current())!=NULL )
	{
		if(fi->absFilePath()==filename)
			setCurrentItem(i);
		++it;
		i++;
	}
}

bool ImageViewer::hasPrev()
{
	if(currentItem()==0)
		return false;
	else
		return true;
}

bool ImageViewer::hasNext()
{
	if(currentItem()==count()-1)
		return false;
	else
		return true;
}

void ImageViewer::nextImage()
{
	int current = currentItem();
	current++;
	if(current>=count())
		current=0;
	setCurrentItem(current);
}

void ImageViewer::prevImage()
{
	int current = currentItem();
	current--;
	if(current<0)
		current=count()-1;
	setCurrentItem(current);
}

void ImageViewer::setSortSpec( QDir::SortSpec sortspec)
{
	sort = sortspec;
//	clear();
	buildList();
}

void ImageViewer::hoverOnItem(QListBoxItem *item)
{
	if(item!=NULL && item!=lastOne)
	{
		lastOne=item;
		QFileInfo *fi = fileList.at(index(item));

		QString tip = "Name:     "+ fi->fileName()+"\n"+
			"Size:     "+QString::number(fi->size()/1024)+"KB\n"+
			"Modified:  "+fi->lastModified().toString();
		
		QToolTip::remove(viewport(),itemRect(lastOne));
		QToolTip::add(viewport(),itemRect(item),tip);
		
	}
}

void ImageViewer::viewImage(QListBoxItem * item)
{
	QFileInfo *fi = fileList.at(index(item));
	QString filename = fi->absFilePath();
	emit onImage(filename);
}

void ImageViewer::buildList()
{
	// get the fileinfo list
	QDir dir;
	dir.setPath(d_path);
	if (d_path.right(1) != "/")
		d_path.append('/');

	clear();
	fileList.clear();
	
	const QFileInfoList *list = dir.entryInfoList(QDir::Files,sort);

	
	QFileInfoListIterator it( * list);
	const QFileInfo * fi;
	// add items
	const QString folder = d_path+thumbDir;
	while ((fi = it.current()) != 0)
	{
		QString strExt=fi->extension(false).lower();
		if(fi->created()>dtFilter && (strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
			strExt=="mng" || strExt=="png" || strExt=="bmp"))
		{
			const QString file = QString( "shadow_cache_%1_%2.png" ).arg(fi->fileName()).arg( thumbSize );
			if ( !QFile::exists( folder + file ) )
			{
				QString filename = fi->fileName();
				genThumb(d_path, filename);
			}
			if ( QFile::exists( folder + file ) )
			{
				QPixmap target( folder + file );
				
				fileList.append( new QFileInfo(*fi) );
				insertItem(target);
			}
		}			
		++it;
	}

}
#include <imageviewer.moc>
