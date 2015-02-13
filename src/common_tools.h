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

#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"
#include "ace/Signal.h"

#include "common_exports.h"
#include "common.h"

// forward declaration(s)
class ACE_Log_Msg_Backend;
class ACE_Event_Handler;

ACE_THR_FUNC_RETURN threadpool_event_dispatcher_function (void*);

class Common_Export Common_Tools
{
 public:
  // use this to generate a "condensed" period string
  // - uses snprintf internally: "%H:%M:%S.usec"
  static bool period2String (const ACE_Time_Value&, // period
                             std::string&);         // return value: corresp. string

  static std::string sanitizeURI (const std::string&); // URI
  static std::string sanitize (const std::string&); // string
  static std::string strip (const std::string&); // string

  // ---------------------------------------------------------------------------

  static bool isLinux ();

  static bool initResourceLimits (bool = false, // #file descriptors (i.e. open handles)
                                  bool = true); // stack trace/sizes (i.e. core file sizes)

  static void getCurrentUserName (std::string&,  // return value: username
                                  std::string&); // return value: "real" name
  static std::string getHostName (); // return value: hostname

  static bool initLogging (const std::string&,           // program name (i.e. argv[0])
                           const std::string&,           // log file {"" --> disable}
                           bool = false,                 // log to syslog ?
                           bool = false,                 // enable tracing messages ?
                           bool = false,                 // enable debug messages ?
                           ACE_Log_Msg_Backend* = NULL); // logger backend {NULL --> disable}

  static bool preInitSignals (ACE_Sig_Set&,             // signal set (*NOTE*: IN/OUT)
                              bool,                     // use reactor ?
                              Common_SignalActions_t&); // return value: previous actions
  static bool initSignals (ACE_Sig_Set&,             // signal set (*NOTE*: IN/OUT)
                           ACE_Event_Handler*,       // event handler handle
                           Common_SignalActions_t&); // return value: previous actions
  static void finiSignals (const ACE_Sig_Set&,             // signal set
                           bool,                           // use reactor ?
                           const Common_SignalActions_t&); // previous actions
  static void retrieveSignalInfo (int,               // signal
                                  const siginfo_t&,  // info
                                  const ucontext_t*, // context
                                  std::string&);     // return value: info

  // event loop
  static bool initEventDispatch (bool,         // use reactor ? : proactor
                                 unsigned int, // # dispatching threads
                                 bool&);       // return value: output requires serialization
  static bool startEventDispatch (bool,         // use reactor ? : proactor
                                  unsigned int, // # dispatching threads
                                  int&);        // return value: thread group id
  // *NOTE*: this call blocks until all dispatching threads have joined
  static void finiEventDispatch (bool, // stop reactor ?
                                 bool, // stop proactor ?
                                 int); // thread group id

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&));
};

#endif
