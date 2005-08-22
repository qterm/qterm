#include "imageviewerui.h"
#include <qlistview.h>

class QString;
class QTermCanvas;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class ImageViewer : public QListView
{
	Q_OBJECT
public:
	ImageViewer(const QString &, const QString &, QWidget * parent=0);
	~ImageViewer();
	
	QSize sizeHint() const {
		return QSize(300,10);//Tell me how to do it correctly :(
	}
	

public slots:
	void viewImage(QListViewItem * );
	void imageChanged(const QString &);
signals:
	void selectionChanged(const QString &);
private:
	QString d_path;
	QString d_shadow;
};

class QTermImage : public QTermImageUI
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
signals:
	void changeImage(const QString &);
private:
	ImageViewer * d_viewer;
	QString d_path;
	QString d_shadow;
	QPtrList<QString> * d_list;
	int d_index;
	bool d_extensionShown;
};
