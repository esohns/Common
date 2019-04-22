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
#include <locale>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <OleAuto.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "common_string_tools.h"

#include "ace/Log_Msg.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

#include "common_error_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
std::string
Common_String_Tools::to_2 (const BSTR string_in,
                           UINT codePage_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::to_2"));

  // initialize return value(s)
  std::string return_value;

  // request content length in single-chars through a terminating
  //  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
  //  so this will only convert through the first nullchar.
  int result = WideCharToMultiByte (codePage_in, // CodePage
                                    0,           // dwFlags
                                    string_in,   // lpWideCharStr
                                    -1,          // cchWideChar
                                    NULL,        // lpMultiByteStr
                                    0,           // cbMultiByte
                                    NULL,        // lpDefaultChar
                                    NULL);       // lpUsedDefaultChar
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WideCharToMultiByte(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    return return_value;
  } // end IF
  return_value.resize (result);
  result = WideCharToMultiByte (codePage_in,
                                0,
                                string_in,
                                -1,
                                &return_value[0],
                                result,
                                NULL,
                                NULL);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WideCharToMultiByte(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    return return_value;
  } // end IF

  return return_value;
}

BSTR
Common_String_Tools::to_2 (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::to_2"));

  // initialize return value(s)
  BSTR return_value = NULL;

  DWORD dwFlags = (MB_PRECOMPOSED       |
                   MB_ERR_INVALID_CHARS |
                   MB_USEGLYPHCHARS);
  // sanity check(s)
  // *NOTE*: see also: https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-multibytetowidechar
  UINT code_page_i = GetACP ();
  // *TODO*: ???
  if ((code_page_i == 50220) || (code_page_i == 50221) || (code_page_i == 50222) ||
      (code_page_i == 50225) || (code_page_i == 50227) || (code_page_i == 50229) ||
      ((code_page_i == 57002) || (code_page_i == 57003) || (code_page_i == 57004) ||
       (code_page_i == 57005) || (code_page_i == 57006) || (code_page_i == 57007) ||
       (code_page_i == 57008) || (code_page_i == 57009) || (code_page_i == 57010) ||
       (code_page_i == 57011)) ||
       (code_page_i == 65000) ||
       (code_page_i == 42))
    dwFlags = 0;
  if ((code_page_i == 65001)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      || (code_page_i == GB18030))
#else
      || (code_page_i == 54936))
#endif // _WIN32_WINNT_VISTA
    dwFlags = MB_ERR_INVALID_CHARS;
  int result = MultiByteToWideChar (CP_ACP,
                                    dwFlags,
                                    string_in.c_str (),
                                    -1,
                                    NULL,
                                    0);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::MultiByteToWideChar(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    return return_value;
  } // end IF
  return_value = SysAllocStringByteLen (NULL,
                                        result);
  if (!return_value)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%s\", aborting\n")));
    return return_value;
  } // end IF
  result = MultiByteToWideChar (CP_ACP,
                                dwFlags,
                                string_in.c_str (),
                                -1,
                                return_value,
                                result);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::MultiByteToWideChar(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    SysFreeString (return_value); return_value = NULL;
    return return_value;
  } // end IF

  return return_value;
}

LPWSTR
Common_String_Tools::to (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::to"));

  // initialize return value(s)
  LPWSTR return_value = NULL;

  DWORD dwFlags = (MB_PRECOMPOSED       |
                   MB_ERR_INVALID_CHARS |
                   MB_USEGLYPHCHARS);
  // sanity check(s)
  // *NOTE*: see also: https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-multibytetowidechar
  UINT code_page_i = GetACP ();
  // *TODO*: ???
  if ((code_page_i == 50220) || (code_page_i == 50221) || (code_page_i == 50222) ||
      (code_page_i == 50225) || (code_page_i == 50227) || (code_page_i == 50229) ||
      ((code_page_i == 57002) || (code_page_i == 57003) || (code_page_i == 57004) ||
       (code_page_i == 57005) || (code_page_i == 57006) || (code_page_i == 57007) ||
       (code_page_i == 57008) || (code_page_i == 57009) || (code_page_i == 57010) ||
       (code_page_i == 57011)) ||
       (code_page_i == 65000) ||
       (code_page_i == 42))
    dwFlags = 0;
  if ((code_page_i == 65001)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      || (code_page_i == GB18030))
#else
      || (code_page_i == 54936))
#endif // _WIN32_WINNT_VISTA
    dwFlags = MB_ERR_INVALID_CHARS;
  int result = MultiByteToWideChar (CP_ACP,
                                    dwFlags,
                                    string_in.c_str (),
                                    -1,
                                    NULL,
                                    0);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::MultiByteToWideChar(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    return return_value;
  } // end IF
  ACE_NEW_NORETURN (return_value,
                    WCHAR[result]);
  if (!return_value)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%s\", aborting\n")));
    return return_value;
  } // end IF
  result = MultiByteToWideChar (CP_ACP,
                                dwFlags,
                                string_in.c_str (),
                                -1,
                                return_value,
                                result);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::MultiByteToWideChar(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
    delete [] (return_value); return_value = NULL;
    return return_value;
  } // end IF

  return return_value;
}
#endif // ACE_WIN32 || ACE_WIN64

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

std::string
Common_String_Tools::toupper (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_String_Tools::toupper"));

  // initialize return value
  std::string return_value;

  std::locale locale;
  for (std::string::size_type i = 0;
       i < string_in.length ();
       ++i)
    return_value += std::toupper (string_in[i], locale);

  return return_value;
}
