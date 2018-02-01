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
#include "ace/Synch_Traits.h"

#include "common_task_base.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType, // implements Common_ILock_T/Common_IRecursiveLock_T
          typename MessageType = ACE_Message_Block>
class Common_Task_T
 : public Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType>
 , public Common_ITaskHandler_T<MessageType>
{
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType> inherited;

 public:
  // convenient types
  typedef Common_ITaskHandler_T<MessageType> ITASKHANDLER_T;

  inline virtual ~Common_Task_T () {}

  // implement Common_ITaskHandler_T
  // *NOTE*: the default implementation does nothing; it frees all messages
  virtual void handle (MessageType*&); // message handle

 protected:
  Common_Task_T (const std::string&,                           // thread name
                 int,                                          // (thread) group id
                 unsigned int = 1,                             // # thread(s)
                 bool = true,                                  // auto-start ?
                 typename inherited::MESSAGE_QUEUE_T* = NULL); // queue handle

 private:
//  // convenient types
//  typedef Common_Task_T<ACE_SYNCH_USE,
//                        TimePolicyType,
//                        LockType,
//                        MessageType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Common_Task_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T (const Common_Task_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_T& operator= (const Common_Task_T&))

  virtual int svc (void);
};

// include template definition
#include "common_task.inl"

#endif
