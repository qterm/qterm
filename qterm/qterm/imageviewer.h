#include "imageviewerui.h"

class QString;
class QTermCanvas;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class ImageViewer : public ImageViewerUI 
{
	Q_OBJECT
public:
	ImageViewer(const QString &, const QString &, QWidget * parent=0);
	~ImageViewer();

public slots:
	void viewImage(QListViewItem * );
signals:
	void selectionChanged(const QString &);
private:
	QString d_path;
	QString d_shadow;
};

class QTermImage : public QDialog
{
	Q_OBJECT
public:
	QTermImage(const QString &, const QString &, QWidget * parent=0);
	~QTermImage();
	
public slots:
	void next();
	void previous();
	void browser();
	void onChange(const QString &);
	
private:
	QTermCanvas * d_canvas;
	ImageViewer * d_viewer;
	QString d_path;
	QString d_shadow;
	QPushButton * d_previous;
	QPushButton * d_next;
	QPushButton * d_browser;
	QPtrList<QString> * d_list;
	int d_index;
};
