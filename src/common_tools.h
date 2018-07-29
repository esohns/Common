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

#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <guiddef.h>
#else
#include <sys/capability.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

#include "common.h"
#include "common_configuration.h"
#include "common_iinitialize.h"

// forward declaration(s)
class ACE_Event_Handler;
class ACE_Log_Msg_Backend;

ACE_THR_FUNC_RETURN common_event_dispatch_function (void*);

class Common_Tools
 : public Common_SInitializeFinalize_T<Common_Tools>
{
 public:
  static void initialize (bool = false); // initialize random number generator ?
  static void finalize ();

  // --- debug ---
  static bool inDebugSession ();

  // --- strings ---
  static std::string sanitizeURI (const std::string&); // URI
  // replace ' ' with '_'
  static std::string sanitize (const std::string&); // string
  // remove leading and trailing whitespace
  static std::string strip (const std::string&); // string
  static bool isspace (const std::string&); // string

  // --- platform ---
  static unsigned int getNumberOfCPUs (bool = true); // consider logical cores (i.e. 'hyperthreading') ?
  static std::string getHostName (); // return value: host name (see: man hostname(2))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *WARNING*: limited to 9 characters
  static void setThreadName (const std::string&, // thread name
                             DWORD = 0);         // thread id (0: caller)
#endif // ACE_WIN32 || ACE_WIN64
  static bool isOperatingSystem (enum Common_OperatingSystemType);
#if defined (ACE_LINUX)
  static enum Common_OperatingSystemDistributionType getDistribution (unsigned int&,  // return value: major version
                                                                      unsigned int&,  // return value: minor version
                                                                      unsigned int&); // return value: micro version
#endif // ACE_LINUX

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

  // --- process ---
  static std::string commandLineToString (int,           // argc
                                          ACE_TCHAR*[]); // argv

  // *NOTE*: this uses system(3), piping stdout into a temporary file
  //         --> the command line must not have piped stdout already
  // *TODO*: while this should work on most platforms, there are more efficient
  //         alternatives (e.g. see also: man popen() for Linux)
  // *TODO*: enhance the API to return the exit status
  static bool command (const std::string&, // command line
                       int&,               // return value: exit status
                       std::string&);      // return value: stdout
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *NOTE*: the Linux implementation relies on the 'pidofproc' command, which
  //         may not be available on all versions of all distributions
  // *TODO*: parse /proc manually, or find a better alternative (e.g. 'libproc',
  //         testing for 'PID-files' in /var/run, ...)
  static pid_t getProcessId (const std::string&); // executable (base-)name

  // *NOTE*: as used by polkit (i.e. queries /proc/self/stat)
  static uint64_t getStartTime ();
#endif // ACE_WIN32 || ACE_WIN64
  static bool enableCoreDump (bool = true); // enable ? : disable
  static bool setResourceLimits (bool = false,  // #file descriptors (i.e. open handles)
                                 bool = true,   // stack trace/sizes (i.e. core file sizes)
                                 bool = false); // pending (rt) signals

  // --- user ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static void getUserName (std::string&,  // return value: user name
                           std::string&); // return value: "real" name (if any)
#else
  static void getUserName (uid_t,         // user id {-1: euid}
                           std::string&,  // return value: user name
                           std::string&); // return value: "real" name (if any)

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

  // --- registry ---
  static bool deleteKey (HKEY,                // parent key
                         const std::string&); // subkey
  static bool deleteKeyValue (HKEY,               // parent key
                              const std::string&, // subkey
                              const std::string&); // value

  // --- error ---
  static std::string errorToString (DWORD,         // error
                                    bool = false); // ? use AMGetErrorText() : use FormatMessage()
#endif // ACE_WIN32 || ACE_WIN64

  // --- logging ---
  static bool initializeLogging (const std::string&,           // program name (i.e. argv[0])
                                 const std::string&,           // log file {"" --> disable}
                                 bool = false,                 // log to syslog ?
                                 bool = false,                 // enable tracing messages ?
                                 bool = false,                 // enable debug messages ?
                                 ACE_Log_Msg_Backend* = NULL); // logger backend {NULL --> disable}
  static void finalizeLogging ();

  // --- event loop ---
  // *NOTE*: the configuration is updated with the platform-specific proactor/
  //         reactor types and corresponding options; hence the non-const
  //         argument
  static bool initializeEventDispatch (struct Common_EventDispatchConfiguration&); // configuration (i/o)
  // *NOTE*: the state handle is updated with the thread group ids (if any);
  //         hence the non-const argument
  // *WARNING*: iff any worker thread(s) is/are spawned, a handle to the first
  //            argument is passed to the dispatch thread function as argument
  //            --> ensure it does not fall off the stack prematurely
  static bool startEventDispatch (struct Common_EventDispatchState&); // thread data (i/o)
  static void dispatchEvents (bool,      // dispatch reactor ? : proactor
                              int = -1); // dispatch thread group id
  static void finalizeEventDispatch (int,           // proactor thread group id {-1: nop}
                                     int,           // reactor thread group id  {-1: nop}
                                     bool = false); // wait for completion ?

  // --- (locally installed-) (UNIX) commands / programs ---
  // *NOTE*: the Linux implementation relies on 'locate' [-b '\$@' -c -e -l 1]
  //         (and 'which' to locate 'locate' itself)
  static bool isInstalled (const std::string&, // executable (base-)name
                           std::string&);      // return value: (FQ) path

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static ACE_HANDLE      debugHeapLogFileHandle_;
#endif // ACE_WIN32 || ACE_WIN64
#endif /* _DEBUG */

  static unsigned int    randomSeed_;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static char            randomStateBuffer_[BUFSIZ];
#endif // ACE_WIN32 || ACE_WIN64

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))
};

#endif
