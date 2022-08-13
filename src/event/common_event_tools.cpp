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
#include "stdafx.h"

#include "common_event_tools.h"

#include <sstream>
#include <string>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
#include "ace/OS_Memory.h"
#endif // __sun && __SVR4
#include "ace/POSIX_CB_Proactor.h"
#include "ace/POSIX_Proactor.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
#include "ace/SUN_Proactor.h"
#endif // ACE_HAS_AIO_CALLS && sun
#include "ace/Time_Value.h"
#include "ace/TP_Reactor.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Configuration.h"
#include "ace/WIN32_Proactor.h"
#include "ace/WFMO_Reactor.h"
#else
#include "ace/Dev_Poll_Reactor.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_defines.h"
#include "common_macros.h"

#include "common_signal_tools.h"

#include "common_event_defines.h"

bool
Common_Event_Tools::defaultPlatformReactorIsSelectBased ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Event_Tools::defaultPlatformReactorIsSelectBased"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return true;
#elif defined (ACE_LINUX)
  return true;
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif
}

bool
Common_Event_Tools::initializeEventDispatch (struct Common_EventDispatchConfiguration& configuration_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Event_Tools::initializeEventDispatch"));

  // step0: initialize configuration
  // *NOTE*: the appropriate type of event dispatch mechanism may depend on
  //         several factors:
  //         - targeted platform(s) (i.e. supported mechanisms, (software) environment)
  //         - envisioned application type (i.e. library, plugin)
  // *TODO*: the chosen event dispatch implementation should be based on
  //         #defines only (!, see above, common.h)
  // sanity check(s)
  if (unlikely (configuration_inout.numberOfReactorThreads))
  {
    if (configuration_inout.numberOfReactorThreads > 1)
    {
      if ((configuration_inout.reactorType == COMMON_REACTOR_SELECT) ||
          ((configuration_inout.reactorType == COMMON_REACTOR_ACE_DEFAULT) &&
           Common_Event_Tools::defaultPlatformReactorIsSelectBased ()))
      {
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("cannot use the 'select' reactor from multiple threads, adjusting configuration\n")));
        configuration_inout.reactorType = COMMON_REACTOR_THREAD_POOL;
      } // end IF

      if (!configuration_inout.callbacksRequireSynchronization)
      {
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("multi-threaded reactors require handler synchronization, adjusting configuration\n")));
        configuration_inout.callbacksRequireSynchronization = true;
      } // end IF
    } // end IF
  } // end IF

  // step1: initialize reactor
  if (configuration_inout.numberOfReactorThreads
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      || true // (currently) need this for proactor connector cancellations
#else
#endif // ACE_WIN32 || ACE_WIN64
      )
  {
    ACE_Reactor_Impl* reactor_impl_p = NULL;
    switch (configuration_inout.reactorType)
    {
      case COMMON_REACTOR_ACE_DEFAULT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using ACE default (platform-specific) reactor\n")));
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      case COMMON_REACTOR_DEV_POLL:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using /dev/poll reactor\n")));
        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_Dev_Poll_Reactor (COMMON_EVENT_MAXIMUM_HANDLES,    // max num handles
                                                true,                            // restart after EINTR ?
                                                NULL,                            // signal handler handle
                                                NULL,                            // timer queue handle
                                                ACE_DISABLE_NOTIFY_PIPE_DEFAULT, // disable notify pipe ?
                                                NULL,                            // notification handler handle
                                                1,                               // mask signals ?
                                                ACE_DEV_POLL_TOKEN::FIFO));      // signal queue
        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case COMMON_REACTOR_SELECT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using select reactor\n")));
        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_Select_Reactor (static_cast<size_t> (COMMON_EVENT_MAXIMUM_HANDLES), // max num handles
                                              true,                                               // restart after EINTR ?
                                              NULL,                                               // signal handler handle
                                              NULL,                                               // timer queue handle
                                              ACE_DISABLE_NOTIFY_PIPE_DEFAULT,                    // disable notification pipe ?
                                              NULL,                                               // notification handler handle
                                              true,                                               // mask signals ?
                                              ACE_SELECT_TOKEN::FIFO));                           // signal queue
        break;
      }
      case COMMON_REACTOR_THREAD_POOL:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using thread-pool reactor\n")));
        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_TP_Reactor (static_cast<size_t> (COMMON_EVENT_MAXIMUM_HANDLES), // max num handles
                                          true,                                               // restart after EINTR ?
                                          NULL,                                               // signal handler handle
                                          NULL,                                               // timer queue handle
                                          true,                                               // mask signals ?
                                          ACE_Select_Reactor_Token::FIFO));                   // signal queue
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case COMMON_REACTOR_WFMO:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using WFMO reactor\n")));
        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_WFMO_Reactor (ACE_WFMO_Reactor::DEFAULT_SIZE, // max num handles (62 [+ 2])
                                            0,                              // unused
                                            NULL,                           // signal handler handle
                                            NULL,                           // timer queue handle
                                            NULL));                         // notification handler handle

        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      ///////////////////////////////////
      case COMMON_REACTOR_INVALID:
      case COMMON_REACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown reactor type (was: %d), aborting\n"),
                    configuration_inout.reactorType));
        return false;
      }
    } // end SWITCH
    ACE_Reactor* reactor_p = NULL;
    if (likely (reactor_impl_p))
    {
      ACE_NEW_NORETURN (reactor_p,
                        ACE_Reactor (reactor_impl_p, // implementation handle
                                     1));            // delete in dtor ?
      if (unlikely (!reactor_p))
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        delete reactor_impl_p; reactor_impl_p = NULL;
        return false;
      } // end IF
    } // end IF

    // make this the "default" reactor
    ACE_Reactor* previous_reactor_p =
      ACE_Reactor::instance (reactor_p, // reactor handle
                             1);        // delete in dtor ?
    if (previous_reactor_p)
    {
      delete previous_reactor_p; previous_reactor_p = NULL;
    } // end IF
  } // end IF

  // step2: initialize proactor
  if (configuration_inout.numberOfProactorThreads)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    struct aioinit aioinit_s;
    ACE_OS::memset (&aioinit_s, 0, sizeof (struct aioinit));
    aioinit_s.aio_threads = configuration_inout.numberOfProactorThreads; // default: 20
    aioinit_s.aio_num =
      COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS;                        // default: 64
//    aioinit_s.aio_locks = 0;
//    aioinit_s.aio_usedba = 0;
//    aioinit_s.aio_debug = 0;
//    aioinit_s.aio_numusers = 0;
    aioinit_s.aio_idle_time = 1;                                         // default: 1
//    aioinit_s.aio_reserved = 0;
    aio_init (&aioinit_s);
#endif // ACE_WIN32 || ACE_WIN64

    ACE_Proactor_Impl* proactor_impl_p = NULL;
    switch (configuration_inout.proactorType)
    {
      case COMMON_PROACTOR_ACE_DEFAULT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using ACE default (platform-specific) proactor\n")));
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      case COMMON_PROACTOR_POSIX_AIOCB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX AIOCB proactor\n")));
        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_AIOCB_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
      case COMMON_PROACTOR_POSIX_CB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX CB proactor\n")));
        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_CB_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
      case COMMON_PROACTOR_POSIX_SIG:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX RT-signal proactor\n")));
        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_SIG_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
      case COMMON_PROACTOR_POSIX_SUN:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using SunOS proactor\n")));
        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_SUN_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
#endif // ACE_HAS_AIO_CALLS && sun
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case COMMON_PROACTOR_WIN32:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using Win32 proactor\n")));
        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_WIN32_Proactor (1,       // #concurrent thread(s)/I/O completion port [0: #processors]
                                              false)); // N/A

        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      ///////////////////////////////////
      case COMMON_PROACTOR_INVALID:
      case COMMON_PROACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown proactor type (was: %d), aborting\n"),
                    configuration_inout.proactorType));
        return false;
      }
    } // end SWITCH
    ACE_Proactor* proactor_p = NULL;
    if (likely (proactor_impl_p))
    {
      ACE_NEW_NORETURN (proactor_p,
                        ACE_Proactor (proactor_impl_p, // implementation handle --> create new ?
//                                      false,  // *NOTE*: --> call close() manually
//                                              // (see finalizeEventDispatch() below)
                                      true,            // delete in dtor ?
                                      NULL));          // timer queue handle --> create new
      if (unlikely (!proactor_p))
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        return false;
      } // end IF

      // make this the "default" proactor
      ACE_Proactor* previous_proactor_p =
          ACE_Proactor::instance (proactor_p, // proactor handle
                                  1);         // delete in dtor ?
      if (previous_proactor_p)
        delete previous_proactor_p;
    } // end IF
    else
      proactor_p = ACE_Proactor::instance ();
  } // end IF

  return true;
}

ACE_THR_FUNC_RETURN
common_event_dispatch_function (void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_event_dispatch_function"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME),
                                     NULL);
#else
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME),
                                     0);
#endif // _WIN32_WINNT_WIN10
#else
// *TODO*
//  pid_t result_2 = syscall (SYS_gettid);
//  if (result_2 == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to syscall(SYS_gettid): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): thread (id: %t) starting\n"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME)));

  ACE_THR_FUNC_RETURN result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = std::numeric_limits<unsigned long>::max (); // DWORD
#else
  result = arg_in;
#endif // ACE_WIN32 || ACE_WIN64

  struct Common_EventDispatchState* state_p =
    reinterpret_cast<struct Common_EventDispatchState*> (arg_in);
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();

  // sanity check(s)
  ACE_ASSERT (state_p);
  ACE_ASSERT (state_p->configuration);
  ACE_ASSERT (thread_manager_p);

  int result_2 = -1;
  int group_id_i = -1;
  result_2 = thread_manager_p->get_grp (ACE_OS::thr_self (),
                                        group_id_i);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::get_grp(%t): \"%m\", returning\n")));
    return result;
  } // end IF

  // dispatch events
  if (group_id_i == state_p->reactorGroupId)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);

    // *NOTE*: transfer 'ownership' of the (select) reactor ?
    // *TODO*: determining the default ACE reactor type is not specified
    //         (platform-specific)
    if ((state_p->configuration->reactorType == COMMON_REACTOR_ACE_DEFAULT) ||
        (state_p->configuration->reactorType == COMMON_REACTOR_SELECT))
    { ACE_ASSERT (state_p->configuration->numberOfReactorThreads == 1);
      ACE_thread_t thread_2;
      result_2 = reactor_p->owner (ACE_OS::thr_self (),
                                   &thread_2);
      if (unlikely (result_2 == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::owner(%t): \"%m\", continuing\n")));
    } // end IF

    result_2 = reactor_p->run_reactor_event_loop (NULL);
  } // end IF
  else if (group_id_i == state_p->proactorGroupId)
  {
    // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
    //                   multithreaded application, the disposition of a
    //                   particular signal is the same for all threads."
    //                   (see man 7 signal)

//    // unblock [SIGRTMIN,SIGRTMAX] IFF on POSIX AND using the
//    // ACE_POSIX_SIG_Proactor (the default)
//    // *IMPORTANT NOTE*: the proactor implementation dispatches the signals in
//    //                   worker thread(s)
//    //                   (see also: Asynch_Pseudo_Task.cpp:56)
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    ACE_POSIX_Proactor* proactor_impl_p =
        dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    ACE_ASSERT (proactor_impl_p);
    ACE_POSIX_Proactor::Proactor_Type proactor_type =
        proactor_impl_p->get_impl_type ();
    if (proactor_type == ACE_POSIX_Proactor::PROACTOR_SIG)
    {
      sigset_t original_mask;
      Common_Signal_Tools::unblockRealtimeSignals (original_mask);
    } // end IF
#endif // ACE_WIN32 || ACE_WIN64
    result_2 = proactor_p->proactor_run_event_loop (NULL);
  } // end ELSE IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown event thread group id (was: %d), aborting\n"),
                group_id_i));
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (result);
    ACE_NOTREACHED (return result;)
  } // end ELSE
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%t) failed to handle events: \"%m\", aborting\n")));
  else
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = static_cast<ACE_THR_FUNC_RETURN> (result_2);
#else
    result = ((result_2 == 0) ? NULL : arg_in);
#endif // ACE_WIN32 || ACE_WIN64
  } // end ELSE
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): thread (id: %t) leaving\n"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME)));

  return result;
}

bool
Common_Event_Tools::startEventDispatch (struct Common_EventDispatchState& dispatchState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Event_Tools::startEventDispatch"));

  // sanity check(s)
  ACE_ASSERT (dispatchState_inout.configuration);
  ACE_ASSERT (dispatchState_inout.proactorGroupId == -1);
  ACE_ASSERT (dispatchState_inout.reactorGroupId == -1);

  // reset event dispatch
  ACE_Reactor* reactor_p = ACE_Reactor::instance ();
  ACE_ASSERT (reactor_p);
  reactor_p->reset_reactor_event_loop ();

  int result = -1;
  ACE_Proactor* proactor_p = ACE_Proactor::instance ();
  ACE_ASSERT (proactor_p);
  result = proactor_p->proactor_reset_event_loop ();
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Proactor::proactor_reset_event_loop: \"%m\", aborting\n")));
    return false;
  } // end IF

  // spawn worker thread(s) ?
  int loop_i = 0, group_id_i = -1;
  unsigned int number_of_threads_i = 0;
  ACE_hthread_t* thread_handles_p = NULL;
  const char** thread_names_p = NULL;
  char* thread_name_p;
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  ACE_THR_FUNC function_p =
    static_cast<ACE_THR_FUNC> (::common_event_dispatch_function);
  void* arg_p = &dispatchState_inout;
  std::string buffer;
  std::ostringstream converter;

spawn:
  number_of_threads_i =
      (!!loop_i ? dispatchState_inout.configuration->numberOfProactorThreads
                : dispatchState_inout.configuration->numberOfReactorThreads);
  // *NOTE*: if #dispatch threads == 0, event dispatch takes place in the main
  //         thread --> do not spawn any dedicated thread(s)
  if (!number_of_threads_i)
    goto continue_;

  // spawn (a group of-) thread(s)
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[number_of_threads_i]);
  if (unlikely (!thread_handles_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (ACE_hthread_t) * number_of_threads_i));
    return false;
  } // end IF
//  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[number_of_threads_i]);
  if (unlikely (!thread_names_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (const char*) * number_of_threads_i));
    delete [] thread_handles_p;
    return false;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (const char*) * number_of_threads_i);
  for (unsigned int i = 0;
       i < number_of_threads_i;
       ++i)
  {
    thread_name_p = NULL;
    // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
    //         available)
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (unlikely (!thread_name_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                  sizeof (char[BUFSIZ])));
      // clean up
      delete [] thread_handles_p;
      for (unsigned int j = 0; j < i; ++j)
        delete [] thread_names_p[j];
      delete [] thread_names_p;
      return false;
    } // end IF
    ACE_OS::memset (thread_name_p, 0, sizeof (char[BUFSIZ]));
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (i + 1);
//    buffer = (!!loop_i ? ACE_TEXT_ALWAYS_CHAR ("proactor ")
//                       : ACE_TEXT_ALWAYS_CHAR ("reactor "));
    buffer += ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME);
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_OS::strcpy (thread_name_p, buffer.c_str ());
#else
    ACE_OS::strncpy (thread_name_p, buffer.c_str (), COMMON_THREAD_PTHREAD_NAME_MAX_LENGTH - 1);
#endif // ACE_WIN32 || ACE_WIN64
    thread_names_p[i] = thread_name_p;
  } // end FOR
  group_id_i =
    thread_manager_p->spawn_n (number_of_threads_i,          // #threads
                               function_p,                   // function
                               arg_p,                        // argument
                               (THR_NEW_LWP      |
                                THR_JOINABLE     |
                                THR_INHERIT_SCHED),          // flags
                               ACE_DEFAULT_THREAD_PRIORITY,  // priority
                               (!!loop_i ? COMMON_EVENT_PROACTOR_THREAD_GROUP_ID
                                         : COMMON_EVENT_REACTOR_THREAD_GROUP_ID), // group id
                               NULL,                         // task
                               thread_handles_p,             // handle(s)
                               NULL,                         // stack(s)
                               NULL,                         // stack size(s)
                               thread_names_p);              // name(s)
  if (unlikely (group_id_i == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                number_of_threads_i));
    delete [] thread_handles_p;
    for (unsigned int i = 0; i < number_of_threads_i; ++i)
      delete [] thread_names_p[i];
    delete [] thread_names_p;
    return false;
  } // end IF
  if (!!loop_i)
    dispatchState_inout.proactorGroupId = group_id_i;
  else
    dispatchState_inout.reactorGroupId = group_id_i;

  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
//    __uint64_t thread_id = 0;
#endif
  for (unsigned int i = 0;
       i < number_of_threads_i;
       ++i)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
//    ::pthread_getthreadid_np (&thread_handles_p[i], &thread_id);
#endif // ACE_WIN32 || ACE_WIN64
    converter << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
              << ACE_TEXT_ALWAYS_CHAR (" ")
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0502) // _WIN32_WINNT_WS03
              << ::GetThreadId (thread_handles_p[i])
#else
              << thread_handles_p[i]
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0502)
#else
              << thread_handles_p[i]
//              << thread_id
#endif // ACE_WIN32 || ACE_WIN64
              << ACE_TEXT_ALWAYS_CHAR ("\n");
    delete [] thread_names_p[i]; thread_names_p[i] = NULL;
  } // end FOR

  buffer = converter.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): spawned %u dispatch thread(s) (group id: %d):\n%s"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME),
              number_of_threads_i,
              group_id_i,
              ACE_TEXT (buffer.c_str ())));

  delete [] thread_handles_p; thread_handles_p = NULL;
  delete [] thread_names_p; thread_names_p = NULL;

continue_:
  if (!loop_i)
  {
    ++loop_i;
    group_id_i = -1;
    goto spawn;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch; spawned %u thread(s)\n"),
              dispatchState_inout.configuration->numberOfProactorThreads + dispatchState_inout.configuration->numberOfReactorThreads));

  return true;
}

void
Common_Event_Tools::finalizeEventDispatch (struct Common_EventDispatchState& dispatchState_inout,
                                           bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Event_Tools::finalizeEventDispatch"));

  int result = -1;

  // step1: stop default reactor/proactor
  ACE_Proactor* proactor_p = ACE_Proactor::instance ();
  ACE_ASSERT (proactor_p != NULL);
  result = proactor_p->end_event_loop ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Proactor::end_event_loop: \"%m\", continuing\n")));

  ACE_Reactor* reactor_p = ACE_Reactor::instance ();
  ACE_ASSERT (reactor_p != NULL);
  result = reactor_p->end_event_loop ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Reactor::end_event_loop: \"%m\", continuing\n")));

  // step2: wait for any worker thread(s) ?
  ACE_Thread_Manager* thread_manager_p = NULL;
  if (likely (!waitForCompletion_in))
    goto continue_;

  thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);

  if (dispatchState_inout.proactorGroupId != -1)
  {
    result = thread_manager_p->wait_grp (dispatchState_inout.proactorGroupId);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  dispatchState_inout.proactorGroupId));
    dispatchState_inout.proactorGroupId = -1;
  } // end IF

  if (dispatchState_inout.reactorGroupId != -1)
  {
    result =
      thread_manager_p->wait_grp (dispatchState_inout.reactorGroupId);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  dispatchState_inout.reactorGroupId));
    dispatchState_inout.reactorGroupId = -1;
  } // end IF

continue_:
  // *WARNING*: on UNIX; this could prevent proper signal reactivation (see
  //            finalizeSignals())
  ACE_Proactor::close_singleton ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopped event dispatch\n")));
}

void
Common_Event_Tools::dispatchEvents (struct Common_EventDispatchState& dispatchState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Event_Tools::dispatchEvents"));

  // sanity check(s)
  ACE_ASSERT (dispatchState_inout.configuration);

  int result = -1;
  int group_id_i = -1;
  ACE_Thread_Manager *thread_manager_p = NULL;
  enum Common_EventDispatchType dispatch_e = COMMON_EVENT_DISPATCH_PROACTOR;
  //std::vector<unsigned int> number_of_threads;
  //number_of_threads.push_back (dispatchState_inout.configuration->numberOfProactorThreads);
  //number_of_threads.push_back (dispatchState_inout.configuration->numberOfReactorThreads);
  //bool do_not_iterate_b = false;

  if (!dispatchState_inout.configuration->numberOfProactorThreads &&
      !dispatchState_inout.configuration->numberOfReactorThreads)
  {
    //do_not_iterate_b = true;
    dispatch_e = dispatchState_inout.configuration->dispatch;
  } // end IF

next:
  switch (dispatch_e)
  {
    case COMMON_EVENT_DISPATCH_PROACTOR:
    {
      if (likely (dispatchState_inout.configuration->numberOfProactorThreads > 0))
      {
        group_id_i = dispatchState_inout.proactorGroupId;
        goto wait_group;
      } // end IF

      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->proactor_run_event_loop (NULL);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to ACE_Proactor::proactor_run_event_loop(): \"%m\", returning\n")));
        return;
      } // end IF

      goto continue_;
    }
    case COMMON_EVENT_DISPATCH_REACTOR:
    {
      if (likely (dispatchState_inout.configuration->numberOfReactorThreads > 0))
      {
        group_id_i = dispatchState_inout.reactorGroupId;
        goto wait_group;
      } // end IF

      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->run_reactor_event_loop (NULL);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", returning\n")));
        return;
      } // end IF

      goto continue_;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown dispatch type (was: %d), returning\n"),
                  dispatchState_inout.configuration->dispatch));
      return;
    }
  } // end SWITCH
  ACE_NOTREACHED (goto continue_;)

wait_group:
  // sanity check(s)
  if (unlikely (group_id_i == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid group id (was: %d), returning\n"),
                group_id_i));
    return;
  } // end IF

  thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  result = thread_manager_p->wait_grp (group_id_i);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", returning\n"),
                group_id_i));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("joined dispatch thread group (id was: %d)\n"),
              group_id_i));

  // wait for the next mechanism ?
  dispatch_e = static_cast<enum Common_EventDispatchType> (dispatch_e + 1);
  if (dispatch_e < COMMON_EVENT_DISPATCH_MAX)
    goto next;

continue_:
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("event dispatch complete\n")));
}
