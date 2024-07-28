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

#ifndef COMMON_EVENT_COMMON_H
#define COMMON_EVENT_COMMON_H

#include "ace/config-lite.h"

#include "common_event_defines.h"

enum Common_EventDispatchType
{
  COMMON_EVENT_DISPATCH_INVALID = -1,
  ////////////////////////////////////////
  COMMON_EVENT_DISPATCH_PROACTOR = 0,
  COMMON_EVENT_DISPATCH_REACTOR,
  ////////////////////////////////////////
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

struct Common_EventDispatchConfiguration
{
  Common_EventDispatchConfiguration ()
   : callbacksRequireSynchronization (false)
   , dispatch (COMMON_EVENT_DEFAULT_DISPATCH)
   , numberOfProactorThreads (0)
   , proactorType (COMMON_EVENT_PROACTOR_TYPE)
   , numberOfReactorThreads (0)
   , reactorType (COMMON_EVENT_REACTOR_TYPE)
  {
    // *NOTE*: the default implementation uses the threadpool reactor in this
    //         case
    if (numberOfReactorThreads > 1)
    {
      callbacksRequireSynchronization = true;
      reactorType = COMMON_REACTOR_THREAD_POOL;
    } // end IF
  }

  // *NOTE*: iff the implementation dispatches multiple (i/o) event handlers for
  //         the same (!) handle (i.e. socket file descriptor) in parallel,
  //         shared resources may (!) need to be protected by mutexes
  bool                          callbacksRequireSynchronization; // reactor-only
  // *IMPORTANT NOTE*: that several dispatch mechanisms may be active at once
  enum Common_EventDispatchType dispatch; // main-

  unsigned int                  numberOfProactorThreads;
  enum Common_ProactorType      proactorType;

  unsigned int                  numberOfReactorThreads; // read: dedicated-
  // *IMPORTANT NOTE*: the default implementation uses the 'select' reactor
  //                   'in-line' (i.e. the 'main' thread runs the event dispatch
  //                   for numberOfReactorThreads == 0), and spawns one thread
  //                   for numberOfReactorThreads == 1, accordingly.
  //                   Iff numberOfReactorThreads > 1, the 'threadpool' reactor
  //                   is used instead (see above)
  enum Common_ReactorType       reactorType;
};

struct Common_EventDispatchState
{
  Common_EventDispatchState ()
   : configuration (NULL)
   , proactorGroupId (-1)
   , reactorGroupId (-1)
  {}

  struct Common_EventDispatchConfiguration* configuration;
  int proactorGroupId;
  int reactorGroupId;
};

#endif
