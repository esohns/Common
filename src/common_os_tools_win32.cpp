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

#include "common_os_tools.h"

#include <sstream>

#include "ks.h"
#include "Security.h"
#define INITGUID
#include "strmif.h"
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0602) // _WIN32_WINNT_WIN8
#include "processthreadsapi.h"
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)

#include "ace/String_Base.h"
#include "ace/Configuration.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_macros.h"

#include "common_error_tools.h"

#include "common_time_common.h"

//////////////////////////////////////////

BOOL CALLBACK
common_locale_cb_function (LPWSTR name_in,
                           DWORD flags_in,
                           LPARAM parameter_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_locale_cb_function"));

  // sanity check(s)
  ACE_ASSERT (parameter_in);

  std::vector<std::string>* locales_p =
    reinterpret_cast<std::vector<std::string>*> (parameter_in);

  // sanity check(s)
  ACE_ASSERT (locales_p);

  locales_p->push_back (ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (name_in)));

  return TRUE;
}

//////////////////////////////////////////

void
Common_OS_Tools::printLocales ()
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::printLocales"));

  std::vector<std::string> locales;
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0600) // _WIN32_WINNT_VISTA
  if (unlikely (!EnumSystemLocalesEx (common_locale_cb_function,
                                      LOCALE_ALL,
                                      reinterpret_cast<LPARAM> (&locales),
                                      NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumSystemLocalesEx(): \"%s\", returning\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return;
  } // end IF
#else
  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#endif // COMMON_OS_WIN32_TARGET_PLATFORM (0x0600)

  int index_i = 1;
  for (std::vector<std::string>::const_iterator iterator = locales.begin ();
       iterator != locales.end ();
       ++iterator, ++index_i)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%d: \"%s\"\n"),
                index_i,
                ACE_TEXT ((*iterator).c_str ())));
}

std::string
Common_OS_Tools::environment (const std::string& variable_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::environment"));

  std::string result;

  // sanity check(s)
  ACE_ASSERT (!variable_in.empty ());

  char* string_p = ACE_OS::getenv (variable_in.c_str ());
  if (string_p)
    result.assign (string_p);
  else
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("\"%s\" environment variable not set, returning\n"),
                ACE_TEXT (variable_in.c_str ())));

  return result;
}

bool
Common_OS_Tools::setResourceLimits (bool fileDescriptors_in,
                                    bool stackTraces_in,
                                    bool pendingSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::setResourceLimits"));

  if (fileDescriptors_in)
  {
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("file descriptor limits are not available on this platform, continuing\n")));
  } // end IF

// -----------------------------------------------------------------------------

  if (stackTraces_in)
  {
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("core file limits/stack trace dumps are not available on this platform, continuing\n")));
  } // end IF

  if (pendingSignals_in)
  {
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("pending signal limits are not available on this platform, continuing\n")));
  } // end IF

  return true;
}

void
Common_OS_Tools::getUserName (std::string& username_out,
                              std::string& realname_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::getUserName"));

  // initialize return value(s)
  username_out.clear ();
  realname_out.clear ();

  char user_name[ACE_MAX_USERID];
  ACE_OS::memset (user_name, 0, sizeof (user_name));
  if (unlikely (!ACE_OS::cuserid (user_name,
                                  ACE_MAX_USERID)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ACE_OS::cuserid(): \"%m\", falling back\n")));

    username_out =
      ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_ENVIRONMENT_USER_LOGIN_BASE)));

    return;
  } // end IF
  username_out = user_name;

  ACE_TCHAR buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
  DWORD buffer_size = sizeof (ACE_TCHAR[BUFSIZ]);
  if (unlikely (!ACE_TEXT_GetUserNameEx (NameDisplay, // EXTENDED_NAME_FORMAT
                                         buffer_a,
                                         &buffer_size)))
  {
    DWORD error = GetLastError ();
    if (error != ERROR_NONE_MAPPED)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to %s(): \"%m\", continuing\n"),
                  ACE_TEXT (ACE_TEXT_GetUserNameEx)));
  } // end IF
  else
    realname_out = ACE_TEXT_ALWAYS_CHAR (buffer_a);
}

std::string
Common_OS_Tools::GUIDToString (REFGUID GUID_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::GUIDToString"));

  std::string result;

  OLECHAR GUID_string[CHARS_IN_GUID];
  ACE_OS::memset (GUID_string, 0, sizeof (GUID_string));
  int result_2 = StringFromGUID2 (GUID_in,
                                  GUID_string, CHARS_IN_GUID);
  ACE_ASSERT (result_2 == CHARS_IN_GUID);

#if defined (OLE2ANSI)
  result = GUID_string;
#else
  result = ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (GUID_string));
#endif // OLE2ANSI

  return result;
}

struct _GUID
Common_OS_Tools::StringToGUID (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::StringToGUID"));

  struct _GUID result = GUID_NULL;

  HRESULT result_2 = E_FAIL;
#if defined (OLE2ANSI)
  result_2 = CLSIDFromString (string_in.c_str (),
                              &result);
#else
  result_2 =
    CLSIDFromString (ACE_TEXT_ALWAYS_WCHAR (string_in.c_str ()),
                     &result);
#endif // OLE2ANSI
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CLSIDFromString(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result_2).c_str ())));
    return GUID_NULL;
  } // end IF

  return result;
}

bool
Common_OS_Tools::isGUID (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::isGUID"));

  struct _GUID GUID_s = GUID_NULL;

  HRESULT result =
#if defined (OLE2ANSI)
    CLSIDFromString (string_in.c_str (),
                     &GUID_s);
#else
    CLSIDFromString (ACE_TEXT_ALWAYS_WCHAR (string_in.c_str ()),
                     &GUID_s);
#endif // OLE2ANSI

  return !FAILED (result);
}

std::string
Common_OS_Tools::getKeyValue (HKEY parentKey_in,
                              const std::string& subkey_in,
                              const std::string& value_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::getKeyValue"));

  // initialize return value(s)
  std::string return_value;

  HKEY key_p =
    ACE_Configuration_Win32Registry::resolve_key (parentKey_in,
                                                  ACE_TEXT_CHAR_TO_TCHAR (subkey_in.c_str ()),
                                                  0); // do not create
  if (unlikely (!key_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::resolve_key(%@,\"%s\"): \"%m\", aborting\n"),
                parentKey_in,
                ACE_TEXT (subkey_in.c_str ())));
    return return_value;
  } // end IF
  ACE_Configuration_Win32Registry registry (key_p);
  ACE_TString value_string;
  int result =
      registry.get_string_value (registry.root_section (),
                                 ACE_TEXT_CHAR_TO_TCHAR (value_in.c_str ()),
                                 value_string);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::get_string_value(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_in.c_str ())));
    return return_value;
  } // end IF
  return_value = value_string.c_str ();

  return return_value;
}

bool
Common_OS_Tools::deleteKey (HKEY parentKey_in,
                            const std::string& subkey_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::deleteKey"));

  HKEY key_p =
    ACE_Configuration_Win32Registry::resolve_key (parentKey_in,
                                                  ACE_TEXT_CHAR_TO_TCHAR (subkey_in.c_str ()),
                                                  0); // do not create
  if (unlikely (!key_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::resolve_key(%@,\"%s\"): \"%m\", aborting\n"),
                parentKey_in,
                ACE_TEXT (subkey_in.c_str ())));
    return false;
  } // end IF
  ACE_Configuration_Win32Registry registry (key_p);
  int result = registry.remove_section (registry.root_section (),
                                        ACE_TEXT_CHAR_TO_TCHAR (""),
                                        true); // recursive
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::remove_section(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (subkey_in.c_str ())));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("removed registry key \"%s\"\n"),
              ACE_TEXT (subkey_in.c_str ())));

  return true;
}

bool
Common_OS_Tools::deleteKeyValue (HKEY parentKey_in,
                                const std::string& subkey_in,
                                const std::string& value_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::deleteKeyValue"));

  HKEY key_p =
    ACE_Configuration_Win32Registry::resolve_key (parentKey_in,
                                                  ACE_TEXT_CHAR_TO_TCHAR (subkey_in.c_str ()),
                                                  0); // do not create
  if (unlikely (!key_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::resolve_key(%@,\"%s\"): \"%m\", aborting\n"),
                parentKey_in,
                ACE_TEXT (subkey_in.c_str ())));
    return false;
  } // end IF
  ACE_Configuration_Win32Registry registry (key_p);
  int result = registry.remove_value (registry.root_section (),
                                      ACE_TEXT_CHAR_TO_TCHAR (value_in.c_str ()));
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Win32Registry::remove_value(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_in.c_str ())));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("removed value \"%s\"\n"),
              ACE_TEXT (value_in.c_str ())));

  return true;
}

bool
Common_OS_Tools::isInstalled (const std::string& executableName_in,
                              std::string& executablePath_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::isInstalled"));

  // initialize return value(s)
  executablePath_out.clear ();
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!executableName_in.empty ());

  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)

  return result;
}
