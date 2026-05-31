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

#ifndef COMMON_UI_WINDOWTYPE_CONVERTER_H
#define COMMON_UI_WINDOWTYPE_CONVERTER_H

#if defined (X11_SUPPORT)
typedef unsigned long XID;
typedef XID Window;
// #include "X11/X.h"
// #include "X11/Xlib.h"
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if GTK_CHECK_VERSION (4,0,0)
#include "gdk/win32/gdkwin32.h"
#else
#include "gdk/gdkwin32.h"
#endif // GTK_CHECK_VERSION (4,0,0)
#else
#if defined (X11_SUPPORT)
#include "gdk/gdkx.h"
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
#endif // ACE_WIN32 || ACE_WIN64
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
//#define QT_NO_SIGNALS_SLOTS_KEYWORDS
#include "qwindow.h"
#undef signals
#endif // QT_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN32)
#else
#define Bool int
#define Status int
#endif // ACE_WIN32 || ACE_WIN32

#if defined (WXWIDGETS_SUPPORT)
#include "wx/window.h"
#endif // WXWIDGETS_SUPPORT

#include "ace/Global_Macros.h"

#include "common_ui_window.h"

template <typename NativeWindowType>
class Common_UI_WindowTypeConverter_T
{
 public:
  inline virtual ~Common_UI_WindowTypeConverter_T () {}

 protected:
  inline Common_UI_WindowTypeConverter_T () {}

  NativeWindowType convert (const struct Common_UI_Window&);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inline void getWindowType (const struct Common_UI_Window& windowType_in, HWND& windowType_out) { windowType_out = windowType_in; }

  inline void getWindowType (const HWND windowType_in, HWND& windowType_out) { windowType_out = windowType_in; }
#if defined (CURSES_SUPPORT)
  inline void getWindowType (const HWND windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  inline void getWindowType (const GdkSurface* windowType_in, HWND& windowType_out) { ACE_ASSERT (gdk_win32_surface_is_win32 (const_cast<GdkSurface*> (windowType_in))); windowType_out = gdk_win32_surface_get_impl_hwnd (const_cast<GdkSurface*> (windowType_in)); }
#else
  inline void getWindowType (const GdkWindow* windowType_in, HWND& windowType_out) { ACE_ASSERT (gdk_win32_window_is_win32 (const_cast<GdkWindow*> (windowType_in))); windowType_out = gdk_win32_window_get_impl_hwnd (const_cast<GdkWindow*> (windowType_in)); }
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT
#else
#if defined (X11_SUPPORT)
  inline void getWindowType (const struct Common_UI_Window& windowType_in, Window& windowType_out) { windowType_out = windowType_in; }

  inline void getWindowType (const Window windowType_in, Window& windowType_out) { windowType_out = windowType_in; }
#endif // X11_SUPPORT

#if defined (CURSES_SUPPORT)
#if defined (X11_SUPPORT)
  inline void getWindowType (const Window windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#endif // X11_SUPPORT
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if defined (X11_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
  inline void getWindowType (const GdkWindow* windowType_in, Window& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = gdk_x11_window_get_xid (const_cast<GdkWindow*> (windowType_in)); }
#elif (GTK_CHECK_VERSION (2,0,0))
  inline void getWindowType (const GdkWindow* windowType_in, Window& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = gdk_x11_drawable_get_xid (GDK_DRAWABLE (const_cast<GdkWindow*> (windowType_in))); }
#endif // GTK_CHECK_VERSION
#endif // X11_SUPPORT
#endif // GTK_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#if defined (CURSES_SUPPORT)
#if defined (X11_SUPPORT)
  inline void getWindowType (const WINDOW* windowType_in, Window& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = 0; }
#endif // X11_SUPPORT

  inline void getWindowType (const WINDOW* windowType_in, WINDOW*& windowType_out) { windowType_out = const_cast<WINDOW*> (windowType_in); }

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  inline void getWindowType (const GdkSurface* windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#else
  inline void getWindowType (const GdkWindow* windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
  inline void getWindowType (const QWindow* windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
  inline void getWindowType (const wxWindow* windowType_in, WINDOW*& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = NULL; }
#endif // WXWIDGETS_SUPPORT
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  inline void getWindowType (const struct Common_UI_Window& windowType_in, GdkSurface*& windowType_out) { windowType_out = windowType_in; }

  inline void getWindowType (const GdkSurface* windowType_in, GdkSurface*& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = const_cast<GdkSurface*> (windowType_in); }
#else
  inline void getWindowType (const struct Common_UI_Window& windowType_in, GdkWindow*& windowType_out) { windowType_out = windowType_in; }

  inline void getWindowType (const GdkWindow* windowType_in, GdkWindow*& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = const_cast<GdkWindow*> (windowType_in); }
#endif // GTK_CHECK_VERSION(4,0,0)
#endif // GTK_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
#if defined (X11_SUPPORT)
  inline void getWindowType (const wxWindow* windowType_in, Window& windowType_out) { ACE_ASSERT (false); ACE_NOTSUP; windowType_out = 0; }
#endif // X11_SUPPORT
#endif // WXWIDGETS_SUPPORT

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WindowTypeConverter_T (const Common_UI_WindowTypeConverter_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WindowTypeConverter_T& operator= (const Common_UI_WindowTypeConverter_T&))
};

// include template definition
#include "common_ui_windowtype_converter.inl"

#endif
