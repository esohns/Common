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

#include <list>
#include <map>
#include <string>
#include <vector>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <guiddef.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"
#include "ace/OS_NS_signal.h"
#include "ace/Signal.h"
#include "ace/Synch_Traits.h"

#include "common_defines.h"
#include "common_ilock.h"
#include "common_itask.h"
#include "common_itaskcontrol.h"

// *** platform ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct common_equal_guid
{
  inline bool operator() (const struct _GUID& lhs_in, const struct _GUID& rhs_in) const { return static_cast<bool> (InlineIsEqualGUID (lhs_in, rhs_in)); }
};
struct common_less_guid
{
  inline bool operator() (const struct _GUID& lhs_in, const struct _GUID& rhs_in) const { return (lhs_in.Data1 < rhs_in.Data1); }
};

typedef std::list<struct _GUID> Common_Identifiers_t;
typedef Common_Identifiers_t::iterator Common_IdentifiersIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

enum Common_OperatingSystemType
{
  COMMON_OPERATINGSYSTEM_GNU_LINUX = 0,
  COMMON_OPERATINGSYSTEM_WIN32,
  ///////////////////////////////////////
  COMMON_OPERATINGSYSTEM_INVALID,
  COMMON_OPERATINGSYSTEM_MAX
};

enum Common_OperatingSystemDistributionType
{
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN = 0,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_WIN32_MICROSOFT,
  ///////////////////////////////////////
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID,
  COMMON_OPERATINGSYSTEM_DISTRIBUTION_MAX
};

// *** signals ***

enum Common_SignalDispatchType
{
  COMMON_SIGNAL_DISPATCH_PROACTOR = 0,
  COMMON_SIGNAL_DISPATCH_REACTOR,
  COMMON_SIGNAL_DISPATCH_SIGNAL, // inline (i.e. signal handler context restrictions apply)
  /////////////////////////////////////
  COMMON_SIGNAL_DISPATCH_MAX,
  COMMON_SIGNAL_DISPATCH_INVALID
};

struct Common_Signal
{
  Common_Signal ()
   : signal (-1)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  , siginfo (ACE_INVALID_HANDLE)
  , ucontext (-1)
#else
  , siginfo ()
  , ucontext ()
#endif // ACE_WIN32 || ACE_WIN64
  {}

  int              signal;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct siginfo_t siginfo;
#else
  siginfo_t        siginfo;
#endif // ACE_WIN32 || ACE_WIN64
  ucontext_t       ucontext;
};
typedef std::vector <struct Common_Signal> Common_Signals_t;
typedef Common_Signals_t::const_iterator Common_SignalsIterator_t;

typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

// *** user/groups ***

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef std::vector <std::string> Common_UserGroups_t;
typedef Common_UserGroups_t::const_iterator Common_UserGroupsIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

// *** (ACE) event dispatch ***
enum Common_EventDispatchType
{
  COMMON_EVENT_DISPATCH_NONE = 0,
  COMMON_EVENT_DISPATCH_PROACTOR,
  COMMON_EVENT_DISPATCH_REACTOR,
  ///////////////////////////////////////
  COMMON_EVENT_DISPATCH_INVALID,
  COMMON_EVENT_DISPATCH_MAX
};

enum Common_ProactorType
{
  COMMON_PROACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  COMMON_PROACTOR_POSIX_AIOCB,     // POSIX only
  COMMON_PROACTOR_POSIX_SIG,       // POSIX only
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
  COMMON_PROACTOR_POSIX_SUN,       // POSIX only
#endif // ACE_HAS_AIO_CALLS && sun
  COMMON_PROACTOR_POSIX_CB,        // POSIX only
#endif // ACE_WIN32 || ACE_WIN64
///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  COMMON_PROACTOR_WIN32,           // Win32 only
#endif // ACE_WIN32 || ACE_WIN64
  ///////////////////////////////////////
  COMMON_PROACTOR_MAX,
  COMMON_PROACTOR_INVALID
};

enum Common_ReactorType
{
  COMMON_REACTOR_ACE_DEFAULT = 0, // --> (somewhat) platform-specific
  ///////////////////////////////////////
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  COMMON_REACTOR_DEV_POLL,        // POSIX only
#endif // ACE_WIN32 || ACE_WIN64
  COMMON_REACTOR_SELECT,
  COMMON_REACTOR_THREAD_POOL,     // *IMPORTANT NOTE*: currently, only a select()-based implementation is available
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  COMMON_REACTOR_WFMO,            // Win32 only
#endif // ACE_WIN32 || ACE_WIN64
  ///////////////////////////////////////
  COMMON_REACTOR_MAX,
  COMMON_REACTOR_INVALID
};

struct Common_EventDispatchConfiguration;
struct Common_EventDispatchState
{
  Common_EventDispatchState ()
   : configuration (NULL)
   , proactorGroupId (-1)
   , reactorGroupId (-1)
  {}

  struct Common_EventDispatchConfiguration* configuration;
  int                                       proactorGroupId;
  int                                       reactorGroupId;
};

// *** parser ***

struct Common_ScannerState
{
  Common_ScannerState ()
   : offset (0)
  {}

  unsigned int offset;
};

// *** statistic ***

enum Common_StatisticActionType
{
  COMMON_STATISTIC_ACTION_COLLECT = 0,
  COMMON_STATISTIC_ACTION_REPORT,
  ////////////////////////////////////////
  COMMON_STATISTIC_ACTION_MAX,
  COMMON_STATISTIC_ACTION_INVALID
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

// *** application ***

struct Common_ApplicationVersion
{
  unsigned int majorVersion;
  unsigned int minorVersion;
  unsigned int microVersion;
};

enum Common_ApplicationModeType
{
  COMMON_APPLICATION_MODE_DEBUG = 0,
  COMMON_APPLICATION_MODE_INSTALL,
  COMMON_APPLICATION_MODE_PRINT, // i.e. usage/version/etc
  COMMON_APPLICATION_MODE_RUN,
  COMMON_APPLICATION_MODE_TEST,
  COMMON_APPLICATION_MODE_UNINSTALL,
  ////////////////////////////////////////
  COMMON_APPLICATION_MODE_MAX,
  COMMON_APPLICATION_MODE_INVALID
};

#endif
