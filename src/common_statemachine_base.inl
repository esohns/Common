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

#include <ace/Guard_T.h>
#include <ace/Log_Msg.h>

#include "common_macros.h"

template <ACE_SYNCH_DECL,
          typename StateType>
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::Common_StateMachine_Base_T (ACE_SYNCH_MUTEX_T* lock_in,
                                                                   StateType state_in)
 : condition_ (NULL)
 , stateLock_ (NULL)
 , state_ (state_in)
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::Common_StateMachine_Base_T"));

  if (lock_in)
    if (!initialize (*lock_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_StateMachine_Base_T::initialize, continuing\n")));
    } // end IF
}

template <ACE_SYNCH_DECL,
          typename StateType>
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::~Common_StateMachine_Base_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::~Common_StateMachine_Base_T"));

  // clean up
  if (condition_)
    delete condition_;
}

template <ACE_SYNCH_DECL,
          typename StateType>
StateType
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::current () const
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::current"));

  if (!stateLock_)
    return state_;

  StateType result = static_cast<StateType> (-1);

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, result);

    result = state_;
  } // end lock scope

  return result;
}

template <ACE_SYNCH_DECL,
          typename StateType>
bool
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::initialize (const ACE_SYNCH_MUTEX_T& lock_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::initialize"));

  //*NOTE*: unfortunately, ACE conditions cannot be reinitialized yet
  if (condition_)
  {
    delete condition_;
    condition_ = NULL;
  } // end IF

  ACE_NEW_NORETURN (condition_,
                    ACE_SYNCH_CONDITION_T (const_cast<ACE_SYNCH_MUTEX_T&> (lock_in), // lock
                                           USYNC_THREAD,                             // mode
                                           NULL,                                     // name
                                           NULL));                                   // arg
  if (!condition_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  stateLock_ = &const_cast<ACE_SYNCH_MUTEX_T&> (lock_in);

  isInitialized_ = true;

  return true;
}

template <ACE_SYNCH_DECL,
          typename StateType>
bool
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::change (StateType newState_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::change"));

  // sanity check(s)
  if (!isInitialized_)
    return false;
  ACE_ASSERT (condition_);
  ACE_ASSERT (stateLock_);

  StateType previous_state;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, false);

    previous_state = state_;
  } // end lock scope

  // invoke callback...
  // *IMPORTANT NOTE*: note that the stateLock_ is NOT held during the callback
  // *TODO*: implement a consistent (thread-safe/reentrant) policy
  bool result = true;
  try {
    this->onChange (newState_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStateMachine_T::onChange(\"%s\"), aborting\n"),
                ACE_TEXT (this->state2String (newState_in).c_str ())));
    result = false;
  }

  bool signal = true;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *stateLock_, false);

    // *NOTE*: if the implementation is 'passive', the whole operation
    //         pertaining to newState_in may have been processed 'inline' by the
    //         current thread and may have completed by 'now'. In this case the
    //         callback has updated the state already
    //         --> leave the state alone (*TODO*: signal waiters in this case ?)
    // *TODO*: this may not be the best way to implement that case (see above)
    if (previous_state == state_)
      state_ = newState_in;
  } // end lock scope
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("\"%s\" --> \"%s\"\n"),
  //            ACE_TEXT (this->state2String (previous_state).c_str ()),
  //            ACE_TEXT (this->state2String (newState_in).c_str ())));

  // signal any waiting threads
  if (signal)
  {
    int result_2 = condition_->broadcast ();
    if (result_2 == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\", continuing\n")));
  } // end IF

  return result;
}

template <ACE_SYNCH_DECL,
          typename StateType>
bool
Common_StateMachine_Base_T<ACE_SYNCH_USE,
                           StateType>::wait (StateType state_in,
                                             const ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::wait"));

  ACE_UNUSED_ARG (state_in);
  ACE_UNUSED_ARG (timeout_in);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;)
}
