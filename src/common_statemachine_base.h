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

#ifndef COMMON_STATEMACHINE_BASE_H
#define COMMON_STATEMACHINE_BASE_H

#include "ace/Condition_T.h"
#include "ace/Global_Macros.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Synch_Traits.h"

#include "common_istatemachine.h"

// forward declarations
class ACE_Time_Value;

template <typename StateType>
class Common_StateMachine_Base_T
 : virtual public Common_IStateMachine_T<StateType>
{
 public:
  Common_StateMachine_Base_T (StateType = static_cast<StateType> (-1));
  virtual ~Common_StateMachine_Base_T ();

  // implement (part of) Common_IStateMachine_T
  virtual const StateType& current () const;

 protected:
  virtual bool change (StateType); // new state

  //   *IMPORTANT NOTE*: MUST be recursive, so child classes can retrieve the
  //                     current state from within onStateChange without
  //                     deadlock
  //ACE_SYNCH_CONDITION<ACE_SYNCH_RECURSIVE_MUTEX> condition_;
  ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX> condition_;
  mutable ACE_SYNCH_RECURSIVE_MUTEX        stateLock_;

  StateType                                state_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T (const Common_StateMachine_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T& operator= (const Common_StateMachine_Base_T&))

  // implement (part of) Common_IStateMachine_T
  virtual bool wait (const ACE_Time_Value* = NULL);
};

// include template implementation
#include "common_statemachine_base.inl"

#endif
