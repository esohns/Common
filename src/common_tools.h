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

//#include "signal.h"

#include <string>

#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/OS.h"

#include "common.h"
#include "common_exports.h"

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
  static void initialize ();

  //// --- singleton ---
  //static Common_ITimer* getTimerManager ();

  // --- strings ---
  // *NOTE*: uses ::snprintf internally: "HH:MM:SS.usec"
  static bool period2String (const ACE_Time_Value&, // period
                             std::string&);         // return value: corresp. string
  // *NOTE*: uses ::snprintf internally: "YYYY-MM-DD HH:MM:SS.usec"
  static bool timestamp2String (const ACE_Time_Value&, // timestamp
                                bool,                  // UTC ? : localtime
                                std::string&);         // return value: corresp. string

  static std::string sanitizeURI (const std::string&); // URI
  static std::string sanitize (const std::string&); // string
  static std::string strip (const std::string&); // string

  // --- platform ---
  static bool isLinux ();

  // --- capabilities ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static bool hasCapability (unsigned long); // capability
  static void printCapabilities ();
  static bool setCapability (unsigned long); // capability
  static bool dropCapability (unsigned long); // capability
#endif

  // --- user ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  static bool setRootPriviledges ();
  static void dropRootPriviledges ();
#endif
  static void printPriviledges ();

  static bool enableCoreDump (bool = true); // enable ? : disable
  static bool setResourceLimits (bool = false,  // #file descriptors (i.e. open handles)
                                 bool = true,   // stack trace/sizes (i.e. core file sizes)
                                 bool = false); // pending (rt) signals

  static void getCurrentUserName (std::string&,  // return value: username
                                  std::string&); // return value: "real" name
  static std::string getHostName (); // return value: hostname

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::string error2String (DWORD); // error
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

  static unsigned int randomSeed_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))
};

#endif
