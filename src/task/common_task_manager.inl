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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_defines.h"

#include "common_timer_manager_common.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::Common_Task_Manager_T ()
 : configuration_(NULL)
 , isActive_(true)
 , resetTimeoutHandler_ (this)
 , resetTimeoutHandlerId_ (-1)
 , tasks_ ()
 , lock_ ()
 , condition_ (lock_)
 , userData_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::Common_Task_Manager_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::~Common_Task_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::~Common_Task_Manager_T"));

  if (unlikely (resetTimeoutHandlerId_ != -1))
  {
    Common_ITimer_Manager_t* timer_interface_p =
      COMMON_TIMERMANAGER_SINGLETON::instance ();
    const void* act_p = NULL;
    int result = timer_interface_p->cancel_timer (resetTimeoutHandlerId_,
                                                  &act_p);
    if (unlikely (result <= 0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                  resetTimeoutHandlerId_));
  } // end IF

  bool do_abort = false;
  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    if (!unlikely (tasks_.is_empty ()))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%u remaining tasks(s) in dtor, continuing\n"),
                  tasks_.size ()));
      do_abort = true;
    } // end IF
  } // end lock scope
  if (unlikely (do_abort))
    abort ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
bool
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
bool
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::lock (bool block_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::lock"));

  ACE_UNUSED_ARG (block_in);

  int result = lock_.acquire ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SYNCH_RECURSIVE_MUTEX::acquire(): \"%m\", aborting\n")));

  return (result == 0);
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
int
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::unlock (bool unblock_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::unlock"));

  ACE_UNUSED_ARG (unblock_in);

  int result = lock_.release ();
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SYNCH_RECURSIVE_MUTEX::release(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  return 0;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
Common_Task_2<ACE_SYNCH_USE,
              TimePolicyType,
              StatisticContainerType>*
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::operator[] (unsigned int index_in) const
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::operator[]"));

  TASK_T* task_p = NULL;

  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, NULL);
    unsigned int index = 0;
    for (CONTAINER_ITERATOR_T iterator (const_cast<CONTAINER_T&> (tasks_));
         iterator.next (task_p);
         iterator.advance (), index++)
      if (index == index_in)
        break;
    if (unlikely (!task_p))
      return NULL;
  } // end lock scope

  return task_p;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
bool
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::register_ (ACE_Task_Base* task_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::register_"));

  // sanity check(s)
  ACE_ASSERT (configuration_);
  TASK_T* task_p = static_cast<TASK_T*> (task_in);
  ACE_ASSERT (task_p);

  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, false);
    if (unlikely (!isActive_ || // --> (currently) rejecting new tasks
                  (configuration_->maximumNumberOfConcurrentTasks && // 0 ? --> no limit
                   (tasks_.size () >= configuration_->maximumNumberOfConcurrentTasks))))
      return false;

    if (!unlikely (tasks_.insert_tail (task_p)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_DLList::insert_tail(): \"%m\", aborting\n")));
      return false;
    } // end IF

    if (likely (!task_p->isRunning ()))
      task_p->start (NULL);
  } // end lock scope

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::deregister (ACE_Task_Base* task_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::deregister"));

  // sanity check(s)
  TASK_T* task_p = static_cast<TASK_T*> (task_in);
  ACE_ASSERT (task_p);

  TASK_T* task_2 = NULL;
  bool found = false;
  int result = -1;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (tasks_);
         iterator.next (task_2);
         iterator.advance ())
      if (unlikely (task_2 == task_p))
      {
        found = true;
        result = iterator.remove ();
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_DLList_Iterator::remove(): \"%m\", continuing\n")));
        break;
      } // end IF
    if (unlikely (!found))
    {
      // *NOTE*: most probably cause: handle already deregistered (--> check
      //         implementation !)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("task handle (was: 0x%@) not found, returning\n"),
                  task_p));
      return;
    } // end IF
    ACE_ASSERT (task_2);

    // sanity check(s)
    if (unlikely (task_2->isRunning () && !task_2->isShuttingDown ()))
      task_2->stop (false,  // wait ?
                    false); // high priority ?

    // signal any waiters
    result = condition_.broadcast ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\", continuing\n")));
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::execute (ACE_Task_Base* task_in,
                                              ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::execute"));

  ACE_UNUSED_ARG (timeout_in);

  // sanity check(s)
  TASK_T* task_p = static_cast<TASK_T*> (task_in);
  ACE_ASSERT (task_p);

  TASK_T* task_2 = NULL;
  bool register_b = true;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (tasks_);
         iterator.next (task_2);
         iterator.advance ())
      if (unlikely (task_2 == task_p))
      {
        register_b = false;
        break;
      } // end IF
    if (unlikely (!register_b))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("task handle (was: 0x%@) already registered, returning\n"),
                  task_p));
      return;
    } // end IF
  } // end lock scope
  ACE_ASSERT (register_b);

  if (!register_ (task_in))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to register task (handle was: 0x%@), returning\n"),
                task_in));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::finished (ACE_Task_Base* task_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::finished"));

  // sanity check(s)
  TASK_T* task_p = static_cast<TASK_T*> (task_in);
  ACE_ASSERT (task_p);

  TASK_T* task_2 = NULL;
  bool found_b = false;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (tasks_);
         iterator.next (task_2);
         iterator.advance ())
      if (unlikely (task_2 == task_p))
      {
        found_b = true;
        break;
      } // end IF
    if (unlikely (!found_b))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("task handle (was: 0x%@) not registered, returning\n"),
                  task_p));
      return;
    } // end IF
  } // end lock scope
  ACE_ASSERT (found_b);

  deregister (task_in);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("task (handle was: 0x%@) finished...\n"),
              task_in));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
unsigned int
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::count () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::count"));

  unsigned int result = 0;

  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, 0);
    result = static_cast<unsigned int> (tasks_.size ());
  } // end lock scope

  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
bool
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::start"));

  ACE_UNUSED_ARG (timeout_in);

  // sanity check(s)
  ACE_ASSERT (configuration_);

  int result = -1;
  Common_ITimer_Manager_t* timer_interface_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  const void* act_p = NULL;

  // (re-)schedule the visitor interval timer
  if (unlikely (resetTimeoutHandlerId_ != -1))
  {
    result = timer_interface_p->cancel_timer (resetTimeoutHandlerId_,
                                              &act_p);
    if (unlikely (result <= 0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                  resetTimeoutHandlerId_));
    resetTimeoutHandlerId_ = -1;
  } // end IF
  resetTimeoutHandlerId_ =
    timer_interface_p->schedule_timer (&resetTimeoutHandler_,          // event handler handle
                                       NULL,                           // asynchronous completion token
                                       ACE_Time_Value::zero,           // first wakeup time
                                       configuration_->visitInterval); // interval
  if (unlikely (resetTimeoutHandlerId_ == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_ITimer::schedule_timer(%#T): \"%m\", aborting\n"),
                &configuration_->visitInterval));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("scheduled visitor interval timer (id: %d, interval: %#T)\n"),
              resetTimeoutHandlerId_,
              configuration_->visitInterval));

  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, false);
    isActive_ = true;
  } // end lock scope

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::stop (bool waitForCompletion_in,
                                           bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::stop"));

  ACE_UNUSED_ARG (highPriority_in);

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    isActive_ = false;
  } // end lock scope

  if (unlikely (resetTimeoutHandlerId_ != -1))
  {
    Common_ITimer_Manager_t* timer_interface_p =
        COMMON_TIMERMANAGER_SINGLETON::instance ();
    const void* act_p = NULL;
    int result = timer_interface_p->cancel_timer (resetTimeoutHandlerId_,
                                                  &act_p);
    if (unlikely (result <= 0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                  resetTimeoutHandlerId_));
    resetTimeoutHandlerId_ = -1;
  } // end IF

  if (likely (waitForCompletion_in))
    wait (true); // N/A
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::abort (bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::abort"));

  TASK_T* task_p = NULL;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (tasks_);
         iterator.next (task_p);
         iterator.advance ())
    { ACE_ASSERT (task_p);
      try {
        task_p->stop (waitForCompletion_in,
                      false); // high priority ?
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_ITask::stop(), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::wait (bool) const
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::wait"));

  int result = -1;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    while (!tasks_.is_empty ())
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("waiting for %u task(s)...\n"),
                  tasks_.size ()));
      result = condition_.wait ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_RECURSIVE_CONDITION::wait(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::abortLeastRecent ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::abortLeastRecent"));

  TASK_T* task_p = NULL;
  int result = -1;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    if (unlikely (tasks_.is_empty ()))
      return;

    // close "oldest" task --> list head
    result = tasks_.get (task_p, 0);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_DLList::get(0): \"%m\", returning\n")));
      return;
    } // end IF
    ACE_ASSERT (task_p);
    try {
      task_p->stop (false,
                    false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask::stop(), continuing\n")));
    }
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::abortMostRecent ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::abortMostRecent"));

  TASK_T* task_p = NULL;
  int result = -1;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    if (unlikely (tasks_.is_empty ()))
      return;

    // close "newest" task --> list tail
    CONTAINER_REVERSEITERATOR_T iterator (tasks_);
    result = iterator.next (task_p);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_DLList_Reverse_Iterator::next(): \"%m\", returning\n")));
      return;
    } // end IF
    ACE_ASSERT (task_p);
    try {
      task_p->stop (false,
                    false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask::stop(), continuing\n")));
    }
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::reset"));

  TASK_T* task_p = NULL;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (const_cast<CONTAINER_T&> (tasks_));
         iterator.next (task_p);
         iterator.advance ())
    { ACE_ASSERT (task_p);
      try {
        task_p->update (configuration_->visitInterval);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_IStatistic_T::update(), continuing\n")));
      }
      task_p = NULL;
    } // end FOR
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename StatisticContainerType,
          typename ConfigurationType,
          typename UserDataType>
void
Common_Task_Manager_T<ACE_SYNCH_USE,
                      TimePolicyType,
                      StatisticContainerType,
                      ConfigurationType,
                      UserDataType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Manager_T::dump_state"));

  TASK_T* task_p = NULL;

  { ACE_GUARD (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_);
    for (CONTAINER_ITERATOR_T iterator (const_cast<CONTAINER_T&> (tasks_));
         iterator.next (task_p);
         iterator.advance ())
    { ACE_ASSERT (task_p);
      try {
        task_p->dump_state ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
      }
      task_p = NULL;
    } // end FOR
  } // end lock scope
}
