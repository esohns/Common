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

#ifndef COMMON_DBUS_TOOLS_H
#define COMMON_DBUS_TOOLS_H

#if defined (SD_BUS_SUPPORT)
#include <string>
#endif // SD_BUS_SUPPORT

#if defined (DBUS_SUPPORT)
#include "dbus/dbus.h"
#endif // DBUS_SUPPORT

#include "ace/Global_Macros.h"

//#include "common_exports.h"

// forward declarations
#if defined (SD_BUS_SUPPORT)
struct sd_bus;
#endif // SD_BUS_SUPPORT

//class Common_Export Common_DBus_Tools
class Common_DBus_Tools
{
 public:
  inline virtual ~Common_DBus_Tools () {}

#if defined (DBUS_SUPPORT)
  // *IMPORTANT NOTE* fire-and-forget the second argument
  static struct DBusMessage* exchange (struct DBusConnection*,       // connection handle
                                       struct DBusMessage*&,         // outbound message handle
                                       int = DBUS_TIMEOUT_INFINITE); // timeout (ms) {default: block}
  static bool validateType (struct DBusMessageIter&,  // message iterator
                            int = DBUS_TYPE_INVALID); // expected type
#endif // DBUS_SUPPORT

#if defined (SD_BUS_SUPPORT)
  static std::string unitToObjectPath (struct sd_bus*,
                                       const std::string&); // unit name (e.g. 'NetworkManager.service')

  static bool isUnitRunning (const std::string&); // unit name (e.g. 'NetworkManager.service')
  static bool toggleUnit (const std::string&, // unit name (e.g. 'NetworkManager.service')
                          bool = false);      // wait for completion ?
#endif // SD_BUS_SUPPORT

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools ())
//  ACE_UNIMPLEMENTED_FUNC (virtual ~Common_DBus_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools (const Common_DBus_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools& operator= (const Common_DBus_Tools&))
};

#endif
