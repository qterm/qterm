#include "pallete.h"

#include <QMouseEvent>
#include <QMessageBox>

namespace QTerm
{

Pallete::Pallete( QWidget* parent, Qt::WFlags fl )
    : QWidget( parent, fl )
{
	setupUi(this);
	frameList << clr0Frame << clr1Frame << clr2Frame << clr3Frame << 
		clr4Frame << clr5Frame << clr6Frame << clr7Frame << 
		clr8Frame << clr9Frame << clr10Frame << clr11Frame << 
		clr12Frame << clr13Frame << clr14Frame << clr15Frame;

	installEventFilter(this);
}

bool Pallete::eventFilter(QObject *obj, QEvent *e)
{
	// ignore context menu which is often triggered by right mouse button
	if (e->type() == QEvent::ContextMenu) {
		return true;
	}
	if (e->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *me = static_cast<QMouseEvent *>(e);
		QFrame *frame = qobject_cast<QFrame*>(childAt(me->pos()));
		int n = 0;
		if ( (n=frameList.indexOf(frame)) == -1)
			return false;
		switch (me->button())
		{
		case Qt::LeftButton:
			fgLabel->setStyleSheet(frame->styleSheet());
			emit colorChanged(n, 0);
			break;
		case Qt::RightButton:
			if (n<8) {
				bgLabel->setStyleSheet(frame->styleSheet());
				emit colorChanged(n, 1);
			}
			break;
		default:
			return false;
		}
		return true;
	}
	return QWidget::eventFilter(obj, e);
}

}
#include <pallete.moc>