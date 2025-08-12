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

#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename NativeWindowType>
NativeWindowType
Common_UI_WindowTypeConverter_T<NativeWindowType>::convert (const struct Common_UI_Window& window_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WindowTypeConverter_T::convert"));

  NativeWindowType result = 0;

  switch (window_in.type)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    case Common_UI_Window::TYPE_WIN32:
    {
      getWindowType (window_in.win32_hwnd, result);
      break;
    }
#else
    case Common_UI_Window::TYPE_X11:
    {
      getWindowType (window_in.x11_window, result);
      break;
    }
#endif
#if defined (CURSES_SUPPORT)
    case Common_UI_Window::TYPE_CURSES:
    {
      getWindowType (window_in.curses_window, result);
      break;
    }
#endif // CURSES_SUPPORT
#if defined (GTK_SUPPORT)
    case Common_UI_Window::TYPE_GTK:
    {
#if GTK_CHECK_VERSION (4,0,0)
      getWindowType (window_in.gdk_surface, result);
#else
      getWindowType (window_in.gdk_window, result);
#endif // GTK_CHECK_VERSION (4,0,0)
      break;
    }
#endif // GTK_SUPPORT
#if defined (QT_SUPPORT)
    case Common_UI_Window::TYPE_QT:
    {
      getWindowType (window_in.qt_window, result);
      break;
    }
#endif // QT_SUPPORT
#if defined (WXWIDGETS_SUPPORT)
    case Common_UI_Window::TYPE_WXWIDGETS:
    {
      getWindowType (window_in.wxwidgets_window, result);
      break;
    }
#endif // WXWIDGETS_SUPPORT
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown window type (was: %d), aborting\n"),
                  window_in.type));
      break;
    }
  } // end SWITCH

  return result;
}
