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
//#include "common_exports.h"

// forward declaration(s)
class ACE_Event_Handler;
class ACE_Log_Msg_Backend;

ACE_THR_FUNC_RETURN threadpool_event_dispatcher_function (void*);

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

  static bool isLinux ();

  // --- capabilities ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
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
  static bool initializeEventDispatch (bool,                      // use reactor ? : proactor
                                       bool,                      // use thread pool ?
                                       unsigned int,              // number of (dispatching) threads
                                       enum Common_ProactorType&, // return value: proactor type
                                       enum Common_ReactorType&,  // return value: reactor type
                                       bool&);                    // return value: output requires serialization
  // *NOTE*: the first argument is passed to the thread function as argument,
  //         so must reside on the stack (hence the reference)
  static bool startEventDispatch (const struct Common_EventDispatchThreadData&, // thread data
                                  int&);                                        // return value: thread group id
  static void dispatchEvents (bool,      // use reactor ? : proactor
                              int = -1); // thread group id
  // *NOTE*: this call blocks until all dispatching threads have joined
  static void finalizeEventDispatch (bool, // stop reactor ?
                                     bool, // stop proactor ?
                                     int); // thread group id

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
