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
#include "stdafx.h"

#include "common_timer_manager.h"

#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_itimer.h"
#include "common_macros.h"

// define static variables
Common_TimePolicy_t Common_Timer_Manager::timePolicy_;

Common_Timer_Manager::Common_Timer_Manager ()
 : inherited (ACE_Thread_Manager::instance (), // thread manager --> use default
              NULL)                            // timer queue --> allocate (dummy) temp first :( *TODO*
 , timerHandler_ ()
 , timerQueue_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::Common_Timer_Manager"));

  int result = -1;

  // set timer queue
  //ACE_NEW_NORETURN (timerQueue_,
  //                  Common_TimerQueueImpl_t ((COMMON_TIMER_PREALLOCATE_TIMER_SLOTS ? COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS
  //                                                                                 : 0), // preallocated slots
  //                                           COMMON_TIMER_PREALLOCATE_TIMER_SLOTS,       // preallocate timer nodes ?
  //                                           &timerHandler_,                             // upcall functor
  //                                           NULL,                                       // freelist --> allocate
  //                                           timePolicy_));                              // time policy
  ACE_NEW_NORETURN (timerQueue_,
                    Common_TimerQueueImpl_t (&timerHandler_, // upcall functor
                                             NULL,           // freelist --> allocate
                                             timePolicy_));  // time policy
  //ACE_NEW_NORETURN (timerQueue_,
  //                  Common_TimerQueueImpl_t (ACE_DEFAULT_TIMER_WHEEL_SIZE,
  //                                           ACE_DEFAULT_TIMER_WHEEL_RESOLUTION,
  //                                           (COMMON_PREALLOCATE_TIMER_SLOTS ? COMMON_DEF_NUM_TIMER_SLOTS
  //                                                                           : 0), // preallocate timer nodes ?
  //                                           &timerHandler_,                       // upcall functor
  //                                           NULL,                                 // freelist --> allocate
  //                                           timePolicy_));                        // time policy
  if (!timerQueue_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate Common_TimerQueueImpl_t, returning\n")));
    return;
  } // end IF
  result = inherited::timer_queue (timerQueue_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", returning\n")));
    return;
  } // end IF

  // OK: spawn the dispatching worker thread
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
  result = inherited::activate ((THR_NEW_LWP      |
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

Common_Timer_Manager::~Common_Timer_Manager ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::~Common_Timer_Manager"));

  int result = -1;

  if (isRunning ())
    stop ();

  // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
  result = inherited::timer_queue (NULL);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", continuing\n")));
  delete timerQueue_;
}

void
Common_Timer_Manager::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::start"));

  ACE_ASSERT (false);
  ACE_NOTREACHED (return;)
}

void
Common_Timer_Manager::stop (bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::stop"));

  inherited::deactivate ();
  // make sure the dispatcher thread has joined...
  inherited::wait ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) joined worker thread...\n"),
              ACE_TEXT (COMMON_TIMER_THREAD_NAME)));

  // clean up
  if (lockedAccess_in)
    inherited::mutex ().lock ();
  fini_timers ();
  if (lockedAccess_in)
    inherited::mutex ().release ();
}

bool
Common_Timer_Manager::isRunning () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::isRunning"));

  return (inherited::thr_count () > 0);
}

void
Common_Timer_Manager::fini_timers ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::fini_timers"));

  int result = -1;

  const void* act_p = NULL;
  long timer_id = 0;
  Common_TimerQueueImplIterator_t iterator (*inherited::timer_queue ());
  for (iterator.first ();
       !iterator.isdone ();
       iterator.next ())
  {
    act_p = NULL;
    timer_id = iterator.item ()->get_timer_id ();
    result = inherited::cancel (timer_id, &act_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                  timer_id));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("cancelled timer (ID: %d)...\n"),
                  timer_id));
  } // end FOR
}

void
Common_Timer_Manager::resetInterval (long timerID_in,
                                     const ACE_Time_Value& interval_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::resetInterval"));

  int result = -1;

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (inherited::mutex ());

    Common_TimerQueueImpl_t* timer_queue_p = inherited::timer_queue ();
    ACE_ASSERT (timer_queue_p);
    result = timer_queue_p->reset_interval (timerID_in, interval_in);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Abstract_Timer_Queue::reset_interval() (timer ID was: %u): \"%m\", returning\n"),
                  timerID_in));
      return;
    } // end IF
  } // end lock scope
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("reset timer interval (ID: %u)...\n"),
//               timerID_in));
}

void
Common_Timer_Manager::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::dump_state"));

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (const_cast<Common_Timer_Manager*> (this)->mutex ());

    // *TODO*: print some meaningful data
    ACE_ASSERT (false);
  } // end lock scope
}
