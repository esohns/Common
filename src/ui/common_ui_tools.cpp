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
#include "stdafx.h"

#include "common_ui_tools.h"

#include <sstream>

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#include <gtkgl/gdkgl.h>
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
#include <gtkgl/gdkgl.h>
#else
#include <gtk/gtkgl.h> // gtkglext
#endif
#endif
#endif

#include <ace/Log_Msg.h>

#include "common_macros.h"

std::string
Common_UI_Tools::UTF82Locale (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::UTF82Locale"));

  return Common_UI_Tools::UTF82Locale (string_in.c_str (),
                                       -1);
}

std::string
Common_UI_Tools::UTF82Locale (const gchar* string_in,
                              const gssize& length_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::UTF82Locale"));

  // initialize result
  std::string result;

  GError* error_p = NULL;
  gchar* string_p = g_locale_from_utf8 (string_in, // text
                                        length_in, // number of bytes
                                        NULL,      // bytes read (don't care)
                                        NULL,      // bytes written (don't care)
                                        &error_p); // return value: error
  if (error_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_locale_from_utf8(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in),
                ACE_TEXT (error_p->message)));

    // clean up
    g_error_free (error_p);
  } // end IF
  else
    result = string_p;

  // clean up
  g_free (string_p);

  return result;
}

gchar*
Common_UI_Tools::Locale2UTF8 (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::Locale2UTF8"));

  // initialize result
  gchar* result_p = NULL;

  GError* error_p = NULL;
  result_p = g_locale_to_utf8 (string_in.c_str (), // text
                               -1,                 // \0-terminated
                               NULL,               // bytes read (don't care)
                               NULL,               // bytes written (don't care)
                               &error_p);          // return value: error
  if (error_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_locale_to_utf8(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in.c_str ()),
                ACE_TEXT (error_p->message)));

      // clean up
    g_error_free (error_p);

    return NULL;
  } // end IF

  return result_p;
}

void
Common_UI_Tools::GtkInfo ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::GtkInfo"));

  std::ostringstream converter;
  std::string information_string =
      ACE_TEXT_ALWAYS_CHAR ("Gtk library version: ");
  converter << GTK_MAJOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_MINOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_MICRO_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (" [age (binary/interface): ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_BINARY_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (", ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_INTERFACE_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR ("]");

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  information_string +=
      ACE_TEXT_ALWAYS_CHAR ("\nGtk GL library version: ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MAJOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MINOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MICRO_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (" [age (binary/interface): ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_BINARY_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (", ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_INTERFACE_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR ("]");
#endif
#endif
#endif

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (information_string.c_str ())));
}

#if defined (GTKGL_SUPPORT)
void
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
Common_UI_Tools::OpenGLInfo (GdkGLContext* context_in)
#else
Common_UI_Tools::OpenGLInfo ()
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
Common_UI_Tools::OpenGLInfo ()
#else
Common_UI_Tools::OpenGLInfo (GdkGLContext* context_in)
#endif
#endif
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::OpenGLInfo"));

  std::ostringstream converter;
  std::string information_string;

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  gint result = ggla_query ();
  if (!result)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL not supported\n")));
    return;
  } // end IF

  gchar* info_string_p = ggla_get_info ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("OpenGL information: \"%s\"\n"),
              ACE_TEXT (info_string_p)));
#endif
#endif

  int version_major, version_minor;

  information_string = ACE_TEXT_ALWAYS_CHAR ("OpenGL version: ");
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  gdk_gl_context_get_version (context_in,
                              &version_major,
                              &version_minor);
#else
  information_string.clear ();
  goto continue_;
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  information_string.clear ();
  goto continue_;
#else
  if (!gdk_gl_query_extension ())
  {
    information_string += ACE_TEXT_ALWAYS_CHAR ("not supported");
    goto continue_;
  } // end IF

  if (!gdk_gl_query_version (&version_major, &version_minor))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_query_version(), returning\n")));
    return;
  } // end IF
#endif
#endif

  converter << version_major;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << version_minor;
  information_string += converter.str ();

continue_:
  if (!information_string.empty ())
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s\n"),
                ACE_TEXT (information_string.c_str ())));
}
#endif
