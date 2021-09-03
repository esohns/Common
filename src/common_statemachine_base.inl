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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::Common_StateMachine_Base_T (ACE_SYNCH_MUTEX_T* lock_in,
                                                                       StateType state_in)
 : condition_ (NULL)
 , stateLock_ (lock_in)
 , state_ (state_in)
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::Common_StateMachine_Base_T"));

  if (likely (stateLock_))
    if (unlikely (!initialize (*stateLock_)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Common_StateMachine_Base_T::initialize, continuing\n"),
                  ACE_TEXT (StateMachineName)));
    } // end IF
}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::~Common_StateMachine_Base_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::~Common_StateMachine_Base_T"));

  // clean up
  if (likely (condition_))
    delete condition_;
}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
StateType
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::current () const
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::current"));

  StateType result = static_cast<StateType> (-1);

  if (unlikely (!stateLock_))
    return state_;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, result);
    result = state_;
  } // end lock scope

  return result;
}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
bool
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::wait (StateType state_in,
                                                 const ACE_Time_Value* timeout_in) const
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::wait"));

  bool result = false;

  // sanity check(s)
  ACE_ASSERT (isInitialized_);
  ACE_ASSERT (stateLock_);

  int result_2 = -1;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, false);
    while (state_ < state_in)
    { ACE_ASSERT (condition_);
      result_2 = condition_->wait (timeout_in);
      if (unlikely (result_2 == -1))
      {
        int error = ACE_OS::last_error ();
        if (error != ETIME) // 137: timed out
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to ACE_Condition::wait(%#T): \"%m\", aborting\n"),
                      ACE_TEXT (StateMachineName),
                      timeout_in));
        goto continue_; // timed out ?
      } // end IF
    } // end WHILE
    if (unlikely (state_ != state_in))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: reached state %s (requested: %s), continuing\n"),
                  ACE_TEXT (StateMachineName),
                  ACE_TEXT (this->stateToString (state_).c_str ()),
                  ACE_TEXT (this->stateToString (state_in).c_str ())));
    } // end IF
    //else
    //{
    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("reached state \"%s\"\n"),
    //              ACE_TEXT (this->stateToString (state_in).c_str ())));
    //} // end ELSE
  } // end lock scope
  result = true;

continue_:
  return result;
}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
bool
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::initialize (const ACE_SYNCH_MUTEX_T& lock_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::initialize"));

  // *NOTE*: (unfortunately,) ACE conditions cannot be reinitialized yet
  if (unlikely (condition_))
  {
    delete condition_;
    condition_ = NULL;
  } // end IF

  ACE_NEW_NORETURN (condition_,
                    ACE_SYNCH_CONDITION_T (const_cast<ACE_SYNCH_MUTEX_T&> (lock_in), // lock
                                           USYNC_THREAD,                             // mode
                                           NULL,                                     // name
                                           NULL));                                   // arg
  if (unlikely (!condition_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  stateLock_ = &const_cast<ACE_SYNCH_MUTEX_T&> (lock_in);

  isInitialized_ = true;

  return true;
}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
bool
Common_StateMachine_Base_T<StateMachineName,
                           ACE_SYNCH_USE,
                           StateType,
                           InterfaceType>::change (StateType newState_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::change"));

  // sanity check(s)
  if (unlikely (!isInitialized_))
    return false;
  ACE_ASSERT (condition_);
  ACE_ASSERT (stateLock_);

  StateType previous_state;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, false);
    previous_state = state_;
  } // end lock scope

  // invoke callback
  // *IMPORTANT NOTE*: note that the stateLock_ is NOT held during the callback
  // *TODO*: implement a consistent (thread-safe/reentrant) policy
  bool result = true;
  try {
    this->onChange (newState_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStateMachine_T::onChange(%s), aborting\n"),
                ACE_TEXT (StateMachineName),
                ACE_TEXT (this->stateToString (newState_in).c_str ())));
    result = false;
  }

  bool signal = true;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, false);
    // *NOTE*: if the implementation is 'passive', the whole operation
    //         pertaining to newState_in 'may' have been processed 'inline' by
    //         the current thread and 'may' have completed by 'now'. In this
    //         case the callback 'may' have updated the state already
    //         --> leave the state alone if it has changed
    // *TODO*: still signal waiters ?
    // *TODO*: this may not be the best way to implement 'inline' processing
    if (likely (previous_state == state_))
      state_ = newState_in;
  } // end lock scope
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%s: \"%s\" --> \"%s\"\n"),
//              ACE_TEXT (StateMachineName),
//              ACE_TEXT (this->stateToString (previous_state).c_str ()),
//              ACE_TEXT (this->stateToString (newState_in).c_str ())));
//#endif // _DEBUG

  // signal any waiting threads
  if (likely (signal))
  {
    int result_2 = condition_->broadcast ();
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Condition::broadcast(): \"%m\", continuing\n"),
                  ACE_TEXT (StateMachineName)));
  } // end IF

  return result;
}
