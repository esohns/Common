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

#ifndef COMMON_UI_TOOLS_H
#define COMMON_UI_TOOLS_H

#include <string>

#include "gtk/gtk.h"
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
//#if GTK_CHECK_VERSION (3,16,0)
//#else
//#include "gtkgl/gdkgl.h"
//#endif
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#else
#include "gtk/gtkgl.h" // gtkglext
#endif
#endif
#endif

#include "ace/Global_Macros.h"

//#include "common_ui_exports.h"

class Common_UI_Tools
{
 public:
  inline virtual ~Common_UI_Tools () {};

  static std::string UTF82Locale (const std::string&); // string
  static std::string UTF82Locale (const gchar*,        // string
                                  const gssize& = -1); // length in bytes (-1: \0-terminated)
  // *IMPORTANT NOTE*: return value needs to be g_free()'d !
  static gchar* Locale2UTF8 (const std::string&); // string

  static void dump (GtkWidget*); // widget handle

  // print Gtk library information
  static void GtkInfo ();
#if defined (GTKGL_SUPPORT)
  // print OpenGL library information
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  static void OpenGLInfo (GdkGLContext*); // OpenGL context handle
#else
  static void OpenGLInfo ();
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  static void OpenGLInfo ();
#else
  static void OpenGLInfo (GdkGLContext*); // OpenGL context handle
#endif
#endif
#endif

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools ());
  //ACE_UNIMPLEMENTED_FUNC (~Common_UI_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools (const Common_UI_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools& operator= (const Common_UI_Tools&));
};

#endif
