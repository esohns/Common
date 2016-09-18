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

#include "ace/Log_Msg.h"

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
Common_UI_Tools::info ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::info"));

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

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (information_string.c_str ())));
}
