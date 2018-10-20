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
#include <d3d9.h>
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
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

Common_UI_DisplayAdapters_t
Common_UI_Tools::getAdapters (bool excludeMirroringDevices_in)
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
#if defined (_DEBUG)
  std::string device_feature_string;
#endif // _DEBUG
  do
  {
    if (!EnumDisplayDevices (NULL,              // lpDevice
                             index_i,           // iDevNum
                             &display_device_s, // lpDisplayDevice
                             flags_i))          // dwFlags
      break;
#if defined (_DEBUG)
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
#endif // _DEBUG
    if (excludeMirroringDevices_in &&
        (display_device_s.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
      goto continue_;
    // check if it is another head on an existing adapter
    display_adapter_s.id =
#if defined (UNICODE)
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceID));
#else
      display_device_s.DeviceID;
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
    display_adapter_p->id =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceID));
    display_adapter_p->description =
      ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_s.DeviceString));
#else
    display_adapter_head_s.device = display_device_s.DeviceName;
    display_adapter_head_s.key = display_device_s.DeviceKey;
    display_adapter_p->id = display_device_s.DeviceID;
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

  for (Common_UI_DisplayAdaptersConstIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (!ACE_OS::strcmp ((*iterator).id.c_str (),
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
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)
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
  Common_UI_DisplayAdapters_t display_adapters_a =
    Common_UI_Tools::getAdapters ();
  //DWORD index_i = 0;
  DISPLAY_DEVICE display_device_2;
  ACE_OS::memset (&display_device_2, 0, sizeof (DISPLAY_DEVICE));
  display_device_2.cb = sizeof (DISPLAY_DEVICE);
  DWORD flags_i = EDD_GET_DEVICE_INTERFACE_NAME;
  //HDC device_context_p = NULL;
  //DEVMODE device_mode_s;
  //ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  //device_mode_s.dmSize = sizeof (DEVMODE);
  ////device_mode_s.dmDriverExtra = 0;
  //DWORD dwFlags = (EDS_RAWMODE |
  //                 EDS_ROTATEDMODE);
  for (Common_UI_DisplayAdaptersConstIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
    for (Common_UI_DisplayAdapterHeadsConstIterator_t iterator_2 = (*iterator).heads.begin ();
         iterator_2 != (*iterator).heads.end ();
         ++iterator_2)
    {
      //index_i = 0;
      //do
      //{
#if defined (UNICODE)
        if (!EnumDisplayDevices (ACE_TEXT_ALWAYS_WCHAR ((*iterator_2).device.c_str ()), // lpDevice
#else
        if (!EnumDisplayDevices ((*iterator_2).device.c_str (),                         // lpDevice
#endif // UNICODE
                                 //index_i,                                               // iDevNum
                                 (*iterator_2).index,
                                 &display_device_2,                                     // lpDisplayDevice
                                 flags_i))                                              // dwFlags
          break;
#if defined (_DEBUG)
#if defined (UNICODE)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\" [%s]: found display device #%d: \"%s\" [%s]\n"),
                  ACE_TEXT ((*iterator).description.c_str ()),
                  ACE_TEXT ((*iterator_2).device.c_str ()),
                  //index_i,
                  (*iterator_2).index,
                  ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceString),
                  ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceName)));

#else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\" [%s]: found display device #%d: \"%s\" [%s]\n"),
                  ACE_TEXT ((*iterator).description.c_str ()),
                  ACE_TEXT ((*iterator_2).device.c_str ()),
                  //index_i,
                  (*iterator_2).index,
                  ACE_TEXT (display_device_2.DeviceString),
                  ACE_TEXT (display_device_2.DeviceName)));
#endif // UNICODE
#endif // _DEBUG
        display_device_s.id =
#if defined (UNICODE)
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceID);
#else
          display_device_2.DeviceID;
#endif // UNICODE
        display_device_s.key =
#if defined (UNICODE)
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceKey);
#else
          display_device_2.DeviceKey;
#endif // UNICODE
        display_device_s.device =
#if defined (UNICODE)
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceName);
#else
          display_device_2.DeviceName;
#endif // UNICODE
        display_device_s.description =
#if defined (UNICODE)
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (display_device_2.DeviceString);
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
      //  ++index_i;
      //} while (true);
    } // end FOR
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
Common_UI_Tools::getDisplay (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getDisplay"));

  // initialize return value(s)
  struct Common_UI_DisplayDevice result;

  Common_UI_DisplayDevices_t display_devices_a =
    Common_UI_Tools::getDisplays ();

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

struct Common_UI_DisplayAdapter
Common_UI_Tools::getAdapter (const struct Common_UI_DisplayDevice& device_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::getAdapter"));

  struct Common_UI_DisplayAdapter result;

  // sanity check(s)
  ACE_ASSERT (device_in.handle != NULL);

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
      if ((*iterator_2).handle == device_in.handle)
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

// ---------------------------------------

Common_UI_Resolutions_t
Common_UI_Tools::get (const std::string& deviceIdentifier_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Tools::get"));

  Common_UI_Resolutions_t result;

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
  } // end ELSE
  DWORD index_i = 0;
  DEVMODE device_mode_s;
  ACE_OS::memset (&device_mode_s, 0, sizeof (DEVMODE));
  device_mode_s.dmSize = sizeof (DEVMODE);
  //device_mode_s.dmDriverExtra = 0;
  DWORD dwFlags = 0;
    //(EDS_RAWMODE |
    // EDS_ROTATEDMODE);
  Common_UI_Resolution_t resolution_s;
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
    result.push_back (resolution_s);
  } while (true);
#else
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (false);
    ACE_NOTREACHED (return false;)
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
    if ((*iterator).cx > resolution_in.cx)
      return *iterator;
    if ((*iterator).cx == resolution_in.cx)
    {
      if ((*iterator).cy >= resolution_in.cy)
        return *iterator;
      iterator_2 = iterator;
      if (++iterator_2 == resolutions_a.end ())
        return *iterator;
      if ((*iterator_2).cx > resolution_in.cx)
        return *iterator;
      if ((*iterator_2).cy > resolution_in.cy)
      { // the current y-resolution is smaller, the next larger
        // --> return the 'nearer' one
        return (((*iterator_2).cy - resolution_in.cy) < (resolution_in.cy - (*iterator).cy) ? *iterator_2
                                                                                            : *iterator);
      } // end IF
      continue;
    } // end IF
    iterator_2 = iterator;
    if (++iterator_2 == resolutions_a.end ())
      return *iterator;
    if ((*iterator_2).cx > resolution_in.cx)
    { // the current x-resolution is smaller, the next resolution larger
      // --> return the 'nearest' one
      return (((*iterator_2).cx - resolution_in.cx) < (resolution_in.cx - (*iterator).cx) ? *iterator_2
                                                                                          : *iterator);
    } // end IF
  } // end FOR

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (return_value);
  ACE_NOTREACHED (return return_value;)
}
