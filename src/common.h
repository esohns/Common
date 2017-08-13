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
#include "ace/Synch_Traits.h"

#include "common_defines.h"
#include "common_itask.h"

// forward declaration(s)
class ACE_Message_Queue_Base;
class ACE_Sig_Action;

struct Common_ParserConfiguration
{
  Common_ParserConfiguration ()
   : block (true)
   , messageQueue (NULL)
   , useYYScanBuffer (true)
   , debugParser (COMMON_PARSER_DEFAULT_YACC_TRACE)
   , debugScanner (COMMON_PARSER_DEFAULT_LEX_TRACE)
  {};

  bool                    block; // block in parse (i.e. wait for data in yywrap() ?)
  ACE_Message_Queue_Base* messageQueue; // queue (if any) to use for yywrap
  bool                    useYYScanBuffer; // yy_scan_buffer() ? : yy_scan_bytes() (C parsers only)

  // debug
  bool                    debugParser;
  bool                    debugScanner;
};

struct Common_SignalInformation
{
  Common_SignalInformation ()
   : signal (-1)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  , siginfo (ACE_INVALID_HANDLE)
  , ucontext (-1)
#else
  , siginfo ()
  , ucontext ()
#endif
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    ACE_OS::memset (&siginfo, 0, sizeof (siginfo));
    ACE_OS::memset (&ucontext, 0, sizeof (ucontext));
#endif
  };

  int        signal;
  siginfo_t  siginfo;
  ucontext_t ucontext;
};
struct Common_SignalHandlerConfiguration
{
  Common_SignalHandlerConfiguration ()
   : hasUI (false)
   , useReactor (false)
   //, useReactor (NET_EVENT_USE_REACTOR)
  {};

  bool hasUI;
  bool useReactor;
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
   : mode (COMMON_TIMER_DEFAULT_MODE)
   , queueType (COMMON_TIMER_DEFAULT_QUEUE)
  {};

  enum Common_TimerMode      mode;
  enum Common_TimerQueueType queueType;
};

// *** signals ***
typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

// *** (ACE) event-dispatch specific
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
  COMMON_PROACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
  COMMON_PROACTOR_POSIX_AIOCB,     // POSIX only
  COMMON_PROACTOR_POSIX_SIG,       // POSIX only
  COMMON_PROACTOR_POSIX_SUN,       // POSIX only
  COMMON_PROACTOR_POSIX_CB,        // POSIX only
  ///////////////////////////////////////
  COMMON_PROACTOR_WIN32,           // Win32 only
  ///////////////////////////////////////
  COMMON_PROACTOR_MAX,
  COMMON_PROACTOR_INVALID
};

enum Common_ReactorType
{
  COMMON_REACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
  COMMON_REACTOR_DEV_POLL,        // POSIX only
  COMMON_REACTOR_SELECT,
  COMMON_REACTOR_THREAD_POOL,     // *IMPORTANT NOTE*: currently, only a select()-based implementation is available
  COMMON_REACTOR_WFMO,            // Win32 only
  ///////////////////////////////////////
  COMMON_REACTOR_MAX,
  COMMON_REACTOR_INVALID
};

struct Common_DispatchThreadData
{
  inline Common_DispatchThreadData ()
   : numberOfDispatchThreads (0)
   , proactorType (COMMON_EVENT_PROACTOR_TYPE)
   , reactorType (COMMON_EVENT_REACTOR_TYPE)
   , useReactor (true)
  {};

  unsigned int             numberOfDispatchThreads;
  enum Common_ProactorType proactorType;
  enum Common_ReactorType  reactorType;
  bool                     useReactor;
};

// *** task ***
typedef Common_ITaskControl_T<ACE_MT_SYNCH> Common_ITaskControl_t;
typedef Common_ITask_T<ACE_MT_SYNCH> Common_ITask_t;

// *** log ***
typedef std::deque<std::string> Common_MessageStack_t;
typedef Common_MessageStack_t::const_iterator Common_MessageStackConstIterator_t;
typedef Common_MessageStack_t::const_reverse_iterator Common_MessageStackConstReverseIterator_t;
//typedef std::deque<ACE_Log_Record> Common_LogRecordStack_t;

#endif
