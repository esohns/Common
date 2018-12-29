﻿/***************************************************************************
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

#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/OS_Memory.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_time_common.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_tools.h"

#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::Common_TaskBase_T (const std::string& threadName_in,
                                                int threadGroupId_in,
                                                unsigned int threadCount_in,
                                                bool autoStart_in,
                                                MESSAGE_QUEUE_T* messageQueue_in)
#if defined (ACE_THREAD_MANAGER_LACKS_STATICS)
 : inherited (ACE_THREAD_MANAGER_SINGLETON::instance (), // thread manager instance
#else /* ! ACE_THREAD_MANAGER_LACKS_STATICS */
 : inherited (ACE_Thread_Manager::instance (),           // thread manager instance
#endif /* ACE_THREAD_MANAGER_LACKS_STATICS */
              messageQueue_in)                           // message queue handle
 , threadCount_ (threadCount_in)
 , threadName_ (threadName_in)
 , threads_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::Common_TaskBase_T"));

  // set group id for worker thread(s)
  inherited::grp_id (threadGroupId_in);

  // auto-start ?
  if (autoStart_in)
  {
    int result = open (NULL);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::open(): \"%m\", continuing\n")));
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::~Common_TaskBase_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::~Common_TaskBase_T"));

  int result = -1;

  // sanity check(s)
  if (unlikely (inherited::thr_count_ > 0))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: %d active thread(s) in dtor --> check implementation\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%d active thread(s) in dtor --> check implementation\n"),
                  inherited::thr_count_));

    // *WARNING*: there already may or may not be a message queue at this stage
    result = close (1);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::close(1): \"%m\", continuing\n")));

    wait (false);
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_hthread_t handle = ACE_INVALID_HANDLE;
  for (THREAD_IDS_ITERATOR_T iterator = threads_.begin ();
       iterator != threads_.end ();
       ++iterator)
  {
    handle = (*iterator).handle ();
    if (unlikely (handle != ACE_INVALID_HANDLE))
      if (!::CloseHandle (handle))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                    handle,
                    ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
  } // end FOR
#endif // ACE_WIN32 || ACE_WIN64
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
bool
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::lock (bool block_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::lock"));

  int result = -1;

  result = (block_in ? lock_.tryacquire () : lock_.acquire ());
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (block_in)
    {
      if (error != EBUSY)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX::tryacquire(): \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
  } // end IF

  return (result == 0);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::close"));

  // *NOTE*: this method may be invoked
  //         - by external threads shutting down the active object (arg_in: 1)
  //         - by worker thread(s) upon returning from svc() (arg_in: 0)
  switch (arg_in)
  {
    case 0:
    {
      // final thread ? --> clean up
      if (likely (ACE_OS::thr_equal (ACE_OS::thr_self (),
                                     inherited::last_thread ())))
      {
        { ACE_GUARD_RETURN (typename ITASKCONTROL_T::MUTEX_T, aGuard, lock_, -1);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_hthread_t handle = ACE_INVALID_HANDLE;
          for (THREAD_IDS_ITERATOR_T iterator = threads_.begin ();
               iterator != threads_.end ();
               ++iterator)
          {
            handle = (*iterator).handle ();
            if (unlikely (handle != ACE_INVALID_HANDLE))
              if (!::CloseHandle (handle))
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                            handle,
                            ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
          } // end FOR
#endif // ACE_WIN32 || ACE_WIN64
          threads_.clear ();
        } // end lock scope

        // *NOTE*: iff the task had several worker thread(s), there will
        //         potentially still be STOP message(s) in the queue (see below)
        //         --> remove them
        if (likely (inherited::msg_queue_))
        {
          int result = inherited::msg_queue_->flush ();
          if (unlikely (result == -1))
          {
            if (inherited::mod_)
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("%s: failed to ACE_Message_Queue::flush(): \"%m\", continuing\n"),
                          inherited::mod_->name ()));
            else
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to ACE_Message_Queue::flush(): \"%m\", continuing\n")));
          } // end IF
#if defined (_DEBUG)
          if (unlikely (result))
          {
            if (inherited::mod_)
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("%s: flushed %u message(s)\n"),
                          inherited::mod_->name (),
                          result));
            else
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("flushed %u message(s)\n"),
                          result));
          } // end IF
#endif // _DEBUG
        } // end IF

        break;
      } // end IF

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (unlikely (inherited::thr_count_ == 0))
        break; // nothing to do

      // *NOTE*: make sure there is a message queue
      if (likely (inherited::msg_queue_))
        stop (false, // wait for completion ?
              true); // N/A
      else
      {
        if (inherited::mod_)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: cannot signal %d worker thread(s) (no message queue) --> check implementation\n"),
                      inherited::mod_->name (),
                      inherited::thr_count_));
        else
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("cannot signal %d worker thread(s) (no message queue) --> check implementation\n"),
                      inherited::thr_count_));
      } // end ELSE

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
          typename TimePolicyType,
          typename LockType>
ACE_thread_t
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::start"));

  // initialize return value(s)
  ACE_thread_t return_value = 0;

  // sanity check(s)
  ACE_ASSERT (threadCount_);
  ACE_ASSERT (inherited::thr_count_ <= threadCount_);
  if (unlikely (inherited::thr_count_ == threadCount_))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: object already active (thread pool size: %u), aborting\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("object already active (thread pool size: %u), aborting\n"),
                  inherited::thr_count_));
    return return_value;
  } // end IF
#if defined (_DEBUG)
  if (unlikely (inherited::thr_count_ > 0))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: object already active (current thread pool size: %u), continuing\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("object already active (current thread pool size: %u), continuing\n"),
                  inherited::thr_count_));
  } // end IF
#endif // _DEBUG

  // spawn one worker thread
  ACE_thread_t* thread_ids_p = NULL;
  ACE_NEW_NORETURN (thread_ids_p,
                    ACE_thread_t[1]);
  if (unlikely (!thread_ids_p))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory (%u), aborting\n"),
                  inherited::mod_->name (),
                  sizeof (ACE_thread_t)));
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  sizeof (ACE_thread_t)));
    return return_value;
  } // end IF
  ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
  ACE_hthread_t* thread_handles_p = NULL;
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[1]);
  if (unlikely (!thread_handles_p))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory (%u), aborting\n"),
                  inherited::mod_->name (),
                  sizeof (ACE_hthread_t)));
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  sizeof (ACE_hthread_t)));
    delete [] thread_ids_p; thread_ids_p = NULL;
    return return_value;
  } // end IF
  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  const char** thread_names_p = NULL;
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[1]);
  if (unlikely (!thread_names_p))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory (%u), aborting\n"),
                  inherited::mod_->name (),
                  sizeof (const char*)));
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  sizeof (const char*)));
    delete [] thread_ids_p; thread_ids_p = NULL;
    delete [] thread_handles_p; thread_handles_p = NULL;
    return return_value;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  char* thread_name_p = NULL;
  ACE_NEW_NORETURN (thread_name_p,
                    char[BUFSIZ]);
  if (unlikely (!thread_name_p))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory (%u), aborting\n"),
                  inherited::mod_->name (),
                  sizeof (char) * BUFSIZ));
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  sizeof (char) * BUFSIZ));
    delete [] thread_ids_p; thread_ids_p = NULL;
    delete [] thread_handles_p; thread_handles_p = NULL;
    delete [] thread_names_p; thread_names_p = NULL;
    return return_value;
  } // end IF
  ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
  ACE_OS::strcpy (thread_name_p,
                  threadName_.c_str ());
  thread_names_p[0] = thread_name_p;

  int result = -1;
  ACE_Thread_ID thread_id;
  { ACE_GUARD_RETURN (typename ITASKCONTROL_T::MUTEX_T, aGuard, lock_, return_value);
    result =
        inherited::activate ((THR_NEW_LWP      |
                              THR_JOINABLE     |
                              THR_INHERIT_SCHED),         // flags
                             1,                           // #threads
                             1,                           // force active ?
                             ACE_DEFAULT_THREAD_PRIORITY, // priority
                             inherited::grp_id (),        // group id (see above)
                             NULL,                        // task base
                             thread_handles_p,            // thread handle(s)
                             NULL,                        // stack(s)
                             NULL,                        // stack size(s)
                             thread_ids_p,                // thread id(s)
                             thread_names_p);             // thread name(s)
    if (unlikely (result == -1))
    {
      if (inherited::mod_)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Task::activate(%u): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    1));
      else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Task::activate(%u): \"%m\", aborting\n")));
      delete [] thread_ids_p; thread_ids_p = NULL;
      delete [] thread_handles_p; thread_handles_p = NULL;
      delete [] thread_names_p[0]; thread_names_p[0] = NULL;
      delete [] thread_names_p; thread_names_p = NULL;
      return return_value;
    } // end IF
    thread_id.handle (thread_handles_p[0]);
    thread_id.id (thread_ids_p[0]);
    threads_.push_back (thread_id);
  } // end lock scope
#if defined (_DEBUG)
  std::ostringstream string_stream;
  string_stream << thread_ids_p[0];
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("spawned a worker thread (\"%s\", group: %d, id: %s)\n"),
              ACE_TEXT (threadName_.c_str ()),
              inherited::grp_id_,
              ACE_TEXT (string_stream.str ().c_str ())));
#endif // _DEBUG
  return_value = thread_ids_p[0];

  // clean up
  delete [] thread_ids_p; thread_ids_p = NULL;
  delete [] thread_handles_p; thread_handles_p = NULL;
  delete [] thread_names_p[0]; thread_names_p[0] = NULL;
  delete [] thread_names_p; thread_names_p = NULL;

  return return_value;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::stop (bool waitForCompletion_in,
                                   bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  control (ACE_Message_Block::MB_STOP,
           true);                      // high-priority

  if (waitForCompletion_in)
    wait (true);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::idle ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::idle"));

  // sanity check(s)
  if (unlikely (!inherited::msg_queue_))
    return; // no queue --> nothing to do

  ACE_Time_Value one_second (1, 0);
  int result = -1;
  size_t count = 0;
#if defined (_DEBUG)
  bool has_waited = false;
#endif // _DEBUG

  do
  {
    count = inherited::msg_queue_->message_count ();
    if (!count)
      break;
#if defined (_DEBUG)
    has_waited = true;
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s%swaiting for %u message(s)...\n"),
                (inherited::mod_ ? inherited::mod_->name () : ACE_TEXT ("")),
                (inherited::mod_ ? ACE_TEXT (": ") : ACE_TEXT ("")),
                count));
#endif // _DEBUG

    result = ACE_OS::sleep (one_second);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &one_second));
  } while (true);

#if defined (_DEBUG)
  if (has_waited)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s%swaiting for message(s)...DONE\n"),
                (inherited::mod_ ? inherited::mod_->name () : ACE_TEXT ("")),
                (inherited::mod_ ? ACE_TEXT (": ") : ACE_TEXT (""))));
#endif // _DEBUG
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::finished ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::finished"));

  int result = close (1);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_TaskBase_T::close(1): \"%m\", continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::wait (bool waitForMessageQueue_in) const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::wait"));

  int result = -1;
  OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);

  // step1: wait for the message queue to empty
  if (likely (waitForMessageQueue_in))
    this_p->idle ();

  // step2: wait for any worker thread(s)
  try {
    result = this_p->TASK_T::wait ();
  } catch (...) {
    // *NOTE*: on Win32 systems, ::CloseHandle() behaves peculiarly:
    //         "...If the application is running under a debugger, the function
    //         will throw an exception if it receives either a handle value that
    //         is not valid or a 'pseudo-handle' value. This can happen if you
    //         close a handle twice, ..." (see OS_NS_Thread.inl:3027)
    enum ACE_Log_Priority log_priority = LM_ERROR;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    log_priority = LM_DEBUG;
#endif // ACE_WIN32 || ACE_WIN64
    if (inherited::mod_)
      ACE_DEBUG ((log_priority,
                  ACE_TEXT ("%s: caught exception in ACE_Task::wait(), returning\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((log_priority,
                  ACE_TEXT ("caught exception in ACE_Task::wait(), returning\n")));
  }
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

// *** BEGIN dummy stub methods ***
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Error_Tools::setThreadName (threadName_,
                                     0);
#endif // ACE_WIN32 || ACE_WIN64
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
              ACE_TEXT (threadName_.c_str ()),
              inherited::grp_id_));
#endif // _DEBUG

  ACE_Message_Block* message_block_p = NULL;
  ACE_Message_Block::ACE_Message_Type message_type;
  int result = 1;
  int error = -1;
  int result_2 = -1;

  do
  {
    result = inherited::getq (message_block_p, NULL);
    if (unlikely (result == -1))
    {
      error = ACE_OS::last_error ();
      if (error != ESHUTDOWN)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("(%s): worker thread (id: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
                    ACE_TEXT (threadName_.c_str ())));
      break;
    } // end IF
    ACE_ASSERT (message_block_p);

    message_type = message_block_p->msg_type ();
    if (unlikely (message_type == ACE_Message_Block::MB_STOP))
    {
      if (inherited::thr_count_ > 1)
      {
        result_2 = inherited::putq (message_block_p, NULL);
        if (result_2 == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("(%s): failed to ACE_Task::putq(): \"%m\", aborting\n"),
                      ACE_TEXT (threadName_.c_str ())));
        else
          result = 0;
      } // end IF
      else
        result = 0;

      // clean up
      message_block_p->release ();

      break; // done
    } // end IF

    // clean up
    message_block_p->release ();
    message_block_p = NULL;
  } while (true);

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (threadName_.c_str ())));
#endif // _DEBUG

  return result;
}
// *** END dummy stub methods ***

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::open (void* args_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::open"));

  ACE_UNUSED_ARG (args_in);

  // sanity check(s)
  if (unlikely (inherited::thr_count_ > 0))
    return 0; // nothing to do

  ACE_thread_t* thread_ids_p = NULL;
  ACE_hthread_t* thread_handles_p = NULL;
  const char** thread_names_p = NULL;
  int result = -1;
  // spawn the worker thread(s)
  ACE_NEW_NORETURN (thread_ids_p,
                    ACE_thread_t[threadCount_]);
  if (unlikely (!thread_ids_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (ACE_thread_t) * threadCount_)));
    return -1;
  } // end IF
  ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[threadCount_]);
  if (unlikely (!thread_handles_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (ACE_hthread_t) * threadCount_)));
    delete [] thread_ids_p; thread_ids_p = NULL;
    return -1;
  } // end IF
  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[threadCount_]);
  if (unlikely (!thread_names_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (const char*) * threadCount_)));
    delete [] thread_ids_p; thread_ids_p = NULL;
    delete [] thread_handles_p; thread_handles_p = NULL;
    return -1;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  char* thread_name_p = NULL;
  std::string buffer;
  std::ostringstream converter;
  for (unsigned int i = 0;
       i < threadCount_;
       i++)
  {
    thread_name_p = NULL;
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (unlikely (!thread_name_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  (sizeof (char) * BUFSIZ)));
      delete [] thread_ids_p; thread_ids_p = NULL;
      delete [] thread_handles_p; thread_handles_p = NULL;
      for (unsigned int j = 0; j < i; j++)
        delete [] thread_names_p[j];
      delete [] thread_names_p; thread_names_p = NULL;
      return -1;
    } // end IF
    ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (i + 1);
    buffer = threadName_;
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::strcpy (thread_name_p,
                    buffer.c_str ());
    thread_names_p[i] = thread_name_p;
  } // end FOR

#if defined (_DEBUG)
  std::ostringstream string_stream;
#endif // _DEBUG
  { ACE_GUARD_RETURN (typename ITASKCONTROL_T::MUTEX_T, aGuard, lock_, -1);
    result =
      inherited::activate ((THR_NEW_LWP      |
                            THR_JOINABLE     |
                            THR_INHERIT_SCHED),             // flags
                           static_cast<int> (threadCount_), // # threads
                           0,                               // force active ?
                           ACE_DEFAULT_THREAD_PRIORITY,     // priority
                           inherited::grp_id (),            // group id (see above)
                           NULL,                            // task base
                           thread_handles_p,                // thread handle(s)
                           NULL,                            // stack(s)
                           NULL,                            // stack size(s)
                           thread_ids_p,                    // thread id(s)
                           thread_names_p);                 // thread name(s)
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::activate(%u): \"%m\", aborting\n"),
                  threadCount_));
      delete [] thread_ids_p; thread_ids_p = NULL;
      delete [] thread_handles_p; thread_handles_p = NULL;
      for (unsigned int i = 0; i < threadCount_; i++)
        delete [] thread_names_p[i];
      delete [] thread_names_p; thread_names_p = NULL;
      return result;
    } // end IF

    ACE_Thread_ID thread_id;
    for (unsigned int i = 0;
         i < threadCount_;
         i++)
    {
#if defined (_DEBUG)
      string_stream << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
                    << ACE_TEXT_ALWAYS_CHAR (" ")
                    << thread_ids_p[i]
                    << ACE_TEXT_ALWAYS_CHAR ("\n");
#endif // _DEBUG

      // clean up
      delete [] thread_names_p[i]; thread_names_p[i] = NULL;

      thread_id.handle (thread_handles_p[i]);
      thread_id.id (thread_ids_p[i]);
      threads_.push_back (thread_id);
    } // end FOR
  } // end lock scope
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): spawned %u dispatch thread(s) (group id: %d):\n%s"),
              ACE_TEXT (threadName_.c_str ()),
              threadCount_,
              inherited::grp_id_,
              ACE_TEXT (string_stream.str ().c_str ())));
#endif // _DEBUG

  // clean up
  delete [] thread_ids_p; thread_ids_p = NULL;
  delete [] thread_handles_p; thread_handles_p = NULL;
  delete [] thread_names_p; thread_names_p = NULL;

  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::control (int messageType_in,
                                      bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::control"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;

  // sanity check(s)
  if (unlikely (!inherited::msg_queue_))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: task has no message queue, returning\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("task has no message queue, returning\n")));
    return;
  } // end IF

  // enqueue a control message
  ACE_NEW_NORETURN (message_block_p,
                    ACE_Message_Block (0,                                  // size
                                       messageType_in,                     // type
                                       NULL,                               // continuation
                                       NULL,                               // data
                                       NULL,                               // buffer allocator
                                       NULL,                               // locking strategy
                                       ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                       ACE_Time_Value::zero,               // execution time
                                       ACE_Time_Value::max_time,           // deadline time
                                       NULL,                               // data block allocator
                                       NULL));                             // message allocator
  if (unlikely (!message_block_p))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate ACE_Message_Block: \"%m\", returning\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ACE_Message_Block: \"%m\", returning\n")));
    return;
  } // end IF

  result = (highPriority_in ? inherited::ungetq (message_block_p, NULL)
                            : inherited::putq (message_block_p, NULL));
  if (unlikely (result == -1))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task::putq(): \"%m\", continuing\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));

    message_block_p->release (); message_block_p = NULL;
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType>
bool
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  LockType>::hasShutDown ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::hasShutDown"));

  // sanity check(s)
  if (unlikely (!inherited::msg_queue_))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: task has no message queue, returning\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("task has no message queue, returning\n")));
    return false; // *TODO*: avoid false negative
  } // end IF

  bool result = false;
  ACE_Message_Block* message_block_p = NULL;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, inherited::msg_queue_->lock (), false);
    for (MESSAGE_QUEUE_ITERATOR_T iterator (*inherited::msg_queue_);
         iterator.next (message_block_p);
         iterator.advance ())
    { ACE_ASSERT (message_block_p);
      if (unlikely (message_block_p->msg_type () == ACE_Message_Block::MB_STOP))
      {
        result = true;
        break;
      } // end IF
      message_block_p = NULL;
    } // end FOR
  } // end lock scope

  return result;
}
