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

#include "ace/Asynch_IO.h"
#include "ace/Event_Handler.h"
#include "ace/Log_Msg.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"
#include "ace/Thread_Manager.h"

#include "common.h"
//#include "common_defines.h"
#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_timer_defines.h"
#include "common_timer_handler.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::Common_Timer_Manager_T ()
 : inherited (ACE_Thread_Manager::instance (), // thread manager --> use default
              NULL)                            // timer queue --> allocate (dummy) temporary first
 , configuration_ (NULL)
 , dispatch_ (COMMON_TIMER_DEFAULT_DISPATCH)
 , isInitialized_ (false)
 , queueType_ (COMMON_TIMER_DEFAULT_QUEUE)
 , timerHandler_ ()
 , timerQueue_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::Common_Timer_Manager_T"));

}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::~Common_Timer_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::~Common_Timer_Manager_T"));

  int result = -1;

  if (unlikely (isRunning ()))
    stop (true,  // wait for completion ?
          true); // locked access ?

  // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
  result = inherited::timer_queue (NULL);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", continuing\n")));
  if (timerQueue_)
    delete timerQueue_;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::start (ACE_thread_t& threadId_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::start"));

  // initialize return value(s)
  threadId_out = 0;

  // sanity check(s)
  if (unlikely (isRunning ()))
    return;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    case COMMON_TIMER_DISPATCH_REACTOR:
    case COMMON_TIMER_DISPATCH_SIGNAL:
      break;
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_Task_Base& task_base_r = const_cast<ACE_Task_Base&> (getR_5 ());

      // spawn the dispatching worker thread
      int result = -1;
      ACE_thread_t thread_ids_a[1];
      ACE_OS::memset (thread_ids_a, 0, sizeof (ACE_thread_t[1]));
      ACE_hthread_t thread_handles_a[1];
      ACE_OS::memset (thread_handles_a, 0, sizeof (ACE_hthread_t[1]));
      char thread_name_a[BUFSIZ];
      ACE_OS::memset (thread_name_a, 0, sizeof (char[BUFSIZ]));
      ACE_OS::strcpy (thread_name_a,
                      ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_THREAD_NAME));
      const char* thread_names_a[1];
      ACE_OS::memset (thread_names_a, 0, sizeof (const char*[1]));
      thread_names_a[0] = thread_name_a;
      result =
        task_base_r.activate ((THR_NEW_LWP      |
                               THR_JOINABLE     |
                               THR_INHERIT_SCHED),          // flags
                              1,                            // # threads --> 1
                              0,                            // force active ?
                              ACE_DEFAULT_THREAD_PRIORITY,  // priority
                              COMMON_TIMER_THREAD_GROUP_ID, // group id
                              NULL,                         // task base
                              thread_handles_a,             // thread handle(s)
                              NULL,                         // stack(s)
                              NULL,                         // stack size(s)
                              thread_ids_a,                 // thread id(s)
                              thread_names_a);              // thread name(s)
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::activate(): \"%m\", returning\n")));
        return;
      } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_THREAD_NAME),
                                         thread_ids_a[0]);
#endif // ACE_WIN32 || ACE_WIN64
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%s): spawned dispatch thread (group id: %d)\n"),
                  ACE_TEXT (COMMON_TIMER_THREAD_NAME),
                  inherited::grp_id_));
#endif // _DEBUG
      threadId_out = thread_ids_a[0];
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), returning\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::stop (bool waitForCompletion_in,
                                                     bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::stop"));

  // sanity check(s)
//  if (unlikely (!isRunning ()))
//    return;

  int result = -1;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor_Timer_Queue* timer_queue_p = NULL;
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      // sanity check(s)
      ACE_ASSERT (proactor_p);

      timer_queue_p = proactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::close(): \"%m\", continuing\n")));
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Timer_Queue* timer_queue_p = NULL;
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      // sanity check(s)
      ACE_ASSERT (reactor_p);

      timer_queue_p = reactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::close(): \"%m\", continuing\n")));
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP;

      ACE_NOTREACHED (return;)
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      // *NOTE*: deactivate the timer queue and wake up the worker thread
      inherited::deactivate ();

      if (likely (waitForCompletion_in))
      {
        ACE_Task_Base& task_base_r = const_cast<ACE_Task_Base&> (getR_5 ());

        result = task_base_r.wait ();
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("(%s) joined worker thread\n"),
                    ACE_TEXT (COMMON_TIMER_THREAD_NAME)));
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), continuing\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH

  // clean up
  unsigned int flushed_timers = flushTimers (lockedAccess_in);
  if (flushed_timers)
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("flushed %u timer(s)\n"),
                flushed_timers));
#else
    ;
#endif // _DEBUG
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::isRunning () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::isRunning"));

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      // sanity check(s)
      ACE_ASSERT (proactor_p);
      return !proactor_p->proactor_event_loop_done ();
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      // sanity check(s)
      ACE_ASSERT (reactor_p);
      return !reactor_p->event_loop_done ();
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
      return true;
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_Task_Base& task_base_r = const_cast<ACE_Task_Base&> (getR_5 ());
      return (task_base_r.thr_count () > 0);
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), continuing\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH

  return false;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::wait (bool waitForMessageQueue_in) const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::wait"));

  ACE_UNUSED_ARG (waitForMessageQueue_in);

  int result = -1;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor_Timer_Queue* timer_queue_p = NULL;
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      // sanity check(s)
      ACE_ASSERT (proactor_p);

      timer_queue_p = proactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      ACE_Time_Value one_second (1, 0);
      while (!timer_queue_p->is_empty ())
      {
        result = ACE_OS::sleep (one_second);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                      &one_second));
      } // end WHILE
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Timer_Queue* timer_queue_p = NULL;
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      // sanity check(s)
      ACE_ASSERT (reactor_p);

      timer_queue_p = reactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      ACE_Time_Value one_second (1, 0);
      while (!timer_queue_p->is_empty ())
      {
        result = ACE_OS::sleep (one_second);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                      &one_second));
      } // end WHILE
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
      break;
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_Task_Base& task_base_r = const_cast<ACE_Task_Base&> (getR_5 ());

      result = task_base_r.wait ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%s) joined worker thread\n"),
                  ACE_TEXT (COMMON_TIMER_THREAD_NAME)));
#endif // _DEBUG
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), continuing\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
unsigned int
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::flushTimers (bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::flushTimers"));

  // initialize return value(s)
  unsigned int return_value = 0;

  int result = -1;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor_Timer_Queue* timer_queue_p = NULL;
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      // sanity check(s)
      ACE_ASSERT (proactor_p);

      timer_queue_p = proactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::close(): \"%m\", continuing\n")));
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Timer_Queue* timer_queue_p = NULL;
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      // sanity check(s)
      ACE_ASSERT (reactor_p);

      timer_queue_p = reactor_p->timer_queue ();
      // sanity check(s)
      ACE_ASSERT (timer_queue_p);

      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::close(): \"%m\", continuing\n")));

      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
      ACE_ASSERT (timer_queue_p);
      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_ITimerQueue_t::close(): \"%m\", continuing\n")));
      break;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_SYNCH_RECURSIVE_MUTEX& mutex_r = inherited::mutex ();
      if (lockedAccess_in)
      {
        result = mutex_r.acquire ();
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_MT_SYNCH::RECURSIVE_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

//      const void* ACT_p = NULL;
//      long timer_id = 0;
//      Common_TimerQueueImplIterator_t iterator (*inherited::timer_queue ());
//      for (iterator.first ();
//           !iterator.isdone ();
//           iterator.next (), return_value++)
//      {
//        ACT_p = NULL;
//        timer_id = iterator.item ()->get_timer_id ();
//        result = inherited::cancel (timer_id, &ACT_p);
//        if (result == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to cancel timer (id: %d): \"%m\", continuing\n"),
//                      timer_id));
//        else
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("cancelled timer (id: %d)\n"),
//                      timer_id));
//      } // end FOR
      Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
      ACE_ASSERT (timer_queue_p);
      result = timer_queue_p->close ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_ITimerQueue_t::close(): \"%m\", continuing\n")));

      if (lockedAccess_in)
      {
        result = mutex_r.release ();
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_MT_SYNCH::RECURSIVE_MUTEX::release(): \"%m\", continuing\n")));
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), continuing\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH

  return return_value;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::initializeTimerQueue ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::initializeTimerQueue"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  int result = -1;
  Common_ITimerQueue_t* timer_queue_p = NULL;
  bool was_running = isRunning ();
  if (unlikely (was_running))
    stop (true,  // wait for completion ?
          true); // locked access ?

  if (unlikely (timerQueue_))
  {
    flushTimers ();
    // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
    result = inherited::timer_queue (NULL);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", continuing\n")));
    delete timerQueue_; timerQueue_ = NULL;
  } // end IF

  switch (queueType_)
  {
    case COMMON_TIMER_QUEUE_HEAP:
    {
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using heap timer queue\n")));
#endif // _DEBUG
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
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using list timer queue\n")));
#endif // _DEBUG
      ACE_NEW_NORETURN (timer_queue_p,
                        Common_TimerQueueListImpl_t (&timerHandler_,       // upcall functor
                                                     NULL,                 // freelist --> allocate
                                                     COMMON_TIME_POLICY)); // time policy
      break;
    }
    case COMMON_TIMER_QUEUE_WHEEL:
    {
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using wheel timer queue\n")));
#endif // _DEBUG
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
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer queue type (was: %d), aborting\n"),
                  queueType_));
      return false;
    }
  } // end SWITCH
  if (unlikely (!timer_queue_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  timerQueue_ = dynamic_cast<TIMER_QUEUE_T*> (timer_queue_p);
  if (unlikely (!timerQueue_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ACE_Abstract_Timer_Queue> failed, aborting\n")));
    delete timer_queue_p; timer_queue_p = NULL;
    return false;
  } // end IF
  result = inherited::timer_queue (timerQueue_);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", aborting\n")));
    delete timerQueue_; timerQueue_ = NULL;
    return false;
  } // end IF

  if (unlikely (was_running))
  {
    ACE_thread_t thread_id = 0;
    start (thread_id);
    ACE_UNUSED_ARG (thread_id);
  } // end IF

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
long
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::schedule_timer (Common_Timer_Handler* handler_in,
                                                               const void* ACT_in,
                                                               const ACE_Time_Value& delay_in,
                                                               const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::schedule_timer"));

  long result = -1;

  // sanity check(s)
  ACE_ASSERT (isInitialized_);
  ACE_ASSERT (handler_in);

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      // sanity check(s)
      ACE_ASSERT (proactor_p);

      ACE_Handler* handler_p = handler_in;
      if (interval_in != ACE_Time_Value::zero)
        result = proactor_p->schedule_repeating_timer (*handler_p,
                                                       ACT_in,
                                                       interval_in);
      else
        result = proactor_p->schedule_timer (*handler_p,
                                             ACT_in,
                                             delay_in);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Proactor::schedule_(repeating_)timer(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      result = inherited::schedule (handler_in,
                                    ACT_in,
                                    delay_in,
                                    interval_in);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::schedule(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      // sanity check(s)
      ACE_ASSERT (reactor_p);

      ACE_Event_Handler* handler_p = handler_in;
      result = reactor_p->schedule_timer (handler_p,
                                          ACT_in,
                                          delay_in,
                                          interval_in);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::schedule_timer(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      result = inherited::schedule (handler_in,
                                    ACT_in,
                                    delay_in,
                                    interval_in);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Asynch_Timer_Queue_Adapter::schedule(): \"%m\", aborting\n")));
        return -1;
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      return -1;
    }
  } // end SWITCH
  handler_in->set (result);
#if defined (_DEBUG)
   ACE_DEBUG ((LM_DEBUG,
               ACE_TEXT ("scheduled timer (id: %d)\n"),
               result));
#endif // _DEBUG

  return result;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
int
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::reset_timer_interval (long timerId_in,
                                                                     const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::reset_timer_interval"));

  int result = -1;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    case COMMON_TIMER_DISPATCH_REACTOR:
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (-1);

      ACE_NOTREACHED (return -1;)
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_SYNCH_RECURSIVE_MUTEX& mutex_r =
        const_cast<ACE_SYNCH_RECURSIVE_MUTEX&> (getR_3 ());
      { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, mutex_r, -1);
        Common_ITimerQueue_t* timer_queue_p = inherited::timer_queue ();
        ACE_ASSERT (timer_queue_p);
        result = timer_queue_p->reset_interval (timerId_in, interval_in);
      } // end lock scope
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::reset_interval() (timer ID was: %d): \"%m\", aborting\n"),
                    timerId_in));
        return -1;
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      return -1;
    }
  } // end SWITCH
#if defined (_DEBUG)
   ACE_DEBUG ((LM_DEBUG,
               ACE_TEXT ("reset timer interval (id was: %d)\n"),
               timerId_in));
#endif // _DEBUG
  return result;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
int
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::cancel_timer (long timerId_in,
                                                             const void** ACT_out,
                                                             int doNotCallHandleClose_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::cancel_timer"));

  ACE_UNUSED_ARG (doNotCallHandleClose_in);

  int result = -1;

  // *TODO*: remove type inference
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->cancel_timer (timerId_in, ACT_out);
      if (unlikely (result <= 0))
      { // *NOTE*: 0: timer had already expired
        ACE_DEBUG ((((result == -1) ? LM_ERROR : LM_DEBUG),
                    ACE_TEXT ("failed to ACE_Proactor::cancel_timer() (id was: %d): \"%m\", %s\n"),
                    timerId_in,
                    ((result == -1) ? ACE_TEXT ("aborting") : ACE_TEXT ("returning"))));
        return result;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      result = inherited::cancel (timerId_in, ACT_out);
      if (unlikely (result <= 0))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::cancel() (id was: %d): \"%m\", %s\n"),
                    timerId_in,
                    ((result == -1) ? ACE_TEXT ("aborting") : ACE_TEXT ("returning"))));
        return result;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->cancel_timer (timerId_in, ACT_out);
      if (unlikely (result <= 0))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::cancel_timer() (id was: %d): \"%m\", %s\n"),
                    timerId_in,
                    ((result == -1) ? ACE_TEXT ("aborting") : ACE_TEXT ("returning"))));
        return result;
      } // end IF
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      result = inherited::cancel (timerId_in, ACT_out);
      if (unlikely (result <= 0))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Asynch_Timer_Queue_Adapter::cancel() (id was: %d): \"%m\", %s\n"),
                    timerId_in,
                    ((result == -1) ? ACE_TEXT ("aborting") : ACE_TEXT ("returning"))));
        return result;
      } // end IF
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      return -1;
    }
  } // end SWITCH
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("cancelled timer (id was: %d)\n"),
              timerId_in));
#endif // _DEBUG
  return result;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
void
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::dump_state"));

  ACE_SYNCH_RECURSIVE_MUTEX& mutex_r =
    const_cast<ACE_SYNCH_RECURSIVE_MUTEX&> (getR_3 ());
  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, mutex_r);
    ACE_ASSERT (false);
    ACE_NOTSUP;

    ACE_NOTREACHED (return;)
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
bool
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::initialize"));

  // sanity check(s)
  bool was_running = isRunning ();
  if (unlikely (was_running))
    stop (true);

  if (unlikely (isInitialized_))
  {
    configuration_ = NULL;
    isInitialized_ = false;
    dispatch_ = COMMON_TIMER_DEFAULT_DISPATCH;
    queueType_ = COMMON_TIMER_DEFAULT_QUEUE;

    isInitialized_ = false;
  } // end IF
  configuration_ = &const_cast<ConfigurationType&> (configuration_in);
  // *TODO*: remove type inferences
  dispatch_ = configuration_in.dispatch;
  queueType_ = configuration_in.queueType;

  // sanity check(s)
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
      break;
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      ACE_SYNCH_RECURSIVE_MUTEX& mutex_r =
        const_cast<ACE_SYNCH_RECURSIVE_MUTEX&> (getR_3 ());
      { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, mutex_r, false);
        if (likely (!timerQueue_))
          if (unlikely (!initializeTimerQueue ()))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to initialize timer queue: \"%m\", aborting\n")));
            return false;
          } // end IF
      } // end lock scope

      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    case COMMON_TIMER_DISPATCH_SIGNAL:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      return false;
    }
  } // end SWITCH

  if (unlikely (was_running))
  {
    ACE_thread_t thread_id = 0;
    start (thread_id);
    ACE_UNUSED_ARG (thread_id);
  } // end IF

  isInitialized_ = true;

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
const ACE_SYNCH_RECURSIVE_MUTEX&
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::getR_3 () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::getR_3"));

  // sanity check(s)
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    case COMMON_TIMER_DISPATCH_REACTOR:
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (ACE_SYNCH_RECURSIVE_MUTEX ());
      ACE_NOTREACHED (return ACE_SYNCH_RECURSIVE_MUTEX ();)
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);
      return this_p->inherited::mutex ();
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (ACE_SYNCH_RECURSIVE_MUTEX ());
  ACE_NOTREACHED (return ACE_SYNCH_RECURSIVE_MUTEX ();)
}
//template <ACE_SYNCH_DECL,
//          typename ConfigurationType,
//          typename TimerQueueAdapterType>
//const typename TimerQueueAdapterType::TIMER_QUEUE&
//Common_Timer_Manager_T<ACE_SYNCH_USE,
//                       ConfigurationType,
//                       TimerQueueAdapterType>::getR_4 () const
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::getR_4"));
//
//  // sanity check(s)
//  switch (dispatch_)
//  {
//    case COMMON_TIMER_DISPATCH_PROACTOR:
//    case COMMON_TIMER_DISPATCH_REACTOR:
//    {
//      ACE_ASSERT (false);
//      ACE_NOTSUP_RETURN (typename TimerQueueAdapterType::TIMER_QUEUE ());
//
//      ACE_NOTREACHED (return typename TimerQueueAdapterType::TIMER_QUEUE ();)
//    }
//    case COMMON_TIMER_DISPATCH_QUEUE:
//      return *(inherited::timer_queue ());
//    case COMMON_TIMER_DISPATCH_SIGNAL:
//      return inherited::timer_queue ();
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
//                  dispatch_));
//      break;
//    }
//  } // end SWITCH
//
//  return typename TimerQueueAdapterType::TIMER_QUEUE ();
//}
template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
const ACE_Task_Base&
Common_Timer_Manager_T<ACE_SYNCH_USE,
                       ConfigurationType,
                       TimerQueueAdapterType>::getR_5 () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager_T::getR_5"));

  // sanity check(s)
  switch (dispatch_)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    case COMMON_TIMER_DISPATCH_REACTOR:
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (ACE_Task_Base ());
      ACE_NOTREACHED (return ACE_Task_Base ();)
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
      return *this;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), aborting\n"),
                  dispatch_));
      break;
    }
  } // end SWITCH

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (ACE_Task_Base ());
  ACE_NOTREACHED (return ACE_Task_Base ();)
}
