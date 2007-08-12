#ifndef IMAGEVIERWER_H
#define IMAGEVIERWER_H

#include "ui_imageviewer.h"

//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>

class QString;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace QTerm
{
class QTermCanvas;

class ImageViewer : public QTreeWidget
{
	Q_OBJECT
public:
	ImageViewer(const QString &, const QString &, QWidget * parent=0);
	~ImageViewer();
	
	QSize sizeHint() const {
		return QSize(300,10);//Tell me how to do it correctly :(
	}
	static void genThumb(const QString &, const QString &, const QString &);

public slots:
	void viewImage(int n);
	void imageChanged(const QString &);
signals:
	void selectionChanged(const QString &);
private:
	static const int thumbSize = 100;
	static const QString thumbDir;
	QString d_path;
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
signals:
	void changeImage(const QString &);
private:
	Ui::QTermImage ui;
	ImageViewer * d_viewer;
	QString d_path;
	QString d_shadow;
	QStringList * d_list;
	int d_index;
	bool d_extensionShown;
};

} // namespace QTerm

#endif
