/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include "ace/OS.h"
#include "ace/Thread.h"

#include "common_macros.h"

#include "common_ui_defines.h"

#include "common_ui_curses_defines.h"
#include "common_ui_curses_tools.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::Common_UI_Curses_Manager_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_EVENT_THREAD_NAME), // thread name
              COMMON_UI_EVENT_THREAD_GROUP_ID,                    // group id
              1,                                                  // # threads
              false)                                              // do NOT auto-start !
 , configuration_ (NULL)
 , state_ ()
 , UIIsInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::Common_UI_Curses_Manager_T"));

}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
bool
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
bool
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::start"));

  ACE_UNUSED_ARG (timeout_in);

  int result = inherited::open (NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_TaskBase_T::open(NULL): \"%m\", aborting\n")));
    return false;
  } // end IF
  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
void
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::stop (bool waitForCompletion_in,
                                             bool)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::stop"));

  int result = close (1);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_Curses_Manager_T::close(1): \"%m\", continuing\n")));

  if (waitForCompletion_in)
    inherited::wait (false);
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
int
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::close"));

  // *NOTE*: this method may be invoked
  //         - by an external thread closing down the active object
  //           (arg_in == 1 !)
  //         - by the worker thread which calls this after returning from svc()
  //           (arg_in == 0 !) --> in this case, this should be a NOP
  switch (arg_in)
  {
    case 0:
    { ACE_ASSERT (ACE_OS::thr_equal (ACE_Thread::self (), inherited::last_thread ()));
      break;
    }
    case 1:
    {
      if (unlikely (inherited::thr_count_ == 0))
        return 0; // nothing to do

      // schedule UI finalization
      { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, -1);
        state_.finished = true;
      } // end lock scope

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument: %u, aborting\n"),
                  arg_in));
      return -1;
    }
  } // end SWITCH

  return 0;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType>
int
Common_UI_Curses_Manager_T<ACE_SYNCH_USE,
                           ConfigurationType,
                           StateType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Manager_T::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (inherited::threadName_,
                                     NULL);
#else
  Common_Error_Tools::setThreadName (inherited::threadName_,
                                     0);
#endif // _WIN32_WINNT_WIN10
#endif // ACE_WIN32 || ACE_WIN64
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
              ACE_TEXT (inherited::threadName_.c_str ()),
              inherited::grp_id_));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  int result = -1;

  if (configuration_->hooks.initHook &&
      !configuration_->hooks.initHook (&state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%s): failed to Common_UI_Curses_EventHookConfiguration::initHook(), aborting\n"),
                ACE_TEXT (inherited::threadName_.c_str ())));
    return -1;
  } // end IF

  ACE_ASSERT (configuration_->hooks.mainHook);
  if (!configuration_->hooks.mainHook (&state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%s): failed to Common_UI_Curses_EventHookConfiguration::mainHook(), aborting\n"),
                ACE_TEXT (inherited::threadName_.c_str ())));
    goto continue_;
  } // end IF

  result = 0;

continue_:
  if (configuration_->hooks.finiHook &&
      !configuration_->hooks.finiHook (&state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%s): failed to Common_UI_Curses_EventHookConfiguration::finiHook(), aborting\n"),
                ACE_TEXT (inherited::threadName_.c_str ())));
    result = -1;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (inherited::threadName_.c_str ())));

  return result;
}
