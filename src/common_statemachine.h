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

#ifndef COMMON_STATEMACHINE_H
#define COMMON_STATEMACHINE_H

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Synch_Traits.h"

#include "common_statemachine_base.h"
#include "common_task.h"

template <const char* StateMachineName, // *TODO*: use a variadic character array
          ACE_SYNCH_DECL,
          typename StateType,     // implements enum
          typename InterfaceType> // implements Common_IStateMachine_T<StateType>
class Common_StateMachine_T
 : public Common_StateMachine_Base_T<StateMachineName,
                                     ACE_SYNCH_USE,
                                     StateType,
                                     InterfaceType>
{
  typedef Common_StateMachine_Base_T<StateMachineName,
                                     ACE_SYNCH_USE,
                                     StateType,
                                     InterfaceType> inherited;

 public:
  // convenient types
  typedef Common_StateMachine_T<StateMachineName,
                                ACE_SYNCH_USE,
                                StateType,
                                InterfaceType> STATEMACHINE_T;

  inline virtual ~Common_StateMachine_T () {}

 protected:
  Common_StateMachine_T (ACE_SYNCH_MUTEX_T*,                       // lock handle
                         StateType = static_cast<StateType> (-1)); // (default) state

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_T (const Common_StateMachine_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_T& operator= (const Common_StateMachine_T&))
};

//////////////////////////////////////////

template <const char* StateMachineName, // *TODO*: use a variadic character array
          ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType,       // implements Common_ILock_T/Common_IRecursiveLock_T
          typename StateType>      // implements enum
class Common_StateMachineAsynch_T
 : public Common_Task_T<ACE_SYNCH_USE,
                        TimePolicyType,
                        LockType,
                        ACE_Message_Block>
 , public Common_StateMachine_Base_T<StateMachineName,
                                     ACE_SYNCH_USE,
                                     StateType,
                                     Common_IStateMachine_2<StateType> >
{
  typedef Common_Task_T<ACE_SYNCH_USE,
                        TimePolicyType,
                        LockType,
                        ACE_Message_Block> inherited;
  typedef Common_StateMachine_Base_T<StateMachineName,
                                     ACE_SYNCH_USE,
                                     StateType,
                                     Common_IStateMachine_2<StateType> > inherited2;

 public:
  // convenient types
  typedef Common_Task_T<ACE_SYNCH_USE,
                        TimePolicyType,
                        LockType,
                        ACE_Message_Block> TASK_T;
  typedef Common_StateMachineAsynch_T<StateMachineName,
                                      ACE_SYNCH_USE,
                                      TimePolicyType,
                                      LockType,
                                      StateType> STATEMACHINE_T;

  inline virtual ~Common_StateMachineAsynch_T () {}

  // implement Common_ITaskHandler_T
  virtual void handle (ACE_Message_Block*&); // message handle

//  // override (part of) Common_ITaskControl_T
//  virtual void start () = 0;
//  virtual void stop (bool = true,      // wait for completion ?
//                     bool = true) = 0; // locked access ?
//  virtual bool isRunning () const = 0;
//  virtual void idle () = 0;
//  virtual void finished () = 0;
//  virtual void wait (bool = true) const = 0; // wait for the message queue ? : worker thread(s) only

  // implement (part of) Common_IStateMachine_2
  using inherited2::stateToString;
  inline virtual void finished () {}

 protected:
  Common_StateMachineAsynch_T (const std::string&,                       // thread name
                               int,                                      // (thread) group id
                               ACE_SYNCH_MUTEX_T*,                       // lock handle
                               StateType = static_cast<StateType> (-1)); // (default) state

  // override (part of) Common_IStateMachine_T
  inline virtual bool change (StateType nextState_in) { inherited::control (nextState_in, false); return true; }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachineAsynch_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachineAsynch_T (const Common_StateMachineAsynch_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachineAsynch_T& operator= (const Common_StateMachineAsynch_T&))
};

// include template definition
#include "common_statemachine.inl"

#endif
