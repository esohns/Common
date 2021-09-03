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

#include "common_macros.h"

template <typename TimerManagerType>
Common_Timer_SecondPublisher_T<TimerManagerType>::Common_Timer_SecondPublisher_T ()
 : handler_ (this)
 , lock_ ()
 , subscribers_ ()
 , timerId_ (-1)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::Common_Timer_SecondPublisher_T"));

}

template <typename TimerManagerType>
Common_Timer_SecondPublisher_T<TimerManagerType>::~Common_Timer_SecondPublisher_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::~Common_Timer_SecondPublisher_T"));

  if (unlikely (isRunning ()))
    stop (true,  // N/A
          true); // N/A
}

template <typename TimerManagerType>
bool
Common_Timer_SecondPublisher_T<TimerManagerType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::start"));

  ACE_UNUSED_ARG (timeout_in);

  // sanity check(s)
  if (unlikely (isRunning ()))
    return true;

  timerId_ = toggleTimer ();
  if (unlikely (timerId_ == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to schedule second-granularity timer: \"%m\" aborting\n")));
    return false;
  } // end IF
  return true;
}

template <typename TimerManagerType>
void
Common_Timer_SecondPublisher_T<TimerManagerType>::stop (bool,
                                                        bool)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::stop"));

  // sanity check(s)
  if (unlikely (!isRunning ()))
    return;

  timerId_ = toggleTimer ();
  ACE_ASSERT (timerId_ == -1);
}

template <typename TimerManagerType>
void
Common_Timer_SecondPublisher_T<TimerManagerType>::subscribe (Common_ICounter* interface_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::subscribe"));

  // sanity check(s)
  ACE_ASSERT (interface_in);

  { ACE_GUARD (ACE_Recursive_Thread_Mutex, aGuard, lock_);
    subscribers_.push_back (interface_in);
  } // end lock scope
}

template <typename TimerManagerType>
void
Common_Timer_SecondPublisher_T<TimerManagerType>::unsubscribe (Common_ICounter* interface_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::unsubscribe"));

  SUBSCRIBERS_ITERATOR_T iterator;
  { ACE_GUARD (ACE_Recursive_Thread_Mutex, aGuard, lock_);
    for (iterator = subscribers_.begin ();
         iterator != subscribers_.end ();
         ++iterator)
      if ((*iterator) == interface_in)
        break;
    if (iterator != subscribers_.end ())
      subscribers_.erase (iterator);
  } // end lock scope
}

template <typename TimerManagerType>
void
Common_Timer_SecondPublisher_T<TimerManagerType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::dump_state"));

  // *TODO*
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

template <typename TimerManagerType>
void
Common_Timer_SecondPublisher_T<TimerManagerType>::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::reset"));

  { ACE_GUARD (ACE_Recursive_Thread_Mutex, aGuard, lock_);
    for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_.begin ();
         iterator != subscribers_.end ();
         ++iterator)
    { ACE_ASSERT (*iterator);
      try {
        (*iterator)->reset ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_ICounter::reset(), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

template <typename TimerManagerType>
long
Common_Timer_SecondPublisher_T<TimerManagerType>::toggleTimer ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_SecondPublisher_T::toggleTimer"));

  typename TimerManagerType::INTERFACE_T* itimer_manager_p =
    TimerManagerType::SINGLETON_T::instance ();
  ACE_ASSERT (itimer_manager_p);

  if (timerId_ == -1)
  {
    ACE_Time_Value one_second (1, 0); // one-second interval
    timerId_ =
      itimer_manager_p->schedule_timer (&handler_,                    // event handler handle
                                        NULL,                         // asynchronous completion token
                                        COMMON_TIME_NOW + one_second, // first wakeup time
                                        one_second);                  // interval
    if (unlikely (timerId_ == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::schedule_timer(%#T): \"%m\", aborting\n"),
                  &one_second));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("scheduled second-interval timer (id: %d)\n"),
                  timerId_));
  } // end IF
  else
  {
    int result = -1;
    const void* act_p = NULL;
    result = itimer_manager_p->cancel_timer (timerId_,
                                             &act_p);
    if (unlikely (result <= 0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                  timerId_));
    ACE_UNUSED_ARG (act_p);
    timerId_ = -1;
  } // end ELSE

  return timerId_;
}
