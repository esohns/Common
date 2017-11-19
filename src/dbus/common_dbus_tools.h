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

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "dbus/dbus.h"
#endif

#include "ace/Global_Macros.h"

//#include "common_exports.h"

//class Common_Export Common_DBus_Tools
class Common_DBus_Tools
{
 public:
  inline virtual ~Common_DBus_Tools () {}

  // *IMPORTANT NOTE* fire-and-forget the second argument
  static struct DBusMessage* exchange (struct DBusConnection*,       // connection handle
                                       struct DBusMessage*&,         // outbound message handle
                                       int = DBUS_TIMEOUT_INFINITE); // timeout (ms) {default: block}
  static bool validateType (struct DBusMessageIter&,  // message iterator
                            int = DBUS_TYPE_INVALID); // expected type

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools ())
//  ACE_UNIMPLEMENTED_FUNC (virtual ~Common_DBus_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools (const Common_DBus_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_DBus_Tools& operator= (const Common_DBus_Tools&))
};

#endif
