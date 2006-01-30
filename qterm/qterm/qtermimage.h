#ifndef QTERMIMAGE_H
#define QTERMIMAGE_H

#include <qwidget.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qlabel.h>
#include <qdatetime.h>

class QComboBox;
class ImageViewer;
class QTermCanvas;
class QPopupMenu;
class QPushButton;
class QLabel;
class QLineEdit;
class QCheckBox;


class QTermImage : public QWidget
{
        Q_OBJECT
public:
        QTermImage(QWidget *parent, const QString&, bool);
        ~QTermImage();

	void loadImage(const QString&);
	void updateList();
	void setTimeFilter(const QDateTime&);

protected:
	void loadSetting();
	void mouseReleaseEvent(QMouseEvent *);
	void keyPressEvent(QKeyEvent *);

protected slots:
	void imageSelected(const QString&);
	void previousImage();
	void nextImage();
	void listImage();
	void toggleList(bool);
	void sortBy(int);
	void fullScreen();
	void resizeWindow(const QSize&);
protected:
        QGridLayout *layout;
	QHBoxLayout *h_layout;
        QTermCanvas *canvas;
        ImageViewer *list;
	QComboBox *cmb;
	QPopupMenu *m_pMenu;
	QPushButton *btnPrev,*btnList,*btnNext;
	bool before;
};
#endif //QTERMIMAGE_H
