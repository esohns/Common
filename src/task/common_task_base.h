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

#ifndef COMMON_TASK_BASE_H
#define COMMON_TASK_BASE_H

#include <string>
#include <vector>

#include "ace/Condition_Thread_Mutex.h"
#include "ace/Global_Macros.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Synch_Traits.h"
#include "ace/Thread_Mutex.h"
#include "ace/Time_Value.h"

#include "common_iget.h"
#include "common_time_common.h"

#include "common_idumpstate.h"
#include "common_itask.h"
#include "common_itaskcontrol.h"

// forward declaration(s)
class ACE_Task_Base;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Module;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType, // i.e. ACE_Message_Queue<> or ACE_Message_Queue_Ex<>
          typename TaskType> // i.e. ACE_Task<> or ACE_Task_Ex<>
class Common_TaskBase_T
 : public TaskType
 , public Common_IAsynchTask
 , public Common_ITaskControl
 , public Common_IDumpState
 , public Common_IGet_T<unsigned int>
{
  typedef TaskType inherited;

 public:
  // convenient types
  typedef ACE_SYNCH_USE SYNCH_T;
  typedef std::vector<ACE_Thread_ID> THREAD_IDS_T;
  typedef THREAD_IDS_T::iterator THREAD_IDS_ITERATOR_T;
  typedef THREAD_IDS_T::const_iterator THREAD_IDS_CONSTITERATOR_T;

  virtual ~Common_TaskBase_T ();

  // implement Common_IAsynchTask
  virtual void idle () const;
  inline virtual bool isRunning () const { return (threadCount_ ? (inherited::thr_count_ > 0) : false); }
  virtual bool start (ACE_Time_Value* = NULL); // duration ? (relative !) timeout : run until finished
  virtual void wait (bool = true) const; // wait for the message queue ? : worker thread(s) only
  virtual void pause () const;
  virtual void resume () const;

  // implement Common_ITaskControl
  inline virtual void execute (ACE_Time_Value* timeout_in = NULL) { start (timeout_in); } // duration ? (relative !) timeout : run until finished

  // stub Common_IDumpState
  inline virtual void dump_state () const {}

  // implement Common_IGet_T
  inline virtual const unsigned int get () const { return threadCount_; }

 protected:
  // convenient types
  typedef MessageType MESSAGE_T;
  typedef QueueType MESSAGE_QUEUE_T;
  typedef TaskType TASK_T;

  Common_TaskBase_T (const std::string&,       // thread name
                     int,                      // (thread) group id
                     unsigned int = 1,         // # thread(s)
                     bool = true,              // auto-start ?
                     MESSAGE_QUEUE_T* = NULL); // queue handle

  // override/hide ACE_Task_Base members
  // *NOTE*: spawns threadCount_ worker thread(s)
  virtual int open (void* = NULL);
  //  inline virtual int put (ACE_Message_Block* messageBlock_in, ACE_Time_Value* timeout_in) { ACE_ASSERT (inherited::thr_count_); return inherited::putq (static_cast<MessageType*> (messageBlock_in), timeout_in); }

  // implement Common_IAsynchTask
  // *NOTE*: calls close(1)
  virtual void finished ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool                 closeHandles_;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Time_Value       deadline_;
  bool                 lockActivate_; // grab inherited::lock_ in open() ?
  // *NOTE*: this is the 'configured' (not the 'current') thread count
  //         --> see ACE_Task::thr_count_ for that
  unsigned int         threadCount_;
  std::string          threadName_;
  THREAD_IDS_T         threadIds_;

 private:
  // convenient types
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
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

  // helper methods
  // *IMPORTANT NOTE*: must usually (!, see lockActivate_) be called with
  //                   inherited::lock_ held, otherwise it just does the same as
  //                   ACE_Task_Base::activate()
  int activate_i (long,            // flags
                  int,             // n_threads
                  int,             // force_active
                  long,            // priority
                  int,             // grp_id
                  ACE_Task_Base*,  // task
                  ACE_hthread_t[], // thread_handles
                  void*[],         // stack
                  size_t[],        // stack_size
                  ACE_thread_t[],  // thread_ids
                  const char*[]);  // thr_name

  ACE_Condition_Thread_Mutex condition_;
};

//////////////////////////////////////////

// partial specialization (fully synchronous)
template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
class Common_TaskBase_T<ACE_NULL_SYNCH,
                        TimePolicyType,
                        MessageType,
                        QueueType,
                        TaskType>
 : public TaskType
 , public Common_ITask
 , public Common_ITaskControl
 , public Common_IDumpState
{
  typedef TaskType inherited;

 public:
  // convenient types
  typedef ACE_NULL_SYNCH SYNCH_T;

  inline virtual ~Common_TaskBase_T () {}

  // implement Common_ITask
  inline virtual void idle () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual bool isRunning () const { return (!stopped_ && (threadId_.handle () != ACE_INVALID_HANDLE)); }
  inline virtual bool isShuttingDown () const { return stopped_; }
  virtual bool start (ACE_Time_Value* timeout_in = NULL);
  virtual void stop (bool = true,   // wait for completion ?
                     bool = false); // N/A
  virtual void wait (bool = true) const; // N/A
  virtual void pause () const;
  virtual void resume () const;

  // implement Common_ITaskControl
  inline virtual void execute (ACE_Time_Value* timeout_in = NULL) { start (timeout_in); }

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

  ACE_Time_Value                     deadline_;
  bool                               stopped_;
  ACE_Thread_ID                      threadId_;

 private:
  // convenient types
  typedef Common_TaskBase_T<ACE_NULL_SYNCH,
                            TimePolicyType,
                            MessageType,
                            QueueType,
                            TaskType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T (const Common_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T& operator= (const Common_TaskBase_T&))

  // override/hide ACE_Task_Base members
  inline virtual int module_closed (void) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }

  mutable ACE_Condition_Thread_Mutex condition_;
  bool                               finished_;
};

//////////////////////////////////////////

// include template definition
#include "common_task_base.inl"

#endif
