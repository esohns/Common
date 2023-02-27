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

#ifndef COMMON_OS_TOOLS_H
#define COMMON_OS_TOOLS_H

#include <sstream>
#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define __CGUID_H__
#include "combaseapi.h"
#include "guiddef.h"
#include "minwindef.h"
#else
#include "sys/capability.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/ACE.h"
#include "ace/Global_Macros.h"

#include "common.h"
#include "common_configuration.h"
#include "common_iinitialize.h"

// forward declaration(s)
class ACE_Event_Handler;

class Common_OS_Tools
{
 public:
  // --- locale / internationalization ---
  static void printLocales ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // --- capabilities ---
  static std::string capabilityToString (unsigned long); // capability
  inline static bool canCapability (unsigned long capability_in) { return Common_Tools::hasCapability (capability_in, CAP_PERMITTED); }
  static bool hasCapability (unsigned long,               // capability
                             cap_flag_t = CAP_EFFECTIVE); // set
  inline static bool isCapable (unsigned long capability_in) { return Common_Tools::hasCapability (capability_in, CAP_EFFECTIVE); }
  static bool setCapability (unsigned long,               // capability
                             cap_flag_t = CAP_EFFECTIVE); // set
  static bool dropCapability (unsigned long,               // capability
                              cap_flag_t = CAP_EFFECTIVE); // set
  static void printCapabilities ();
#endif // ACE_WIN32 || ACE_WIN64

  // --- resource usage ---
  static bool setResourceLimits (bool = false,  // #file descriptors (i.e. open handles)
#if defined (_DEBUG)
                                 bool = true,   // stack trace/sizes (i.e. core file sizes)
#else
                                 bool = false,  // stack trace/sizes (i.e. core file sizes)
#endif // _DEBUG
                                 bool = false); // pending (rt) signals

  // --- user ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static void getUserName (std::string&,  // return value: user name
                           std::string&); // return value: "real" name (if any)
#else
  static void getUserName (uid_t,         // user id {-1: euid}
                           std::string&,  // return value: user name
                           std::string&); // return value: "real" name (if any)
  static std::string getUserName (); // return value: current user name

  // (effective) uid
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  static bool switchUser (uid_t); // {-1: uid}

  // group
  static Common_UserGroups_t getUserGroups (uid_t); // user id {-1: euid}

  // *NOTE*: the persisting version needs write access to /etc/gshadow
  // *NOTE*: the 'cap_dac_override' capability does not grant this permission
  // *TODO*: find out why
  static bool addGroupMember (uid_t,        // user id {-1: euid}
                              gid_t,        // group id
                              bool = true); // persist ?
  static bool isGroupMember (uid_t,  // user id {-1: euid}
                             gid_t); // group id

  static std::string groupIdToString (gid_t); // group id
  static gid_t stringToGroupId (const std::string&); // group name
#endif // ACE_WIN32 || ACE_WIN64
  static void printUserIds ();

  // --- environment ---
  static std::string environment (const std::string&); // environment variable

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // --- UID ---
  static std::string GUIDToString (REFGUID);
  static struct _GUID StringToGUID (const std::string&);
  static bool isGUID (const std::string&);

  // --- registry ---
  static std::string getKeyValue (HKEY,                // parent key
                                  const std::string&,  // subkey
                                  const std::string&); // value
  static bool deleteKey (HKEY,                // parent key
                         const std::string&); // subkey
  static bool deleteKeyValue (HKEY,                // parent key
                              const std::string&,  // subkey
                              const std::string&); // value
#endif // ACE_WIN32 || ACE_WIN64

  // --- (locally installed-) (UNIX) commands / programs ---
  // *NOTE*: the Linux implementation relies on 'locate', 'xargs' and 'find'
  //         (and 'which' to locate 'locate' itself)
  static bool isInstalled (const std::string&, // executable (base-)name
                           std::string&);      // return value: (FQ) path

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_OS_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_OS_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_OS_Tools (const Common_OS_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_OS_Tools& operator= (const Common_OS_Tools&))

  // --- (locally installed-) (UNIX) commands / programs ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *NOTE*: the Linux implementation relies on 'which'
  static bool findProgram (const std::string&);
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
