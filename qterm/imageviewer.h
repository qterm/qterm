#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <qlistbox.h>
#include <qdir.h>
#include <qdatetime.h>

class QString;
class QTermCanvas;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class ImageViewer : public QListBox
{
	Q_OBJECT
public:
	ImageViewer(const QString &, QWidget * parent=0);
	~ImageViewer();
	
	// static methods
	static void genThumb(const QString &, const QString &);
	
	void setTimeFilter(const QDateTime&);
	void setSortSpec(QDir::SortSpec);
	
	void nextImage();
	void prevImage();

	bool hasNext();
	bool hasPrev();

	// setCurrentItem by string
	void setCurrentItemS(const QString&);

	void buildList();

public slots:
	void refreshList(){int i=currentItem();clear();buildList();setCurrentItem(i);}

signals:
	void onImage(const QString&);

protected slots:
	void viewImage(QListBoxItem * );
	void hoverOnItem(QListBoxItem *);

private:
	static const int thumbSize = 100;
	static const QString thumbDir;
	QString d_path;
	QListBoxItem *lastOne;
	QFileInfoList fileList;
	QDir::SortSpec sort;
	QDateTime dtFilter;
};

#endif
