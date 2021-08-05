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

#ifndef COMMON_ITASK_H
#define COMMON_ITASK_H

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/OS.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"

template <ACE_SYNCH_DECL,
          typename LockType> // implements Common_ILock_T/Common_IRecursiveLock_T
class Common_ITask_T
 : virtual public LockType
{
 public:
  virtual void start (ACE_thread_t&) = 0; // return value: thread handle (if any)
  virtual void stop (bool = true,      // wait for completion ?
                     bool = true,      // high priority ? (i.e. do not wait for queued messages)
                     bool = true) = 0; // locked access ?
  virtual bool isRunning () const = 0;

  virtual void idle () = 0;
};

template <typename MessageType = ACE_Message_Block>
class Common_ITaskHandler_T
{
 public:
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void handle (MessageType*&) = 0; // message handle
};

//////////////////////////////////////////

typedef Common_ITask_T<ACE_MT_SYNCH,
                       Common_ILock_T<ACE_MT_SYNCH> > Common_ITask_t;
typedef Common_ITask_T<ACE_MT_SYNCH,
                       Common_IRecursiveLock_T<ACE_MT_SYNCH> > Common_IRecursiveTask_t;

#endif
