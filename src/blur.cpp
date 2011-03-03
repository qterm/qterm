/*
    Shamelessly copied from the patch for konsole at:
    https://bugs.kde.org/show_bug.cgi?id=198175
    with some obvious modifications for QTerm

    This file is part of Konsole, a terminal emulator for KDE.

    Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

#include "blur.h"

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace QTerm
{

  //_________________________________________________________________________
  void BlurHelper::updateBlurRegion( const QWidget* widget, const QRegion& region ) const
  {
#ifdef Q_WS_X11
    // Atom
    static Atom atom = XInternAtom( QX11Info::display(), "_KDE_NET_WM_BLUR_BEHIND_REGION", False);

    QVector<unsigned long> data;
    foreach( const QRect& rect, region.rects() )
    { data << rect.x() << rect.y() << rect.width() << rect.height(); }

    XChangeProperty(
      QX11Info::display(), widget->window()->winId(), atom, XA_CARDINAL, 32, PropModeReplace,
      reinterpret_cast<const unsigned char *>(data.constData()), data.size() );
#endif
  }

}

