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

ImageViewer::ImageViewer(const QString & image, const QString & path, QWidget * parent)
	:ImageViewerUI(parent)
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

	imageList->setColumnWidth(0, thumbsize+50);
	imageList->setItemMargin(5);
	//imageList->setColumnAlignment(0, Qt::AlignCenter);
	imageList->setColumnAlignment(1, Qt::AlignVCenter);
	imageList->setAllColumnsShowFocus(true);
	//imageList->setVariableHeight(false);


	const QString folder = d_path+"shadow-cache/";
	while ((fi = it.current()) != 0) {
		QString strExt=fi->extension(false).lower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
			strExt=="mng" || strExt=="png" || strExt=="bmp") {
			const QString file = QString( "shadow_cache_%1_%2.png" ).arg(fi->fileName()).arg( thumbsize );
			if ( QFile::exists( folder + file ) ) {
				QPixmap target( folder + file );
				QListViewItem * item = new QListViewItem(imageList, "", fi->fileName());
				item->setPixmap(0, target);
				imageList->insertItem(item);
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
				QListViewItem * item = new QListViewItem(imageList, "", fi->fileName());
				item->setPixmap(0, QPixmap(target));
				imageList->insertItem(item);
			}
		}			
		++it;
	}

	connect(imageList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(viewImage(QListViewItem *)));

}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::viewImage(QListViewItem * item)
{
	QString filename = item->text(1);
	emit selectionChanged(filename);
}

QTermImage::QTermImage(const QString & image, const QString & path, QWidget * parent)
	:QDialog(parent), d_index(0)
{
	d_canvas = new QTermCanvas(this, NULL, 0);
	d_canvas->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, d_canvas->sizePolicy().hasHeightForWidth() ) );
	d_previous = new QPushButton( "&Previous", this, "previous" );
	d_browser = new QPushButton( "&Browser", this, "previous" );
	d_next = new QPushButton( "&Next", this, "previous" );
	d_viewer = new ImageViewer(image, path, parent);
	d_list = new QPtrList<QString>;

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->addWidget(d_canvas);
	QHBoxLayout * hbox = new QHBoxLayout(vbox);
	hbox->addWidget(d_previous);
	hbox->addWidget(d_browser);
	hbox->addWidget(d_next);
	resize(QSize(800, 600).expandedTo(minimumSizeHint())); 

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
			QString * filename = new QString(fi->filePath());
			d_list->append( filename);
		}
		++it;
	}
	if (!d_list->isEmpty())
		d_canvas->loadImage(*(d_list->first()));
	else
		QMessageBox::warning(this, "Sorry", "You have no picture in pool to view");
}

QTermImage::~QTermImage()
{
	delete d_canvas;
	delete d_list;
	delete d_previous;
	delete d_browser;
	delete d_next;
}

void QTermImage::next()
{
	if (d_list->isEmpty())
		return;
	if (d_index < d_list->count() - 1) {
		++d_index;
		d_canvas->loadImage(*(d_list->at(d_index)));
	}
}

void QTermImage::previous()
{
	if (d_list->isEmpty())
		return;
	if (d_index <= 0)
		return;
	--d_index;
	d_canvas->loadImage(*(d_list->at(d_index)));
}

void QTermImage::browser()
{
	if (d_list->isEmpty())
		return;
	ImageViewer * browser = new ImageViewer(d_shadow, d_path, this);
	connect(browser, SIGNAL(selectionChanged(const QString&)), this, SLOT(onChange(const QString&)));
	browser->exec();
}

void QTermImage::onChange(const QString & filename)
{
	int position = d_list->find(&filename);
	if (position != -1)
		d_index = position;
	d_canvas->loadImage(d_path+filename);
}
