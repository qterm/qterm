/*
 * trayicon_win.cpp - Windows trayicon, adapted from Qt example
 * Copyright (C) 2003  Justin Karneges
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

#if 0

#include "trayicon.h"

#include <qwidget.h>
#include <qapplication.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qcursor.h>

#include <qt_windows.h>

static uint MYWM_TASKBARCREATED = 0;
#define MYWM_NOTIFYICON	(WM_APP+101)

class TrayIcon::TrayIconPrivate : public QWidget
{
public:
    TrayIconPrivate( TrayIcon *object )
        : QWidget( 0 ), hIcon( 0 ), hMask( 0 ), iconObject( object )
    {
	if ( !MYWM_TASKBARCREATED ) {
		QT_WA( {
			MYWM_TASKBARCREATED = RegisterWindowMessageW( (TCHAR*)"TaskbarCreated" );
		}, {
			MYWM_TASKBARCREATED = RegisterWindowMessageA( "TaskbarCreated" );
		} )
	}
    }

    ~TrayIconPrivate()
    {
        if ( hMask )
            DeleteObject( hMask );
	if ( hIcon )
	    DestroyIcon( hIcon );
    }

    bool trayMessage( DWORD msg )
    {
	bool res;

	QT_WA( {
		NOTIFYICONDATAW tnd;
		memset( &tnd, 0, sizeof(NOTIFYICONDATAW) );
		tnd.cbSize		= sizeof(NOTIFYICONDATAW);
		tnd.hWnd		= winId();

		if ( msg != NIM_DELETE ) {
			tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage= MYWM_NOTIFYICON;
			tnd.hIcon		= hIcon;
			if ( !iconObject->toolTip().isNull() ) {
				// Tip is limited to 63 + NULL; lstrcpyn appends a NULL terminator.
				QString tip = iconObject->toolTip().left( 63 ) + QChar();
				lstrcpynW(tnd.szTip, (TCHAR*)qt_winTchar( tip, FALSE ), QMIN( tip.length()+1, 64 ) );
			}
		}

		res = Shell_NotifyIconW(msg, &tnd);
	}, {
		NOTIFYICONDATAA tnd;
		memset( &tnd, 0, sizeof(NOTIFYICONDATAA) );
		tnd.cbSize		= sizeof(NOTIFYICONDATAA);
		tnd.hWnd		= winId();

		if ( msg != NIM_DELETE ) {
			tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage= MYWM_NOTIFYICON;
			tnd.hIcon		= hIcon;
			if ( !iconObject->toolTip().isNull() ) {
				// Tip is limited to 63 + NULL; lstrcpyn appends a NULL terminator.
				QString tip = iconObject->toolTip().left( 63 ) + QChar();
				lstrcpynA(tnd.szTip, (const char*)tip.local8Bit(), QMIN( tip.length()+1, 64 ) );
			}
		}

		res = Shell_NotifyIconA(msg, &tnd);
	} )

	return res;
    }

    bool iconDrawItem(LPDRAWITEMSTRUCT lpdi)
    {
	if (!hIcon)
	    return FALSE;

	DrawIconEx(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, hIcon,
		0, 0, 0, NULL, DI_NORMAL );

	return TRUE;
    }

    bool winEvent( MSG *m )
    {
	switch(m->message) {
	case WM_DRAWITEM:
	    return iconDrawItem( (LPDRAWITEMSTRUCT)m->lParam );
	case MYWM_NOTIFYICON:
	    {
		QMouseEvent *e = 0;
		QPoint gpos = QCursor::pos();
		switch (m->lParam)
		{
		case WM_MOUSEMOVE:
		    e = new QMouseEvent( QEvent::MouseMove, mapFromGlobal( gpos ), gpos, 0, 0 );
		    break;
		case WM_LBUTTONDOWN:
		    e = new QMouseEvent( QEvent::MouseButtonPress, mapFromGlobal( gpos ), gpos, LeftButton, LeftButton );
		    break;
		case WM_LBUTTONUP:
		    e = new QMouseEvent( QEvent::MouseButtonRelease, mapFromGlobal( gpos ), gpos, LeftButton, LeftButton );
		    break;
		case WM_LBUTTONDBLCLK:
		    e = new QMouseEvent( QEvent::MouseButtonDblClick, mapFromGlobal( gpos ), gpos, LeftButton, LeftButton );
		    break;
		case WM_RBUTTONDOWN:
		    e = new QMouseEvent( QEvent::MouseButtonPress, mapFromGlobal( gpos ), gpos, RightButton, RightButton );
		    break;
		case WM_RBUTTONUP:
		    e = new QMouseEvent( QEvent::MouseButtonRelease, mapFromGlobal( gpos ), gpos, RightButton, RightButton );
		    break;
		case WM_RBUTTONDBLCLK:
		    e = new QMouseEvent( QEvent::MouseButtonDblClick, mapFromGlobal( gpos ), gpos, RightButton, RightButton );
		    break;
		case WM_MBUTTONDOWN:
		    e = new QMouseEvent( QEvent::MouseButtonPress, mapFromGlobal( gpos ), gpos, MidButton, MidButton );
		    break;
		case WM_MBUTTONUP:
		    e = new QMouseEvent( QEvent::MouseButtonRelease, mapFromGlobal( gpos ), gpos, MidButton, MidButton );
		    break;
		case WM_MBUTTONDBLCLK:
		    e = new QMouseEvent( QEvent::MouseButtonDblClick, mapFromGlobal( gpos ), gpos, MidButton, MidButton );
		    break;
		case WM_CONTEXTMENU:
		    e = new QMouseEvent( QEvent::MouseButtonRelease, mapFromGlobal( gpos ), gpos, RightButton, RightButton );
		    break;
		default:
		    break;
		}
		if ( e ) {
		    bool res = QApplication::sendEvent( iconObject, e );
		    delete e;
		    return res;
		}
	    }
	    break;
	default:
	    if ( m->message == MYWM_TASKBARCREATED ) {
		trayMessage( NIM_ADD );
	    }
	    break;
	}

	return QWidget::winEvent( m );
    }

    HICON		hIcon;
    HBITMAP             hMask;
    TrayIcon		*iconObject;
};

static HBITMAP createIconMask( const QPixmap &qp )
{
    QImage bm = qp.convertToImage();
    int w = bm.width();
    int h = bm.height();
    int bpl = ((w+15)/16)*2;			// bpl, 16 bit alignment
    uchar *bits = new uchar[bpl*h];
    bm.invertPixels();
    for ( int y=0; y<h; y++ )
	memcpy( bits+y*bpl, bm.scanLine(y), bpl );
    HBITMAP hbm = CreateBitmap( w, h, 1, 1, bits );
    delete [] bits;
    return hbm;
}

static HICON createIcon( const QPixmap &pm, HBITMAP &hbm )
{
    QPixmap maskpm( pm.size(), pm.depth(), QPixmap::NormalOptim );
    QBitmap mask( pm.size(), FALSE, QPixmap::NormalOptim );
    if ( pm.mask() ) {
        maskpm.fill( Qt::black );			// make masked area black
        bitBlt( &mask, 0, 0, pm.mask() );
    } else {
        maskpm.fill( Qt::color1 );
    }

    bitBlt( &maskpm, 0, 0, &pm);
    ICONINFO iconInfo;
    iconInfo.fIcon    = TRUE;
    iconInfo.hbmMask  = createIconMask(mask);
    hbm = iconInfo.hbmMask;
    iconInfo.hbmColor = pm.hbm();

    return CreateIconIndirect( &iconInfo );
}

void TrayIcon::sysInstall()
{
    if ( d )
	return;

    d = new TrayIconPrivate( this );
    d->hIcon = createIcon( pm, d->hMask );

	d->trayMessage( NIM_ADD );
}

void TrayIcon::sysRemove()
{
    if ( !d )
	return;

	d->trayMessage( NIM_DELETE );

    delete d;
    d = 0;
}

void TrayIcon::sysUpdateIcon()
{
    if ( !d )
	return;

    if ( d->hMask )
       DeleteObject( d->hMask );
    if ( d->hIcon )
	DestroyIcon( d->hIcon );

    d->hIcon = createIcon( pm, d->hMask );

	d->trayMessage( NIM_MODIFY );
}

void TrayIcon::sysUpdateToolTip()
{
    if ( !d )
	return;

	d->trayMessage( NIM_MODIFY );
}
#endif

