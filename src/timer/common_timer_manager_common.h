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

#ifndef COMMON_TIMER_MANAGER_COMMON_H
#define COMMON_TIMER_MANAGER_COMMON_H

#include "ace/Event_Handler.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
 // *NOTE*: try not to include this header in .h files (it's complicated)
 // *TODO*: replace inclusion of Condition_Recursive_Thread_Mutex.h
 //         with Synch_Traits.h --> submit an ACE issue
#include "ace/Timer_Queue_Adapters.h"

#include "common_timer_common.h"
#include "common_timer_manager.h"

//// forward declarations
//template <class TQ, class TYPE>
//class ACE_Async_Timer_Queue_Adapter;
//template <class TQ, class TYPE>
//class ACE_Thread_Timer_Queue_Adapter;

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

//////////////////////////////////////////

typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerHeapBase_t> Common_Timer_ManagerHeap_t;
typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerListBase_t> Common_Timer_ManagerList_t;
typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerWheelBase_t> Common_Timer_ManagerWheel_t;

typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerAsynchHeapBase_t> Common_Timer_ManagerAsynchHeap_t;
typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerAsynchListBase_t> Common_Timer_ManagerAsynchList_t;
typedef Common_Timer_Manager_T<ACE_NULL_SYNCH,
                               struct Common_TimerConfiguration,
                               Common_TimerManagerAsynchWheelBase_t> Common_Timer_ManagerAsynchWheel_t;

//////////////////////////////////////////

// *NOTE*: this specifies the (default) strategy
typedef Common_Timer_ManagerHeap_t Common_Timer_Manager_t;
typedef Common_Timer_ManagerAsynchHeap_t Common_Timer_Manager_Asynch_t;

typedef typename Common_Timer_Manager_t::INTERFACE_T Common_ITimer_Manager_t;
// *TODO*: this will not quite compile as of yet
//typedef typename Common_Timer_Manager_Asynch_t::INTERFACE_T Common_ITimer_Manager_Asynch_t;

typedef ACE_Singleton<Common_Timer_Manager_t,
                      ACE_SYNCH_MUTEX> COMMON_TIMERMANAGER_SINGLETON;
typedef ACE_Singleton<Common_Timer_Manager_Asynch_t,
                      ACE_SYNCH_MUTEX> COMMON_ASYNCHTIMERMANAGER_SINGLETON;
//COMMON_SINGLETON_DECLARE (ACE_Singleton,
//                          Common_Timer_Manager_t,
//                          ACE_SYNCH_MUTEX);
//COMMON_SINGLETON_DECLARE (ACE_Singleton,
//                          Common_Timer_Manager_Asynch_t,
//                          ACE_SYNCH_MUTEX);

#endif
