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

#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <random>
#include <sstream>
#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
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

ACE_THR_FUNC_RETURN
common_event_dispatch_function (void*);

class Common_Tools
 : public Common_SInitializeFinalize_T<Common_Tools>
{
 public:
  static void initialize (bool = false); // initialize random number generator ?
  static void finalize ();

  // --- platform ---
  template <typename ValueType>
  static ValueType byteswap (ValueType);
  static unsigned int getNumberOfCPUs (bool = true); // consider logical cores (i.e. 'hyperthreading') ?
  static std::string getPlatformName ();
  static enum Common_OperatingSystemType getOperatingSystem ();
  inline static bool is (enum Common_OperatingSystemType operatingSytem_in) { return (Common_Tools::getOperatingSystem () == operatingSytem_in); }
#if defined (ACE_LINUX)
  static enum Common_OperatingSystemDistributionType getDistribution (unsigned int&,  // return value: major version
                                                                      unsigned int&,  // return value: minor version
                                                                      unsigned int&); // return value: micro version
#endif // ACE_LINUX
  static std::string getHostName (); // return value: host name (see: man hostname(2))

  // --- compiler ---
  // *NOTE*: these (currently) return static (i.e. compile-time) information
  inline static std::string compilerName_ACE () { return ACE_TEXT_ALWAYS_CHAR (ACE::compiler_name ()); }
  static std::string compilerName ();
  static std::string compilerPlatformName ();
  static std::string compilerVersion ();

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

  // --- event loop ---
  // *WARNING*: the configuration may (!) be updated with platform-specific
  //            settings; hence the non-const argument
  static bool initializeEventDispatch (struct Common_EventDispatchConfiguration&); // configuration (i/o)
  // *NOTE*: the state handle is updated with the thread group ids (if any);
  //         hence the non-const argument
  // *WARNING*: iff any worker thread(s) is/are spawned, a handle to the first
  //            argument is passed to the dispatch thread function as argument
  //            --> ensure it does not fall off the stack prematurely
  static bool startEventDispatch (struct Common_EventDispatchState&); // thread data (i/o)
  static void dispatchEvents (struct Common_EventDispatchState&); // thread data (i/o)
  static void finalizeEventDispatch (struct Common_EventDispatchState&, // thread data (i/o)
                                     bool = false); // wait for completion ?

  // --- (locally installed-) (UNIX) commands / programs ---
  // *NOTE*: the Linux implementation relies on 'locate' [-b '\$@' -c -e -l 1]
  //         (and 'which' to locate 'locate' itself)
  static bool isInstalled (const std::string&, // executable (base-)name
                           std::string&);      // return value: (FQ) path

  // --- randomization ---
  template <typename ValueType>
  static std::enable_if_t<std::is_integral<ValueType>::value, ValueType> getRandomNumber (ValueType,  // start (inclusive)
                                                                                          ValueType); // end   (inclusive)
  template <typename ValueType>
  static std::enable_if_t<!std::is_integral<ValueType>::value, ValueType> getRandomNumber (ValueType,  // start (inclusive)
                                                                                           ValueType); // end   (inclusive)
  template <typename ValueType>
  inline static ValueType getRandomNumber (std::uniform_int_distribution<ValueType>& distribution_in) { return distribution_in (Common_Tools::randomEngine); }
  template <typename ValueType>
  static ValueType getRandomNumber (std::uniform_real_distribution<ValueType>& distribution_in) { return distribution_in (Common_Tools::randomEngine); }

  static unsigned int                  randomSeed;

  // --- libraries ---
  inline static std::string compiledVersion_ACE () { std::ostringstream converter; converter << ACE_MAJOR_VERSION; converter << ACE_TEXT_ALWAYS_CHAR ("."); converter << ACE_MINOR_VERSION; converter << ACE_TEXT_ALWAYS_CHAR ("."); converter << ACE_MICRO_VERSION; return converter.str (); }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))

  // --- randomization ---
  //typedef std::array<COMMON_APPLICATION_RNG_ENGINE::result_type,
  //                   COMMON_APPLICATION_RNG_ENGINE::state_size> PRNG_SEED_ARRAY_T;
  //typedef PRNG_SEED_ARRAY_T::iterator_type PRNG_SEED_ARRAY_ITERATOR_T;

  static COMMON_APPLICATION_RNG_ENGINE randomEngine;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static char                          randomStateBuffer[BUFSIZ];
#endif // ACE_WIN32 || ACE_WIN64

  // --- event loop ---
  // *IMPORTANT NOTE*: -in the sense of what COMMON_REACTOR_ACE_DEFAULT maps to
  //                   on this (!) platform (check ACE source code)
  static bool defaultPlatformReactorIsSelectBased();
};

// include template definition
#include "common_tools.inl"

#endif
