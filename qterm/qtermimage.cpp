#include <qapplication.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qscrollview.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>


#include "qtermimage.h"
#include "imageviewer.h"
#include "qtermcanvas.h"
#include "qtermconfig.h"

extern QString pathPic;
extern QString fileCfg;

QTermImage::QTermImage(QWidget *parent, const QString& pathPool, bool showList)
	:QWidget(parent, NULL, WType_TopLevel)
{
	layout = new QGridLayout(this);

	// layout for top row
	h_layout= new QHBoxLayout();
	// serveral toolbuttons
	h_layout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
	btnPrev = new QPushButton(tr("Previous"),this);
	connect(btnPrev, SIGNAL(clicked()), this, SLOT(previousImage()));
	h_layout->addWidget(btnPrev);
	
	h_layout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
	btnNext = new QPushButton(tr("Next"),this);
	h_layout->addWidget(btnNext);
	connect(btnNext, SIGNAL(clicked()), this, SLOT(nextImage()));

	h_layout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
	btnList = new QPushButton(tr("List"),this);
	btnList->setToggleButton(true);
	connect(btnList, SIGNAL(toggled(bool)), this, SLOT(toggleList(bool)));
	h_layout->addWidget(btnList);

	h_layout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
	QPushButton *btnFixed = new QPushButton(tr("Fixed"),this);
	btnFixed->setToggleButton(true);
	h_layout->addWidget(btnFixed);

	h_layout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
	QPushButton *btn = new QPushButton(tr("Close"),this);
	h_layout->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(close()));

	
	layout->addItem(h_layout,0,0);

	// sortby combobox
	cmb = new QComboBox(this);
	cmb->insertItem("By Date");
	cmb->insertItem("By Name");
	cmb->insertItem("By Size");
	connect(cmb, SIGNAL(activated(int)), this,SLOT(sortBy(int)));

	// update children geometry
	layout->addWidget(cmb,0,1);

	// scrollview for imageviewer
	canvas = new QTermCanvas(this);
	connect(canvas, SIGNAL(resizeWindow(const QSize&)), this, SLOT(resizeWindow(const QSize&)));
	layout->addWidget(canvas,1,0);
	connect(btnFixed, SIGNAL(toggled(bool)), canvas, SLOT(setFixed(bool)));

	// images list
	list = new ImageViewer(pathPool,this);
	list->setFixedWidth(150);
	connect(list, SIGNAL(onImage(const QString&)), this, SLOT(imageSelected(const QString&)));
	layout->addWidget(list,1,1);

	connect(canvas, SIGNAL(filesChanged()), list, SLOT(refreshList()));

	// initialization
	btnList->setOn(showList);
	toggleList(showList);
	
	// propagate all geometries
	layout->activate();

	// the popup menu
        m_pMenu = new QPopupMenu(this);
        m_pMenu->insertItem( tr("zoom 1:1"), canvas, SLOT(oriSize()), Key_Z );
        m_pMenu->insertItem( tr("fit window"), canvas, SLOT(fitWin()), Key_X );
        m_pMenu->insertSeparator();
        m_pMenu->insertItem( tr("zoom in"), canvas, SLOT(zoomIn()), Key_Equal );
        m_pMenu->insertItem( tr("zoom out"), canvas, SLOT(zoomOut()), Key_Minus );
        m_pMenu->insertItem( tr("fullscreen"), this, SLOT(fullScreen()), Key_F );
        m_pMenu->insertSeparator();
        m_pMenu->insertItem( tr("rotate CW 90"), canvas, SLOT(cwRotate()), Key_BracketRight );
        m_pMenu->insertItem( tr("rotate CCW 90"), canvas, SLOT(ccwRotate()), Key_BracketLeft );

        m_pMenu->insertSeparator();
        m_pMenu->insertItem( tr("save as"), canvas, SLOT(saveImage()), Key_S );
        m_pMenu->insertItem( tr("copy to"), canvas, SLOT(copyImage()), Key_C );
        m_pMenu->insertItem( tr("silent copy"), canvas, SLOT(silentCopy()), Key_S+SHIFT );
        m_pMenu->insertItem( tr("delete"), canvas, SLOT(deleteImage()), Key_D );

        m_pMenu->insertSeparator();
	m_pMenu->insertItem( tr("list"), this, SLOT(listImage()), Key_L );
        m_pMenu->insertItem( tr("exit"), this, SLOT(close()), Key_Q );

	// dont let other widgets than canvas steal the keyboard focus
	list->setFocusPolicy(QWidget::NoFocus);
	cmb->setFocusPolicy(QWidget::NoFocus);
	btnPrev->setFocusPolicy(QWidget::NoFocus);
	btnNext->setFocusPolicy(QWidget::NoFocus);
	btnList->setFocusPolicy(QWidget::NoFocus);
	btn->setFocusPolicy(QWidget::NoFocus);
	btnFixed->setFocusPolicy(QWidget::NoFocus);


}

QTermImage::~QTermImage()
{
	delete layout;
	delete canvas;
	delete list;
	delete cmb;
	delete btnPrev;
	delete btnNext;
	delete btnList;
}

void QTermImage::loadImage(const QString& filename)
{
	list->setCurrentItemS(filename);
}
void QTermImage::updateList()
{
	list->buildList();
}
void QTermImage::setTimeFilter(const QDateTime& dt)
{
	list->setTimeFilter(dt);
}
void QTermImage::mouseReleaseEvent(QMouseEvent *e)
{
	if(e->button()&RightButton)
	{
		QWidget *child = childAt(e->pos());
		if(child==canvas->viewport() || child==canvas->picLabel())
			m_pMenu->popup(e->globalPos());
	}
}

void QTermImage::keyPressEvent(QKeyEvent *e)
{
        switch(e->key())
        {
                case Qt::Key_Escape:
                        if(isFullScreen())
                                showNormal();
                        else
                                close();
                        break;
                case Qt::Key_Left:
                        canvas->moveImage(-0.05,0);
                        break;
                case Qt::Key_Right:
                        canvas->moveImage(0.05,0);
                        break;
                case Qt::Key_Up:
			if(canvas->isScrollbarVisible())
                        	canvas->moveImage(0,-0.05);
			else //alternative
				previousImage();
                        break;
                case Qt::Key_Down:
			if(canvas->isScrollbarVisible())
                        	canvas->moveImage(0,0.05);
			else //alternative
				nextImage();
                        break;
		case Qt::Key_PageUp:
			previousImage();
			break;
		case Qt::Key_PageDown:
			nextImage();
			break;
        }

}

/******************************************************
 *                    Slots
 ******************************************************/

// onImage signal from list
void QTermImage::imageSelected(const QString& filename)
{
        btnPrev->setEnabled(list->hasPrev());
        btnNext->setEnabled(list->hasNext());
	canvas->loadImage(filename);
}

// resizeWindow signal from canvas
void QTermImage::resizeWindow(const QSize& szCanvas)
{
	if(!isFullScreen())
	{
		QSize diff = szCanvas-canvas->size();
		resize(size()+diff);
	}
}

// toggle list menu slot 
void QTermImage::listImage()
{
	btnList->setOn(!btnList->isOn());
}

// toggle list button slot
void QTermImage::toggleList(bool on)
{
	if(on)
	{
		cmb->show();
		list->show();
	}
	else
	{	
		list->hide();
		cmb->hide();
	}
}

void QTermImage::fullScreen()
{
	if(!isFullScreen())
	{
		before = btnList->isOn();
		btnList->setOn(false);
		toggleList(false);
		showFullScreen();
	}
	else
	{
		toggleList(btnList->isOn() || before);
		showNormal();
	}
}


void QTermImage::nextImage()
{
	list->nextImage();
}

void QTermImage::previousImage()
{
	list->prevImage();
}


void QTermImage::sortBy(int sort)
{
	QDir::SortSpec sortspec=QDir::Name;
	switch(sort)
	{
		case 0:
			sortspec=QDir::Time;
			break;
		case 1:
			sortspec=QDir::Name;
			break;
		case 2:
			sortspec=QDir::Size;
			break;
	}
	list->setSortSpec(sortspec);
}
/*
int main(int argc, char **argv)
{
	QApplication qApp(argc, argv);

	pathPic="/usr/share/qterm/pic";
	pathPool=QDir::homeDirPath()+"/.qterm/pool/";
	fileCfg=QDir::homeDirPath()+"/.qterm/qterm.cfg";
	QTermImage test;
	
	qApp.setMainWidget(&test);
	test.loadImage("gg.jpg");
	test.showNormal();
	return qApp.exec();

}
*/
#include <qtermimage.moc>
