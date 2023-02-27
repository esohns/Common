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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <list>
#else
#include <regex>
#include <sstream>
#endif // ACE_WIN32 || ACE_WIN64
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "d3d9.h"
#include "WinUser.h"
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
#include "physicalmonitorenumerationapi.h"
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
#else
#include "X11/Xlib.h"
#include "X11/extensions/Xrandr.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Dirent_Selector.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"
#include "common_macros.h"
#include "common_process_tools.h"
#include "common_os_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_ui_common.h"
#include "common_ui_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "common_ui_monitor_setup_xml_handler.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_xml_defines.h"

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
  Common_UI_DisplayDevices_t* devices_p =
    reinterpret_cast<Common_UI_DisplayDevices_t*> (userData_in);
  ACE_ASSERT (devices_p);

  MONITORINFOEX monitor_info_s;
  ACE_OS::memset (&monitor_info_s, 0, sizeof (MONITORINFOEX));
  monitor_info_s.cbSize = sizeof (MONITORINFOEX);
  struct Common_UI_DisplayDevice display_device_s;
  struct Common_UI_DisplayDevice* display_device_p = NULL;
  bool update_b = false;
  std::string device_name_string;
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
  ACE_OS::memset (physical_monitors_p, 0, sizeof (struct _PHYSICAL_MONITOR) * number_of_monitors);
  if (!GetPhysicalMonitorsFromHMONITOR (monitor_in,
                                        number_of_monitors,
                                        physical_monitors_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetPhysicalMonitorsFromHMONITOR(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    goto clean;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  if (!GetMonitorInfo (monitor_in,
                       &monitor_info_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    goto clean;
  } // end IF
  device_name_string =
#if defined (UNICODE)
    ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info_s.szDevice));
#else
    monitor_info_s.szDevice;
#endif // UNICODE
  // *NOTE*: update any existing results
  display_device_p = NULL;
  update_b = false;
  for (Common_UI_DisplayDevicesIterator_t iterator = devices_p->begin ();
       iterator != devices_p->end ();
       ++iterator)
  {
    if ((*iterator).device.find (device_name_string, 0) == 0)
    {
      display_device_p = &(*iterator);
      update_b = true;
      break;
    } // end IF
  } // end FOR
  if (!display_device_p)
    display_device_p = &display_device_s;
  ACE_ASSERT (display_device_p);
  display_device_p->clippingArea = *clippingArea_in;
  if (!update_b)
  {
    display_device_p->description =
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (physical_monitors_p[0].szPhysicalMonitorDescription));
#else
#if defined (UNICODE)
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info_s.szDevice));
#else
      monitor_info_s.szDevice;
#endif // UNICODE
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
    display_device_p->device =
#if defined (UNICODE)
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info_s.szDevice));
#else
      monitor_info_s.szDevice;
#endif // UNICODE
  } // end IF
  display_device_p->handle = monitor_in;
  if (!update_b)
    devices_p->push_back (*display_device_p);

  result = TRUE;

clean:
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  if (physical_monitors_p)
    delete [] physical_monitors_p;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  return result;
};
#else
int
common_ui_adapter_selector_cb (const dirent* entry_in)
{
  //COMMON_TRACE (ACE_TEXT ("common_ui_adapter_selector_cb"));

  // *NOTE*: select directories following the naming schema "card%u"
  if (ACE::isdotdir (entry_in->d_name))
    return 0;
  std::string directory_string = ACE_TEXT_ALWAYS_CHAR ("/sys/class/drm");
  directory_string += ACE_DIRECTORY_SEPARATOR_STR;
  directory_string += ACE_TEXT_ALWAYS_CHAR (entry_in->d_name);
  if (!Common_File_Tools::isDirectory (directory_string))
    return 0;

  std::string regex_string = ACE_TEXT_ALWAYS_CHAR ("^(?:card)([[:digit:]]+)$");
  std::regex regex (regex_string);
  std::cmatch match_results;
  if (!std::regex_match (entry_in->d_name,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
    return 0;
  ACE_ASSERT (match_results.ready () && !match_results.empty ());
  ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());

  return 1;
}

int
common_ui_adapter_comparator_cb (const dirent** d1,
                                 const dirent** d2)
{
  //COMMON_TRACE (ACE_TEXT ("common_ui_adapter_comparator_cb"));

  return ACE_OS::strcmp ((*d1)->d_name,
                         (*d2)->d_name);
}

int
common_ui_display_selector_cb (const dirent* entry_in)
{
  //COMMON_TRACE (ACE_TEXT ("common_ui_display_selector_cb"));

  // *NOTE*: select directories following the naming schema "card%u"
  if (ACE::isdotdir (entry_in->d_name))
    return 0;
  std::string regex_string = ACE_TEXT_ALWAYS_CHAR ("^(card(?:[[:digit:]]+))-(.+)$");
  std::regex regex (regex_string);
  std::cmatch match_results;
  if (!std::regex_match (entry_in->d_name,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
    return 0;
  ACE_ASSERT (match_results.ready () && !match_results.empty ());
  ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());
  std::string directory_string = ACE_TEXT_ALWAYS_CHAR ("/sys/class/drm");
  directory_string += ACE_DIRECTORY_SEPARATOR_STR;
  directory_string += match_results[1].str ();
  directory_string += ACE_DIRECTORY_SEPARATOR_STR;
  directory_string += ACE_TEXT_ALWAYS_CHAR (entry_in->d_name);
  if (!Common_File_Tools::isDirectory (directory_string))
    return 0;

  return 1;
}

int
common_ui_display_comparator_cb (const dirent** d1,
                                 const dirent** d2)
{
  //COMMON_TRACE (ACE_TEXT ("common_ui_display_comparator_cb"));

  return ACE_OS::strcmp ((*d1)->d_name,
                         (*d2)->d_name);
}
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

bool
Common_UI_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::initialize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Status result = XInitThreads ();
  if (unlikely (result == 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XInitThreads(): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return true;
}
bool
Common_UI_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::finalize"));

  return true;
}

std::string
Common_UI_Tools::toString (enum Common_UI_EventType event_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::toString"));

  // initialize return value(s)
  std::string result = ACE_TEXT_ALWAYS_CHAR ("INVALID");

  switch (event_in)
  {
    case COMMON_UI_EVENT_CONTROL:
      result = ACE_TEXT_ALWAYS_CHAR ("CONTROL"); break;
    case COMMON_UI_EVENT_DATA:
      result = ACE_TEXT_ALWAYS_CHAR ("DATA"); break;
    case COMMON_UI_EVENT_SESSION:
      result = ACE_TEXT_ALWAYS_CHAR ("SESSION"); break;
    case COMMON_UI_EVENT_FINISHED:
      result = ACE_TEXT_ALWAYS_CHAR ("FINISHED"); break;
    case COMMON_UI_EVENT_PAUSED:
      result = ACE_TEXT_ALWAYS_CHAR ("PAUSED"); break;
    case COMMON_UI_EVENT_RESET:
      result = ACE_TEXT_ALWAYS_CHAR ("RESET"); break;
    case COMMON_UI_EVENT_STARTED:
      result = ACE_TEXT_ALWAYS_CHAR ("STARTED"); break;
    case COMMON_UI_EVENT_STOPPED:
      result = ACE_TEXT_ALWAYS_CHAR ("STOPPED"); break;
    case COMMON_UI_EVENT_ABORT:
      result = ACE_TEXT_ALWAYS_CHAR ("ABORT"); break;
    case COMMON_UI_EVENT_CONNECT:
      result = ACE_TEXT_ALWAYS_CHAR ("CONNECT"); break;
    case COMMON_UI_EVENT_DISCONNECT:
      result = ACE_TEXT_ALWAYS_CHAR ("DISCONNECT"); break;
    case COMMON_UI_EVENT_STATISTIC:
      result = ACE_TEXT_ALWAYS_CHAR ("STATISTIC"); break;
    case COMMON_UI_EVENT_RESIZE:
      result = ACE_TEXT_ALWAYS_CHAR ("RESIZE"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown session event type (was: %d), aborting\n"),
                  event_in));
      break;
    }
  } // end SWITCH

  return result;
}

Common_UI_DisplayAdapters_t
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Common_UI_Tools::getAdapters (bool excludeMirroringDevices_in)
#else
Common_UI_Tools::getAdapters ()
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getAdapters"));

  // initialize return value(s)
  Common_UI_DisplayAdapters_t result;

  struct Common_UI_DisplayAdapter display_adapter_s;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_UI_DisplayAdaptersIterator_t iterator;
  struct Common_UI_DisplayAdapterHead display_adapter_head_s;
  struct Common_UI_DisplayAdapter* display_adapter_p = NULL;
  DWORD index_i = 0;
  DISPLAY_DEVICE display_device_s;
  ACE_OS::memset (&display_device_s, 0, sizeof (DISPLAY_DEVICE));
  display_device_s.cb = sizeof (DISPLAY_DEVICE);
  DWORD flags_i = EDD_GET_DEVICE_INTERFACE_NAME;
  std::string device_feature_string;
  do
  {
    if (!EnumDisplayDevices (NULL,              // lpDevice
                             index_i,           // iDevNum
                             &display_device_s, // lpDisplayDevice
                             flags_i))          // dwFlags
      break;
    device_feature_string.clear ();
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0500) // _WIN32_WINNT_WIN2K
    if (display_device_s.StateFlags & DISPLAY_DEVICE_ACTIVE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("active ");
    if (display_device_s.StateFlags & DISPLAY_DEVICE_ATTACHED)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("attached ");
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0500)
    if (display_device_s.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("desktop ");
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0500) // _WIN32_WINNT_WIN2K
    if (display_device_s.StateFlags & DISPLAY_DEVICE_DISCONNECT)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("disconnect ");
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0500)
    if (display_device_s.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("mirror ");
    if (display_device_s.StateFlags & DISPLAY_DEVICE_MODESPRUNED)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("modes_pruned ");
    if (display_device_s.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("multi ");
    if (display_device_s.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("primary ");
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0500) // _WIN32_WINNT_WIN2K
    if (display_device_s.StateFlags & DISPLAY_DEVICE_REMOTE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("remote ");
    if (display_device_s.StateFlags & DISPLAY_DEVICE_REMOVABLE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("removable ");
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0500)
    if (display_device_s.StateFlags & DISPLAY_DEVICE_TS_COMPATIBLE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("ts_compatible ");
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_LONGHORN
    if (display_device_s.StateFlags & DISPLAY_DEVICE_UNSAFE_MODES_ON)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("unsafe_modes ");
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
    if (display_device_s.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE)
     device_feature_string += ACE_TEXT_ALWAYS_CHAR ("vga_compatible ");
#if defined (UNICODE)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display adapter #%d: \"%s\" [%s] {%s}\n"),
                index_i,
                ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceString),
                ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceName),
                ACE_TEXT (device_feature_string.c_str ())));
#else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display adapter #%d: \"%s\" [%s] {%s}\n"),
                index_i,
                ACE_TEXT (display_device_s.DeviceString),
                ACE_TEXT (display_device_s.DeviceName),
                ACE_TEXT (device_feature_string.c_str ())));
#endif // UNICODE
    if (excludeMirroringDevices_in &&
        (display_device_s.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
      goto continue_;
    // check if it is another head on an existing adapter
    display_adapter_s.description =
#if defined (UNICODE)
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceString));
#else
      display_device_s.DeviceString;
#endif // UNICODE
    iterator =
      std::find (result.begin (), result.end (), display_adapter_s);
    if (iterator != result.end ())
      display_adapter_p = &(*iterator);
    else
      display_adapter_p = &display_adapter_s;
    display_adapter_head_s.index = index_i;
#if defined (UNICODE)
    display_adapter_head_s.device =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceName));
    display_adapter_head_s.key =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceKey));
    display_adapter_p->device =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceID));
    display_adapter_p->description =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceString));
#else
    display_adapter_head_s.device = display_device_s.DeviceName;
    display_adapter_head_s.key = display_device_s.DeviceKey;
    display_adapter_p->device = display_device_s.DeviceID;
    display_adapter_p->description = display_device_s.DeviceString;
#endif // UNICODE
    if (iterator == result.end ())
    {
      result.push_back (display_adapter_s);
      iterator =
        std::find (result.begin (), result.end (), display_adapter_s);
      ACE_ASSERT (iterator != result.end ());
    } // end ELSE
    if (iterator != result.end ())
      (*iterator).heads.push_back (display_adapter_head_s);
continue_:
    ++index_i;
  } while (true);
#elif defined (ACE_LINUX)
  // retrieve all display adapters from /sys/class/drm
  ACE_Dirent_Selector entries;
  int result_2 = entries.open (ACE_TEXT ("/sys/class/drm"),
                               &common_ui_adapter_selector_cb,
                               &common_ui_adapter_comparator_cb);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT ("/sys/class/drm")));
    return result;
  } // end IF
  // *NOTE*: the /sys/class/drm/cardx/'device' contains the pci-bus slot address
  //         that can be used to retrieve the adapter name with 'lspci'
  // *TODO*: this should work on most Linux systems, but is in fact a really bad
  //         idea:
  //         - 'lspci' tool dependency
  //         - system(3) call
  //         --> very inefficient; replace ASAP

  std::string device_file, pci_bus_address, uevent_file;
  std::string pci_devices_string, buffer_string;
  std::string regex_string =
      ACE_TEXT_ALWAYS_CHAR ("^(?:\\.\\./)*(?:[[:digit:]]{4}:)(.+)$");
  std::string regex_string_2 = ACE_TEXT_ALWAYS_CHAR ("^(?:DRIVER=)(.+)$");
  std::string regex_string_3 =
      ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]:\\.]+) (?:VGA compatible controller: )(.+)$");
  std::regex regex (regex_string);
  std::regex regex_2 (regex_string_2);
  std::regex regex_3 (regex_string_3);
  std::smatch match_results;
  std::istringstream converter;
  char buffer_a [BUFSIZ];
  std::string command_line_string = ACE_TEXT_ALWAYS_CHAR ("lspci");
  int exit_status_i = 0;
  uint8_t* buffer_p = NULL;
  ACE_UINT64 file_size_i = 0;
  for (int i = entries.length () - 1;
       i >= 0;
       i--)
  {
    display_adapter_s.device = ACE_TEXT_ALWAYS_CHAR ("/sys/class/drm");
    display_adapter_s.device += ACE_DIRECTORY_SEPARATOR_STR;
    display_adapter_s.device += ACE_TEXT_ALWAYS_CHAR (entries[i]->d_name);

    // retrieve slot address
    device_file = display_adapter_s.device;
    device_file += ACE_DIRECTORY_SEPARATOR_STR;
    device_file += ACE_TEXT_ALWAYS_CHAR ("device");
    ACE_ASSERT (Common_File_Tools::isLink (device_file));
    pci_bus_address = Common_File_Tools::linkTarget (device_file);
    if (!std::regex_match (pci_bus_address,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to parse pci slot address (was: \"%s\"), aborting\n"),
                  ACE_TEXT (pci_bus_address.c_str ())));
      return result;
    } // end IF
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());
    pci_bus_address = match_results[1].str ();

    // retrieve driver
    uevent_file = device_file;
    uevent_file += ACE_DIRECTORY_SEPARATOR_STR;
    uevent_file += ACE_TEXT_ALWAYS_CHAR ("uevent");
    ACE_ASSERT (Common_File_Tools::isReadable (uevent_file));
    if (!Common_File_Tools::load (uevent_file,
                                  buffer_p,
                                  file_size_i,
                                  0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                  ACE_TEXT (uevent_file.c_str ())));
      return result;
    } // end IF
    ACE_ASSERT (buffer_p);
    buffer_string = reinterpret_cast<char*> (buffer_p);
    delete [] buffer_p; buffer_p = NULL;
    converter.str (buffer_string);
    // parse display adapter entries
    do
    {
      converter.getline (buffer_a, sizeof (char[BUFSIZ]));
      buffer_string = buffer_a;
      if (!std::regex_match (buffer_string,
                             match_results,
                             regex_2,
                             std::regex_constants::match_default))
        continue;
      ACE_ASSERT (match_results.ready () && !match_results.empty ());
      ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());
      display_adapter_s.driver = match_results[1].str ();
      break;
    } while (!converter.fail ());

    // retrieve adapter description
    // *NOTE*: (qtcreator) gdb fails to debug this (hangs) unless the
    //         "Debug all children" option is disabled
    if (unlikely (!Common_Process_Tools::command (command_line_string.c_str (),
                                                  exit_status_i,
                                                  pci_devices_string)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n"),
                  ACE_TEXT (command_line_string.c_str ())));
      return result;
    } // end IF
    converter.str (pci_devices_string);
    // parse display adapter entries
    do
    {
      converter.getline (buffer_a, sizeof (char[BUFSIZ]));
      buffer_string = buffer_a;
      if (!std::regex_match (buffer_string,
                             match_results,
                             regex_3,
                             std::regex_constants::match_default))
        continue;
      ACE_ASSERT (match_results.ready () && !match_results.empty ());
      ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());
      ACE_ASSERT (match_results[2].matched && !match_results[2].str ().empty ());
      if (ACE_OS::strcmp (match_results[1].str ().c_str (),
                          pci_bus_address.c_str ()))
        continue;
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found display adapter \"%s\" (slot: %s, driver: %s) at /sys/class/drm/%s\n"),
                  ACE_TEXT (match_results[2].str ().c_str ()),
                  ACE_TEXT (match_results[1].str ().c_str ()),
                  ACE_TEXT (display_adapter_s.driver.c_str ()),
                  ACE_TEXT (entries[i]->d_name)));
      display_adapter_s.description = match_results[2].str ();
      display_adapter_s.slot = match_results[1].str ();
      result.push_back (display_adapter_s);
      break;
    } while (!converter.fail ());
  } // end FOR
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

struct Common_UI_DisplayAdapter
Common_UI_Tools::getAdapter (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getAdapter"));

  // initialize return value(s)
  struct Common_UI_DisplayAdapter result;

  Common_UI_DisplayAdapters_t display_adapters_a =
    Common_UI_Tools::getAdapters ();

  for (Common_UI_DisplayAdaptersIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (!ACE_OS::strcmp ((*iterator).device.c_str (),
                         deviceIdentifier_in.c_str ()))
      return *iterator;
    for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator_2 = (*iterator).heads.begin ();
         iterator_2 != (*iterator).heads.end ();
         ++iterator_2)
    {
      if (deviceIdentifier_in.empty ())
      {
        if (!(*iterator_2).index)
          return *iterator;
      } // end IF
      else
        if (!ACE_OS::strcmp ((*iterator_2).device.c_str (),
                             deviceIdentifier_in.c_str ()))
          return *iterator;
    } // end FOR
#else
    if (deviceIdentifier_in.empty () ||
        !ACE_OS::strcmp ((*iterator).device.c_str (),
                         deviceIdentifier_in.c_str ()))
      return *iterator;
#endif // ACE_WIN32 || ACE_WIN64
  } // end FOR

  return result;
}

Common_UI_DisplayDevices_t
Common_UI_Tools::getDisplays (const struct Common_UI_DisplayAdapter& adapter_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplays"));

  // initialize return value(s)
  Common_UI_DisplayDevices_t result;

  struct Common_UI_DisplayDevice display_device_s;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD index_i = 0;
  DISPLAY_DEVICE display_device_2;
  ACE_OS::memset (&display_device_2, 0, sizeof (DISPLAY_DEVICE));
  display_device_2.cb = sizeof (DISPLAY_DEVICE);
  DWORD flags_i = 0;
  for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator = adapter_in.heads.begin ();
       iterator != adapter_in.heads.end ();
       ++iterator)
  {
    index_i = 0;
    //do
    //{
#if defined (UNICODE)
      if (!EnumDisplayDevices (ACE_TEXT_ALWAYS_WCHAR ((*iterator).device.c_str ()), // lpDevice
#else
      if (!EnumDisplayDevices ((*iterator).device.c_str (),                         // lpDevice
#endif // UNICODE
                               index_i,                                             // iDevNum
                               &display_device_2,                                   // lpDisplayDevice
                               flags_i))                                            // dwFlags
        break;
      display_device_s =
#if defined (UNICODE)
        Common_UI_Tools::getDisplay (ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceName)));
#else
        Common_UI_Tools::getDisplay (display_device_2.DeviceName);
#endif // UNICODE
      result.push_back (display_device_s);
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found display device on adapter \"%s\": #%d: \"%s\"/\"%s\"\n"),
                  ACE_TEXT (adapter_in.description.c_str ()),
                  index_i,
                  ACE_TEXT (display_device_s.device.c_str ()),
                  ACE_TEXT (display_device_s.description.c_str ())));
#endif // _DEBUG
    //  ++index_i;
    //} while (true);
  } // end FOR
#else
  // retrieve all displays from /sys/class/drm/cardx
  ACE_ASSERT (Common_File_Tools::isDirectory (adapter_in.device));
  ACE_Dirent_Selector entries;
  int result_2 = entries.open (ACE_TEXT (adapter_in.device.c_str ()),
                               &common_ui_display_selector_cb,
                               &common_ui_display_comparator_cb);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (adapter_in.device.c_str ())));
    return result;
  } // end IF
  // *NOTE*: the /sys/class/drm/cardx/cardx-yyyy/status file(s) contain the
  //         status of the display connectors available on each adapter (i.e.
  //         'connected'|'disconnected')
  std::string regex_string = ACE_TEXT_ALWAYS_CHAR ("^(?:card(?:[[:digit:]]+))-(.+)$");
  std::regex regex (regex_string);
  std::cmatch match_results;
  std::string status_file;
  uint8_t* buffer_p = NULL;
  ACE_UINT64 file_size_i = 0;
  for (int i = entries.length () - 1;
       i >= 0;
       i--)
  {
    if (!std::regex_match (entries[i]->d_name,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to parse display entry (was: \"%s\"), aborting\n"),
                  ACE_TEXT (entries[i]->d_name)));
      return result;
    } // end IF
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());

    status_file = adapter_in.device;
    status_file += ACE_DIRECTORY_SEPARATOR_STR;
    status_file += ACE_TEXT_ALWAYS_CHAR (entries[i]->d_name);
    status_file += ACE_DIRECTORY_SEPARATOR_STR;
    status_file += ACE_TEXT_ALWAYS_CHAR ("status");
    ACE_ASSERT (Common_File_Tools::isReadable (status_file));
    if (!Common_File_Tools::load (status_file,
                                  buffer_p,
                                  file_size_i,
                                  0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                  ACE_TEXT (status_file.c_str ())));
      return result;
    } // end IF
    ACE_ASSERT (buffer_p);
    if (ACE_OS::strcmp (reinterpret_cast<char*> (buffer_p),
                        ACE_TEXT_ALWAYS_CHAR ("connected\n")))
    {
      delete [] buffer_p; buffer_p = NULL;
      continue;
    } // end IF
    delete [] buffer_p; buffer_p = NULL;
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display device on adapter \"%s\": \"%s\"\n"),
                ACE_TEXT (adapter_in.description.c_str ()),
                ACE_TEXT (match_results[1].str ().c_str())));
    display_device_s.device = match_results[1].str ();
    result.push_back (display_device_s);
  } // end FOR
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

// ---------------------------------------

Common_UI_DisplayDevices_t
Common_UI_Tools::getDisplays ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplays"));

  // initialize return value(s)
  Common_UI_DisplayDevices_t result;

  struct Common_UI_DisplayDevice display_device_s;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD index_i = 0, index_2 = 0;
  DISPLAY_DEVICE display_device_2;
  ACE_OS::memset (&display_device_2, 0, sizeof (DISPLAY_DEVICE));
  display_device_2.cb = sizeof (DISPLAY_DEVICE);
  DWORD flags_i = EDD_GET_DEVICE_INTERFACE_NAME;
  std::string device_name_string;
  //HDC device_context_p = NULL;
  //DEVMODE device_mode_s;
  //ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  //device_mode_s.dmSize = sizeof (DEVMODE);
  ////device_mode_s.dmDriverExtra = 0;
  //DWORD dwFlags = (EDS_RAWMODE |
  //                 EDS_ROTATEDMODE);
  do
  {
    if (!EnumDisplayDevices (NULL,                                                  // lpDevice
                             index_i++,                                             // iDevNum
                             &display_device_2,                                     // lpDisplayDevice
                             flags_i))                                              // dwFlags
      break;
    device_name_string = ACE_TEXT_ALWAYS_CHAR (display_device_2.DeviceName);

    index_2 = 0;
    do
    {
#if defined (UNICODE)
      if (!EnumDisplayDevices (ACE_TEXT_ALWAYS_WCHAR (device_name_string.c_str ()),   // lpDevice
#else
      if (!EnumDisplayDevices (device_name_string.c_str (),                           // lpDevice
#endif // UNICODE
                               index_2++,                                             // iDevNum
                               &display_device_2,                                     // lpDisplayDevice
                               flags_i))                                              // dwFlags
        break;
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found display device #%d: \"%s\" [%s]\n"),
                  index_i,
                  ACE_TEXT (display_device_2.DeviceString),
                  ACE_TEXT (display_device_2.DeviceName)));

      display_device_s.id =
#if defined (UNICODE)
        ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceID));
#else
        display_device_2.DeviceID;
#endif // UNICODE
      display_device_s.key =
#if defined (UNICODE)
        ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceKey));
#else
        display_device_2.DeviceKey;
#endif // UNICODE
      display_device_s.device =
#if defined (UNICODE)
        ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceName));
#else
        display_device_2.DeviceName;
#endif // UNICODE
      display_device_s.description =
#if defined (UNICODE)
        ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceString));
#else
        display_device_2.DeviceString;
#endif // UNICODE
    // *IMPORTANT NOTE*: EnumDisplayMonitors() appears to be partially broken;
    //                   it will not accept device contexts
    //                   --> update any found device entries manually
    // *TODO*: this workaround probably works for desktop devices only
      result.push_back (display_device_s);
//#if defined (UNICODE)
//      if !EnumDisplaySettingsEx (ACE_TEXT_ALWAYS_WCHAR ((*iterator).device.c_str ()), // lpszDeviceName
//#else
//      if (!EnumDisplaySettingsEx ((*iterator).device.c_str (),                         // lpszDeviceName
//#endif // UNICODE
//                                  ENUM_REGISTRY_SETTINGS,                              // iModeNum
//                                  &device_mode_s,                                      // lpDevMode
//                                  dwFlags))                                            // dwFlags
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to EnumDisplaySettingsEx(\"%s\"): \"%s\", aborting\n"),
//                    ACE_TEXT ((*iterator).device.c_str ()),
//                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//        result.clear ();
//        return result;
//      } // end IF
//      // WARNING*: cannot supply the monitor device, only the adapter device
//      device_context_p =
//#if defined (UNICODE)
//                  //TEXT ("DISPLAY"),
//        CreateDC (ACE_TEXT_ALWAYS_WCHAR ((*iterator).device.c_str ()), // pwszDriver
//                  ACE_TEXT_ALWAYS_WCHAR ((*iterator).device.c_str ()), // pwszDevice
//#else
//        CreateDC ((*iterator).device.c_str (), // pwszDriver
//                  (*iterator).device.c_str (), // pwszDevice
//#endif // UNICODE
//                  NULL,                        // pszPort
//                  &device_mode_s);             // pdm
//                  //NULL);                       // pdm
//      if (!device_context_p)
//      {
//#if defined (UNICODE)
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to CreateDC(\"%s\"): \"%s\", aborting\n"),
//                    ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceName),
//                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//#else
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to CreateDC(\"%s\"): \"%s\", aborting\n"),
//                    ACE_TEXT (display_device_2.DeviceName),
//                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//#endif // UNICODE
//        result.clear ();
//        return result;
//      } // end IF
//      if (!EnumDisplayMonitors (device_context_p,                    // hdc
//                                NULL,                                // lprcClip
//                                common_ui_monitor_enum_cb,           // lpfnEnum
//                                reinterpret_cast<LPARAM> (&result))) // dwData
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to EnumDisplayMonitors(0x%@): \"%s\", aborting\n"),
//                    device_context_p,
//                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//        if (!DeleteDC (device_context_p))
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to DeleteDC(): \"%s\", continuing\n"),
//                      ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//        result.clear ();
//        return result;
//      } // end IF
//      if (!DeleteDC (device_context_p))
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to DeleteDC(): \"%s\", continuing\n"),
//                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
//      device_context_p = NULL;
    } while (true);
  } while (true);
  if (!EnumDisplayMonitors (NULL,                                // hdc
                            NULL,                                // lprcClip
                            common_ui_monitor_enum_cb,           // lpfnEnum
                            reinterpret_cast<LPARAM> (&result))) // dwData
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumDisplayMonitors(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    result.clear ();
    return result;
  } // end IF
#else
  int result_2 = -1;
  std::string command_line_string, tool_path_string;

  int monitor_count_i = 0;
  XRROutputInfo* output_info_p = NULL;
  XRRMonitorInfo* monitor_info_p = NULL, *monitor_info_2 = NULL;
  XRRScreenResources* screen_resources_p = NULL;
  Display* display_p = XOpenDisplay (NULL);
  if (unlikely (!display_p))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to XOpenDisplay(): \"%m\", falling back\n")));
    goto fallback;
  } // end IF

   // 0 = active, 1 = inactive, call it twice to get a full list, if there are
   // inactive monitors
  monitor_info_p = XRRGetMonitors (display_p,
                                   DefaultRootWindow (display_p),
                                   0,
                                   &monitor_count_i);
  if (unlikely (!monitor_info_p))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to XRRGetMonitors(): \"%m\", falling back\n")));
    goto fallback;
  } // end IF

  screen_resources_p =
      XRRGetScreenResources (display_p,
                             DefaultRootWindow (display_p));
  ACE_ASSERT (screen_resources_p);

  monitor_info_2 = monitor_info_p;
  for (int i = 0;
       i < monitor_count_i;
       ++i)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found monitor: \"%s\"\n"),
                ACE_TEXT (XGetAtomName (display_p, monitor_info_2->name))));
    ACE_ASSERT (monitor_info_2->noutput == 1);

    output_info_p = XRRGetOutputInfo (display_p,
                                      screen_resources_p,
                                      monitor_info_2->outputs[0]);
    ACE_ASSERT (output_info_p);
    ACE_ASSERT (output_info_p->connection == RR_Connected);
    display_device_s.description = output_info_p->name;
    XRRFreeOutputInfo (output_info_p); output_info_p = NULL;
    display_device_s.device = XGetAtomName (display_p, monitor_info_2->name);
    display_device_s.primary = (monitor_info_2->primary == 1);
    display_device_s.clippingArea.x = monitor_info_2->x;
    display_device_s.clippingArea.y = monitor_info_2->y;
    display_device_s.clippingArea.width = monitor_info_2->width;
    display_device_s.clippingArea.height = monitor_info_2->height;
    result.push_back (display_device_s);

    monitor_info_2++;
  } // end FOR

  XRRFreeScreenResources (screen_resources_p); screen_resources_p = NULL;
  XRRFreeMonitors (monitor_info_p); monitor_info_p = NULL;
  result_2 = XCloseDisplay (display_p);
  ACE_ASSERT (result_2 == 0);

  goto continue_;

fallback:
  // *TODO*: this should work on most systems running Xorg X (and compatible
  //         derivates), but is in fact really a bad idea due to these
  //         dependencies:
  //         - 'xrandr' tool
  //         - temporary files
  //         - system(3) call
  //         --> very inefficient; replace ASAP
  command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_XRANDR);
  if (Common_OS_Tools::isInstalled (command_line_string,
                                    tool_path_string))
  {
    std::string display_records_string;
    std::istringstream converter, converter_2;
    char buffer_a [BUFSIZ];
    std::string regex_string_screen =
        ACE_TEXT_ALWAYS_CHAR ("^Screen ([[:digit:]]+): minimum ([[:digit:]]+) x ([[:digit:]]+), current ([[:digit:]]+) x ([[:digit:]]+), maximum ([[:digit:]]+) x ([[:digit:]]+)$");
    std::string regex_string_display =
        ACE_TEXT_ALWAYS_CHAR ("^([^[:space:]]+) (connected|disconnected) (?:(primary) )?([[:digit:]]+)x([[:digit:]]+)(\\+|-)([[:digit:]]+)(\\+|-)([[:digit:]]+) \\((?:([^[:space:]]+) )+([^\\)]+)\\) ([[:digit:]]+)mm x ([[:digit:]]+)mm$");
    std::regex regex (regex_string_screen);
    std::regex regex_2 (regex_string_display);
    std::smatch match_results, match_results_2;
    std::string buffer_string;
    // *NOTE*: (qtcreator) gdb fails to debug this (hangs) unless you disable the
    //         "Debug all children" option
    int exit_status_i = 0;
    if (unlikely (!Common_Process_Tools::command (tool_path_string.c_str (),
                                                  exit_status_i,
                                                  display_records_string)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n"),
                  ACE_TEXT (command_line_string.c_str ())));
      return result;
    } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("xrandr output: \"%s\"\n"),
//              ACE_TEXT (display_record_string.c_str ())));

    converter.str (display_records_string);
    // parse screen entries
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
      ACE_ASSERT (match_results[4].matched && !match_results[4].str ().empty ());
      ACE_ASSERT (match_results[5].matched && !match_results[5].str ().empty ());
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("found screen %s\n"),
//                  ACE_TEXT (match_results[1].str ().c_str ())));
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      converter_2.str (match_results[4].str ());
      converter_2 >> display_device_s.clippingArea.width;
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      converter_2.str (match_results[5].str ());
      converter_2 >> display_device_s.clippingArea.height;
    } while (!converter.fail ());

    // parse display entries
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter.str (display_records_string);
    do
    {
      converter.getline (buffer_a, sizeof (char[BUFSIZ]));
      buffer_string = buffer_a;
      if (!std::regex_match (buffer_string,
                             match_results_2,
                             regex_2,
                             std::regex_constants::match_default))
        continue;
      ACE_ASSERT (match_results_2.ready () && !match_results_2.empty ());
      ACE_ASSERT (match_results_2[1].matched && !match_results_2[1].str ().empty ());
      ACE_ASSERT (match_results_2[2].matched && !match_results_2[2].str ().empty ());
      ACE_ASSERT (match_results_2[4].matched && !match_results_2[4].str ().empty ());
      ACE_ASSERT (match_results_2[5].matched && !match_results_2[5].str ().empty ());
      ACE_ASSERT (match_results_2[6].matched && !match_results_2[6].str ().empty ());
      ACE_ASSERT (match_results_2[7].matched && !match_results_2[7].str ().empty ());
      ACE_ASSERT (match_results_2[8].matched && !match_results_2[8].str ().empty ());
      ACE_ASSERT (match_results_2[9].matched && !match_results_2[9].str ().empty ());
      if (ACE_OS::strcmp (match_results_2[2].str ().c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("connected")))
        continue;
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found display device \"%s\"\n"),
                  ACE_TEXT (match_results_2[1].str ().c_str ())));
      display_device_s.description = match_results_2[1].str ();
      display_device_s.device = match_results_2[1].str ();
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      if (match_results_2[3].matched)
      { ACE_ASSERT (!ACE_OS::strcmp (match_results_2[3].str ().c_str (), ACE_TEXT_ALWAYS_CHAR ("primary")));
        display_device_s.primary = true;
      } // end IF
      converter_2.str (match_results[4].str ());
      converter_2 >> display_device_s.clippingArea.width;
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      converter_2.str (match_results[5].str ());
      converter_2 >> display_device_s.clippingArea.height;
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      converter_2.str (match_results[7].str ());
      converter_2 >> display_device_s.clippingArea.x;
      if (match_results[6].str ()[0] == '-')
        display_device_s.clippingArea.x = -display_device_s.clippingArea.x;
      converter_2.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter_2.clear ();
      converter_2.str (match_results[9].str ());
      converter_2 >> display_device_s.clippingArea.y;
      if (match_results[8].str ()[0] == '-')
        display_device_s.clippingArea.y = -display_device_s.clippingArea.y;
      result.push_back (display_device_s);
    } while (!converter.fail ());
  } // end IF
  else
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s not installed, falling back\n"),
                ACE_TEXT (command_line_string.c_str ())));
    command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_HWINFO);
    if (Common_OS_Tools::isInstalled (command_line_string,
                                      tool_path_string))
    {
      command_line_string += ACE_TEXT_ALWAYS_CHAR (" --");
      command_line_string +=
          ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_SWITCH_HWINFO_DISPLAY);
      std::string display_records_string;
      std::istringstream converter, converter_2;
      char buffer_a [BUFSIZ];
      std::string regex_string_display_id =
          ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]+): (?:.+)$");
      std::string regex_string_display_model =
          ACE_TEXT_ALWAYS_CHAR ("^(?:[[:space:]]+)Model: \"(.+)\"$");
      std::string regex_string_display_device =
          ACE_TEXT_ALWAYS_CHAR ("^(?:[[:space:]]+)Device: (?:.+)\"(.+)\"$");
      std::string regex_string_display_primary =
          ACE_TEXT_ALWAYS_CHAR ("^Primary display adapter: #([[:digit:]]+)$");
      std::regex regex (regex_string_display_id);
      std::smatch match_results;
      std::string buffer_string;
      // *NOTE*: (qtcreator) gdb fails to debug this (hangs) unless you disable the
      //         "Debug all children" option
      int exit_status_i = 0;
      if (unlikely (!Common_Process_Tools::command (command_line_string.c_str (),
                                                    exit_status_i,
                                                    display_records_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n"),
                   ACE_TEXT (command_line_string.c_str ())));
        return result;
      } // end IF
      //  ACE_DEBUG ((LM_DEBUG,
      //              ACE_TEXT ("hwinfo output: \"%s\"\n"),
      //              ACE_TEXT (display_record_string.c_str ())));

      converter.str (display_records_string);
      unsigned int device_id = 0;
      std::map<unsigned int, struct Common_UI_DisplayDevice> devices_a;
      std::map<unsigned int, struct Common_UI_DisplayDevice>::iterator iterator;
      // parse display entries
next:
      while (!converter.fail ())
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
        converter_2.clear ();
        converter_2.str (match_results[1].str ());
        converter_2 >> device_id;
        break;
      } // end WHILE
      regex.assign (regex_string_display_model);
      while (!converter.fail ())
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
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("%u: found display device \"%s\"\n"),
//                    device_id,
//                    ACE_TEXT (match_results[1].str ().c_str ())));
        display_device_s.description = match_results[1].str ();
        break;
      } // end WHILE
      regex.assign (regex_string_display_device);
      while (!converter.fail ())
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
        display_device_s.device = match_results[1].str ();
        devices_a.insert (std::make_pair (device_id, display_device_s));
        break;
      } // end WHILE
      if (!converter.fail ())
        goto next;

      regex.assign (regex_string_display_primary);
      converter.clear ();
      converter.str (display_records_string);
      while (!converter.fail ())
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
        converter_2.clear ();
        converter_2.str (match_results[1].str ());
        converter_2 >> device_id;
        break;
      } // end WHILE
      iterator = devices_a.find (device_id);
      ACE_ASSERT (iterator != devices_a.end ());
      (*iterator).second.primary = true;
      for (iterator = devices_a.begin ();
           iterator != devices_a.end ();
           ++iterator)
        result.push_back ((*iterator).second);
    } // end IF
  } // end ELSE
continue_:
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

struct Common_UI_DisplayDevice
Common_UI_Tools::getDisplay (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplay"));

  // initialize return value(s)
  struct Common_UI_DisplayDevice result;

  Common_UI_DisplayDevices_t display_devices_a =
    Common_UI_Tools::getDisplays ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (deviceIdentifier_in.empty ())
  { // retrieve primary monitor handle
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
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

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
      if ((*iterator).primary || (display_devices_a.size () == 1))
        return *iterator;
#endif // ACE_WIN32 || ACE_WIN64
    } // end IF
    else
      if (!ACE_OS::strcmp ((*iterator).device.c_str (),
                           deviceIdentifier_in.c_str ()))
        return *iterator;
  } // end FOR

  return result;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Common_UI_Display
Common_UI_Tools::getLogicalDisplay (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getLogicalDisplay"));

  // initialize return value(s)
  struct Common_UI_Display result;

  const char* display_name_p =
      (deviceIdentifier_in.empty () ? NULL
                                    : deviceIdentifier_in.c_str ());
  result.display = XOpenDisplay (display_name_p);
  if (unlikely (!result.display))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XOpenDisplay(\"%s\"), aborting\n"),
                (display_name_p ? ACE_TEXT (display_name_p) : ACE_TEXT ("NULL"))));
    return result;
  } // end IF

  return result;
}
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Common_UI_Tools::displayMatches (const std::string& identifier_in,
                                 const std::string& identifier2_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::displayMatches"));

  std::string regex_string = ACE_TEXT_ALWAYS_CHAR ("^([^-]+)-(?:[[:alpha:]]{1}-)?([[:digit:]]+)$");
  std::regex regex (regex_string);
  std::smatch match_results, match_results_2;
  if (!std::regex_match (identifier_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to parse device identifier (was: %s), aborting\n"),
                ACE_TEXT (identifier_in.c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (match_results.ready () && !match_results.empty ());
  ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());
  ACE_ASSERT (match_results[2].matched && !match_results[2].str ().empty ());
  if (!std::regex_match (identifier2_in,
                         match_results_2,
                         regex,
                         std::regex_constants::match_default))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to parse device identifier (was: %s), aborting\n"),
                ACE_TEXT (identifier2_in.c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (match_results_2.ready () && !match_results_2.empty ());
  ACE_ASSERT (match_results_2[1].matched && !match_results_2[1].str ().empty ());
  ACE_ASSERT (match_results_2[2].matched && !match_results_2[2].str ().empty ());

  return (!ACE_OS::strcmp (match_results[1].str ().c_str (),
                           match_results_2[1].str ().c_str ()) &&
          !ACE_OS::strcmp (match_results[2].str ().c_str (),
                           match_results_2[2].str ().c_str ()));
}
#endif // ACE_WIN32 || ACE_WIN64

struct Common_UI_DisplayAdapter
Common_UI_Tools::getAdapter (const struct Common_UI_DisplayDevice& device_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getAdapter"));

  struct Common_UI_DisplayAdapter result;

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (device_in.handle != NULL);
#else
  ACE_ASSERT (!device_in.device.empty ());
#endif // ACE_WIN32 || ACE_WIN64

  Common_UI_DisplayAdapters_t display_adapters_a =
    Common_UI_Tools::getAdapters ();
  for (Common_UI_DisplayAdaptersIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
  {
    Common_UI_DisplayDevices_t display_devices_a =
      Common_UI_Tools::getDisplays (*iterator);
    for (Common_UI_DisplayDevicesIterator_t iterator_2 = display_devices_a.begin ();
         iterator_2 != display_devices_a.end ();
         ++iterator_2)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if ((*iterator_2).handle == device_in.handle)
#else
      if (Common_UI_Tools::displayMatches ((*iterator_2).device.c_str (),
                                           device_in.device.c_str ()))
#endif // ACE_WIN32 || ACE_WIN64
        return *iterator;
  } // end FOR

  return result;
}

Common_UI_DisplayDevices_t
Common_UI_Tools::getDesktopDisplays ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDesktopDisplays"));

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
  if (unlikely (!Common_Process_Tools::command (command_line_string.c_str (),
                                                exit_status_i,
                                                display_records_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
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

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display device \"%s\"...\n"),
                ACE_TEXT (match_results[1].str ().c_str ())));
    device_s.device = match_results[1].str ();
    result.push_back (device_s);
  } while (!converter.fail ());
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
std::string
Common_UI_Tools::getX11DisplayName (const std::string& outputName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getX11DisplayName"));

  const char* display_p =
      ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_X11_DISPLAY_ENVIRONMENT_VARIABLE));
  if (!display_p)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("environment \"%s\" not set, using default display, continuing\n"),
                ACE_TEXT (COMMON_UI_X11_DISPLAY_ENVIRONMENT_VARIABLE)));

  int result = -1;
  struct _XDisplay* display_2 = XOpenDisplay (display_p);
  if (!display_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XOpenDisplay(%s), aborting\n"),
                (display_p ? ACE_TEXT (display_p) : ACE_TEXT ("NULL"))));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF

  // verify that the output is being used by the X11 session
//  Screen* screen_p = NULL;
  XRRScreenResources* screen_resources_p = NULL;
  XRROutputInfo* output_info_p = NULL;
  XRRCrtcInfo* crtc_info_p = NULL;
  for (int i = 0;
       i < ScreenCount (display_2);
       ++i)
  {
//    screen_p = ScreenOfDisplay (display_2, i);
//    ACE_ASSERT (screen_p); ACE_UNUSED_ARG (screen_p);

    screen_resources_p =
        XRRGetScreenResources (display_2,
                               RootWindow (display_2, i));
    ACE_ASSERT (screen_resources_p);
    for (int j = 0;
         j < screen_resources_p->noutput;
         ++j)
    {
      output_info_p = XRRGetOutputInfo (display_2,
                                        screen_resources_p,
                                        screen_resources_p->outputs[j]);
      ACE_ASSERT (output_info_p);
      if ((output_info_p->connection != RR_Connected) ||
          ACE_OS::strcmp (output_info_p->name,
                          outputName_in.c_str ()))
      {
        XRRFreeOutputInfo (output_info_p); output_info_p = NULL;
        continue;
      } // end IF
      crtc_info_p = XRRGetCrtcInfo (display_2,
                                    screen_resources_p,
                                    output_info_p->crtc);
      ACE_ASSERT (crtc_info_p);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found output \"%s\" in X11 session \"%s\" [%dx%d+%dx%d]\n"),
                  ACE_TEXT (outputName_in.c_str ()),
                  (display_p ? ACE_TEXT (display_p) : ACE_TEXT ("NULL")),
                  crtc_info_p->x, crtc_info_p->y, crtc_info_p->width, crtc_info_p->height));
      XRRFreeCrtcInfo (crtc_info_p); crtc_info_p = NULL;
      XRRFreeOutputInfo (output_info_p); output_info_p = NULL;
    } // end FOR
    XRRFreeScreenResources (screen_resources_p); screen_resources_p = NULL;
  } // end FOR

  result = XCloseDisplay (display_2);
  if (result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XCloseDisplay(), continuing\n")));

  return (display_p ? ACE_TEXT (display_p) : ACE_TEXT ("NULL"));
}

XWindowAttributes
Common_UI_Tools::get (const struct _XDisplay& display_in,
                      Window id_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::get"));

  // initialize return value(s)
  XWindowAttributes return_value;
  ACE_OS::memset (&return_value, 0, sizeof (XWindowAttributes));

  Status result = XGetWindowAttributes (&const_cast<Display&> (display_in),
                                        id_in,
                                        &return_value);
  ACE_ASSERT (result == True); ACE_UNUSED_ARG (result);

  return return_value;
}

void
Common_UI_Tools::dump (const struct _XDisplay& display_in,
                       Drawable id_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::dump"));

  Window window_i = 0;
  int relative_position_x = 0, relative_position_y = 0;
  Common_UI_Resolution_t resolution_s;
  unsigned int border_width_i = 0, depth_i = 0;
  Status result = XGetGeometry (&const_cast<Display&> (display_in),
                                id_in,
                                &window_i,
                                &relative_position_x, &relative_position_y,
                                &resolution_s.width, &resolution_s.height,
                                &border_width_i,
                                &depth_i);
  ACE_ASSERT (result == True); ACE_UNUSED_ARG (result);
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("drawable %u:\n\twindow: %u\n\tx/y: [%u,%u]; size: [%ux%u]\n\tborder: %u; depth: %u\n"),
              id_in,
              window_i,
              relative_position_x, relative_position_y, resolution_s.width, resolution_s.height,
              border_width_i, depth_i));
}

std::string
Common_UI_Tools::toString (const struct _XDisplay& display_in,
                           int errorCode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::toString"));

  // initialize return value(s)
  std::string return_value;

  char buffer_a[BUFSIZ];
  ACE_OS::memset (&buffer_a, 0, sizeof (char[BUFSIZ]));

  Status result = XGetErrorText (&const_cast<Display&> (display_in),
                                 errorCode_in,
                                 buffer_a, sizeof (char[BUFSIZ]));
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XGetErrorText(0x%@,%d): \"%m\", aborting\n"),
                &display_in,
                errorCode_in));
    return return_value;
  } // end IF
  return_value = buffer_a;

  return return_value;
}

Common_UI_Resolution_t
Common_UI_Tools::toResolution (const struct _XDisplay& display_in,
                               Window window_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::toResolution"));

  Common_UI_Resolution_t return_value;

  XWindowAttributes attributes_s;
  ACE_OS::memset (&attributes_s, 0, sizeof (XWindowAttributes));
  Status result = XGetWindowAttributes (&const_cast<Display&> (display_in),
                                        window_in,
                                        &attributes_s);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XGetWindowAttributes(0x%@,%u): \"%s\", aborting\n"),
                &display_in, window_in,
                ACE_TEXT (Common_UI_Tools::toString (display_in, result).c_str ())));
    return return_value;
  } // end IF
  return_value.width = attributes_s.width;
  return_value.height = attributes_s.height;

  return return_value;
}
#endif // ACE_WIN32 || ACE_WIN64

// ---------------------------------------

Common_UI_Resolutions_t
Common_UI_Tools::get (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::get"));

  Common_UI_Resolutions_t result;

  std::string device_identifier_string = deviceIdentifier_in;
  struct Common_UI_DisplayDevice display_device_s;
  if (device_identifier_string.empty ())
  {
    display_device_s = Common_UI_Tools::getDefaultDisplay ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_ASSERT (display_device_s.handle != NULL);
#endif // ACE_WIN32 || ACE_WIN64
    device_identifier_string = display_device_s.device;
  } // end IF
  // *NOTE*: devices becoming plug-and-play these days there is little sense in
  //         handling any errors here
  ACE_ASSERT (Common_UI_Tools::is (device_identifier_string));

  Common_UI_Resolution_t resolution_s;
  std::string device_name_string;
  if (Common_UI_Tools::isAdapter (device_identifier_string))
  {
    struct Common_UI_DisplayAdapter display_adapter_s =
      Common_UI_Tools::getAdapter (device_identifier_string);
    for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator = display_adapter_s.heads.begin ();
         iterator != display_adapter_s.heads.end ();
         ++iterator)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT ((*iterator).index != std::numeric_limits<DWORD>::max ());
#endif // ACE_WIN32 || ACE_WIN64
      // *TODO*: this should make more sense
//      if (!ACE_OS::strcmp ((*iterator).device.c_str (),
//                           device_identifier_string.c_str ()))
//      {
        device_name_string = (*iterator).device;
        break;
//      } // end IF
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (Common_UI_Tools::isDisplay (device_identifier_string));
    display_device_s = Common_UI_Tools::getDisplay (device_identifier_string);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    device_name_string = display_device_s.device;
    ACE_ASSERT (display_device_s.handle != NULL);
    // *NOTE*: apparently, EnumDisplaySettingsEx() supports only adapter modes
    //         --> crop device name pseudo-path to adapter name
    std::string::size_type position =
      device_name_string.find_last_of ('\\', std::string::npos);
    ACE_ASSERT (position != std::string::npos);
    device_name_string.erase (position, std::string::npos);
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("cropping device name %s to adapter name %s for EnumDisplaySettingsEx()\n"),
                ACE_TEXT (display_device_s.device.c_str ()),
                ACE_TEXT (device_name_string.c_str ())));
#endif // _DEBUG
#elif defined (ACE_LINUX)
    struct Common_UI_DisplayAdapter display_adapter_s =
      Common_UI_Tools::getAdapter (display_device_s);
    device_name_string = display_adapter_s.device;
#endif
  } // end ELSE
  ACE_ASSERT (!device_name_string.empty ());
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD index_i = 0;
  DEVMODE device_mode_s;
  ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  device_mode_s.dmSize = sizeof (DEVMODE);
  //device_mode_s.dmDriverExtra = 0;
  DWORD dwFlags = 0;
    //(EDS_RAWMODE |
    // EDS_ROTATEDMODE);
  do
  {
#if defined (UNICODE)
    if (!EnumDisplaySettingsEx (ACE_TEXT_ALWAYS_WCHAR (device_name_string.c_str ()),
#else
    if (!EnumDisplaySettingsEx (device_name_string.c_str (),
#endif // UNICODE
                                index_i++,
                                &device_mode_s,
                                dwFlags))
      break;
    ACE_ASSERT (device_mode_s.dmFields & DM_PELSWIDTH);
    ACE_ASSERT (device_mode_s.dmFields & DM_PELSHEIGHT);
    resolution_s.cx = device_mode_s.dmPelsWidth;
    resolution_s.cy = device_mode_s.dmPelsHeight;
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("device \"%s\" supports resolution %ux%u\n"),
                  ACE_TEXT (deviceIdentifier_in.c_str ()),
                  resolution_s.cx, resolution_s.cy));
#endif // _DEBUG
    result.push_back (resolution_s);
  } while (true);
#else
  // *NOTE*: /sys/class/drm/cardx/cardx-displayy/'modes' contains the supported
  //         resolutions
  std::string modes_file, buffer_string;
  std::string regex_string =
      ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]+)x([[:digit:]]+)$");
  std::regex regex (regex_string);
  std::smatch match_results;
  std::istringstream converter, converter_2;
  uint8_t* buffer_p = NULL;
  char buffer_a[BUFSIZ];
  ACE_UINT64 file_size_i = 0;
  modes_file = device_name_string;
  modes_file += ACE_DIRECTORY_SEPARATOR_STR;
  std::string::size_type position =
    device_name_string.find_last_of ('/', std::string::npos);
  ACE_ASSERT (position != std::string::npos);
  modes_file += device_name_string.substr (position + 1, std::string::npos);
  modes_file += ACE_TEXT_ALWAYS_CHAR ("-");
  modes_file += deviceIdentifier_in;
  modes_file += ACE_DIRECTORY_SEPARATOR_STR;
  modes_file += ACE_TEXT_ALWAYS_CHAR ("modes");
  ACE_ASSERT (Common_File_Tools::isReadable (modes_file));
  if (unlikely (!Common_File_Tools::load (modes_file,
                                          buffer_p,
                                          file_size_i,
                                          0)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                ACE_TEXT (modes_file.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (buffer_p);
  buffer_string = reinterpret_cast<char*> (buffer_p);
  delete [] buffer_p; buffer_p = NULL;
  converter.str (buffer_string);
  // parse resolution entries
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
    ACE_ASSERT (match_results[2].matched && !match_results[2].str ().empty ());
    converter_2.clear ();
    converter_2.str (match_results[1].str ());
    converter_2 >> resolution_s.width;
    converter_2.clear ();
    converter_2.str (match_results[2].str ());
    converter_2 >> resolution_s.height;
    result.push_back (resolution_s);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("device \"%s\" supports resolution %ux%u\n"),
                ACE_TEXT (deviceIdentifier_in.c_str ()),
                resolution_s.width, resolution_s.height));
  } while (!converter.fail ());
#endif // ACE_WIN32 || ACE_WIN64
  result.sort (common_ui_resolution_less ());
  result.unique (common_ui_resolution_equal ());

  return result;
}

bool
Common_UI_Tools::has (const std::string& deviceIdentifier_in,
                      const Common_UI_Resolution_t& resolution_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::has"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::string device_identifier_string = deviceIdentifier_in;
  if (device_identifier_string.empty ())
  {
    struct Common_UI_DisplayDevice display_device_s =
      Common_UI_Tools::getDefaultDisplay ();
    ACE_ASSERT (display_device_s.handle != NULL);
    device_identifier_string = display_device_s.device;
  } // end IF

  std::string device_name_string;
  // *IMPORTANT NOTE*: devices becoming plug-and-play these days there is little
  //                   sense in doing much error handling around here
  ACE_ASSERT (Common_UI_Tools::is (device_identifier_string));
  if (Common_UI_Tools::isAdapter (device_identifier_string))
  {
    struct Common_UI_DisplayAdapter display_adapter_s =
      Common_UI_Tools::getAdapter (device_identifier_string);
    for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator = display_adapter_s.heads.begin ();
         iterator != display_adapter_s.heads.end ();
         ++iterator)
    { ACE_ASSERT ((*iterator).index != std::numeric_limits<DWORD>::max ());
      if (!ACE_OS::strcmp ((*iterator).device.c_str (),
                           device_identifier_string.c_str ()))
      {
        device_name_string = (*iterator).device;
        break;
      } // end IF
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (Common_UI_Tools::isDisplay (device_identifier_string));
    struct Common_UI_DisplayDevice display_device_s =
      Common_UI_Tools::getDisplay (deviceIdentifier_in);
    ACE_ASSERT (display_device_s.handle != NULL);
    device_name_string = display_device_s.device;
  } // end ELSE
  DWORD index_i = 0;
  DEVMODE device_mode_s;
  ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  device_mode_s.dmSize = sizeof (DEVMODE);
  //device_mode_s.dmDriverExtra = 0;
  DWORD dwFlags = (EDS_RAWMODE |
                   EDS_ROTATEDMODE);
  do
  {
#if defined (UNICODE)
    if (!EnumDisplaySettingsEx (ACE_TEXT_ALWAYS_WCHAR (device_name_string.c_str ()),
#else
    if (!EnumDisplaySettingsEx (device_name_string.c_str (),
#endif // UNICODE
                                index_i,
                                &device_mode_s,
                                dwFlags))
      break;
    ACE_ASSERT (device_mode_s.dmFields & DM_PELSWIDTH);
    ACE_ASSERT (device_mode_s.dmFields & DM_PELSHEIGHT);
    if ((device_mode_s.dmPelsWidth == resolution_in.cx) &&
        (device_mode_s.dmPelsHeight == resolution_in.cy))
      return true;
  } while (true);
#else
    ACE_UNUSED_ARG (deviceIdentifier_in);
    ACE_UNUSED_ARG (resolution_in);

    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (false);
    ACE_NOTREACHED (return false;)
#endif // ACE_WIN32 || ACE_WIN64

  return false;
}

struct Common_UI_DisplayMode
Common_UI_Tools::mode (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::mode"));

  struct Common_UI_DisplayMode result;
  ACE_OS::memset (&result, 0, sizeof (struct Common_UI_DisplayMode));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::string device_identifier_string = deviceIdentifier_in;
  if (device_identifier_string.empty ())
  {
    struct Common_UI_DisplayDevice display_device_s =
      Common_UI_Tools::getDefaultDisplay ();
    ACE_ASSERT (display_device_s.handle != NULL);
    device_identifier_string = display_device_s.device;
  } // end IF

  std::string device_name_string;
  // *IMPORTANT NOTE*: devices becoming plug-and-play these days there is little
  //                   sense in doing much error handling around here
  ACE_ASSERT (Common_UI_Tools::is (device_identifier_string));
  if (Common_UI_Tools::isAdapter (device_identifier_string))
  {
    struct Common_UI_DisplayAdapter display_adapter_s =
      Common_UI_Tools::getAdapter (device_identifier_string);
    for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator = display_adapter_s.heads.begin ();
         iterator != display_adapter_s.heads.end ();
         ++iterator)
    { ACE_ASSERT ((*iterator).index != std::numeric_limits<DWORD>::max ());
      if (!ACE_OS::strcmp ((*iterator).device.c_str (),
                           device_identifier_string.c_str ()))
      {
        device_name_string = (*iterator).device;
        break;
      } // end IF
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (Common_UI_Tools::isDisplay (device_identifier_string));
    struct Common_UI_DisplayDevice display_device_s =
      Common_UI_Tools::getDisplay (device_identifier_string);
    ACE_ASSERT (display_device_s.handle != NULL);
    device_name_string = display_device_s.device;
  } // end ELSE
  DEVMODE device_mode_s;
  ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  device_mode_s.dmSize = sizeof (DEVMODE);
  //device_mode_s.dmDriverExtra = 0;
  DWORD dwFlags = (EDS_RAWMODE |
                   EDS_ROTATEDMODE);
#if defined (UNICODE)
  if (!EnumDisplaySettingsEx (ACE_TEXT_ALWAYS_WCHAR (device_name_string.c_str ()),
#else
  if (!EnumDisplaySettingsEx (device_name_string.c_str (),
#endif // UNICODE
                              ENUM_CURRENT_SETTINGS,
                              &device_mode_s,
                              dwFlags))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumDisplaySettingsEx(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (device_name_string.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (device_mode_s.dmFields & DM_BITSPERPEL);
  ACE_ASSERT (device_mode_s.dmFields & DM_PELSWIDTH);
  ACE_ASSERT (device_mode_s.dmFields & DM_PELSHEIGHT);
  ACE_ASSERT (device_mode_s.dmFields & DM_DISPLAYFLAGS);
  ACE_ASSERT (device_mode_s.dmFields & DM_POSITION);
  ACE_ASSERT (device_mode_s.dmFields & DM_DISPLAYORIENTATION);
  result.bitsperpixel =
    static_cast<uint8_t> (device_mode_s.dmBitsPerPel);
  result.frequency =
    static_cast<uint8_t> (device_mode_s.dmDisplayFrequency);
  //result.fullscreen =
  //  (device_mode_s.dmDisplayFlags & );
  result.resolution.cx = device_mode_s.dmPelsWidth;
  result.resolution.cy = device_mode_s.dmPelsHeight;
#else
    ACE_UNUSED_ARG (deviceIdentifier_in);

    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (result);
    ACE_NOTREACHED (return result;)
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

Common_UI_Resolution_t
Common_UI_Tools::nearest (const Common_UI_Resolutions_t& resolutions_in,
                          const Common_UI_Resolution_t& resolution_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::nearest"));

  // initialize return value(s)
  Common_UI_Resolution_t return_value;
  ACE_OS::memset (&return_value, 0, sizeof (Common_UI_Resolution_t));

  // sanity check(s)
  ACE_ASSERT (!resolutions_in.empty ());

  Common_UI_Resolutions_t resolutions_a = resolutions_in;
  resolutions_a.sort (common_ui_resolution_less ());
  resolutions_a.unique (common_ui_resolution_equal ());
  Common_UI_ResolutionsConstIterator_t iterator_2;
  for (Common_UI_ResolutionsConstIterator_t iterator = resolutions_a.begin ();
       iterator != resolutions_a.end ();
       ++iterator)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((*iterator).cx > resolution_in.cx)
#else
    if ((*iterator).width > resolution_in.width)
#endif // ACE_WIN32 || ACE_WIN64
      return *iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((*iterator).cx == resolution_in.cx)
#else
    if ((*iterator).width == resolution_in.width)
#endif // ACE_WIN32 || ACE_WIN64
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if ((*iterator).cy >= resolution_in.cy)
#else
      if ((*iterator).height >= resolution_in.height)
#endif // ACE_WIN32 || ACE_WIN64
        return *iterator;
      iterator_2 = iterator;
      if (++iterator_2 == resolutions_a.end ())
        return *iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if ((*iterator_2).cx > resolution_in.cx)
#else
      if ((*iterator_2).width > resolution_in.width)
#endif // ACE_WIN32 || ACE_WIN64
        return *iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if ((*iterator_2).cy > resolution_in.cy)
#else
      if ((*iterator_2).height > resolution_in.height)
#endif // ACE_WIN32 || ACE_WIN64
      { // the current y-resolution is smaller, the next larger
        // --> return the 'nearer' one
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        return (((*iterator_2).cy - resolution_in.cy) < (resolution_in.cy - (*iterator).cy) ? *iterator_2
#else
        return (((*iterator_2).height - resolution_in.height) < (resolution_in.height - (*iterator).height) ? *iterator_2
#endif // ACE_WIN32 || ACE_WIN64
                                                                                            : *iterator);
      } // end IF
      continue;
    } // end IF
    iterator_2 = iterator;
    if (++iterator_2 == resolutions_a.end ())
      return *iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((*iterator_2).cx > resolution_in.cx)
#else
    if ((*iterator_2).width > resolution_in.width)
#endif // ACE_WIN32 || ACE_WIN64
    { // the current x-resolution is smaller, the next resolution larger
      // --> return the 'nearest' one
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      return (((*iterator_2).cx - resolution_in.cx) < (resolution_in.cx - (*iterator).cx) ? *iterator_2
#else
      return (((*iterator_2).width - resolution_in.width) < (resolution_in.width - (*iterator).width) ? *iterator_2
#endif // ACE_WIN32 || ACE_WIN64
                                                                                          : *iterator);
    } // end IF
  } // end FOR

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (return_value);
  ACE_NOTREACHED (return return_value;)
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Common_UI_Tools::isMaximized (HWND window_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::isMaximized"));

  struct tagWINDOWPLACEMENT window_placement_s;
  ACE_OS::memset (&window_placement_s, 0, sizeof (struct tagWINDOWPLACEMENT));

  window_placement_s.length = sizeof(struct tagWINDOWPLACEMENT);
  if (unlikely (!GetWindowPlacement (window_in,
                                     &window_placement_s)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetWindowPlacement(%@): \"%s\", aborting\n"),
                window_in,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return false; // *TODO*: false negative
  } // end IF

  return (window_placement_s.showCmd == SW_SHOWMAXIMIZED);
}
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Common_UI_Tools::setConsoleFontSize (const struct _COORD& size_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::setConsoleFontSize"));

  HANDLE console_h = GetStdHandle (STD_OUTPUT_HANDLE);
  ACE_ASSERT (console_h != ACE_INVALID_HANDLE);

  struct _CONSOLE_FONT_INFOEX console_font_info_ex_s;
  ACE_OS::memset (&console_font_info_ex_s, 0, sizeof (struct _CONSOLE_FONT_INFOEX));
  console_font_info_ex_s.cbSize = sizeof (struct _CONSOLE_FONT_INFOEX);
  if (GetCurrentConsoleFontEx (console_h,
                               FALSE,
                               &console_font_info_ex_s) == FALSE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetCurrentConsoleFontEx(%@): \"%s\", aborting\n"),
                console_h,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return false;
  } // end IF

  console_font_info_ex_s.dwFontSize = size_in;
  if (SetCurrentConsoleFontEx (console_h,
                               FALSE,
                               &console_font_info_ex_s) == FALSE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetCurrentConsoleFontEx(%@): \"%s\", aborting\n"),
                console_h,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return false;
  } // end IF

  return true;
}

bool
Common_UI_Tools::setConsoleSize (const struct _COORD& size_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::setConsoleSize"));

  HANDLE console_h = GetStdHandle (STD_OUTPUT_HANDLE);
  ACE_ASSERT (console_h != ACE_INVALID_HANDLE);

  BOOL result = SetConsoleScreenBufferSize (console_h,
                                            size_in);
  if (unlikely (result == FALSE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetConsoleScreenBufferSize(%d,%d): \"%s\", returning\n"),
                size_in.X, size_in.Y,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return false;
  } // end IF

  struct _SMALL_RECT small_rect_s = {0, 0, 0, 0};
  small_rect_s.Right = size_in.X - 1;
  small_rect_s.Bottom = size_in.Y - 1;
  result = SetConsoleWindowInfo (console_h,
                                 TRUE,
                                 &small_rect_s);
  if (unlikely (result == FALSE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetConsoleWindowInfo(%@): \"%s\", returning\n"),
                console_h,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return false;
  } // end IF

  return true;
}

struct _SMALL_RECT
Common_UI_Tools::setConsoleMaxWindowSize (ACE_INT16 requestedWidth_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::setConsoleMaxWindowSize"));

  // sanity check(s)
  ACE_ASSERT (requestedWidth_in >= 0);

  HANDLE console_h = GetStdHandle (STD_OUTPUT_HANDLE);
  ACE_ASSERT (console_h != ACE_INVALID_HANDLE);

  struct _CONSOLE_SCREEN_BUFFER_INFOEX console_screen_buffer_info_ex_s;
  ACE_OS::memset (&console_screen_buffer_info_ex_s, 0, sizeof (struct _CONSOLE_SCREEN_BUFFER_INFOEX));
  console_screen_buffer_info_ex_s.cbSize = sizeof (struct _CONSOLE_SCREEN_BUFFER_INFOEX);
  BOOL result = GetConsoleScreenBufferInfoEx (console_h,
                                              &console_screen_buffer_info_ex_s);
  ACE_ASSERT (result == TRUE);

  if (unlikely (requestedWidth_in))
  {
    struct _COORD coord_s;
    coord_s.X = requestedWidth_in;
    coord_s.Y = console_screen_buffer_info_ex_s.dwSize.Y;
    result = SetConsoleScreenBufferSize (console_h,
                                         coord_s);
    ACE_ASSERT (result == TRUE);
    result = GetConsoleScreenBufferInfoEx (console_h,
                                           &console_screen_buffer_info_ex_s);
    ACE_ASSERT (result == TRUE);
  } // end IF

  struct _SMALL_RECT small_rect_s = {0, 0, 0, 0};
  small_rect_s.Right =
    (requestedWidth_in ? requestedWidth_in - 1 : console_screen_buffer_info_ex_s.dwMaximumWindowSize.X - 1);
  small_rect_s.Bottom =
    console_screen_buffer_info_ex_s.dwMaximumWindowSize.Y - 1;
  if (SetConsoleWindowInfo (console_h,
                            TRUE,
                            &small_rect_s) == FALSE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetConsoleWindowInfo(%@): \"%s\", returning\n"),
                console_h,
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false, false).c_str ())));
    return { 0, 0, 0, 0 };
  } // end IF

  return small_rect_s;
}
#else
bool
Common_UI_Tools::setConsoleFontSize (int factor_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::setConsoleFontSize"));

  std::string commandline_string = ACE_TEXT_ALWAYS_CHAR(COMMON_COMMAND_XDOTOOL);
  commandline_string += ACE_TEXT_ALWAYS_CHAR(" key ");
  for (int i = 0;
       i < std::abs (factor_in);
       ++i)
    commandline_string +=
      (factor_in < 0 ? ACE_TEXT_ALWAYS_CHAR (" ctrl+minus")
                     : ACE_TEXT_ALWAYS_CHAR (" ctrl+plus"));

  int exit_status = -1;
  std::string stdout_string;
  return Common_Process_Tools::command (commandline_string,
                                       exit_status,
                                       stdout_string,
                                       false);
}

bool
Common_UI_Tools::setConsoleSize (int width_in,
                                 int height_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::setConsoleSize"));

  std::string commandline_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_RESIZE);
  COMMON_COMMAND_ADD_SWITCH (commandline_string, COMMON_COMMAND_SWITCH_RESIZE_SUN_SET);
  commandline_string += ACE_TEXT_ALWAYS_CHAR (" ");
  std::ostringstream converter;
  converter << height_in;
  commandline_string += converter.str ();
  commandline_string += ACE_TEXT_ALWAYS_CHAR (" ");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << width_in;
  commandline_string += converter.str ();
  int exit_status = -1;
  std::string stdout_string;

  return Common_Process_Tools::command (commandline_string,
                                        exit_status,
                                        stdout_string,
                                        false);
}
#endif // ACE_WIN32 || ACE_WIN64
