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

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Message_Queue_T.h"
#include "ace/Task_T.h"

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
  typedef Common_ITaskHandler_T<ACE_Message_Block> ITASKHANDLER_T;

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
  virtual bool isShuttingDown ();
  // enqueue MB_STOP --> stop worker thread(s)
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // high priority ? (i.e. do not wait for queued messages)

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
                 bool = true,                                  // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL); // queue handle

  // helper methods
  // *NOTE*: 'high priority' effectively means that the message is enqueued at
  //         the head end (i.e. will be the next to dequeue), whereas it would
  //         be enqueued at the tail end otherwise
  void control (int,           // message type
                bool = false); // high-priority ?

  // override ACE_Task_Base members
  virtual int svc (void);

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
                 bool = true,                                  // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL); // queue handle

  // override ACE_Task_Base members
  inline virtual int put (ACE_Message_Block*, ACE_Time_Value*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }

  //virtual int svc (void);

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T (const Common_Task_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T& operator= (const Common_Task_T&))
};

// include template definition
#include "common_task.inl"

#endif
