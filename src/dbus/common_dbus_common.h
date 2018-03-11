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

#ifndef COMMON_DBUS_COMMON_H
#define COMMON_DBUS_COMMON_H

#include <string>

#if defined (SD_BUS_SUPPORT)
struct Common_DBusSignalSystemdJobCBData
{
  Common_DBusSignalSystemdJobCBData ()
   : id (0)
   , objectPath ()
   , result ()
   , unit ()
  {}

  unsigned int id;
  std::string  objectPath;
  std::string  result;
  std::string  unit;
};
#endif // SD_BUS_SUPPORT

#endif
