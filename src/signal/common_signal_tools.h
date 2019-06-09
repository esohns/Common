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

#ifndef COMMON_SIGNAL_TOOLS_H
#define COMMON_SIGNAL_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/os_include/os_signal.h"
#include "ace/Sig_Handler.h"

#include "common_signal.h"

// forward declaration(s)
class ACE_Event_Handler;
class ACE_Sig_Set;

class Common_Signal_Tools
{
 public:
  // *NOTE*: this does the following:
  //         - ignore SIGPIPE
  //         - on non-Win32 systems, iff the proactor framework is used with the
  //           ACE_POSIX_Proactor::PROACTOR_SIG, block RT signals
  //         - on non-Win32 systems, remove SIGSEGV to enable core dumps
  // *NOTE*: call this in the 'main' thread as early as possible; the signal
  //         disposition is inherited by all threads spawned thereafter
  static bool preInitialize (ACE_Sig_Set&,            // signal set (to handle) (*NOTE*: IN/OUT)
                             bool,                    // use reactor ? : proactor
                             Common_SignalActions_t&, // return value: previous action(s)
                             sigset_t&);              // return value: previous mask
  static bool initialize (enum Common_SignalDispatchType, // dispatch mode
                          const ACE_Sig_Set&,             // signal set (to handle)
                          const ACE_Sig_Set&,             // signal set (to ignore)
                          ACE_Event_Handler*,             // event handler handle
                          Common_SignalActions_t&);       // return value: previous action(s)
  static void finalize (enum Common_SignalDispatchType, // dispatch mode
                        const ACE_Sig_Set&,             // signal set (handled)
                        const Common_SignalActions_t&,  // previous action(s)
                        const sigset_t&);               // previous mask

  static std::string signalToString (const struct Common_Signal&); // signal information
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static void unblockRealtimeSignals (sigset_t&); // return value: original mask
#endif // ACE_WIN32 || ACE_WIN64

  static ACE_Sig_Handler signalHandler_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Signal_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Signal_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Signal_Tools (const Common_Signal_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Signal_Tools& operator= (const Common_Signal_Tools&))
};

#endif
