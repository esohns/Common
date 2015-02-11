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

#include "ace/Log_Msg.h"
#include "ace/High_Res_Timer.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_itimer.h"

Common_Timer_Manager::Common_Timer_Manager ()
 : inherited (ACE_Thread_Manager::instance (), // thread manager --> use default
              NULL)                            // timer queue --> allocate (dummy) temp first :( *TODO*
 , timerHandler_ ()
 , timerQueue_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Manager::Common_Timer_Manager"));

  // set time queue
  //ACE_NEW_NORETURN (timerQueue_,
  //								  Common_TimerQueueImpl_t (COMMON_DEF_NUM_TIMER_SLOTS,     // preallocated slots
  //																				 	 COMMON_PREALLOCATE_TIMER_SLOTS, // preallocate timer nodes ?
  //																					 &timerHandler_,                 // upcall functor
  //																					 NULL,                           // freelist --> allocate
  //																					 COMMON_TIME_POLICY));
  ACE_NEW_NORETURN (timerQueue_,
                    Common_TimerQueueImpl_t (ACE_DEFAULT_TIMER_WHEEL_SIZE,
                                             ACE_DEFAULT_TIMER_WHEEL_RESOLUTION,
                                             (COMMON_PREALLOCATE_TIMER_SLOTS ? COMMON_DEF_NUM_TIMER_SLOTS
                                                                             : 0), // preallocate timer nodes ?
                                             &timerHandler_,                       // upcall functor
                                             NULL,                                 // freelist --> allocate
                                             COMMON_TIME_POLICY));
  if (!timerQueue_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate Common_TimerQueueImpl_t, aborting\n")));

    return;
  } // end IF
  // *NOTE*: use a high resolution timer for best accuracy & lowest latency
  //ACE_High_Res_Timer::global_scale_factor();
  //myTimerQueue->gettimeofday(&ACE_High_Res_Timer::gettimeofday_hr);
  if (inherited::timer_queue (timerQueue_) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", aborting\n")));

    return;
  } // end IF

  // OK: spawn the dispatching worker thread
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;
  char thread_name[COMMON_BUFSIZE];
  ACE_OS::memset (thread_name, 0, sizeof (thread_name));
  ACE_OS::strcpy (thread_name,
                  ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_THREAD_NAME));
  const char* thread_names[1];
  thread_names[0] = thread_name;
  if (inherited::activate ((THR_NEW_LWP      |
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
                           thread_names) == -1)          // thread name(s)
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

  if (isRunning ())
    stop ();

  // *IMPORTANT NOTE*: avoid close()ing the timer queue in the base class dtor
  if (inherited::timer_queue (NULL) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Timer_Queue_Adapter::timer_queue(): \"%m\", aborting\n")));
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

  const void* act = NULL;
  long timer_id = 0;
  Common_TimerQueueImplIterator_t iterator (*inherited::timer_queue ());
  for (iterator.first ();
       !iterator.isdone ();
       iterator.next ())
  {
    act = NULL;
    timer_id = iterator.item ()->get_timer_id ();
    if (inherited::cancel (timer_id, &act) == -1)
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

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (inherited::mutex ());

    if (inherited::timer_queue ()->reset_interval (timerID_in, interval_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to reset_interval() for timer (ID: %u): \"%m\", aborting\n"),
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
