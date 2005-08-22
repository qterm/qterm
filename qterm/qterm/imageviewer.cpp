#include "imageviewer.h"
#include "qtermcanvas.h"

#include <qdir.h>
#include <qlayout.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qheader.h>

ImageViewer::ImageViewer(const QString & image, const QString & path, QWidget * parent)
	:QListView(parent)
{
	QDir dir;
	dir.setPath(path);
	d_path = path;
	d_shadow = image;
	if (d_path.right(1) != "/")
		d_path.append('/');
	int thumbsize = 100;

	const QFileInfoList * list = dir.entryInfoList();

	QFileInfoListIterator it( * list);
	QFileInfo * fi;
	addColumn( tr( "ThumbView" ) );
	header()->setClickEnabled( FALSE, this->header()->count() - 1 );
	addColumn( tr( "FileName" ) );
	header()->setClickEnabled( FALSE, this->header()->count() - 1 );
		    
	setColumnWidth(0, thumbsize+50);
	setItemMargin(5);
	//d_imageList->setColumnAlignment(0, Qt::AlignCenter);
	//setColumnAlignment(1, Qt::AlignVCenter);
	//setAllColumnsShowFocus(true);
	//imageList->setVariableHeight(false);

	const QString folder = d_path+"shadow-cache/";
	while ((fi = it.current()) != 0) {
		QString strExt=fi->extension(false).lower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
			strExt=="mng" || strExt=="png" || strExt=="bmp") {
			const QString file = QString( "shadow_cache_%1_%2.png" ).arg(fi->fileName()).arg( thumbsize );
			if ( QFile::exists( folder + file ) ) {
				QPixmap target( folder + file );
				QListViewItem * item = new QListViewItem(this, "", fi->fileName());
				item->setPixmap(0, target);
				insertItem(item);
			}

			else {//generate thumbnail
				QString filename = fi->filePath();
				QImage original(filename);
				original = original.scale(thumbsize, thumbsize, QImage::ScaleMin);
				QImage shadow;
				uint shadowSize;
				if (original.width() > original.height())
				shadowSize = static_cast<uint>(original.width()/100.0*6.0);
				else
				shadowSize = static_cast<uint>(original.height()/100.0*6.0);

				shadow.load(d_shadow);
				shadow = shadow.scale( original.width() + shadowSize, original.height() + shadowSize);

				QImage target(shadow);
				bitBlt (&target, 0, 0, &original);

				target.save( folder + file, "PNG" );
				QListViewItem * item = new QListViewItem(this, "", fi->fileName());
				item->setPixmap(0, QPixmap(target));
				insertItem(item);
			}
		}			
		++it;
	}

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(viewImage(QListViewItem *)));

}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::imageChanged(const QString &filename)
{
	setSelected(findItem(filename, 1), true);
}

void ImageViewer::viewImage(QListViewItem * item)
{
	QString filename = item->text(1);
	emit selectionChanged(filename);
}

QTermImage::QTermImage(const QString & image, const QString & path, QWidget * parent)
	:QTermImageUI(parent), d_index(0)
{
	d_list = new QPtrList<QString>;
	d_viewer = NULL;

	connect(d_next, SIGNAL(clicked()), this, SLOT(next()));
	connect(d_previous, SIGNAL(clicked()), this, SLOT(previous()));
	connect(d_browser, SIGNAL(clicked()), this, SLOT(browser()));
	
	QDir dir;
	dir.setPath(path);
	d_path = path;
	d_shadow = image;
	if (d_path.right(1) != "/")
		d_path.append('/');
	d_list->setAutoDelete ( true );

	const QFileInfoList * list = dir.entryInfoList();

	QFileInfoListIterator it( * list);
	QFileInfo * fi;
	while ((fi = it.current()) != 0) {
		QString strExt=fi->extension(false).lower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
			strExt=="mng" || strExt=="png" || strExt=="bmp") {
			QString * filename = new QString(fi->fileName());
			d_list->append( filename);
		}
		++it;
	}
	if (!d_list->isEmpty()){
		d_canvas->loadImage(d_path+(*d_list->first()));
		d_extensionShown = false;
		d_viewer = new ImageViewer(d_shadow, d_path, 0);
		connect(d_viewer, SIGNAL(selectionChanged(const QString&)), this, SLOT(onChange(const QString&)));
		connect(this, SIGNAL(changeImage(const QString&)), d_viewer, SLOT(imageChanged(const QString&)));
		setExtension(d_viewer);
		setOrientation(Horizontal);
		showExtension( d_extensionShown );
	}
	else
		QMessageBox::warning(this, "Sorry", "You have no picture in pool to view");
}

QTermImage::~QTermImage()
{
	delete d_list;
	delete d_viewer;
	delete d_canvas;
}

void QTermImage::next()
{
	if (d_list->isEmpty())
		return;
	if (d_index < d_list->count() - 1) {
		++d_index;
		emit changeImage(*(d_list->at(d_index)));
	}
}

void QTermImage::previous()
{
	if (d_list->isEmpty())
		return;
	if (d_index <= 0)
		return;
	--d_index;
	emit changeImage(*(d_list->at(d_index)));
}

void QTermImage::browser()
{
	if (d_list->isEmpty())
		return;
	d_extensionShown = !d_extensionShown;
	showExtension( d_extensionShown );
}

void QTermImage::onChange(const QString & filename)
{
	//FIXME: find seems have no effect :(
	//int position = d_list->find(&filename);
	//if (position != -1)
		//d_index = position;
	int position = 0;
	QString * name;
	for (name=d_list->first(); name; name=d_list->next()) {
		if (* name == filename) {
			d_index = position;
			break;
		}
		++position;
	}
	
	d_canvas->loadImage(d_path+filename);
}
