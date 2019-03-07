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

#ifndef COMMON_UI_GTK_TOOLS_H
#define COMMON_UI_GTK_TOOLS_H

#include <limits>
#include <string>

#include "gtk/gtk.h"
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#else
#include "gtk/gtkgl.h" // gtkglext
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // GTKGL_SUPPORT

#include "ace/Global_Macros.h"
#include "ace/OS.h"

#include "common_ui_common.h"

class Common_UI_GTK_Tools
// : public Common_SInitializeFinalize_T<Common_UI_Tools>
{
 public:
  static bool initialize (int,           // argc
                          ACE_TCHAR*[]); // argv
  static bool finalize ();

  static std::string UTF8ToLocale (const gchar*, // string
                                   gssize = -1); // length in byte(s) (-1: '\0'-terminated)
  inline static std::string UTF8ToLocale (const std::string& string_in) { return Common_UI_GTK_Tools::UTF8ToLocale (string_in.c_str (), -1); }
  // *IMPORTANT NOTE*: return value needs to be g_free()'d !
  static gchar* localeToUTF8 (const std::string&); // string

  struct TreeModel_IndexSearch_CBData
  {
    TreeModel_IndexSearch_CBData ()
     : column (0)
     , found (false)
     , index (std::numeric_limits<guint>::max ())
     , value ()
    {
      ACE_OS::memset (&value, 0, sizeof (struct _GValue));
    }

    gint   column;
    bool   found;
    guint  index;
    GValue value;
  };
  // *IMPORTANT NOTE*: convert G_TYPE_STRING values to UTF8 (second argument)
  static guint valueToIndex (GtkTreeModel*,
                             const GValue&,
                             gint = 0);     // column

  static bool getDisplayDevices (Common_UI_DisplayDevices_t&); // return value: connected devices

#if defined (_DEBUG)
  // *NOTE*: recurses into any children
  static void dump (GtkContainer*); // container handle

  // print Gtk library information
  static void dumpGtkLibraryInfo ();
#if defined (GTKGL_SUPPORT)
  // print OpenGL library information
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  // *NOTE*: the context has to be realized before calling this function
  static void dumpGtkOpenGLInfo (GdkGLContext*); // context handle
#else
#if defined (GTKGLAREA_SUPPORT)
  static void dumpGtkOpenGLInfo (GglaContext*); // OpenGL context handle
#else
  static void dumpGtkOpenGLInfo (GdkWindow*); // GtkGLArea window handle
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
  static void dumpGtkOpenGLInfo (GglaContext*); // OpenGL context handle
#else
  static void dumpGtkOpenGLInfo ();
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // GTKGL_SUPPORT
#endif // _DEBUG

  static bool GTKInitialized;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_GTK_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Tools (const Common_UI_GTK_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Tools& operator= (const Common_UI_GTK_Tools&))
};

#endif
