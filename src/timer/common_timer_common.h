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

#ifndef COMMON_TIMER_COMMON_H
#define COMMON_TIMER_COMMON_H

#include <string>

#include "ace/Abstract_Timer_Queue.h"
#include "ace/Event_Handler.h"
#include "ace/Version.h"
#if ((ACE_MAJOR_VERSION > 6) ||   \
     ((ACE_MAJOR_VERSION == 6) && \
      ((ACE_MINOR_VERSION > 0) || \
       (ACE_BETA_VERSION  > 3))))
#include "ace/Event_Handler_Handle_Timeout_Upcall.h"
#else
#include "ace/Timer_Queuefwd.h"
#endif // ACE_VERSION > 6.0.3
#include "ace/Synch_Traits.h"
#include "ace/Timer_Heap_T.h"
#include "ace/Timer_List_T.h"
#include "ace/Timer_Wheel_T.h"
#include "ace/Timer_Queue_T.h"

#include "common_time_common.h"

#include "common_itimer.h"
#include "common_timer_defines.h"

// forward declarations
//template <class TQ, class TYPE>
//class ACE_Async_Timer_Queue_Adapter;
//template <class TQ, class TYPE>
//class ACE_Thread_Timer_Queue_Adapter;

enum Common_TimerDispatchType
{
  COMMON_TIMER_DISPATCH_PROACTOR = 0,
  COMMON_TIMER_DISPATCH_QUEUE,
  COMMON_TIMER_DISPATCH_REACTOR,
  COMMON_TIMER_DISPATCH_SIGNAL,
  /////////////////////////////////////
  COMMON_TIMER_DISPATCH_MAX,
  COMMON_TIMER_DISPATCH_INVALID
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
  Common_TimerConfiguration ()
   : dispatch (COMMON_TIMER_DEFAULT_DISPATCH)
   , publishSeconds (false)
   , queueType (COMMON_TIMER_DEFAULT_QUEUE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , taskType (ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_THREAD_DEFAULT_TASKNAME))
#endif // ACE_WIN32 || ACE_WIN64
  {}

  enum Common_TimerDispatchType dispatch;
  bool                          publishSeconds;
  // *NOTE*: applies to COMMON_TIMER_DISPATCH_QUEUE dispatch only
  enum Common_TimerQueueType    queueType;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::string                   taskType;
#endif // ACE_WIN32 || ACE_WIN64
};

// *** timer queue ***
// *NOTE*: ensure a minimal amount of locking
//typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_NULL_MUTEX> Common_TimeoutUpcall_t;
typedef ACE_Event_Handler_Handle_Timeout_Upcall Common_TimeoutUpcall_t;
// *WARNING*: apparently, ACEs' timer heap implementation currently has some
//            stability issues
typedef ACE_Timer_Heap_T<ACE_Event_Handler*,
                         Common_TimeoutUpcall_t,
                         ACE_SYNCH_NULL_MUTEX,
                         Common_TimePolicy_t> Common_TimerQueueHeapImpl_t;
typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*,
                                  Common_TimeoutUpcall_t,
                                  ACE_SYNCH_NULL_MUTEX,
                                  Common_TimePolicy_t> Common_TimerQueueHeapImplIterator_t;

typedef ACE_Timer_List_T<ACE_Event_Handler*,
                         Common_TimeoutUpcall_t,
                         ACE_SYNCH_NULL_MUTEX,
                         Common_TimePolicy_t> Common_TimerQueueListImpl_t;
typedef ACE_Timer_List_Iterator_T<ACE_Event_Handler*,
                                  Common_TimeoutUpcall_t,
                                  ACE_SYNCH_NULL_MUTEX,
                                  Common_TimePolicy_t> Common_TimerQueueListImplIterator_t;

typedef ACE_Timer_Wheel_T<ACE_Event_Handler*,
                          Common_TimeoutUpcall_t,
                          ACE_SYNCH_NULL_MUTEX,
                          Common_TimePolicy_t> Common_TimerQueueWheelImpl_t;
typedef ACE_Timer_Wheel_Iterator_T<ACE_Event_Handler*,
                                   Common_TimeoutUpcall_t,
                                   ACE_SYNCH_NULL_MUTEX,
                                   Common_TimePolicy_t> Common_TimerQueueWheelImplIterator_t;

typedef ACE_Abstract_Timer_Queue<ACE_Event_Handler*> Common_ITimerQueue_t;

//////////////////////////////////////////

typedef Common_ITimerCB_T<struct Common_TimerConfiguration> Common_ITimerCB_t;

#endif
