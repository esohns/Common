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
#endif

#include "ace/Global_Macros.h"

#include "common.h"
#include "common_configuration.h"
//#include "common_exports.h"

// forward declaration(s)
class ACE_Event_Handler;
class ACE_Log_Msg_Backend;

ACE_THR_FUNC_RETURN common_event_dispatch_function (void*);

//class Common_Export Common_Tools
class Common_Tools
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

  // --- platform ---
  static unsigned int getNumberOfCPUs (bool = true); // 'hyperthreading' ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //// *WARNING*: limited to 9 characters
  static void setThreadName (const std::string&, // thread name
                             DWORD = 0);         // thread id (0: current)
#endif

  static void printLocales ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static bool isLinux (enum Common_OperatingSystemDistributionType&); // return value: distribution
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // --- capabilities ---
  static std::string capabilityToString (unsigned long); // capability
  static bool hasCapability (unsigned long,               // capability
                             cap_flag_t = CAP_EFFECTIVE); // set
  static bool setCapability (unsigned long,               // capability
                             cap_flag_t = CAP_EFFECTIVE); // set
  static bool dropCapability (unsigned long,               // capability
                              cap_flag_t = CAP_EFFECTIVE); // set
  static void printCapabilities ();
#endif

  // --- process ---
  // *NOTE*: this uses ::system() piping stdout into a temporary file
  //         --> the command line must not have piped stdout already
  // *TODO*: while this should work on most platforms, there are more efficient
  //         alternatives (e.g. see also: man popen() for Linux)
  // *TODO*: enhance the API to return the exit status
  static bool command (const std::string&, // command line
                       std::string&);      // return value: stdout
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *NOTE*: the Linux implementation relies on the 'pidofproc' command, which
  //         may not be available on all versions of all distributions
  // *TODO*: parse /proc manually, or find a better alternative (e.g. 'libproc',
  //         testing for 'PID-files' in /var/run, ...)
  static pid_t getProcessId (const std::string&); // executable (base-)name
#endif

  // --- user ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  static bool setRootPrivileges ();
  static void dropRootPrivileges ();
#endif
  static void printPrivileges ();

  static bool enableCoreDump (bool = true); // enable ? : disable
  static bool setResourceLimits (bool = false,  // #file descriptors (i.e. open handles)
                                 bool = true,   // stack trace/sizes (i.e. core file sizes)
                                 bool = false); // pending (rt) signals

  static void getCurrentUserName (std::string&,  // return value: username
                                  std::string&); // return value: "real" name
  static std::string getHostName (); // return value: hostname

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::string GUIDToString (REFGUID);
  static struct _GUID StringToGUID (const std::string&);

  static std::string errorToString (DWORD,         // error
                                    bool = false); // ? use AMGetErrorText() : use FormatMessage()
#endif

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
#endif
#endif /* _DEBUG */

  static unsigned int    randomSeed_;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static char            randomStateBuffer_[BUFSIZ];
#endif

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))
};

#endif
