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

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iinitialize.h"
#include "common_istatemachine.h"

// forward declarations
class ACE_Time_Value;

// static variables
static const char default_statemachine_name_string_[] =
  ACE_TEXT_ALWAYS_CHAR ("StateMachine");

template <const char* StateMachineName, // *TODO*: use a variadic character array
          ACE_SYNCH_DECL,
          typename StateType,           // implements enum
          typename InterfaceType>       // implements Common_IStateMachine_T<StateType>
class Common_StateMachine_Base_T
 : virtual public InterfaceType
 , public Common_IInitialize_T<ACE_SYNCH_MUTEX_T>
{
 public:
  // convenient types
  typedef InterfaceType INTERFACE_T;
  typedef Common_StateMachine_Base_T<StateMachineName,
                                     ACE_SYNCH_USE,
                                     StateType,
                                     InterfaceType> STATEMACHINE_BASE_T;

  virtual ~Common_StateMachine_Base_T ();

  // implement (part of) Common_IStateMachine_T
  virtual StateType current () const;
  inline virtual bool initialize () { reset (); return true; }
  inline virtual void reset () { change (static_cast<StateType> (0)); }
  virtual bool wait (StateType,
                     const ACE_Time_Value* = NULL) const; // timeout (absolute) ? : block

 protected:
  Common_StateMachine_Base_T (ACE_SYNCH_MUTEX_T*,                       // lock handle
                              StateType = static_cast<StateType> (-1)); // (default) state

  // implement (part of) Common_IStateMachine_T
  virtual bool change (StateType); // new state

  // implement Common_IIinitialize_T
  virtual bool initialize (const ACE_SYNCH_MUTEX_T&);

  // *TODO*: SHOULD probably be recursive, so derived classes can retrieve the
  //         current state from within change() without deadlocking
  //ACE_SYNCH_RECURSIVE_CONDITION     condition_;
  ACE_SYNCH_CONDITION_T*     condition_;
  mutable ACE_SYNCH_MUTEX_T* stateLock_;

  StateType                  state_;
  bool                       isInitialized_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T (const Common_StateMachine_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StateMachine_Base_T& operator= (const Common_StateMachine_Base_T&))
};

// include template definition
#include "common_statemachine_base.inl"

#endif
