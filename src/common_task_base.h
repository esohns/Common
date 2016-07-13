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

#include "ace/Global_Macros.h"
#include "ace/Message_Queue_T.h"
#include "ace/Task.h"

#include "common_idumpstate.h"

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Module;

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
class Common_TaskBase_T
 : public ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType>
 , public Common_IDumpState
{
 public:
  virtual ~Common_TaskBase_T ();

  // override ACE_Task_Base members
  virtual int close (u_long = 0);

  // implement Common_IDumpState
  // *NOTE*: this is just a default stub...
  virtual void dump_state () const;

 protected:
  // convenient types
  typedef ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType> MODULE_T;

  Common_TaskBase_T (const std::string&,                         // thread name
                     int,                                        // (thread) group id
                     unsigned int = 1,                           // # thread(s)
                     bool = true,                                // auto-start ?
                     ////////////////////
                     ACE_Message_Queue<ACE_SYNCH_USE,
                                       TimePolicyType>* = NULL); // queue handle

  // override ACE_Task_Base members
  virtual int open (void* = NULL);
  virtual int put (ACE_Message_Block*,
                   ACE_Time_Value*);

  // helper methods
  // enqueue MB_STOP --> stop worker thread(s)
  void shutdown ();

  unsigned int threadCount_;

 private:
  typedef ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType> inherited;

  // override/hide ACE_Task_Base members
  virtual int module_closed (void);
  virtual int svc (void);

  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T (const Common_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_TaskBase_T& operator= (const Common_TaskBase_T&))

  std::string threadName_;
};

// include template implementation
#include "common_task_base.inl"

#endif
