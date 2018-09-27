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

#include <algorithm>

#include "common_string_tools.h"

#include "ace/Log_Msg.h"

std::string
Common_String_Tools::sanitizeURI (const std::string& uri_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::sanitizeURI"));

  std::string result = uri_in;

  std::replace (result.begin (),
                result.end (),
                '\\', '/');
  size_t position;
  do
  {
    position = result.find (' ', 0);
    if (position == std::string::npos)
      break;

    result.replace (position, 1, ACE_TEXT_ALWAYS_CHAR ("%20"));
  } while (true);
  //XMLCh* transcoded_string =
  //	XMLString::transcode (result.c_str (),
  //	                      XMLPlatformUtils::fgMemoryManager);
  //XMLURL url;
  //if (!XMLURL::parse (transcoded_string,
  //	                  url))
  // {
  //   ACE_DEBUG ((LM_ERROR,
  //               ACE_TEXT ("failed to XMLURL::parse(\"%s\"), aborting\n"),
  //               ACE_TEXT (result.c_str ())));

  //   return result;
  // } // end IF
  //XMLUri uri (transcoded_string,
  //	          XMLPlatformUtils::fgMemoryManager);
  //XMLString::release (&transcoded_string,
  //	                  XMLPlatformUtils::fgMemoryManager);
  //char* translated_string =
  //	XMLString::transcode (uri.getUriText(),
  //	                      XMLPlatformUtils::fgMemoryManager);
  //result = translated_string;
  //XMLString::release (&translated_string,
  //	                  XMLPlatformUtils::fgMemoryManager);

  return result;
}

std::string
Common_String_Tools::sanitize (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::sanitize"));

  std::string result = string_in;

  std::replace (result.begin (),
                result.end (),
                ' ', '_');

  return result;
}

std::string
Common_String_Tools::strip (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::strip"));

  std::string result = string_in;

  std::string::size_type position = std::string::npos;
  position =
    result.find_first_not_of (ACE_TEXT_ALWAYS_CHAR (" \t\f\v\n\r"),
                              0);
  if (unlikely (position == std::string::npos))
    result.clear (); // all whitespace
  else if (position)
    result.erase (0, position);
  position =
    result.find_last_not_of (ACE_TEXT_ALWAYS_CHAR (" \t\f\v\n\r"),
                             std::string::npos);
  if (unlikely (position == std::string::npos))
    result.clear (); // all whitespace
  else
    result.erase (position + 1, std::string::npos);

  return result;
}

bool
Common_String_Tools::isspace (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::isspace"));

  for (unsigned int i = 0;
       i < string_in.size ();
       ++i)
    if (!::isspace (static_cast<int> (string_in[i])))
      return false;

  return true;
}
