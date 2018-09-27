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

#include "ace/Synch.h"
#include "common_ui_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <list>
#else
#include <regex>
#include <sstream>
#endif // ACE_WIN32 || ACE_WIN64
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <WinUser.h>
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
#include <physicalmonitorenumerationapi.h>
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"
#include "common_tools.h"

#include "common_error_tools.h"

#include "common_ui_common.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
BOOL CALLBACK
common_ui_monitor_enum_cb (HMONITOR monitor_in,
                           HDC      deviceContext_in,
                           LPRECT   clippingArea_in,
                           LPARAM   userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_ui_monitor_enum_cb"));

  BOOL result = FALSE;

  // sanity check(s)
  ACE_ASSERT (monitor_in);
  ACE_UNUSED_ARG (deviceContext_in);
  ACE_ASSERT (clippingArea_in);
  ACE_ASSERT (userData_in);

  Common_UI_DisplayDevices_t* devices_p =
    reinterpret_cast<Common_UI_DisplayDevices_t*> (userData_in);

  MONITORINFOEX monitor_info;
  struct Common_UI_DisplayDevice device_s;
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  // *NOTE*: more than one physical monitor may be associated with a monitor
  //         handle. Note how this is a race condition
  DWORD number_of_monitors = 0;
  struct _PHYSICAL_MONITOR* physical_monitors_p = NULL;
  if (!GetNumberOfPhysicalMonitorsFromHMONITOR (monitor_in,
                                                &number_of_monitors))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetNumberOfPhysicalMonitorsFromHMONITOR(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return FALSE;
  } // end IF
  ACE_NEW_NORETURN (physical_monitors_p,
                    struct _PHYSICAL_MONITOR[number_of_monitors]);
  if (!physical_monitors_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return FALSE;
  } // end IF
  ACE_OS::memset (physical_monitors_p,
                  0,
                  sizeof (struct _PHYSICAL_MONITOR) * number_of_monitors);
  if (!GetPhysicalMonitorsFromHMONITOR (monitor_in,
                                        number_of_monitors,
                                        physical_monitors_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetPhysicalMonitorsFromHMONITOR(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    goto error;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  monitor_info.cbSize = sizeof (MONITORINFOEX);
  if (!GetMonitorInfo (monitor_in,
                       &monitor_info))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    goto error;
  } // end IF

  device_s.clippingArea = *clippingArea_in;
  device_s.description =
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
    ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (physical_monitors_p[0].szPhysicalMonitorDescription));
#else
#if defined (UNICODE)
    ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info.szDevice));
#else
    monitor_info.szDevice;
#endif // UNICODE
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  device_s.device =
#if defined (UNICODE)
    ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info.szDevice));
#else
    monitor_info.szDevice;
#endif // UNICODE
  device_s.handle = monitor_in;
  devices_p->push_back (device_s);

  result = TRUE;

error:
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  if (physical_monitors_p)
    delete [] physical_monitors_p;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  return result;
};
#endif // ACE_WIN32 || ACE_WIN64

Common_UI_DisplayDevices_t
Common_UI_Tools::getDisplayDevices ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplayDevices"));

  // initialize return value(s)
  Common_UI_DisplayDevices_t result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!EnumDisplayMonitors (NULL,                                // hdc
                            NULL,                                // lprcClip
                            common_ui_monitor_enum_cb,           // lpfnEnum
                            reinterpret_cast<LPARAM> (&result))) // dwData
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumDisplayMonitors(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return result;
  } // end IF
#else
  // *TODO*: this should work on most systems running Xorg X (and compatible
  //         derivates), but is in fact really a bad idea due to these
  //         dependencies:
  //         - 'xrandr' tool
  //         - temporary files
  //         - system(3) call
  //         --> very inefficient; replace ASAP

//  int result = -1;
  std::string display_records_string;
  std::istringstream converter;
  char buffer_a [BUFSIZ];
  std::string regex_string =
      ACE_TEXT_ALWAYS_CHAR ("^(.+) (?:connected)(?: primary)? (.+) \\((?:(.+)\\w*)+\\) ([[:digit:]]+)mm x ([[:digit:]]+)mm$");
  std::regex regex (regex_string);
  std::smatch match_results;
  std::string buffer_string;
  std::string command_line_string = ACE_TEXT_ALWAYS_CHAR ("xrandr");
  // *NOTE*: (qtcreator) gdb fails to debug this (hangs) unless you disable the
  //         "Debug all children" option
  int exit_status_i = 0;
  if (unlikely (!Common_Tools::command (command_line_string.c_str (),
                                        exit_status_i,
                                        display_records_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("xrandr data: \"%s\"\n"),
//              ACE_TEXT (display_record_string.c_str ())));

  struct Common_UI_DisplayDevice device_s;
  converter.str (display_records_string);
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    buffer_string = buffer_a;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());

#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display device \"%s\"...\n"),
                ACE_TEXT (match_results[1].str ().c_str ())));
#endif // _DEBUG
    device_s.device = match_results[1].str ();
    result.push_back (device_s);
  } while (!converter.fail ());
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

struct Common_UI_DisplayDevice
Common_UI_Tools::getDisplayDevice (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplayDevice"));

  // initialize return value(s)
  struct Common_UI_DisplayDevice result;
  ACE_OS::memset (&result, 0, sizeof (struct Common_UI_DisplayDevice));

  Common_UI_DisplayDevices_t display_devices_a =
    Common_UI_Tools::getDisplayDevices ();

  if (deviceIdentifier_in.empty ())
  { // retrieve primary monitor handle
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    struct tagPOINT origin_s;
    ACE_OS::memset (&origin_s, 0, sizeof (struct tagPOINT));
    DWORD flags_i = MONITOR_DEFAULTTONULL;
    result.handle = MonitorFromPoint (origin_s, flags_i);
    if (!result.handle)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to MonitorFromPoint(): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
      return result;
    } // end IF
#else
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (result);
    ACE_NOTREACHED (return result;)
#endif // ACE_WIN32 || ACE_WIN64
  } // end IF

  for (Common_UI_DisplayDevicesIterator_t iterator = display_devices_a.begin ();
       iterator != display_devices_a.end ();
       ++iterator)
  {
    if (deviceIdentifier_in.empty ())
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (result.handle);
      if ((*iterator).handle == result.handle)
        return *iterator;
#else
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (result);
      ACE_NOTREACHED (return result;)
#endif // ACE_WIN32 || ACE_WIN64
    } // end IF
    else
      if (!ACE_OS::strcmp ((*iterator).device.c_str (),
                           deviceIdentifier_in.c_str ()))
        return *iterator;
  } // end FOR

  return result;
}

bool
Common_UI_Tools::displaySupportsResolution (const std::string& deviceIdentifier_in,
                                            const Common_UI_Resolution_t& resolution_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::displaySupportsResolution"));

  // *TODO*
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
}
