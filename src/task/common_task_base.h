﻿/***************************************************************************
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

#ifndef COMMON_TASK_BASE_H
#define COMMON_TASK_BASE_H

#include <string>
#include <vector>

#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Synch_Traits.h"

#include "common_idumpstate.h"
#include "common_itaskcontrol.h"

// forward declaration(s)
class ACE_Message_Block;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Module;
class ACE_Time_Value;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType, // implements Common_ILock_T/Common_IRecursiveLock_T
          typename MessageType,
          typename QueueType, // i.e. ACE_Message_Queue<> or ACE_Message_Queue_Ex<>
          typename TaskType> // i.e. ACE_Task<> or ACE_Task_Ex<>
class Common_TaskBase_T
 : public TaskType
 , virtual public Common_ITaskControl_T<ACE_SYNCH_USE,
                                        LockType>
 , public Common_IDumpState
{
  typedef TaskType inherited;

 public:
  // convenient types
  typedef ACE_SYNCH_USE SYNCH_T;
  typedef Common_ITaskControl_T<ACE_SYNCH_USE,
                                LockType> ITASKCONTROL_T;
  typedef typename ITASKCONTROL_T::ITASK_T ITASK_T;

  virtual ~Common_TaskBase_T ();

  // implement Common_ITaskControl_T
  virtual bool lock (bool = true); // block ?
  inline virtual int unlock (bool = false) { return lock_.release (); }
  inline virtual const typename LockType::MUTEX_T& getR_2 () const { return lock_; }
  // *NOTE*: wraps ACE_Task_Base::activate() to spawn one additional (worker-)
  //         thread (up to threadCount_)
  // *TODO*: derivates may want to implement a dynamic thread pool
  //         --> call ACE_Task_Base::activate() directly in this case
  virtual void start (ACE_thread_t&); // return value: thread handle (if any)
  inline virtual bool isRunning () const { return (threadCount_ ? (inherited::thr_count_ > 0) : false); }
  virtual void idle ();
  // *NOTE*: calls close(1)
  virtual void finished ();
  virtual void wait (bool = true) const; // wait for the message queue ? : worker thread(s) only

  // stub Common_IDumpState
  inline virtual void dump_state () const {}

 protected:
  // convenient types
  typedef MessageType MESSAGE_T;
  typedef QueueType MESSAGE_QUEUE_T;
  typedef TaskType TASK_T;
  typedef ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType> MODULE_T;

  Common_TaskBase_T (const std::string&,       // thread name
                     int,                      // (thread) group id
                     unsigned int = 1,         // # thread(s)
                     bool = true,              // auto-start ?
                     MESSAGE_QUEUE_T* = NULL); // queue handle

  // override ACE_Task_Base members
  // *NOTE*: spawns threadCount_ worker thread(s)
  virtual int open (void* = NULL);
//  inline virtual int put (ACE_Message_Block* messageBlock_in, ACE_Time_Value* timeout_in) { ACE_ASSERT (inherited::thr_count_); return inherited::putq (static_cast<MessageType*> (messageBlock_in), timeout_in); }

  mutable typename LockType::MUTEX_T lock_;

  // *NOTE*: this is the 'configured' (not the 'current') thread count
  //         --> see ACE_Task::thr_count_
  unsigned int                       threadCount_;
  std::string                        threadName_;
  typedef std::vector<ACE_Thread_ID> THREAD_IDS_T;
  typedef THREAD_IDS_T::const_iterator THREAD_IDS_ITERATOR_T;
  THREAD_IDS_T                       threadIds_;

 private:
  // convenient types
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType,
                            MessageType,
                            QueueType,
                            TaskType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T (const Common_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T& operator= (const Common_TaskBase_T&))

  // override/hide ACE_Task_Base members
  virtual int close (u_long = 0);
  inline virtual int module_closed (void) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  // *NOTE*: the default implementation does nothing; it frees all messages
  //         until receiving MB_STOP
//  virtual int svc (void);
};

//////////////////////////////////////////

// partial specialization (fully synchronous)
template <typename TimePolicyType,
          typename LockType, // implements Common_ILock_T/Common_IRecursiveLock_T
          typename MessageType,
          typename QueueType,
          typename TaskType>
class Common_TaskBase_T<ACE_NULL_SYNCH,
                        TimePolicyType,
                        LockType,
                        MessageType,
                        QueueType,
                        TaskType>
 : public TaskType
 , virtual public Common_ITaskControl_T<ACE_NULL_SYNCH,
                                        LockType>
 , public Common_IDumpState
{
  typedef TaskType inherited;

 public:
  // convenient types
  typedef Common_ITaskControl_T<ACE_NULL_SYNCH,
                                LockType> ITASKCONTROL_T;
  typedef typename ITASKCONTROL_T::ITASK_T ITASK_T;

  inline virtual ~Common_TaskBase_T () {}

  // implement Common_ITaskControl_T
  inline virtual bool lock (bool block_in = true) { return ((block_in ? lock_.acquire () : lock_.tryacquire ()) == 0); }
  inline virtual int unlock (bool = false) { return lock_.release (); }
  inline virtual const typename LockType::MUTEX_T& getR () const { return lock_; }
  inline virtual void start (ACE_thread_t& threadId_out) { threadId_out = 0; }
  inline virtual void stop (bool = true, bool = true, bool = true) {}
  inline virtual bool isRunning () const { return true; }
  inline virtual void idle () {}
  inline virtual void finished () {}
  inline virtual void wait (bool = true) const {} // wait for the message queue ? : worker thread(s) only

  // stub Common_IDumpState
  inline virtual void dump_state () const {}

 protected:
  // convenient types
  typedef MessageType MESSAGE_T;
  typedef QueueType MESSAGE_QUEUE_T;
  typedef TaskType TASK_T;
  typedef ACE_Module<ACE_NULL_SYNCH,
                     TimePolicyType> MODULE_T;

  Common_TaskBase_T (const std::string&,       // thread name
                     int,                      // (thread) group id
                     unsigned int = 1,         // # thread(s)
                     bool = true,              // auto-start ?
                     MESSAGE_QUEUE_T* = NULL); // queue handle

  // helper methods
  inline virtual bool hasShutDown () { return true; }

  mutable typename LockType::MUTEX_T lock_;

 private:
  // convenient types
  typedef Common_TaskBase_T<ACE_NULL_SYNCH,
                            TimePolicyType,
                            LockType,
                            MessageType,
                            QueueType,
                            TaskType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T (const Common_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T& operator= (const Common_TaskBase_T&))

  // override/hide ACE_Task_Base members
  inline virtual int module_closed (void) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
};

//////////////////////////////////////////

// include template definition
#include "common_task_base.inl"

#endif
