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

#include "common_timer_common.h"

// forward declaration(s)
class ACE_Event_Handler;
class ACE_Log_Msg_Backend;
class ACE_Sig_Set;
class ACE_Time_Value;
class Common_ITimer;

ACE_THR_FUNC_RETURN threadpool_event_dispatcher_function (void*);

//class Common_Export Common_Tools
class Common_Tools
{
 public:
  static void initialize (bool = false); // initialize random number generator ?
  static void finalize ();

  //// --- singleton ---
  //static Common_ITimer* getTimerManager ();

  // --- strings ---
  // *NOTE*: uses ::snprintf internally: "HH:MM:SS.usec"
  static bool periodToString (const ACE_Time_Value&, // period
                              std::string&);         // return value: corresp. string
  // *NOTE*: uses ::snprintf internally: "YYYY-MM-DD HH:MM:SS.usec"
  static bool timestampToString (const ACE_Time_Value&, // timestamp
                                 bool,                  // UTC ? : localtime
                                 std::string&);         // return value: corresp. string

  static std::string sanitizeURI (const std::string&); // URI
  // replace ' ' with '_'
  static std::string sanitize (const std::string&); // string
  // remove leading and trailing whitespace
  static std::string strip (const std::string&); // string

  // --- platform ---
  static unsigned int getNumberOfCPUs (bool = true); // 'hyperthreading' ?

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

  // --- signals ---
  static bool preInitializeSignals (ACE_Sig_Set&,            // signal set (*NOTE*: IN/OUT)
                                    bool,                    // use reactor ?
                                    Common_SignalActions_t&, // return value: previous actions
                                    sigset_t&);              // return value: previous mask
  static bool initializeSignals (const ACE_Sig_Set&,       // signal set (to handle)
                                 const ACE_Sig_Set&,       // signal set (to ignore)
                                 ACE_Event_Handler*,       // event handler handle
                                 Common_SignalActions_t&); // return value: previous actions
  static void finalizeSignals (const ACE_Sig_Set&,            // signal set
                               const Common_SignalActions_t&, // previous actions
                               const sigset_t&);              // previous mask
  static void retrieveSignalInfo (int,                     // signal
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                  const struct siginfo_t&, // info
#else
                                  const siginfo_t&,        // info
#endif
                                  const ucontext_t*,       // context
                                  std::string&);           // return value: info

  // --- timers ---
  static bool initializeTimers (const struct Common_TimerConfiguration&); // configuration
  static void finalizeTimers (const struct Common_TimerConfiguration&, // configuration
                              bool = true);                            // wait for completion ?

  // --- event loop ---
  static bool initializeEventDispatch (bool,                 // use reactor ? : proactor
                                       bool,                 // use thread pool ?
                                       unsigned int,         // number of (dispatching) threads
                                       Common_ProactorType&, // return value: proactor type
                                       Common_ReactorType&,  // return value: reactor type
                                       bool&);               // return value: output requires serialization
  // *NOTE*: the first argument is passed to the thread function as argument,
  //         so must reside on the stack (hence the reference)
  static bool startEventDispatch (const struct Common_DispatchThreadData&, // thread data
                                  int&);                                   // return value: thread group id
  static void dispatchEvents (bool,      // use reactor ? : proactor
                              int = -1); // thread group id
  // *NOTE*: this call blocks until all dispatching threads have joined
  static void finalizeEventDispatch (bool, // stop reactor ?
                                     bool, // stop proactor ?
                                     int); // thread group id
  //static void unblockRealtimeSignals (sigset_t&); // return value: original mask

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static ACE_HANDLE   debugHeapLogFileHandle_;
#endif
#endif
  static unsigned int randomSeed_;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static char         randomStateBuffer_[BUFSIZ];
#endif

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))
};

#endif
