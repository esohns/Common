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

#include "common_iinitialize.h"
#include "common_istatemachine.h"

// forward declarations
class ACE_Time_Value;

template <ACE_SYNCH_DECL,
          typename StateType>
class Common_StateMachine_Base_T
 : virtual public Common_IStateMachine_T<StateType>
 , public Common_IInitialize_T<ACE_SYNCH_MUTEX_T>
{
 public:
  Common_StateMachine_Base_T (ACE_SYNCH_MUTEX_T*,                       // lock handle
                              StateType = static_cast<StateType> (-1)); // (default) state
  virtual ~Common_StateMachine_Base_T ();

  // implement (part of) Common_IStateMachine_T
  virtual StateType current () const;

  // implement Common_IIinitialize_T
  virtual bool initialize (const ACE_SYNCH_MUTEX_T&);

 protected:
  virtual bool change (StateType); // new state

  //   *IMPORTANT NOTE*: SHOULD probably be recursive, so derived classes can
  //                     retrieve the current state from within change()
  //                     without deadlocking
  //ACE_SYNCH_RECURSIVE_CONDITION     condition_;
  ACE_SYNCH_CONDITION_T*     condition_;
  mutable ACE_SYNCH_MUTEX_T* stateLock_;

  StateType                  state_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T (const Common_StateMachine_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T& operator= (const Common_StateMachine_Base_T&))

  // implement (part of) Common_IStateMachine_T
  virtual bool wait (StateType,
                     const ACE_Time_Value* = NULL);

  bool                       isInitialized_;
};

// include template definition
#include "common_statemachine_base.inl"

#endif
