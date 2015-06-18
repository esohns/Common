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

template <typename StateType>
Common_StateMachine_Base_T<StateType>::Common_StateMachine_Base_T ()
 : state_ (static_cast<StateType> (-1))
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::Common_StateMachine_Base_T"));

}

template <typename StateType>
Common_StateMachine_Base_T<StateType>::~Common_StateMachine_Base_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::~Common_StateMachine_Base_T"));

}

template <typename StateType>
StateType
Common_StateMachine_Base_T<StateType>::current () const
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::current"));

  StateType result = static_cast<StateType> (-1);

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);

    result = state_;
  } // end lock scope

  return result;
}

template <typename StateType>
bool
Common_StateMachine_Base_T<StateType>::change (StateType newState_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::change"));

  //std::string current_state_string, new_state_string;
  //new_state_string = state2String (newState_in);

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);

    //current_state_string = state2String (state_);

    state_ = newState_in;
  } // end lock scope

  // invoke callback...
  try
  {
    onChange (newState_in);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStateMachine_T::onChange(\"%s\"), aborting\n"),
                ACE_TEXT (state2String (newState_in).c_str ())));

    return false;
  }
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("\"%s\" --> \"%s\"\n"),
  //            ACE_TEXT (current_state_string.c_str ()),
  //            ACE_TEXT (new_state_string.c_str ())));

  return true;
}

template <typename StateType>
bool
Common_StateMachine_Base_T<StateType>::wait (const ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_Base_T::wait"));

  ACE_UNUSED_ARG (timeout_in);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

#if defined (_MSC_VER)
  ACE_NOTREACHED (return false);
#endif
}
