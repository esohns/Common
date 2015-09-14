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

#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_defines.h"
#include "common_itimer.h"
#include "common_macros.h"

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::Common_Timer_Manager_T ()
 : inherited (ACE_Thread_Manager::instance (), // thread manager --> use default
              NULL)                            // timer queue --> allocate (dummy) temp first :( *TODO*
 , configuration_ ()
 , timerHandler_ ()
 , timerQueue_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::Common_Timer_Manager_T"));

  // initialize timer queue ?
  if (configuration_.mode == COMMON_TIMER_MODE_QUEUE)
    if (!initializeTimerQueue ())
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize timer queue: \"%m\", continuing\n")));

//  start ();
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::~Common_Timer_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::~Common_Timer_Manager_T"));

  int result = -1;

  if (isRunning ())
    stop (true);

  // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
  result = inherited::timer_queue (NULL);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", continuing\n")));
  if (timerQueue_)
    delete timerQueue_;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::initialize"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::start"));

  int result = -1;

  // sanity check(s)
  if (isRunning ())
    return;

  // spawn the dispatching worker thread
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;
  char thread_name[BUFSIZ];
  ACE_OS::memset (thread_name, 0, sizeof (thread_name));
  ACE_OS::strcpy (thread_name,
                  ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_THREAD_NAME));
  const char* thread_names[1];
  thread_names[0] = thread_name;
  result =
      inherited::activate ((THR_NEW_LWP      |
                            THR_JOINABLE     |
                            THR_INHERIT_SCHED),          // flags
                           1,                            // # threads --> 1
                           0,                            // force active ?
                           ACE_DEFAULT_THREAD_PRIORITY,  // priority
                           COMMON_TIMER_THREAD_GROUP_ID, // group id
                           NULL,                         // task base
                           thread_handles,               // thread handle(s)
                           NULL,                         // stack(s)
                           NULL,                         // stack size(s)
                           thread_ids,                   // thread id(s)
                           thread_names);                // thread name(s)
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::activate(): \"%m\", returning\n")));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) spawned worker thread (group: %d, id: %u)...\n"),
              ACE_TEXT (COMMON_TIMER_THREAD_NAME),
              COMMON_TIMER_THREAD_GROUP_ID,
              thread_ids[0]));
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::stop (bool waitForCompletion_in,
                                                     bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::stop"));

  int result = -1;

  // *NOTE*: deactivate the timer queue and wake up the worker thread
  inherited::deactivate ();

  // *TODO*: this doesn't look quite correct yet...
  if (waitForCompletion_in)
  {
    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s) joined worker thread...\n"),
                ACE_TEXT (COMMON_TIMER_THREAD_NAME)));
  } // end IF

  // clean up
  unsigned int flushed_timers = flushTimers (lockedAccess_in);
  if (flushed_timers)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("flushed %u timer(s)...\n"),
                flushed_timers));
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::isRunning () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::isRunning"));

  return (inherited::thr_count () > 0);
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
unsigned int
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::flushTimers (bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::flushTimers"));

  // initialize return value(s)
  unsigned int return_value = 0;

  int result = -1;
  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
    case COMMON_TIMER_MODE_REACTOR:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (std::numeric_limits<unsigned int>::max ());

      ACE_NOTREACHED (return std::numeric_limits<unsigned int>::max ());
    }
    case COMMON_TIMER_MODE_QUEUE:
    {
      if (lockedAccess_in)
      {
        ACE_SYNCH_RECURSIVE_MUTEX& mutex_r = inherited::mutex ();
        result = mutex_r.acquire ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_MT_SYNCH::RECURSIVE_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

//      const void* act_p = NULL;
//      long timer_id = 0;
//      Common_TimerQueueImplIterator_t iterator (*inherited::timer_queue ());
//      for (iterator.first ();
//           !iterator.isdone ();
//           iterator.next (), return_value++)
//      {
//        act_p = NULL;
//        timer_id = iterator.item ()->get_timer_id ();
//        result = inherited::cancel (timer_id, &act_p);
//        if (result == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
//                      timer_id));
//        else
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("cancelled timer (ID: %d)...\n"),
//                      timer_id));
//      } // end FOR
      Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
      ACE_ASSERT (timer_queue_p);
      result = timer_queue_p->close ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_ITimerQueue_t::close(): \"%m\", continuing\n")));

      if (lockedAccess_in)
      {
        ACE_SYNCH_RECURSIVE_MUTEX& mutex_r = inherited::mutex ();
        result = mutex_r.release ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_MT_SYNCH::RECURSIVE_MUTEX::release(): \"%m\", continuing\n")));
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), continuing\n"),
                  configuration_.mode));
      break;
    }
  } // end SWITCH

  return return_value;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::initializeTimerQueue ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::initializeTimerQueue"));

  int result = -1;

  // sanity check(s)
  bool was_running = isRunning ();
  if (was_running)
    stop (true);

  if (timerQueue_)
  {
    flushTimers ();
    // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
    result = inherited::timer_queue (NULL);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", continuing\n")));
    delete timerQueue_;
    timerQueue_ = NULL;
  } // end IF

  Common_ITimerQueue_t* timer_queue_p = NULL;
  switch (configuration_.queueType)
  {
    case COMMON_TIMER_QUEUE_HEAP:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using heap timer queue...\n")));

      ACE_NEW_NORETURN (timer_queue_p,
                        Common_TimerQueueHeapImpl_t ((COMMON_TIMER_PREALLOCATE_TIMER_SLOTS ? COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS
                                                                                           : 0), // preallocated slots
                                                     COMMON_TIMER_PREALLOCATE_TIMER_SLOTS,       // preallocate timer nodes ?
                                                     &timerHandler_,                             // upcall functor
                                                     NULL,                                       // freelist --> allocate
                                                     COMMON_TIME_POLICY));                       // time policy
      break;
    }
    case COMMON_TIMER_QUEUE_LIST:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using list timer queue...\n")));

      ACE_NEW_NORETURN (timer_queue_p,
                        Common_TimerQueueListImpl_t (&timerHandler_,       // upcall functor
                                                     NULL,                 // freelist --> allocate
                                                     COMMON_TIME_POLICY)); // time policy
      break;
    }
    case COMMON_TIMER_QUEUE_WHEEL:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using wheel timer queue...\n")));

      ACE_NEW_NORETURN (timer_queue_p,
                        Common_TimerQueueWheelImpl_t (ACE_DEFAULT_TIMER_WHEEL_SIZE,
                                                      ACE_DEFAULT_TIMER_WHEEL_RESOLUTION,
                                                      (COMMON_TIMER_PREALLOCATE_TIMER_SLOTS ? COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS
                                                                                            : 0), // preallocate timer nodes ?
                                                      &timerHandler_,                             // upcall functor
                                                      NULL,                                       // freelist --> allocate
                                                      COMMON_TIME_POLICY));                       // time policy
      break;
    }
    case COMMON_TIMER_QUEUE_INVALID:
    case COMMON_TIMER_QUEUE_MAX:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer queue type (was: %d), aborting\n"),
                  configuration_.queueType));
      return false;
    }
  } // end SWITCH
  if (!timer_queue_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  timerQueue_ = dynamic_cast<TimerQueueType*> (timer_queue_p);
  if (!timerQueue_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<TimerQueueType> failed, aborting\n")));

    // clean up
    delete timer_queue_p;

    return false;
  } // end IF
  result = inherited::timer_queue (timerQueue_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", aborting\n")));

    // clean up
    delete timerQueue_;
    timerQueue_ = NULL;

    return false;
  } // end IF

  if (was_running)
    start ();

  return true;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
long
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::schedule_timer (ACE_Handler& handler_in,
                                                               const void* act_in,
                                                               const ACE_Time_Value& delay_in,
                                                               const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::schedule_timer"));

  long result = -1;

  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->schedule_timer (handler_in,
                                           act_in,
                                           delay_in,
                                           interval_in);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Proactor::schedule_timer(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_MODE_QUEUE:
    case COMMON_TIMER_MODE_REACTOR:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  configuration_.mode));
      return -1;
    }
  } // end SWITCH
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("scheduled timer (ID: %u)...\n"),
//               result));

  return result;
}
template <typename TimerQueueType,
          typename TimerQueueAdapterType>
long
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::schedule_timer (ACE_Event_Handler* handler_in,
                                                               const void* act_in,
                                                               const ACE_Time_Value& delay_in,
                                                               const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::schedule_timer"));

  long result = -1;

  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
    {
      ACE_ASSERT (false);
      ACE_NOTREACHED (return -1);
      break;
    }
    case COMMON_TIMER_MODE_QUEUE:
    {
      result = inherited::schedule (handler_in,
                                    act_in,
                                    delay_in,
                                    interval_in);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::schedule(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_MODE_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->schedule_timer (handler_in,
                                          act_in,
                                          delay_in,
                                          interval_in);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::schedule_timer(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  configuration_.mode));
      return -1;
    }
  } // end SWITCH
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("scheduled timer (ID: %u)...\n"),
//               result));

  return result;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
int
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::reset_timer_interval (long timerID_in,
                                                                     const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::reset_timer_interval"));

  int result = -1;

  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
    {
      ACE_ASSERT (false);
      ACE_NOTREACHED (return -1);
      break;
    }
    case COMMON_TIMER_MODE_QUEUE:
    {
      ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (inherited::mutex ());

      Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
      ACE_ASSERT (timer_queue_p);
      result = timer_queue_p->reset_interval (timerID_in, interval_in);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::reset_interval() (timer ID was: %d): \"%m\", aborting\n"),
                    timerID_in));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_MODE_REACTOR:
    {
      ACE_ASSERT (false);
      ACE_NOTREACHED (return -1);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  configuration_.mode));
      return -1;
    }
  } // end SWITCH
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("reset timer interval (ID: %u)...\n"),
//               timerID_in));

  return result;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
int
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::cancel_timer (long timerID_in,
                                                             const void** act_out,
                                                             int dontCallHandleClose_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::cancel_timer"));

  ACE_UNUSED_ARG (dontCallHandleClose_in);
  int result = -1;

  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->cancel_timer (timerID_in, act_out);
      if (result <= 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Proactor::cancel_timer() (timer ID was: %d): \"%m\", aborting\n"),
                    timerID_in));
        return result;
      } // end IF
      break;
    }
    case COMMON_TIMER_MODE_QUEUE:
    {
      result = inherited::cancel (timerID_in, act_out);
      if (result <= 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::cancel() (timer ID was: %d): \"%m\", aborting\n"),
                    timerID_in));
        return result;
      } // end IF
      break;
    }
    case COMMON_TIMER_MODE_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->cancel_timer (timerID_in, act_out);
      if (result <= 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::cancel_timer() (timer ID was: %d): \"%m\", aborting\n"),
                    timerID_in));
        return result;
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  configuration_.mode));
      return -1;
    }
  } // end SWITCH
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("cancelled timer (ID: %u)...\n"),
//               timerID_in));

  return result;
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::dump_state"));

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (const_cast<OWN_TYPE_T*> (this)->mutex ());

    // *TODO*: print some meaningful data
    ACE_ASSERT (false);
  } // end lock scope
}

template <typename TimerQueueType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<TimerQueueType,
                       TimerQueueAdapterType>::initialize (const Common_TimerConfiguration& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::initialize"));

  // sanity check(s)
  bool was_running = isRunning ();
  if (was_running)
    stop (true);

  configuration_ = configuration_in;

  // sanity check(s)
  switch (configuration_.mode)
  {
    case COMMON_TIMER_MODE_PROACTOR:
      break;
    case COMMON_TIMER_MODE_QUEUE:
    {
      ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (inherited::mutex ());

//      Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
//      ACE_ASSERT (timer_queue_p);

      if (!timerQueue_)
        if (!initializeTimerQueue ())
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize timer queue: \"%m\", aborting\n")));
          return false;
        } // end IF

      break;
    }
    case COMMON_TIMER_MODE_REACTOR:
    case COMMON_TIMER_MODE_SIGNAL:
      break;
    case COMMON_TIMER_MODE_INVALID:
    case COMMON_TIMER_MODE_MAX:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  configuration_.mode));
      return false;
    }
  } // end SWITCH

  if (was_running)
    start ();

  return true;
}
