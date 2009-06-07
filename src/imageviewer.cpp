#include "imageviewer.h"
#include "qtermcanvas.h"

// #include <qdir.h>
// #include <qlayout.h>
// #include <qstring.h>
// #include <qpixmap.h>
// #include <qimage.h>
// #include <q3listview.h>
// #include <qpushbutton.h>
// #include <qmessagebox.h>
// #include <q3header.h>
#include <QFile>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDir>
namespace QTerm
{
const QString ImageViewer::thumbDir = "shadow-cache/";

ImageViewer::ImageViewer(const QString & image, const QString & path, QWidget * parent)
	:QTreeWidget(parent)
{
	QDir dir;
	dir.setPath(path);
	d_path = path;
	if (d_path.right(1) != "/")
		d_path.append('/');

	const QFileInfoList list = dir.entryInfoList();

	//QFileInfoListIterator it( list );
	//QFileInfo * fi;
	setColumnCount(2);
	QStringList head;
	head << "ThumbView" << "FileName";
	setHeaderLabels(head);
	setSortingEnabled(false);
// 	FIXME:rewritten is needed
// 	addColumn( tr( "ThumbView" ) );
// 	header()->setClickEnabled( FALSE, this->header()->count() - 1 );
// 	addColumn( tr( "FileName" ) );
// 	header()->setClickEnabled( FALSE, this->header()->count() - 1 );
// 	setSortColumn(-1);

// 	setColumnWidth(0, thumbSize+30);
// 	setItemMargin(5);
	//d_imageList->setColumnAlignment(0, Qt::AlignCenter);
// 	setColumnAlignment(1, Qt::AlignVCenter);
// 	setAllColumnsShowFocus(true);
	//imageList->setVariableHeight(false);

	const QString folder = d_path+thumbDir;
// 	foreach (QFileInfo fi, list) {
// 		QString strExt=fi.extension(false).lower();
// 		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
// 			strExt=="mng" || strExt=="png" || strExt=="bmp") {
// 			const QString file = QString( "shadow_cache_%1_%2.png" ).arg(fi.fileName()).arg( thumbSize );
// 			if ( !QFile::exists( folder + file ) ) {
// 				QString filename = fi.fileName();
// 				genThumb(image, d_path, filename);
// 			}
// 			if ( QFile::exists( folder + file ) ) {
// 				QPixmap target( folder + file );
// 				QListWidgetItem * item = new QListWidgetItem(this, lastItem());
// 				item->setPixmap(0, target);
// 				item->setText(1,fi.fileName());
// 				insertItem(item);
// 			}
// 		}			
// 	}
// 	connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(viewImage(int)));
}

void ImageViewer::genThumb(const QString & background, const QString & path, const QString & filename)
{
	const QString file = QString( "shadow_cache_%1_%2.png" ).arg(filename).arg( thumbSize );
	const QString folder = path+thumbDir;
	QImage original(path+filename);
	original = original.scaled(thumbSize, thumbSize, Qt::KeepAspectRatio);
	QImage shadow;
	uint shadowSize;
	if (original.width() > original.height())
		shadowSize = static_cast<uint>(original.width()/100.0*6.0);
	else
		shadowSize = static_cast<uint>(original.height()/100.0*6.0);
	
	shadow.load(background);
	shadow = shadow.scaled( original.width() + shadowSize, original.height() + shadowSize);

	QImage target(shadow);
	QPainter painter(&target);
	painter.drawPixmap(0, 0, QPixmap::fromImage(original));
// 	bitBlt (&target, 0, 0, &original);

	target.save( folder + file, "PNG" );
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::imageChanged(const QString &filename)
{
// 	setSelected(findItem(filename, 1), true);
}

void ImageViewer::viewImage(int n)
{
// 	QString filename = item(n)->text(1);
// 	emit selectedFileChanged(filename);
}

Image::Image(const QString & image, const QString & path, QWidget * parent)
	:QDialog(parent), d_index(0)
{
	ui.setupUi(this);
	
	d_list = new QStringList;
	d_viewer = NULL;
	ui.d_browser->setText("Show Browser");

	connect(ui.d_next, SIGNAL(clicked()), this, SLOT(next()));
	connect(ui.d_previous, SIGNAL(clicked()), this, SLOT(previous()));
	connect(ui.d_browser, SIGNAL(clicked()), this, SLOT(browser()));
	
	QDir dir;
	dir.setPath(path);
	d_path = path;
	d_shadow = image;
	if (d_path.right(1) != "/")
		d_path.append('/');

	const QFileInfoList list = dir.entryInfoList();

	//QFileInfoListIterator it( list );
	//QFileInfo * fi;
	foreach(QFileInfo fi, list) {
		QString strExt=fi.suffix().toLower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" ||
			strExt=="mng" || strExt=="png" || strExt=="bmp")
			d_list->append(fi.fileName());
	}
	if (!d_list->isEmpty()){
		ui.d_canvas->loadImage(d_path+(d_list->first()));
		d_extensionShown = false;
		d_viewer = new ImageViewer(d_shadow, d_path, 0);
		connect(d_viewer, SIGNAL(selectedFileChanged(const QString&)), this, SLOT(onChange(const QString&)));
		connect(this, SIGNAL(changeImage(const QString&)), d_viewer, SLOT(imageChanged(const QString&)));
		setExtension(d_viewer);
		setOrientation(Qt::Horizontal);
		showExtension( d_extensionShown );
		ui.d_previous->setEnabled(false);
	}
	else {
		QMessageBox::warning(this, "Sorry", "You have no picture in pool to view");
		ui.d_previous->setEnabled(false);
		ui.d_browser->setEnabled(false);
		ui.d_next->setEnabled(false);
	}
		
}

Image::~Image()
{
	delete d_list;
	delete d_viewer;
// 	delete d_canvas;
}

void Image::next()
{
	if (d_list->isEmpty())
		return;
	if (d_index < d_list->count() - 1) {
		++d_index;
		emit changeImage(d_list->at(d_index));
	}
}

void Image::previous()
{
	if (d_list->isEmpty())
		return;
	if (d_index <= 0)
		return;
	--d_index;
	emit changeImage(d_list->at(d_index));
}

void Image::browser()
{
	if (d_list->isEmpty())
		return;
	d_extensionShown = !d_extensionShown;
	showExtension( d_extensionShown );
	if (d_extensionShown)
		ui.d_browser->setText(tr("Hide &Browser"));
	else
		ui.d_browser->setText(tr("Show &Browser"));
}

void Image::onChange(const QString & filename)
{
	int position = d_list->indexOf(filename);
	if (position != -1)
		d_index = position;

	ui.d_previous->setEnabled(true);
	ui.d_next->setEnabled(true);

	if (d_index == 0)
		ui.d_previous->setEnabled(false);
	if (d_index == d_list->count() - 1)
		ui.d_next->setEnabled(false);
	
	ui.d_canvas->loadImage(d_path+filename);
}

} // namespace QTerm

#include <imageviewer.moc>
