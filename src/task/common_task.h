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

#ifndef COMMON_TASK_H
#define COMMON_TASK_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Message_Queue_T.h"
#include "ace/Task_T.h"

#include "common_istatistic.h"
#include "common_itaskcontrol.h"
#include "common_message_queue_iterator.h"

#include "common_task_base.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
class Common_Task_T
 : public Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_SYNCH_USE,
                                              TimePolicyType>,
                            ACE_Task<ACE_SYNCH_USE,
                                     TimePolicyType> >
 , public Common_ITaskHandler_T<ACE_Message_Block>
{
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_SYNCH_USE,
                                              TimePolicyType>,
                            ACE_Task<ACE_SYNCH_USE,
                                     TimePolicyType> > inherited;

 public:
  // convenient types
  typedef Common_ITaskHandler_T<ACE_Message_Block> IHANDLER_T;

  inline virtual ~Common_Task_T () {}

  // override ACE_Task_Base members
  inline virtual int put (ACE_Message_Block* messageBlock_in, ACE_Time_Value* timeout_in) { ACE_ASSERT (inherited::thr_count_); return inherited::putq (messageBlock_in, timeout_in); }

  // implement Common_IAsynchTask
  // *NOTE*: tests for MB_STOP anywhere in the queue. Note that this does not
  //         block, or dequeue any message
  // *NOTE*: ACE_Message_Queue_Iterator does its own locking, i.e. access
  //         happens in lockstep, which is both inefficient and yields
  //         unpredictable results
  //         --> use Common_MessageQueueIterator_T and lock the queue manually
  virtual bool isShuttingDown () const;
  // enqueue MB_STOP --> stop worker thread(s)
  virtual void stop (bool = true,   // wait for completion ?
                     bool = false); // high priority ? (i.e. do not wait for queued messages)

  // implement Common_ITaskHandler_T
  // *NOTE*: the default implementation does nothing; it frees all messages
  virtual void handle (ACE_Message_Block*&); // message handle

 protected:
  // convenient types
  typedef Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                                        TimePolicyType> MESSAGE_QUEUE_ITERATOR_T;

  Common_Task_T (const std::string&,                           // thread name
                 int,                                          // (thread) group id
                 unsigned int = 1,                             // # thread(s)
                 // *WARNING*: auto-starting in the ctor is dangerous, as
                 //            (overridden) svc() may not be available until the
                 //            instance is fully constructed. In this case, the
                 //            thread will execute ACE_Task_Base::svc() which
                 //            does nothing and returns immediately
                 bool = false,                                 // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL); // queue handle

  // helper methods
  // *NOTE*: 'high priority' effectively means that the message is enqueued at
  //         the head end (i.e. will be the next to dequeue), whereas it would
  //         be enqueued at the tail end otherwise
  void control (int,           // message type
                bool = false); // high-priority ?

  // override ACE_Task_Base members
  virtual int svc ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T (const Common_Task_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T& operator= (const Common_Task_T&))
};

//////////////////////////////////////////

// partial specialization (fully synchronous)
template <typename TimePolicyType>
class Common_Task_T<ACE_NULL_SYNCH,
                    TimePolicyType>
 : public Common_TaskBase_T<ACE_NULL_SYNCH,
                            TimePolicyType,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_NULL_SYNCH,
                                              TimePolicyType>,
                            ACE_Task<ACE_NULL_SYNCH,
                                     TimePolicyType> >
{
  typedef Common_TaskBase_T<ACE_NULL_SYNCH,
                            TimePolicyType,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_NULL_SYNCH,
                                              TimePolicyType>,
                            ACE_Task<ACE_NULL_SYNCH,
                                     TimePolicyType> > inherited;

 public:
  inline virtual ~Common_Task_T () {}

 protected:
  Common_Task_T (const std::string&,                           // thread name
                 int,                                          // (thread) group id
                 unsigned int = 1,                             // # thread(s)
                 bool = false,                                 // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL); // queue handle

  // override ACE_Task_Base members
  inline virtual int put (ACE_Message_Block*, ACE_Time_Value*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }

  //virtual int svc (void);

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T (const Common_Task_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T& operator= (const Common_Task_T&))
};

//////////////////////////////////////////

// this version supports statistics and the task manager
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType> // inherits struct Common_Task_Statistic
class Common_Task_2
 : public Common_Task_T<ACE_SYNCH_USE,
                        TimePolicyType>
 , public Common_IStatistic_T<StatisticContainerType>
{
  typedef Common_Task_T<ACE_SYNCH_USE,
                        TimePolicyType> inherited;

 public:
  virtual ~Common_Task_2 ();

  // implement (part of) Common_IStatistic_T
  inline virtual bool collect (StatisticContainerType& container_inout) { ACE_UNUSED_ARG (container_inout); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual void update (const ACE_Time_Value& interval_in) { ACE_UNUSED_ARG (interval_in); ACE_NOTSUP; }
  virtual void report () const;

  // override Common_IAsynchTask
  virtual void finished ();

 protected:
  Common_Task_2 (const std::string&,                          // thread name
                 int,                                         // (thread) group id
                 unsigned int = 1,                            // # thread(s)
                 bool = false,                                // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL, // queue handle
                 /////////////////////////
                 Common_ITaskManager* = NULL);                // manager handle

  bool                   deregisterInDtor_;
  Common_ITaskManager*   manager_; // handle to manager (if any)
  bool                   reportOnFinished_;
  StatisticContainerType statistic_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Task_2 ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_2 (const Common_Task_2&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_2& operator= (const Common_Task_2&))
};

// include template definition
#include "common_task.inl"

#endif
