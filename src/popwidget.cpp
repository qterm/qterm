/*******************************************************************************
FILENAME:      popwidget.cpp
REVISION:      2003.1.13 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "popwidget.h"

#include "qtermframe.h"

#include <qpixmap.h>
#include <qapplication.h>
#if (QT_VERSION>=300)
#include <qdesktopwidget.h>
#endif
#include <qtimer.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPalette>
#include <QX11Info>
//X11 includes
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

extern QString pathLib;

popWidget::popWidget( QTermWindow *win, QWidget *parent, const char *name, Qt::WFlags f )
		: QWidget(parent,f)
{
	QPixmap pxm(QPixmap(pathLib+"pic/popwidget.png") );
	if(!pxm.isNull())
	{
		resize(pxm.width(), pxm.height());
		QPalette palette;
		palette.setBrush(backgroundRole(), QBrush(pxm));
		setPalette(palette);
		//setBackgroundPixmap(pxm);
	}
	else
	{
		resize(200, 120);
	}

	label = new QLabel(this);
	label->setGeometry( QRect( 5, height()/3, width()-10, height()*2/3 ) );
	label->setAlignment( Qt::AlignTop );
	label->setWordWrap(true);
	if(!pxm.isNull()){
		QPalette palette;
		palette.setBrush(label->backgroundRole(), QBrush(pxm));
		label->setPalette(palette);
	//	label->setBackgroundPixmap(pxm);
	}
	else {
		QPalette palette;
		palette.setColor(label->backgroundRole(), QColor(249,250,229));
		label->setPalette(palette);
		//label->setBackgroundColor(QColor(249,250,229));
	}
	//label->setBackgroundOrigin( ParentOrigin );
	label->setFont(QFont(qApp->font().family(), 12));
	
	pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(showTimer()));
	
	nState = -1;

	nStep = 2;

	nInterval = 500/(height()/nStep);

	setFocusPolicy(Qt::NoFocus);

	window = win;
	
	hide();
	
	Display *dsp = QX11Info::display();
	int screen = DefaultScreen(dsp);
	Window root = RootWindow(dsp, screen);
	
	WId winid = this->winId();
	
	XEvent ev;
	memset(&ev, 0, sizeof(ev));

	ev.xclient.type = ClientMessage;
	ev.xclient.window = winid;
	ev.xclient.message_type = XInternAtom (dsp, "_NET_WM_DESKTOP", False );
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = -1;
	ev.xclient.data.l[1] = 0l;
	ev.xclient.data.l[2] = 0l;
	ev.xclient.data.l[3] = 0l;
	ev.xclient.data.l[4] = 0l;

	XSendEvent(dsp, root, False, SubstructureRedirectMask|SubstructureNotifyMask, &ev);
	XSync(dsp, False);
}

popWidget::~popWidget()
{
	delete pTimer;
}

void popWidget::mousePressEvent(QMouseEvent * me)
{
	QWidgetList list = QApplication::topLevelWidgets();

	//((QTermFrame *)qApp->mainWidget())->popupFocusIn(window);

	if(nState==1)
	{
		nState = 2;
		pTimer->setInterval(nInterval);
	}
}


void popWidget::popup()
{
	pTimer->start(nInterval);
	nState = 0;

	rcDesktop = qApp->desktop()->rect();
	ptPos = QPoint( rcDesktop.width()-width()-5, rcDesktop.height()-5 );
	move(ptPos);

	if(!isVisible())
		show();
}

void popWidget::setText(const QString& str)
{
	label->setText(str);
}

void popWidget::showTimer()
{
	switch(nState)
	{
	case 0:		// popup
		if(ptPos.y()+height()+5>rcDesktop.height())
			ptPos.setY( ptPos.y()- nStep );
		else
		{
			nState = 1;
			pTimer->setInterval(5000);
		}
		break;
	case 1:		// wait
		nState = 2;
		pTimer->setInterval(nInterval);
		break;
	case 2:		// hiding
		if(ptPos.y()<rcDesktop.height())
			ptPos.setY( ptPos.y() + nStep );
		else
		{
			nState = -1;
			pTimer->stop();
		}
		break;
	default:
		
		break;
	}
	move(ptPos);
}

#include <popwidget.moc>
