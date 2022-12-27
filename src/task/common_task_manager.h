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

#ifndef COMMON_TASK_MANAGER_T_H
#define COMMON_TASK_MANAGER_T_H

#include "ace/Condition_Recursive_Thread_Mutex.h"
#include "ace/Containers_T.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_icounter.h"
#include "common_isubscribe.h"
#include "common_itaskcontrol.h"
#include "common_task.h"

#include "common_timer_resetcounterhandler.h"

// forward declarations
class ACE_Time_Value;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
class Common_Task_Manager_T
 : public Common_ITaskManager
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IRegister_T<Common_Task_2<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           StatisticContainerType> >
 , public Common_ICounter
{
  // singleton has access to the ctor/dtors
  friend class ACE_Singleton<Common_Task_Manager_T<ACE_SYNCH_USE,
                                                   TimePolicyType,
                                                   StatisticContainerType,
                                                   ConfigurationType,
                                                   UserDataType>,
                             ACE_SYNCH_MUTEX_T>;

 public:
  // convenience types
  typedef Common_ITaskManager INTERFACE_T;
  typedef Common_Task_2<ACE_SYNCH_USE,
                        TimePolicyType,
                        StatisticContainerType> TASK_T;
  typedef ACE_Singleton<Common_Task_Manager_T<ACE_SYNCH_USE,
                                              TimePolicyType,
                                              StatisticContainerType,
                                              ConfigurationType,
                                              UserDataType>,
                        ACE_SYNCH_MUTEX_T> SINGLETON_T;

  // configuration / initialization
  virtual bool initialize (const ConfigurationType&);

  bool lock (bool = true); // block ?
  int unlock (bool = false); // unblock ?

  // implement (part of) Common_ITaskManager
  inline virtual bool isRunning () const { return isActive_; }
  inline virtual bool isShuttingDown () const { return !isRunning (); }
  virtual bool start (ACE_Time_Value* = NULL); // N/A
  virtual void stop (bool = true,   // wait for completion ?
                     bool = false); // N/A
  virtual void wait (bool = true) const; // N/A
  virtual void dump_state () const;
  virtual void abort (bool = false); // wait for completion ?
  virtual unsigned int count () const; // return value: # of tasks

  // implement Common_IRegister_T<TASK_T>
  virtual bool register_ (TASK_T*); // task handle
  virtual void deregister (TASK_T*); // task handle

  virtual TASK_T* operator[] (unsigned int) const; // index

 private:
  Common_Task_Manager_T ();
  ACE_UNIMPLEMENTED_FUNC (Common_Task_Manager_T (const Common_Task_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_Manager_T& operator= (const Common_Task_Manager_T&))
  virtual ~Common_Task_Manager_T ();

  // convenient types
  typedef Common_Task_Manager_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                StatisticContainerType,
                                ConfigurationType,
                                UserDataType> OWN_TYPE_T;

  typedef ACE_DLList<TASK_T> CONTAINER_T;
  typedef ACE_DLList_Iterator<TASK_T> CONTAINER_ITERATOR_T;
  typedef ACE_DLList_Reverse_Iterator<TASK_T> CONTAINER_REVERSEITERATOR_T;

  // override/hide (part of) Common_ITask
  inline virtual void idle () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void pause () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void resume () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  // implement Common_ICounter
  // *NOTE*: visits each task updating its statistic
  virtual void reset ();

  void abortLeastRecent ();
  void abortMostRecent ();

  ConfigurationType*                           configuration_;
  bool                                         isActive_;

  // timer
  Common_Timer_ResetCounterHandler             resetTimeoutHandler_;
  long                                         resetTimeoutHandlerId_;

  CONTAINER_T                                  tasks_;
  // *NOTE*: MUST be recursive, otherwise asynchronous abort is not feasible
  mutable ACE_SYNCH_RECURSIVE_MUTEX            lock_;
  mutable ACE_Condition_Recursive_Thread_Mutex condition_;

  UserDataType*                                userData_;
};

// include template definition
#include "common_task_manager.inl"

#endif
