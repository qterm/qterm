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

extern QString pathLib;

popWidget::popWidget( QTermWindow *win, QWidget *parent, const char *name, WFlags f )
		: QWidget(parent,name,f)
{
	QPixmap pxm(QPixmap(pathLib+"pic/popwidget.png") );
	resize(pxm.width(), pxm.height());
	setBackgroundPixmap(pxm);

	label = new QLabel(this);
	label->setGeometry( QRect( 5, height()/3, width()-10, height()*2/3 ) );
	label->setAlignment( int( QLabel::AlignTop|QLabel::WordBreak ) );
	label->setBackgroundPixmap(pxm);
	label->setBackgroundOrigin( ParentOrigin );
	label->setFont(QFont(qApp->font().family(), 12));
	
	pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(showTimer()));
	
	nState = -1;

	nStep = 2;

	nInterval = 500/(pxm.height()/nStep);

	setFocusPolicy(NoFocus);

	window = win;
	
	hide();
}

popWidget::~popWidget()
{
	delete pTimer;
}

void popWidget::mousePressEvent(QMouseEvent * me)
{
	((QTermFrame *)qApp->mainWidget())->popupFocusIn(window);

	if(nState==1)
	{
		nState = 2;
		pTimer->changeInterval(nInterval);
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
			pTimer->changeInterval(5000);
		}
		break;
	case 1:		// wait
		nState = 2;
		pTimer->changeInterval(nInterval);
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

