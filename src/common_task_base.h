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

#include <ace/Global_Macros.h>
#include <ace/OS_NS_Thread.h>
#include <ace/Task_T.h>

#include "common_idumpstate.h"
#include "common_itask.h"

// forward declaration(s)
class ACE_Message_Block;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Message_Queue;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Module;
class ACE_Time_Value;

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
class Common_TaskBase_T
 : public ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType>
 , public Common_ITask_T<ACE_SYNCH_USE>
 , public Common_IDumpState
{
 public:
  virtual ~Common_TaskBase_T ();

  // override ACE_Task_Base members
  virtual int close (u_long = 0);

  // implement Common_ITask
  // *NOTE*: this wraps ACE_Task_Base::activate() to spawn a single thread;
  //         returns false if the object was already 'active' (or something else
  //         went wrong; check errno)
  // *TODO*: callers may want to implement a dynamic thread pool
  //         --> call ACE_Task_Base::activate() directly in this case
  virtual void start ();
  // enqueue MB_STOP --> stop worker thread(s)
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;
  inline virtual int wait (void) { return ACE_Task_Base::wait (); };

  // implement Common_IDumpState
  // *NOTE*: this is just a default stub
  virtual void dump_state () const;

 protected:
  // convenient types
  typedef ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType> TASK_T;
  typedef ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType> MODULE_T;

  Common_TaskBase_T (const std::string&,                         // thread name
                     int,                                        // (thread) group id
                     unsigned int = 1,                           // # thread(s)
                     bool = true,                                // auto-start ?
                     /////////////////////
                     ACE_Message_Queue<ACE_SYNCH_USE,
                                       TimePolicyType>* = NULL); // queue handle

  // override ACE_Task_Base members
  virtual int open (void* = NULL);
  inline virtual int put (ACE_Message_Block* messageBlock_in,
                          ACE_Time_Value* timeout_in) { return inherited::putq (messageBlock_in, timeout_in); }

  // helper methods
  // *NOTE*: 'high priority' effectively means that the message is enqueued at
  //         the head end (i.e. will be the next to dequeue), whereas it would
  //         be enqueued at the tail end otherwise
  void control (int,           // message type
                bool = false); // high-priority ?

  // *NOTE*: this is the 'configured' (not the 'current') thread count
  //         --> see ACE_Task::thr_count_
  unsigned int threadCount_;
  std::string  threadName_;

  typedef std::vector<ACE_Thread_ID> THREAD_IDS_T;
  typedef THREAD_IDS_T::const_iterator THREAD_IDS_ITERATOR_T;
  THREAD_IDS_T threadIDs_;

 private:
  typedef ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType> inherited;

  // override/hide ACE_Task_Base members
  virtual int module_closed (void);
  virtual int svc (void);

  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T (const Common_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T& operator= (const Common_TaskBase_T&))
};

// include template definition
#include "common_task_base.inl"

#endif
