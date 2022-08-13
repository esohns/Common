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

#ifndef COMMON_H
#define COMMON_H

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <list>

#include "guiddef.h"
#else
#include <string>
#include <vector>
#endif // ACE_WIN32 || ACE_WIN64

#include "common_defines.h"

// *** platform ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct common_equal_guid
{
  inline bool operator() (const struct _GUID& lhs_in, const struct _GUID& rhs_in) const { return static_cast<bool> (InlineIsEqualGUID (lhs_in, rhs_in)); }
};
struct common_less_guid
{
  inline bool operator() (const struct _GUID& lhs_in, const struct _GUID& rhs_in) const { return (lhs_in.Data1 < rhs_in.Data1); }
};

typedef std::list<struct _GUID> Common_Identifiers_t;
typedef Common_Identifiers_t::iterator Common_IdentifiersIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

enum Common_OperatingSystemType
{
  COMMON_OPERATINGSYSTEM_GNU_LINUX = 0,
  COMMON_OPERATINGSYSTEM_WIN32,
  ///////////////////////////////////////
  COMMON_OPERATINGSYSTEM_INVALID,
  COMMON_OPERATINGSYSTEM_MAX
};

enum Common_OperatingSystemDistributionType
{
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN = 0,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_REDHAT,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_WIN32_MICROSOFT,
  ///////////////////////////////////////
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_MAX
};

// *** user/groups ***

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef std::vector<std::string> Common_UserGroups_t;
typedef Common_UserGroups_t::const_iterator Common_UserGroupsIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

// *** application ***

struct Common_ApplicationVersion
{
  unsigned int majorVersion;
  unsigned int minorVersion;
  unsigned int microVersion;
};

enum Common_ApplicationModeType
{
  COMMON_APPLICATION_MODE_DEBUG = 0,
  COMMON_APPLICATION_MODE_INSTALL,
  COMMON_APPLICATION_MODE_PRINT, // i.e. usage/version/etc
  COMMON_APPLICATION_MODE_RUN,
  COMMON_APPLICATION_MODE_TEST,
  COMMON_APPLICATION_MODE_UNINSTALL,
  ////////////////////////////////////////
  COMMON_APPLICATION_MODE_MAX,
  COMMON_APPLICATION_MODE_INVALID
};

#endif
