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

//#include <signal.h>
//#include <ucontext.h>

#include <deque>
#include <map>
#include <string>

#include "ace/OS.h"
#include "ace/Signal.h"

#include "common_defines.h"

// forward declarations
class ACE_Sig_Action;

struct Common_SignalInformation
{
  Common_SignalInformation ()
   : signal (-1)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  , sigInfo (ACE_INVALID_HANDLE)
  , uContext (-1)
#else
  , sigInfo ()
  , uContext ()
#endif
  {
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    ACE_OS::memset (&sigInfo, 0, sizeof (sigInfo));
    ACE_OS::memset (&uContext, 0, sizeof (uContext));
#endif
  };

  int        signal;
  siginfo_t  sigInfo;
  ucontext_t uContext;
};

enum Common_TimerMode
{
  COMMON_TIMER_MODE_PROACTOR = 0,
  COMMON_TIMER_MODE_QUEUE,
  COMMON_TIMER_MODE_REACTOR,
  COMMON_TIMER_MODE_SIGNAL,
  /////////////////////////////////////
  COMMON_TIMER_MODE_MAX,
  COMMON_TIMER_MODE_INVALID
};

enum Common_TimerQueueType
{
  COMMON_TIMER_QUEUE_HEAP = 0,
  COMMON_TIMER_QUEUE_LIST,
  COMMON_TIMER_QUEUE_WHEEL,
  ///////////////////////////////////////
  COMMON_TIMER_QUEUE_MAX,
  COMMON_TIMER_QUEUE_INVALID
};

struct Common_TimerConfiguration
{
  inline Common_TimerConfiguration ()
   : mode (COMMON_TIMER_MANAGER_DEFAULT_MODE)
   , queueType (COMMON_TIMER_MANAGER_DEFAULT_QUEUE)
  {};

  Common_TimerMode      mode;
  Common_TimerQueueType queueType;
};

// *** signals ***
typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

// *** event dispatch
enum Common_DispatchType
{
  COMMON_DISPATCH_INVALID = -1,
  COMMON_DISPATCH_PROACTOR = 0,
  COMMON_DISPATCH_REACTOR,
  ///////////////////////////////////////
  COMMON_DISPATCH_MAX
};

enum Common_ProactorType
{
  COMMON_PROACTOR_DEFAULT = 0, // platform-specific
  ///////////////////////////////////////
  COMMON_PROACTOR_POSIX_AIOCB, // POSIX only !
  COMMON_PROACTOR_POSIX_SIG,   // POSIX only !
  COMMON_PROACTOR_POSIX_SUN,   // POSIX only !
  COMMON_PROACTOR_POSIX_CB,    // POSIX only !
  ///////////////////////////////////////
  COMMON_PROACTOR_WIN32,       // Win32 only !
  ///////////////////////////////////////
  COMMON_PROACTOR_MAX,
  COMMON_PROACTOR_INVALID
};

enum Common_ReactorType
{
  COMMON_REACTOR_DEFAULT = 0, // platform-specific
  COMMON_REACTOR_DEV_POLL,    // POSIX only !
  COMMON_REACTOR_SELECT,
  COMMON_REACTOR_TP,
  COMMON_REACTOR_WFMO,        // Win32 only !
  ///////////////////////////////////////
  COMMON_REACTOR_MAX,
  COMMON_REACTOR_INVALID
};

// *** log ***
typedef std::deque<std::string> Common_MessageStack_t;
typedef Common_MessageStack_t::const_iterator Common_MessageStackConstIterator_t;
//typedef std::deque<ACE_Log_Record> Common_LogRecordStack_t;

#endif
