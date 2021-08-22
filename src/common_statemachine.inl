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

#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_macros.h"

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename StateType,
          typename InterfaceType>
Common_StateMachine_T<StateMachineName,
                      ACE_SYNCH_USE,
                      StateType,
                      InterfaceType>::Common_StateMachine_T (ACE_SYNCH_MUTEX_T* lock_in,
                                                             StateType state_in)
 : inherited (lock_in,
              state_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachine_T::Common_StateMachine_T"));

}

//////////////////////////////////////////

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StateType>
Common_StateMachineAsynch_T<StateMachineName,
                            ACE_SYNCH_USE,
                            TimePolicyType,
                            StateType>::Common_StateMachineAsynch_T (const std::string& threadName_in,
                                                                     int threadGroupId_in,
                                                                     ACE_SYNCH_MUTEX_T* lock_in,
                                                                     StateType state_in)
 : inherited (threadName_in,    // thread name
              threadGroupId_in, // group id
              1,                // # thread(s)
              false,            // auto-start ?
              NULL)             // queue handle
 , inherited2 (lock_in,
               state_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachineAsynch_T::Common_StateMachineAsynch_T"));

}

template <const char* StateMachineName,
          ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StateType>
void
Common_StateMachineAsynch_T<StateMachineName,
                            ACE_SYNCH_USE,
                            TimePolicyType,
                            StateType>::handle (ACE_Message_Block*& message_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_StateMachineAsynch_T::handle"));

  // sanity check(s)
  ACE_ASSERT (message_inout);

  StateType next_state_e = static_cast<StateType> (message_inout->msg_type ());
  message_inout->release ();
  message_inout = NULL;

  bool result = false;
  try {
    result = inherited2::change (next_state_e);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: (%s/%t): caught exception in Common_IStateMachine_T::change(%s), continuing\n"),
                ACE_TEXT (StateMachineName),
                ACE_TEXT (inherited::threadName_.c_str ()),
                ACE_TEXT (stateToString (next_state_e).c_str ())));
//    result = false;
  }
  if (unlikely (!result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: (%s/%t): failed to Common_IStateMachine_T::change(%s), continuing\n"),
                ACE_TEXT (StateMachineName),
                ACE_TEXT (inherited::threadName_.c_str ()),
                ACE_TEXT (stateToString (next_state_e).c_str ())));
}
