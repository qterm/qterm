/*
 * trayicon_x11.cpp - X11 trayicon (for use with KDE and GNOME)
 * Copyright (C) 2003  Justin Karneges
 * GNOME2 Notification Area support: Tomasz Sterna
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "trayicon.h"

#ifdef Q_WS_X11

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


/* for Gnome2 Notification Area */
static XErrorHandler old_handler = 0;
static int dock_xerror = 0;
static int dock_xerrhandler(Display* dpy, XErrorEvent* err)
{
	dock_xerror = err->error_code;
	return old_handler(dpy, err);
}

static void trap_errors()
{
	dock_xerror = 0;
	old_handler = XSetErrorHandler(dock_xerrhandler);
}

static bool untrap_errors()
{
	XSetErrorHandler(old_handler);
	return (dock_xerror == 0);
}

static bool send_message(
	Display* dpy, /* display */
	Window w,	 /* sender (tray icon window) */
	long message, /* message opcode */
	long data1,   /* message data 1 */
	long data2,   /* message data 2 */
	long data3	/* message data 3 */
){
	XEvent ev;

	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = XInternAtom (dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = message;
	ev.xclient.data.l[2] = data1;
	ev.xclient.data.l[3] = data2;
	ev.xclient.data.l[4] = data3;

	trap_errors();
	XSendEvent(dpy, w, False, NoEventMask, &ev);
	XSync(dpy, False);
	return untrap_errors();
}

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2


/*static QPixmap buildIcon(const QPixmap &_in)
{
	QImage image(22,22,32);
	image.fill(0);
	image.setAlphaBuffer(TRUE);

	QImage in = _in.convertToImage();
	//QImage in(22,22,32);

	// make sure it is no bigger than 22x22
	if(in.width() > 22 || in.height() > 22)
		in = in.smoothScale(22,22);

	// draw the pixmap onto the 22x22 image
	int xo = (image.width() - in.width()) / 2;
	int yo = (image.height() - in.height()) / 2;
	for(int n2 = 0; n2 < in.height(); ++n2) {
		for(int n = 0; n < in.width(); ++n) {
			if(qAlpha(in.pixel(n,n2))) {
				image.setPixel(n+xo, n2+yo, in.pixel(n,n2));
			}
		}
	}

	// convert to pixmap and return
	QPixmap icon;
	icon.convertFromImage(image);
	return icon;
}*/

//class TrayIcon::TrayIconPrivate : public QLabel
class TrayIcon::TrayIconPrivate : public QLabel
{
public:
	TrayIconPrivate( TrayIcon *object, const QPixmap &pm, bool _isWMDock )
	: QLabel(NULL, "psidock",  WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WMouseNoMask), iconObject(object)
//	: QLabel( 0, "psidock", WMouseNoMask ), iconObject(object)
	{
		isWMDock = _isWMDock;
		inNetTray = false;
		bInit = false;
		inTray = false;

		setBackgroundMode(X11ParentRelative);
		setBackgroundOrigin(WindowOrigin);
		setMinimumSize(22, 22);
		setPixmap(pm);

		Display *dsp = x11Display(); // get the display
		WId win = winId();		 // get the window
		
		XClassHint classhint;
		classhint.res_name  = (char*)"psidock";
		classhint.res_class = (char*)"Wharf";
		XSetClassHint(dsp, win, &classhint);
		XWMHints *hints;  // hints
		hints = XGetWMHints(dsp, win);  // init hints
		hints->initial_state = WithdrawnState;
		hints->icon_x = 0;
		hints->icon_y = 0;
		hints->icon_window = winId();
		hints->window_group = win;  // set the window hint
		hints->flags = WindowGroupHint | IconWindowHint | IconPositionHint | StateHint; // set the window group hint
		XSetWMHints(dsp, win, hints);  // set the window hints for WM to use.
		XFree( hints );

		// WindowMaker
		if(isWMDock) {
			//QPixmap pix = pm;
			//setPixmap(pix);
			resize(64,64);
			update();

			/*Display *dsp = x11Display();  // get the display
			WId win = winId();     // get the window
			//XClassHint classhint;  // class hints
			//classhint.res_name = "psidock";  // res_name
			//classhint.res_class = "Wharf";  // res_class
			//XSetClassHint(dsp, win, &classhint); // set the class hints
			XWMHints *hints;  // hints
			hints = XGetWMHints(dsp, win);  // init hints
			hints->initial_state = WithdrawnState;
			hints->icon_x = 0;
			hints->icon_y = 0;
			hints->icon_window = winId(); //wharfIcon->winId();
			hints->window_group = win;  // set the window hint
			hints->flags = WindowGroupHint | IconWindowHint | IconPositionHint | StateHint; // set the window group hint
			XSetWMHints(dsp, win, hints);  // set the window hints for WM to use.
			XFree( hints );*/
		}
		// KDE/GNOME
		else {
			setWFlags(WRepaintNoErase);

			//QPixmap pix = pm; //buildIcon(pm);
			//setPixmap(pix);
			resize(22,22);
			update();

			//setMask(*pix.mask());

			/*// dock the widget (adapted from Gabber / gdk)
			Display *dsp = x11Display(); // get the display
			WId win = winId();           // get the window
			int r;
			int data = 1;
			r = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
			XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
			r = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
			XChangeProperty(dsp, win, r, XA_WINDOW, 32, 0, (uchar *)&data, 1);*/

			inNetTray = false;

			// dock the widget (adapted from SIM-ICQ)
			Screen *screen = XDefaultScreenOfDisplay (dsp); // get the screen
			int screen_id = XScreenNumberOfScreen(screen); // and it's number
			char buf[32];
			snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen_id);
			Atom selection_atom = XInternAtom(dsp, buf, false);
			XGrabServer(dsp);
			Window manager_window = XGetSelectionOwner(dsp, selection_atom);
			if (manager_window != None)
				XSelectInput(dsp, manager_window, StructureNotifyMask);
			XUngrabServer(dsp);
			XFlush(dsp);
			if (manager_window != None){
				inNetTray = true;
				if (!send_message(dsp, manager_window, SYSTEM_TRAY_REQUEST_DOCK, win, 0, 0)){
					inNetTray = false;
				}
			}

			Atom kwm_dockwindow_atom = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
			Atom kde_net_system_tray_window_for_atom = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);

			long data[1];
			data[0] = 0;
			XChangeProperty(dsp, win, kwm_dockwindow_atom, kwm_dockwindow_atom,
					32, PropModeReplace,
					(unsigned char*)data, 1);
			XChangeProperty(dsp, win, kde_net_system_tray_window_for_atom, XA_WINDOW,
					32, PropModeReplace,
					(unsigned char*)data, 1);

			resize(22, 22);
			if (!inNetTray) {
				move(-21, -21);
			}
			show();
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

	void closeEvent(QCloseEvent *e)
	{
		iconObject->gotCloseEvent();
		e->accept();
	}

	void paintEvent( QPaintEvent* )
	{
		QPainter p(this);
		p.drawPixmap((width() - pix.width())/2, (height() - pix.height())/2, pix);
	}

	inline void setPixmap ( const QPixmap & pm )
	{
		pix = pm;
	}

	bool x11Event(XEvent *e);
	
	QPixmap pix;
	TrayIcon *iconObject;
	bool isWMDock;
	bool inNetTray;
	bool inTray;
	bool bInit;
};

bool TrayIcon::TrayIconPrivate::x11Event(XEvent *e)
{
	if (e->type == ClientMessage){
		if (!inTray){
			Atom xembed_atom = XInternAtom( qt_xdisplay(), "_XEMBED", FALSE );
			if (e->xclient.message_type == xembed_atom){
				inTray = true;
				bInit = true;
			}
		}
	}
	if ((e->type == ReparentNotify) && !bInit && inNetTray){
		Display *dsp = qt_xdisplay();
		if (e->xreparent.parent == XRootWindow(dsp,
						XScreenNumberOfScreen(XDefaultScreenOfDisplay(dsp)))
		   ){
			inNetTray = false;
		}else{
			inTray = true;
			bInit = true;
			move(0, 0);
			resize(22, 22);
			XResizeWindow(dsp, winId(), 22, 22);
		}
	}
	if (((e->type == FocusIn) || (e->type == Expose)) && !bInit){
		if (!inTray){
			bInit = true;
			setFocusPolicy(NoFocus);
		}
	}
	return QWidget::x11Event(e);
}

void TrayIcon::sysInstall()
{
	if ( d )
		return;

	d = new TrayIconPrivate( this, pm, v_isWMDock);
	sysUpdateToolTip();
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

#endif // Q_WS_X11
