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

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "X11/Xlib.h"
#endif // ACE_WIN32 || ACE_WIN64

#include <string>

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

#include "common_ui_common.h"

class Common_UI_Tools
 : public Common_SInitializeFinalize_T<Common_UI_Tools>
{
 public:
  static bool initialize ();
  static bool finalize ();

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

  // *IMPORTANT NOTE*: for reasons yet unknown, xrandr outputs display
  //                   identifiers of the form HDMI-1, HDMI-2, etc, whereas the
  //                   display adapter connectors under /sys/class/cardx/cardx-yyyy
  //                   have the form HDMI-A-1, HDMI-A-2, etc
  // *TODO*: find out why this is
  static bool displayMatches (const std::string&,
                              const std::string&);
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static void dump (const struct _XDisplay&, // connection handle
                    Drawable);               // drawable (window or pixmap)
  static XWindowAttributes get (const struct _XDisplay&, // connection handle
                                Window);                 // window
  // *NOTE*: queries the display name from the 'DISPLAY' environment to verify
  //         that the X11 session is using the screen connected to the given
  //         output
  static std::string getX11DisplayName (const std::string&); // output name

  static Common_UI_Resolution_t toResolution (const struct _XDisplay&, // display
                                              Window);                 // window
  static std::string toString (const struct _XDisplay&, // display
                               int);                    // error code
#endif // ACE_WIN32 || ACE_WIN64

  // *NOTE*: the identifier may specify either a graphics adapter or a display
  // *NOTE*: iff the identifier specifies a graphics adapter, this returns the
  //         resolutions of the first (!) connected display (if any)
  static Common_UI_Resolutions_t get (const std::string&); // device identifier
  static bool has (const std::string&,                // device identifier
                   const Common_UI_Resolution_t&); // resolution
  static struct Common_UI_DisplayMode mode (const std::string&); // device identifier
  // *NOTE*: this implementation 'prefers' width over height and exactness over
  //         area; IOW it is still pretty naive
  static Common_UI_Resolution_t nearest (const Common_UI_Resolutions_t&, // resolutions
                                         const Common_UI_Resolution_t&); // resolution

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools (const Common_UI_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools& operator= (const Common_UI_Tools&))

  // helper methods
  inline static bool is (const std::string& deviceIdentifier_in) { return (Common_UI_Tools::isAdapter (deviceIdentifier_in) || Common_UI_Tools::isDisplay (deviceIdentifier_in)); }
  inline static bool isAdapter (const std::string& deviceIdentifier_in) { struct Common_UI_DisplayAdapter adapter_s = Common_UI_Tools::getAdapter (deviceIdentifier_in); return !adapter_s.device.empty (); }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inline static bool isDisplay (const std::string& deviceIdentifier_in) { struct Common_UI_DisplayDevice display_s = Common_UI_Tools::getDisplay (deviceIdentifier_in); return (display_s.handle != NULL); }
#else
  inline static bool isDisplay (const std::string& deviceIdentifier_in) { struct Common_UI_DisplayDevice display_s = Common_UI_Tools::getDisplay (deviceIdentifier_in); return !display_s.device.empty (); }
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
