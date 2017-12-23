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

#include <deque>
#include <map>
#include <string>

#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/os_include/os_ucontext.h"
#include "ace/OS_NS_signal.h"
#include "ace/Signal.h"
#include "ace/Synch_Traits.h"

#include "common_defines.h"
#include "common_ilock.h"
#include "common_itask.h"
#include "common_itaskcontrol.h"

// forward declaration(s)
class ACE_Message_Queue_Base;

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
  {};

  int        signal;
  siginfo_t  siginfo;
  ucontext_t ucontext;
};
struct Common_SignalHandlerConfiguration
{
  Common_SignalHandlerConfiguration ()
   : hasUI (false)
   , useReactor (COMMON_EVENT_USE_REACTOR)
  {};

  bool hasUI;
  bool useReactor;
};

// *** signals ***
typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

// *** (ACE) event-dispatch specific
enum Common_DispatchType : int
{
  COMMON_DISPATCH_INVALID = -1,
  COMMON_DISPATCH_PROACTOR = 0,
  COMMON_DISPATCH_REACTOR,
  ///////////////////////////////////////
  COMMON_DISPATCH_MAX
};

enum Common_ProactorType : int
{
  COMMON_PROACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  COMMON_PROACTOR_POSIX_AIOCB,     // POSIX only
  COMMON_PROACTOR_POSIX_SIG,       // POSIX only
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
  COMMON_PROACTOR_POSIX_SUN,       // POSIX only
#endif
  COMMON_PROACTOR_POSIX_CB,        // POSIX only
#endif
///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  COMMON_PROACTOR_WIN32,           // Win32 only
#endif
  ///////////////////////////////////////
  COMMON_PROACTOR_MAX,
  COMMON_PROACTOR_INVALID
};

enum Common_ReactorType : int
{
  COMMON_REACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  COMMON_REACTOR_DEV_POLL,        // POSIX only
#endif
  COMMON_REACTOR_SELECT,
  COMMON_REACTOR_THREAD_POOL,     // *IMPORTANT NOTE*: currently, only a select()-based implementation is available
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  COMMON_REACTOR_WFMO,            // Win32 only
#endif
  ///////////////////////////////////////
  COMMON_REACTOR_MAX,
  COMMON_REACTOR_INVALID
};

struct Common_DispatchThreadData
{
  Common_DispatchThreadData ()
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

struct Common_ScannerState
{
  Common_ScannerState ()
   : offset (0)
  {};

  unsigned int offset;
};

enum Common_StatisticActionType : int
{
  STATISTIC_ACTION_COLLECT = 0,
  STATISTIC_ACTION_REPORT,
  ////////////////////////////////////////
  STATISTIC_ACTION_MAX,
  STATISTIC_ACTION_INVALID
};

// *** task ***
typedef Common_ITaskControl_T<ACE_MT_SYNCH,
                              Common_ILock_T<ACE_MT_SYNCH> > Common_ITaskControl_t;
typedef Common_ITaskControl_T<ACE_MT_SYNCH,
                              Common_IRecursiveLock_T<ACE_MT_SYNCH> > Common_IRecursiveTaskControl_t;
typedef Common_ITask_T<ACE_MT_SYNCH,
                       Common_ILock_T<ACE_MT_SYNCH> > Common_ITask_t;
typedef Common_ITask_T<ACE_MT_SYNCH,
                       Common_IRecursiveLock_T<ACE_MT_SYNCH> > Common_IRecursiveTask_t;

// *** log ***
typedef std::deque<std::string> Common_MessageStack_t;
typedef Common_MessageStack_t::const_iterator Common_MessageStackConstIterator_t;
typedef Common_MessageStack_t::const_reverse_iterator Common_MessageStackConstReverseIterator_t;
//typedef std::deque<ACE_Log_Record> Common_LogRecordStack_t;

#endif
