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

#ifndef COMMON_CONFIGURATION_H
#define COMMON_CONFIGURATION_H

#include "common.h"
#include "common_defines.h"

struct Common_AllocatorConfiguration
{
  Common_AllocatorConfiguration ()
   : defaultBufferSize (BUFSIZ)
   , paddingBytes (0)
  {}

  unsigned int defaultBufferSize;
  // *NOTE*: add x bytes to each malloc(), override as needed
  //         (e.g. flex requires additional 2 YY_END_OF_BUFFER_CHARs). Note that
  //         this affects the ACE_Data_Block capacity, not its allotted size
  unsigned int paddingBytes;
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

#endif
