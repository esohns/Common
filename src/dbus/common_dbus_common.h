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
#include <vector>

typedef std::vector<std::string> Common_DBus_ObjectPaths_t;
typedef Common_DBus_ObjectPaths_t::const_iterator Common_DBus_ObjectPathsIterator_t;

enum Common_DBus_PolicyKit_SubjectType
{
  COMMON_DBUS_POLICYKIT_SUBJECT_BUS_SYSTEM = 0,
  COMMON_DBUS_POLICYKIT_SUBJECT_BUS_USER,
  COMMON_DBUS_POLICYKIT_SUBJECT_PROCESS,
  ////////////////////////////////////////
  COMMON_DBUS_POLICYKIT_SUBJECT_MAX,
  COMMON_DBUS_POLICYKIT_SUBJECT_INVALID
};

typedef std::vector<std::pair<std::string, std::string> > Common_DBus_PolicyKit_Details_t;
typedef Common_DBus_PolicyKit_Details_t::const_iterator Common_DBus_PolicyKit_DetailsIterator_t;

struct Common_DBus_PolicyKit_CheckAuthorizationCBData
{
  Common_DBus_PolicyKit_CheckAuthorizationCBData ()
   : authorized (0)
   , challenge (0)
   , details ()
   , done (false)
  {}

  int                             authorized;
  int                             challenge;
  Common_DBus_PolicyKit_Details_t details;
  bool                            done;
};

#if defined (SD_BUS_SUPPORT)
struct Common_DBus_SignalSystemdJobCBData
{
  Common_DBus_SignalSystemdJobCBData ()
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
