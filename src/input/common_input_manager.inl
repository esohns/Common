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
#include "ace/Reactor.h"
#include "ace/Reactor_Impl.h"
#include "ace/Thread.h"
#include "ace/Thread_Mutex.h"

#include "common_macros.h"

#include "common_event_common.h"
#include "common_event_tools.h"

#include "common_input_defines.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::Common_Input_Manager_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_INPUT_EVENT_THREAD_NAME), // thread name
              COMMON_INPUT_EVENT_THREAD_GROUP_ID,                    // group id
              1,                                                     // # threads
              false)                                                 // do NOT auto-start !
 , configuration_ (NULL)
 , handler_ (NULL)
 , lock_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::Common_Input_Manager_T"));

}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::~Common_Input_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::~Common_Input_Manager_T"));

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    if (unlikely (handler_))
      handler_->deregister ();
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
bool
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::start"));

  ACE_UNUSED_ARG (timeout_in);

  // sanity check(s)
  if (unlikely (!configuration_))
    return false;
  ACE_ASSERT (configuration_->handlerConfiguration);
  if (unlikely (handler_))
    return true;

  bool deregister_b = false;

  ACE_NEW_NORETURN (handler_,
                    HandlerType ());
  if (unlikely (!handler_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!handler_->initialize (*static_cast<typename HandlerType::CONFIGURATION_T*> (configuration_->handlerConfiguration))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to HandlerType::initialize(), aborting\n")));
    delete handler_; handler_ = NULL;
    goto error;
  } // end IF
  if (unlikely (!handler_->register_ ()))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to HandlerType::register_(), aborting\n")));
    delete handler_; handler_ = NULL;
    goto error;
  } // end IF
  deregister_b = true;

  if (unlikely (configuration_->manageEventDispatch))
  {
    int result = inherited::open (NULL);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::open(NULL): \"%m\", aborting\n")));
      goto error;
    } // end IF
  } // end IF

  return true;

error:
  if (handler_)
  {
    if (deregister_b)
      handler_->deregister ();
    else
      delete handler_;
    handler_ = NULL;
  } // end IF

  return false;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
void
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::stop (bool waitForCompletion_in,
                                           bool)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::stop"));

  int result = close (1);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Manager_T::close(1): \"%m\", continuing\n")));

  if (waitForCompletion_in)
    inherited::wait (false);
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
void
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::deregister ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::deregister"));

//  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    handler_ = NULL;
//  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
bool
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::initialize"));

  if (unlikely (configuration_in.manageEventDispatch))
  { ACE_ASSERT (configuration_in.eventDispatchConfiguration);
    ACE_ASSERT (configuration_in.eventDispatchConfiguration->numberOfProactorThreads == 0);
    ACE_ASSERT (configuration_in.eventDispatchConfiguration->numberOfReactorThreads == 0);
    if (unlikely (!Common_Event_Tools::initializeEventDispatch (*configuration_in.eventDispatchConfiguration)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Event_Tools::initializeEventDispatch(), aborting\n")));
      return false;
    } // end IF
  } // end IF

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename HandlerType>
int
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::close"));

  // *NOTE*: this method may be invoked
  //         - by an external thread closing down the active object
  //           (arg_in == 1 !)
  //         - by the worker thread which calls this after returning from svc()
  //           (arg_in == 0 !) --> in this case, this should be a NOP
  switch (arg_in)
  {
    case 0:
    { ACE_ASSERT (ACE_OS::thr_equal (ACE_Thread::self (),
                                     inherited::last_thread ()));
      break;
    }
    case 1:
    {
      // sanity check(s)
      if (unlikely (!configuration_))
        goto continue_; // nothing to do
      if (unlikely (configuration_->manageEventDispatch))
      {
        if (unlikely (inherited::thr_count_ == 0))
          goto continue_; // nothing to do
        ACE_ASSERT (configuration_->eventDispatchState);
        Common_Event_Tools::finalizeEventDispatch (*configuration_->eventDispatchState,
                                                   false,  // wait ?
                                                   false); // release singletons ?
      } // end IF

continue_:
//      { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, lock_, -1);
        if (likely (handler_))
        { // *NOTE*: handler_ cleans itself up [WIN32: eventually]
          handler_->deregister (); handler_ = NULL;
        } // end IF
//      } // end lock scope
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        // *WARNING*: as it turns out SetEvent() (and friends) do NOT work on
        //            file handles, signal the handle so the handler can go away
        // *TODO*: "...Use WaitForMultipleObject instead, and pass 2 handles.
        //          The existing one, and one for an event called something like
        //          'exit'. During app shutdown, SetEvent on the exit event, and
        //          the WaitForMultipleObject will return.... You need to switch
        //          on the return value of WaitForMultipleObject to do the
        //          appropriate behaviour depending on which one of the handles
        //          was triggered. ..."
        ACE_OS::write (ACE_STDIN,
                       static_cast<void*> (ACE_TEXT_ALWAYS_CHAR ("0")),
                       1);
#endif // ACE_WIN32 || ACE_WIN64

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
          typename HandlerType>
int
Common_Input_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       HandlerType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Manager_T::svc"));

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
  ACE_ASSERT (configuration_->eventDispatchConfiguration);
  ACE_ASSERT (configuration_->eventDispatchState);

  if (unlikely (!Common_Event_Tools::startEventDispatch (*configuration_->eventDispatchState)))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_Event_Tools::startEventDispatch(), aborting\n")));
    return -1;
  } // end IF
  switch (configuration_->eventDispatchConfiguration->dispatch)
  {
    case COMMON_EVENT_DISPATCH_PROACTOR:
    {
      configuration_->eventDispatchState->proactorGroupId = inherited::grp_id_;
      break;
    }
    case COMMON_EVENT_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      ACE_Reactor_Impl* reactor_impl_p = reactor_p->implementation ();
      ACE_ASSERT (reactor_impl_p);
      int result = reactor_impl_p->owner (ACE_OS::thr_self ());
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor_Impl::owner(): \"%s\", aborting\n")));
        return -1;
      } // end IF

      configuration_->eventDispatchState->reactorGroupId = inherited::grp_id_;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid/unknown event dispatch type (was: %d), aborting\n"),
                 configuration_->eventDispatchConfiguration->dispatch));
      return -1;
    }
  } // end SWITCH

  Common_Event_Tools::dispatchEvents (*configuration_->eventDispatchState);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (inherited::threadName_.c_str ())));

  return 0;
}
