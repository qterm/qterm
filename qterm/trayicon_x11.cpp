/****************************************************************************
** trayicon_x11.cpp - X11 tray app (for use with KDE and GNOME)
** Copyright (C) 2001, 2002  Justin Karneges
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,USA.
**
****************************************************************************/

//#ifdef Q_WS_X11

#include "trayicon.h"

#include<qapplication.h>
#include<qimage.h>
#include<qpixmap.h>
#include<qbitmap.h>
#include<qcursor.h>
#include<qlabel.h>
#include<qtooltip.h>
#include<qpainter.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>


class TrayIcon::TrayIconPrivate : public QLabel
{
public:
	TrayIconPrivate( TrayIcon *object, const QPixmap &pm, bool _isWMDock )
	: QLabel( 0, "QTerm", WMouseNoMask ), iconObject(object)
	{
		isWMDock = _isWMDock;

		// WindowMaker
		if(isWMDock) {
			QPixmap pix = pm;
			setPixmap(pix);
			resize(64,64);
			update();

			Display *dsp = x11Display();  // get the display
			WId win = winId();     // get the window
			XWMHints *hints;  // hints
			/*XClassHint classhint;  // class hints
			classhint.res_name = "psidock";  // res_name
			classhint.res_class = "Wharf";  // res_class
			XSetClassHint(dsp, win, &classhint); // set the class hints*/
			hints = XGetWMHints(dsp, win);  // init hints
			hints->initial_state = WithdrawnState;
			hints->icon_x = 0;
			hints->icon_y = 0;
			hints->icon_window = winId(); //wharfIcon->winId();
			hints->window_group = win;  // set the window hint
			hints->flags = WindowGroupHint | IconWindowHint | IconPositionHint | StateHint; // set the window group hint
			XSetWMHints(dsp, win, hints);  // set the window hints for WM to use.
			XFree( hints );
		}
		// KDE/GNOME
		else {
			setWFlags(WRepaintNoErase);

			QPixmap pix = pm; //buildIcon(pm);
			setPixmap(pix);
			resize(22,22);
			update();

			//setMask(*pix.mask());

			// dock the widget (adapted from Gabber / gdk)
			Display *dsp = x11Display(); // get the display
			WId win = winId();           // get the window
			int r;
			int data = 1;
			r = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
			XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
			r = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
			XChangeProperty(dsp, win, r, XA_WINDOW, 32, 0, (uchar *)&data, 1);
		}
	}

	~TrayIconPrivate()
	{
	}

	void mouseMoveEvent( QMouseEvent *e )
	{
		QApplication::sendEvent(iconObject, e);
	}

	void mousePressEvent( QMouseEvent *e )
	{
		QApplication::sendEvent(iconObject, e);
	}

	void mouseReleaseEvent( QMouseEvent *e )
	{
		QApplication::sendEvent(iconObject, e);
	}

	void mouseDoubleClickEvent( QMouseEvent *e )
	{
		QApplication::sendEvent(iconObject, e);
	}

	void closeEvent(QCloseEvent *)
	{
		iconObject->gotCloseEvent();
	}

	TrayIcon *iconObject;
	bool isWMDock;
};

void TrayIcon::sysInstall()
{
	if ( d )
		return;

	d = new TrayIconPrivate( this, pm, v_isWMDock);
	QToolTip::add(d, tip);
	d->show();
}

void TrayIcon::sysRemove()
{
	if ( !d )
		return;

	delete d;
	d = 0;
}

void TrayIcon::sysUpdateIcon()
{
	if ( !d )
		return;

	QPixmap pix = pm; //buildIcon(pm);
	d->setPixmap(pix);
	//d->setMask(*pix.mask());
	d->repaint();
}

void TrayIcon::sysUpdateToolTip()
{
	if ( !d )
		return;

	if(tip.isEmpty())
		QToolTip::remove(d);
	else
		QToolTip::add(d, tip);
}

//#endif // Q_WS_X11
