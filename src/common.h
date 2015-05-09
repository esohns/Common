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

// timer queue
#include "ace/Version.h"
#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 0) || \
     (ACE_BETA_VERSION  > 3))
#include "ace/Event_Handler_Handle_Timeout_Upcall.h"
#else
#include "ace/Timer_Queuefwd.h"
#endif
#include "ace/Module.h"
#include "ace/Signal.h"
#include "ace/Synch_Traits.h"
#include "ace/Task.h"
//#include "ace/Timer_Heap_T.h"
#include "ace/Timer_List_T.h"
//#include "ace/Timer_Wheel_T.h"
#include "ace/Timer_Queue_T.h"
#include "ace/Timer_Queue_Adapters.h"
#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 1) || \
     (ACE_BETA_VERSION  > 6))
#include "ace/Time_Policy.h"

#include "common_defines.h"

// *NOTE*: use the high resolution time for accuracy and low latency
typedef ACE_HR_Time_Policy Common_TimePolicy_t;
#else
#error "ACE version > 6.1.6 required"
typedef ACE_System_Time_Policy Common_TimePolicy_t;
#endif

// *NOTE*: ensure a minimal amount of locking
//typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_NULL_MUTEX> Common_TimeoutUpcall_t;
typedef ACE_Event_Handler_Handle_Timeout_Upcall Common_TimeoutUpcall_t;
// *WARNING*: apparently, ACEs' timer heap implementation currently has some
//            stability issue...
//typedef ACE_Timer_Heap_T<ACE_Event_Handler*,
//                         Common_TimeoutUpcall_t,
//                         ACE_SYNCH_NULL_MUTEX,
//                         Common_TimePolicy_t> Common_TimerQueueImpl_t;
//typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*,
//                                  Common_TimeoutUpcall_t,
//                                  ACE_SYNCH_NULL_MUTEX,
//                                  Common_TimePolicy_t> Common_TimerQueueImplIterator_t;
typedef ACE_Timer_List_T<ACE_Event_Handler*,
                         Common_TimeoutUpcall_t,
                         ACE_SYNCH_NULL_MUTEX,
                         Common_TimePolicy_t> Common_TimerQueueImpl_t;
typedef ACE_Timer_List_Iterator_T<ACE_Event_Handler*,
                                  Common_TimeoutUpcall_t,
                                  ACE_SYNCH_NULL_MUTEX,
                                  Common_TimePolicy_t> Common_TimerQueueImplIterator_t;
//typedef ACE_Timer_Wheel_T<ACE_Event_Handler*,
//                          Common_TimeoutUpcall_t,
//                          ACE_SYNCH_NULL_MUTEX,
//                          Common_TimePolicy_t> Common_TimerQueueImpl_t;
//typedef ACE_Timer_Wheel_Iterator_T<ACE_Event_Handler*,
//                                   Common_TimeoutUpcall_t,
//                                   ACE_SYNCH_NULL_MUTEX,
//                                   Common_TimePolicy_t> Common_TimerQueueImplIterator_t;
typedef ACE_Thread_Timer_Queue_Adapter<Common_TimerQueueImpl_t,
                                       ACE_Event_Handler*> Common_TimerQueue_t;

typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

typedef ACE_Task<ACE_MT_SYNCH,
                 Common_TimePolicy_t> Common_Task_t;
typedef ACE_Module<ACE_MT_SYNCH,
                   Common_TimePolicy_t> Common_Module_t;

#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 1) || \
     (ACE_BETA_VERSION  > 6))
// *NOTE*: global time policy (supplies gettimeofday())
#if defined __GNUC__
//#pragma GCC diagnostic ignored "-Wunused-variable"
COMMON_GCC_UNUSED_GUARD static Common_TimePolicy_t COMMON_TIME_POLICY;
//#pragma GCC diagnostic pop
#else
static Common_TimePolicy_t COMMON_TIME_POLICY;
#endif
#define COMMON_TIME_NOW COMMON_TIME_POLICY ()
#else
#define COMMON_TIME_NOW ACE_OS::gettimeofday ()
#endif

typedef std::deque<std::string> Common_MessageStack_t;
typedef Common_MessageStack_t::const_iterator Common_MessageStackConstIterator_t;
//typedef std::deque<ACE_Log_Record> Common_LogRecordStack_t;

#endif
