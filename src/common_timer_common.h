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

#include "ace/Abstract_Timer_Queue.h"
#include "ace/Event_Handler.h"
#include "ace/Version.h"
#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 0) || \
     (ACE_BETA_VERSION  > 3))
#include "ace/Event_Handler_Handle_Timeout_Upcall.h"
#else
#include "ace/Timer_Queuefwd.h"
#endif
#include "ace/Synch_Traits.h"
#include "ace/Timer_Heap_T.h"
#include "ace/Timer_List_T.h"
#include "ace/Timer_Wheel_T.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Queue_T.h"

#include "common_time_common.h"

// *** timer queue ***
// *NOTE*: ensure a minimal amount of locking
//typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_NULL_MUTEX> Common_TimeoutUpcall_t;
typedef ACE_Event_Handler_Handle_Timeout_Upcall Common_TimeoutUpcall_t;
//// *WARNING*: apparently, ACEs' timer heap implementation currently has some
////            stability issue...
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
typedef ACE_Async_Timer_Queue_Adapter<Common_TimerQueueHeapImpl_t,
                                      ACE_Event_Handler*> Common_TimerManagerAsynchHeapBase_t;
typedef ACE_Async_Timer_Queue_Adapter<Common_TimerQueueListImpl_t,
                                      ACE_Event_Handler*> Common_TimerManagerAsynchListBase_t;
typedef ACE_Async_Timer_Queue_Adapter<Common_TimerQueueWheelImpl_t,
                                      ACE_Event_Handler*> Common_TimerManagerAsynchWheelBase_t;
typedef ACE_Thread_Timer_Queue_Adapter<Common_TimerQueueHeapImpl_t,
                                       ACE_Event_Handler*> Common_TimerManagerHeapBase_t;
typedef ACE_Thread_Timer_Queue_Adapter<Common_TimerQueueListImpl_t,
                                       ACE_Event_Handler*> Common_TimerManagerListBase_t;
typedef ACE_Thread_Timer_Queue_Adapter<Common_TimerQueueWheelImpl_t,
                                       ACE_Event_Handler*> Common_TimerManagerWheelBase_t;

typedef ACE_Abstract_Timer_Queue<ACE_Event_Handler*> Common_ITimerQueue_t;

#endif
