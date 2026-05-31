/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMON_UI_WINDOW_H
#define COMMON_UI_WINDOW_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "windef.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (X11_SUPPORT)
typedef unsigned long XID;
typedef XID Window;
//#include "X11/Xlib.h"
// forward declarations
struct _XDisplay;
#endif // X11_SUPPORT

#if defined (WAYLAND_SUPPORT)
#include "wayland-client.h"
#endif // WAYLAND_SUPPORT

#if defined (CURSES_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN32)
#undef MOUSE_MOVED
#include "curses.h"
// *NOTE*: the olc PixelGameEngine complains about MOUSE_MOVED and OK
#undef MOUSE_MOVED
#define MOUSE_MOVED 0x0001 // see: wincontypes.h:103
#undef OK
#else
#include "ncurses.h"
// *NOTE*: the ncurses "scroll" macro conflicts with Qt. Since not currently
//         used, it's safe to undefine
#undef scroll
// *NOTE*: the ncurses "OK" macro conflicts with olc::PixelGameEngine. Since not
//         currently used, it's safe to undefine
#undef OK
// *NOTE*: the ncurses "timeout" macro conflicts with
//         ACE_Synch_Options::timeout. Since not currently used, it's safe to
//         undefine
#undef timeout
#endif // ACE_WIN32 || ACE_WIN32
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
//#define QT_NO_SIGNALS_SLOTS_KEYWORDS
#if defined (X11_SUPPORT)
// *NOTE*: some X11 macros conflict with Qt. Since not currently used, they're
//         safe to undefine
#undef Bool
#undef CursorShape
#undef Expose
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef KeyPress
#undef KeyRelease
#undef None
#undef Status
#endif // X11_SUPPORT
#include "qwindow.h"
#undef signals
#if defined (X11_SUPPORT)
#define Bool int
#define Status int
#endif // X11_SUPPORT
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
#include "wx/window.h"
#endif // WXWIDGETS_SUPPORT

#include "ace/Assert.h"

struct Common_UI_Window
{
  enum Type
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    TYPE_WIN32 = 0,
#else
    TYPE_WAYLAND = 0,
    TYPE_X11,
#endif // ACE_WIN32 || ACE_WIN64
    TYPE_CURSES,
    TYPE_GTK,
    TYPE_QT,
    TYPE_WXWIDGETS,
    //////////////////////////////////////
    TYPE_MAX,
    TYPE_INVALID,
  };

  Common_UI_Window () : type (TYPE_INVALID), window (NULL) {}
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_UI_Window (HWND window_in) : type (TYPE_WIN32), win32_hwnd (window_in) {}

  operator HWND () const { ACE_ASSERT (type == TYPE_WIN32); return win32_hwnd; }
#else
#if defined (X11_SUPPORT)
  Common_UI_Window (Window window_in) : type (TYPE_X11), x11_window (window_in) {}

  operator Window () const { ACE_ASSERT (type == TYPE_X11); return x11_window; }
#endif // WAYLAND_SUPPORT

#if defined (WAYLAND_SUPPORT)
  Common_UI_Window (wl_surface* window_in) : type (TYPE_WAYLAND), wayland_window (window_in) {}

  operator wl_surface* () const { ACE_ASSERT (type == TYPE_WAYLAND); return wayland_window; }
#endif // WAYLAND_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#if defined (CURSES_SUPPORT)
  Common_UI_Window (WINDOW* window_in) : type (TYPE_CURSES), curses_window (window_in) {}

  operator WINDOW* () const { ACE_ASSERT (type == TYPE_CURSES); return curses_window; }
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  Common_UI_Window (GdkSurface* window_in) : type (TYPE_GTK), gdk_surface (window_in) {}

  operator GdkSurface* () const { ACE_ASSERT (type == TYPE_GTK); return gdk_surface; }
#else
  Common_UI_Window (GdkWindow* window_in) : type (TYPE_GTK), gdk_window (window_in) {}

  operator GdkWindow* () const { ACE_ASSERT (type == TYPE_GTK); return gdk_window; }
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
  Common_UI_Window (QWindow* window_in) : type (TYPE_QT), qt_window (window_in) {}

  operator QWindow* () const { ACE_ASSERT (type == TYPE_QT); return qt_window; }
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
  Common_UI_Window (wxWindow* window_in) : type (TYPE_WXWIDGETS), wxwidgets_window (window_in) {}

  operator wxWindow* () const { ACE_ASSERT (type == TYPE_WXWIDGETS); return wxwidgets_window; }
#endif // WXWIDGETS_SUPPORT

  enum Common_UI_Window::Type type;

  union
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND win32_hwnd;
#else
#if defined (X11_SUPPORT)
    Window x11_window;
#endif // X11_SUPPORT

#if defined (WAYLAND_SUPPORT)
    struct wl_surface* wayland_window;
#endif // WAYLAND_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#if defined (CURSES_SUPPORT)
    WINDOW* curses_window;
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
    GdkSurface* gdk_surface;
#else
    GdkWindow* gdk_window;
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
    QWindow* qt_window;
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
    wxWindow* wxwidgets_window;
#endif // WXWIDGETS_SUPPORT

    void* window;
  };
};

#endif
