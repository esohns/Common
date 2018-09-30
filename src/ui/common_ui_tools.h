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

#include "ace/Global_Macros.h"

#include "common_ui_common.h"

class Common_UI_Tools
{
 public:
  // *NOTE*: these refer to graphics adapters (i.e. cards/chips) connected
  //         to the motherboard/CPU, NOT 'physical' display devices
  inline static struct Common_UI_DisplayAdapter getDefaultAdapter () { return Common_UI_Tools::getAdapter (ACE_TEXT_ALWAYS_CHAR ("")); }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: FRYU, EnumDisplayDevices() returns multiple records for a single
  //         graphics adapter (one for each output (or 'head'), e.g.
  //         '\\.\DISPLAY1', '\\.\DISPLAY2', ...) --> compare 'DeviceID'
  static Common_UI_DisplayAdapters_t getAdapters (bool = true); // exclude 'mirroring' devices ?
#else
  static Common_UI_DisplayAdapters_t getAdapters (); // return value: available devices
#endif // ACE_WIN32 || ACE_WIN64
  // *NOTE*: on Win32 systems, callers may specify either a 'DeviceID' or
  //         'DeviceName', identifying an adapter or specific output (aka
  //         'head'), respectively
  static struct Common_UI_DisplayAdapter getAdapter (const std::string&); // device identifier
  static Common_UI_DisplayDevices_t getDisplays (const struct Common_UI_DisplayAdapter&); // return value: attached devices

  // *NOTE*: these refer to output (!) display devices (i.e. monitors, screens,
  //         projectors, headsets, etc), NOT graphics adapters
  inline static struct Common_UI_DisplayDevice getDefaultDisplay () { return Common_UI_Tools::getDisplay (ACE_TEXT_ALWAYS_CHAR ("")); }
  static Common_UI_DisplayDevices_t getDisplays (); // return value: connected devices
  static struct Common_UI_DisplayDevice getDisplay (const std::string&); // device identifier
  static struct Common_UI_DisplayAdapter getAdapter (const struct Common_UI_DisplayDevice&); // return value: corresponding graphics adapter (if any)
  static Common_UI_DisplayDevices_t getDesktopDisplays (); // return value: devices forming the desktop

  static Common_UI_Resolutions_t get (const std::string&); // device identifier
  static bool has (const std::string&,             // device identifier
                   const Common_UI_Resolution_t&); // resolution
  static struct Common_UI_DisplayMode mode (const std::string&); // device identifier

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools (const Common_UI_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools& operator= (const Common_UI_Tools&));

  // helper methods
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inline static bool is (const std::string& deviceIdentifier_in) { return (Common_UI_Tools::isAdapter (deviceIdentifier_in) || Common_UI_Tools::isDisplay (deviceIdentifier_in)); }
  inline static bool isAdapter (const std::string& deviceIdentifier_in) { struct Common_UI_DisplayAdapter adapter_s = Common_UI_Tools::getAdapter (deviceIdentifier_in); return !adapter_s.id.empty (); }
  inline static bool isDisplay (const std::string& deviceIdentifier_in) { struct Common_UI_DisplayDevice device_s = Common_UI_Tools::getDisplay (deviceIdentifier_in); return (device_s.handle != NULL); }
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
