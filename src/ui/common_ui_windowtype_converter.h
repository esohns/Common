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

#if defined (CURSES_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN32)
#include "curses.h"
// *NOTE*: the olc PixelGameEngine complains about MOUSE_MOVED and OK
#define MOUSE_MOVED 0x0001
#undef OK
#else
#include "ncurses.h"
 // *NOTE*: the ncurses "timeout" macros conflict with
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
#include "gdk/gdkx.h"
#endif // ACE_WIN32 || ACE_WIN64
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
#include "qwindow.h"
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
#include "wx/window.h"
#endif // WXWIDGETS_SUPPORT

#include "ace/Global_Macros.h"

union Common_UI_Window
{
  // *TODO*: add more window types (e.g. Qt, etc.)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_UI_Window () : win32_hwnd (NULL) {}
  Common_UI_Window (HWND window_in) : win32_hwnd (window_in) {}

  operator HWND () const { return win32_hwnd; }
#else
  Common_UI_Window () : x11_window (0) {}
  Common_UI_Window (Window window_in) : x11_window (window_in) {}

  operator Window () const { return x11_window; }
#endif // ACE_WIN32 || ACE_WIN64

#if defined (CURSES_SUPPORT)
  Common_UI_Window (WINDOW* window_in) : curses_window (window_in) {}

  operator WINDOW* () const { return curses_window; }
#endif // CURSES_SUPPORT

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  Common_UI_Window (GdkSurface* window_in) : gdk_surface (window_in) {}

  operator GdkSurface* () const { return gdk_surface; }
#else
  Common_UI_Window (GdkWindow* window_in) : gdk_window (window_in) {}

  operator GdkWindow* () const { return gdk_window; }
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT

#if defined (QT_SUPPORT)
  Common_UI_Window (QWindow* window_in) : qt_window (window_in) {}

  operator QWindow* () const { return qt_window; }
#endif // QT_SUPPORT

#if defined (WXWIDGETS_SUPPORT)
  Common_UI_Window (wxWindow* window_in) : wxwidgets_window (window_in) {}

  operator wxWindow* () const { return wxwidgets_window; }
#endif // WXWIDGETS_SUPPORT

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HWND win32_hwnd;
#else
  Window x11_window;
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
};

template <typename WindowType>
class Common_UI_WindowTypeConverter_T
{
 public:
  inline virtual ~Common_UI_WindowTypeConverter_T () {}

 protected:
  inline Common_UI_WindowTypeConverter_T () {}

//  template <typename T> MediaType getMediaType (const T& mediaType_in) { MediaType result; getMediaType_impl (mediaType_in, result); return result; }
//  template <typename T, typename U> T getMediaType_2 (const U& mediaType_in) { T result; getMediaType_impl (mediaType_in, result); return result; }
//  template <typename T, typename U> T getMediaType_2 (const U&) { T result; ACE_ASSERT (false); ACE_NOTSUP_RETURN (result); ACE_NOTREACHED (return result;) }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inline void getWindowType (const HWND windowType_in, HWND& windowType_out) { windowType_out = windowType_in; }
#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  inline void getWindowType (const GdkSurface* windowType_in, HWND& windowType_out) { ACE_ASSERT (gdk_win32_surface_is_win32 (const_cast<GdkSurface*> (windowType_in))); windowType_out = gdk_win32_surface_get_impl_hwnd (const_cast<GdkSurface*> (windowType_in)); }
#else
  // *TODO*: consider static_cast<HWND> (GDK_WINDOW_HWND (window_p));
  inline void getWindowType (const GdkWindow* windowType_in, HWND& windowType_out) { ACE_ASSERT (gdk_win32_window_is_win32 (const_cast<GdkWindow*> (windowType_in))); windowType_out = gdk_win32_window_get_impl_hwnd (const_cast<GdkWindow*> (windowType_in)); }
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTK_SUPPORT
#else
  inline void getWindowType (const Window windowType_in, Window& windowType_out) { windowType_out = windowType_in; }
#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
  inline void getWindowType (const GdkWindow* windowType_in, Window& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = gdk_x11_window_get_xid (const_cast<GdkWindow*> (windowType_in)); }
#elif (GTK_CHECK_VERSION (2,0,0))
  inline void getWindowType (const GdkWindow* windowType_in, Window& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = gdk_x11_drawable_get_xid (GDK_DRAWABLE (const_cast<GdkWindow*> (windowType_in))); }
#endif // GTK_CHECK_VERSION
#endif // GTK_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GTK_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  inline void getWindowType (const GdkSurface* windowType_in, GdkSurface*& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = const_cast<GdkSurface*> (windowType_in); }
#else
  inline void getWindowType (const GdkWindow* windowType_in, GdkWindow*& windowType_out) { ACE_ASSERT (windowType_in); /*g_object_ref (windowType_in);*/ windowType_out = const_cast<GdkWindow*> (windowType_in); }
#endif // GTK_CHECK_VERSION(4,0,0)
#endif // GTK_SUPPORT

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WindowTypeConverter_T (const Common_UI_WindowTypeConverter_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WindowTypeConverter_T& operator= (const Common_UI_WindowTypeConverter_T&))
};

// include template definition
#include "common_ui_windowtype_converter.inl"

#endif
