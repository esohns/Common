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

#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/OS_Memory.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
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
          typename MessageType,
          typename QueueType,
          typename TaskType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::Common_TaskBase_T (const std::string& threadName_in,
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , closeHandles_ (false)
#endif // ACE_WIN32 || ACE_WIN64
 , deadline_ (ACE_Time_Value::zero)
 , lockActivate_ (true)
 , threadCount_ (threadCount_in)
 , threadName_ (threadName_in)
 //, lock_ ()
 , threadIds_ ()
 , condition_ (inherited::lock_)
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
          typename MessageType,
          typename QueueType,
          typename TaskType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::~Common_TaskBase_T ()
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
    wait (false); // don't wait for queue
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_hthread_t handle = ACE_INVALID_HANDLE;
  if (unlikely (closeHandles_))
    for (THREAD_IDS_ITERATOR_T iterator = threadIds_.begin ();
         iterator != threadIds_.end ();
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
          typename MessageType,
          typename QueueType,
          typename TaskType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::close"));

  // *NOTE*: this method may be invoked
  //         - by external threads shutting down the active object (arg_in: 1)
  //         - by worker thread(s) upon returning from svc() (arg_in: 0)
  switch (arg_in)
  {
    case 0:
    {
      ACE_thread_t handle = ACE_OS::thr_self ();
      // final thread ? --> clean up
      if (likely (ACE_OS::thr_equal (handle,
                                     inherited::last_thread ())))
      {
        { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, inherited::lock_, -1);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_hthread_t handle_2 = ACE_INVALID_HANDLE;
          if (unlikely (closeHandles_))
            for (THREAD_IDS_CONSTITERATOR_T iterator = threadIds_.begin ();
                 iterator != threadIds_.end ();
                 ++iterator)
            {
              handle_2 = (*iterator).handle ();
              ACE_ASSERT (handle_2 != ACE_INVALID_HANDLE);
              if (!::CloseHandle (handle_2))
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                            handle_2,
                            ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
            } // end FOR
#endif // ACE_WIN32 || ACE_WIN64
          threadIds_.clear ();
        } // end lock scope

        // *NOTE*: iff the task had several worker thread(s), there will
        //         potentially still be STOP message(s) in the queue (see below)
        //         --> remove them
        ACE_ASSERT (inherited::msg_queue_);
        int result = inherited::msg_queue_->flush ();
        if (unlikely (result == -1))
        {
          if (inherited::mod_)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to QueueType::flush(): \"%m\", continuing\n"),
                        inherited::mod_->name ()));
          else
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to QueueType::flush(): \"%m\", continuing\n")));
        } // end IF
        if (unlikely (result))
        {
          if (inherited::mod_)
            ACE_DEBUG ((LM_WARNING,
                        ACE_TEXT ("%s: flushed %u message(s)\n"),
                        inherited::mod_->name (),
                        result));
          else
            ACE_DEBUG ((LM_WARNING,
                        ACE_TEXT ("flushed %u message(s)\n"),
                        result));
        } // end IF

        inherited::msg_queue_->deactivate ();

        break;
      } // end IF
      else
      {
        { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, inherited::lock_, -1);
          for (THREAD_IDS_ITERATOR_T iterator = threadIds_.begin ();
               iterator != threadIds_.end ();
               ++iterator)
            if ((*iterator).id () == handle)
            {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
              ACE_hthread_t handle_2 = ACE_INVALID_HANDLE;
              handle_2 = (*iterator).handle ();
              ACE_ASSERT (handle_2 != ACE_INVALID_HANDLE);
              if (unlikely (closeHandles_))
                if (!::CloseHandle (handle_2))
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                              handle_2,
                              ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
              threadIds_.erase (iterator);
              break;
            } // end IF
        } // end lock scope
      } // end ELSE
      break;
    }
    case 1:
    {
      if (unlikely (inherited::thr_count_ == 0))
        break; // nothing (more) to do
      this->stop (false, // wait for completion ?
                  true); // high priority ?
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
          typename MessageType,
          typename QueueType,
          typename TaskType>
bool
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::start"));

  // sanity check(s)
  ACE_ASSERT (threadCount_);

  int result = -1;

  if (unlikely (inherited::thr_count_))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: object already active (thread pool size: %u), continuing\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("object already active (thread pool size: %u), continuing\n"),
                  inherited::thr_count_));
  } // end IF
  else
  {
    deadline_ = (timeout_in ? COMMON_TIME_NOW + *timeout_in
                            : ACE_Time_Value::zero);

    // sanity check(s)
    ACE_ASSERT (inherited::msg_queue_);

    result = inherited::msg_queue_->activate ();
    if (unlikely (result == -1))
    {
      if (inherited::mod_)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Message_Queue::activate(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
      else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end ELSE

  result = open (NULL);
  if (unlikely (result == -1))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task_Base::open(NULL): \"%m\", aborting\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Base::open(NULL): \"%m\", aborting\n")));
    result = inherited::msg_queue_->deactivate ();
    if (unlikely (result == -1))
    {
      if (inherited::mod_)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Message_Queue::deactivate(): \"%m\", continuing\n"),
                    inherited::mod_->name ()));
      else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Queue::deactivate(): \"%m\", continuing\n")));
    } // end IF
    return false;
  } // end IF
  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::idle () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::idle"));

  // sanity check(s)
  ACE_ASSERT (inherited::msg_queue_);

  ACE_Time_Value one_second (1, 0);
  int result = -1;
  size_t count = 0;
  bool has_waited = false;

  do
  {
    count = inherited::msg_queue_->message_count ();
    if (!count || inherited::msg_queue_->deactivated ())
      break;
    has_waited = true;
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s%swaiting for %u message(s)...\n"),
                (inherited::mod_ ? inherited::mod_->name () : ACE_TEXT ("")),
                (inherited::mod_ ? ACE_TEXT (": ") : ACE_TEXT ("")),
                count));

    result = ACE_OS::sleep (one_second);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &one_second));
  } while (true);

  if (has_waited)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s%swaiting for message(s)...DONE\n"),
                (inherited::mod_ ? inherited::mod_->name () : ACE_TEXT ("")),
                (inherited::mod_ ? ACE_TEXT (": ") : ACE_TEXT (""))));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::finished ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::finished"));

  int result = close (1);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_TaskBase_T::close(1): \"%m\", continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::wait (bool waitForMessageQueue_in) const
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
                  ACE_TEXT ("%s: caught exception in TaskType::wait(), returning\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((log_priority,
                  ACE_TEXT ("caught exception in TaskType::wait(), returning\n")));
  }
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TaskType::wait(): \"%m\", continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::pause () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::pause"));

  OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);

  int result = this_p->inherited::suspend ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::suspend(): \"%m\", continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::resume () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::resume"));

  OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);

  int result = this_p->inherited::resume ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::resume(): \"%m\", continuing\n")));
}

// *** BEGIN dummy stub methods ***
//template <ACE_SYNCH_DECL,
//          typename TimePolicyType,
//          typename LockType,
//          typename MessageType,
//          typename QueueType,
//          typename TaskType>
//int
//Common_TaskBase_T<ACE_SYNCH_USE,
//                  TimePolicyType,
//                  LockType,
//                  MessageType,
//                  QueueType,
//                  TaskType>::svc (void)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::svc"));

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  Common_Error_Tools::setThreadName (threadName_,
//                                     0);
//#endif // ACE_WIN32 || ACE_WIN64
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
//              ACE_TEXT (threadName_.c_str ()),
//              inherited::grp_id_));
//#endif // _DEBUG

//  MessageType* message_p = NULL;
//  int result = -1;
//  int error = -1;
//  int result_2 = -1;

//  do
//  {
//    result = inherited::getq (message_p, NULL);
//    if (unlikely (result == -1))
//    {
//      error = ACE_OS::last_error ();
//      if (error != ESHUTDOWN)
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("(%s): worker thread (id: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
//                    ACE_TEXT (threadName_.c_str ())));
//      else
//        result = 0; // OK, queue has been close()d
//      break;
//    } // end IF
//    ACE_ASSERT (message_p);
//    if (unlikely (message_p->msg_type () == ACE_Message_Block::MB_STOP))
//    {
//      result = 0;
//      if (inherited::thr_count_ > 1)
//      {
//        result_2 = inherited::putq (message_p, NULL);
//        if (result_2 == -1)
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("(%s): failed to ACE_Task::putq(): \"%m\", aborting\n"),
//                      ACE_TEXT (threadName_.c_str ())));
//          result = -1;
//        } // end IF
//        message_p = NULL;
//      } // end IF
//      // clean up ?
//      if (message_p)
//      {
//        message_p->release (); message_p = NULL;
//      } // end IF
//      break; // done
//    } // end IF

//    // do nothing...

//    // clean up
//    message_p->release (); message_p = NULL;
//  } while (true);

//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
//              ACE_TEXT (threadName_.c_str ())));
//#endif // _DEBUG

//  return result;
//}
// *** END dummy stub methods ***

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::open (void* args_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::open"));

  ACE_UNUSED_ARG (args_in);

  // sanity check(s)
  // *NOTE*: when 'this' is part of an ACE_Module, control may (!) reach here in
  //         ACE_Stream::push_module()
  if (!threadCount_)
    return 0; // nothing to do here
  if (unlikely (inherited::thr_count_ > 0))
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: object already active (thread pool size: %u), continuing\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("object already active (thread pool size: %u), continuing\n"),
                  inherited::thr_count_));
  } // end IF

  ACE_thread_t* thread_ids_p = NULL;
  ACE_hthread_t* thread_handles_p = NULL;
  const char** thread_names_p = NULL;
  char* thread_name_p = NULL;
  std::string buffer;
  std::ostringstream converter;
  int result = -1;
  ACE_Thread_ID thread_id;
  bool release_lock_b = false;

  ACE_NEW_NORETURN (thread_ids_p,
                    ACE_thread_t[threadCount_]);
  if (unlikely (!thread_ids_p))
    goto allocation_error;
  ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[threadCount_]);
  if (unlikely (!thread_handles_p))
    goto allocation_error;
  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[threadCount_]);
  if (unlikely (!thread_names_p))
    goto allocation_error;
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  for (unsigned int i = 0;
       i < threadCount_;
       i++)
  {
    thread_name_p = NULL;
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (unlikely (!thread_name_p))
    { // clean up
      for (unsigned int j = 0; j < i; j++)
        delete [] thread_names_p[j];
      goto allocation_error;
    } // end IF
    ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << (i + 1);
    buffer = threadName_;
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::strcpy (thread_name_p,
                    buffer.c_str ());
    thread_names_p[i] = thread_name_p;
  } // end FOR
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();

  // protect threadIds_ ?
  if (likely (lockActivate_))
  {
    result = inherited::lock_.acquire ();
    if (unlikely (result == -1))
    { // clean up
      for (unsigned int i = 0; i < threadCount_; i++)
        delete [] thread_names_p[i];
      goto mutex_lock_error;
    } // end IF
    release_lock_b = true;
  } // end IF

  // spawn the worker thread(s)
  // *NOTE*: calling inherited::activate would start a race condition, as
  //         it releases inherited::lock_ upon returning; there are
  //         situations where this is undesirable
  result =
    //inherited::activate ((THR_NEW_LWP      |
    activate_i ((THR_NEW_LWP |
                 THR_JOINABLE     |
                 THR_INHERIT_SCHED),             // flags
                 static_cast<int> (threadCount_), // # threads
                 1,                               // force active ?
                 ACE_DEFAULT_THREAD_PRIORITY,     // priority
                 inherited::grp_id (),            // group id (see above)
                 NULL,                            // task base
                 thread_handles_p,                // thread handle(s)
                 NULL,                            // stack(s)
                 NULL,                            // stack size(s)
                 thread_ids_p,                    // thread id(s)
                 thread_names_p);                 // thread name(s)
  if (unlikely (result == -1))
  { // clean up
    for (unsigned int i = 0; i < threadCount_; i++)
      delete [] thread_names_p[i];
    goto activate_i_error;
  } // end IF

  for (unsigned int i = 0;
       i < threadCount_;
       i++)
  {
    converter << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
              << ACE_TEXT_ALWAYS_CHAR (" ")
              << thread_ids_p[i]
              << ACE_TEXT_ALWAYS_CHAR ("\t")
              << thread_handles_p[i]
              << ACE_TEXT_ALWAYS_CHAR ("\n");

    // clean up
    delete [] thread_names_p[i]; thread_names_p[i] = NULL;

    thread_id.handle (thread_handles_p[i]);
    thread_id.id (thread_ids_p[i]);
    threadIds_.push_back (thread_id);
  } // end FOR
  if (inherited::mod_)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: (%s): spawned %u dispatch thread(s) (group id: %d, total: %u):\n%s"),
                inherited::mod_->name (),
                ACE_TEXT (threadName_.c_str ()),
                threadCount_,
                inherited::grp_id_,
                inherited::thr_count_,
                ACE_TEXT (converter.str ().c_str ())));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s): spawned %u dispatch thread(s) (group id: %d, total: %u):\n%s"),
                ACE_TEXT (threadName_.c_str ()),
                threadCount_,
                inherited::grp_id_,
                inherited::thr_count_,
                ACE_TEXT (converter.str ().c_str ())));

  goto continue_;

allocation_error:
  ACE_DEBUG ((LM_CRITICAL,
              ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  goto continue_;

mutex_lock_error:
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to ACE_Thread_Mutex::acquire(): \"%m\", aborting\n")));
  goto continue_;

activate_i_error:
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to Common_TaskBase_T::activate_i(%u): \"%m\", aborting\n"),
              threadCount_));

continue_:
  if (likely (release_lock_b))
  {
    result = inherited::lock_.release ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Mutex::release(): \"%m\", aborting\n")));
  } // end IF

  // clean up
  delete [] thread_ids_p; thread_ids_p = NULL;
  delete [] thread_handles_p; thread_handles_p = NULL;
  delete [] thread_names_p; thread_names_p = NULL;

  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::activate_i (long flags,
                                         int n_threads,
                                         int force_active,
                                         long priority,
                                         int grp_id,
                                         ACE_Task_Base* task,
                                         ACE_hthread_t thread_handles[],
                                         void* stack[],
                                         size_t stack_size[],
                                         ACE_thread_t thread_ids[],
                                         const char* thr_name[])
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::activate_i"));

  // If the task passed in is zero, we will use <this>
  if (task == 0)
    task = this;

  if (inherited::thr_count_ > 0 && force_active == 0)
    return 1; // Already active.
  else
    {
      if ((inherited::thr_count_ > 0 || grp_id == -1) &&
        inherited::grp_id_ != -1)
        // If we're joining an existing group of threads then make
        // sure to (re)use its group id.
        grp_id = inherited::grp_id_;
      else if (grp_id != -1)
        // make sure to reset the cached grp_id
        inherited::grp_id_ = -1;
      inherited::thr_count_ += n_threads;
    }

  // Use the ACE_Thread_Manager singleton if we're running as an
  // active object and the caller didn't supply us with a
  // Thread_Manager.
  if (inherited::thr_mgr_ == 0)
# if defined (ACE_THREAD_MANAGER_LACKS_STATICS)
    inherited::thr_mgr_ = ACE_THREAD_MANAGER_SINGLETON::instance ();
# else /* ! ACE_THREAD_MANAGER_LACKS_STATICS */
    inherited::thr_mgr_ = ACE_Thread_Manager::instance ();
# endif /* ACE_THREAD_MANAGER_LACKS_STATICS */

  int grp_spawned = -1;
  if (thread_ids == 0)
    // Thread Ids were not specified
    grp_spawned =
      inherited::thr_mgr_->spawn_n (n_threads,
                                    &ACE_Task_Base::svc_run,
                                    (void *) this,
                                    flags,
                                    priority,
                                    grp_id,
                                    task,
                                    thread_handles,
                                    stack,
                                    stack_size,
                                    thr_name);
  else
    // thread names were specified
    grp_spawned =
      inherited::thr_mgr_->spawn_n (thread_ids,
                                    n_threads,
                                    &ACE_Task_Base::svc_run,
                                    (void *) this,
                                    flags,
                                    priority,
                                    grp_id,
                                    stack,
                                    stack_size,
                                    thread_handles,
                                    task,
                                    thr_name);
  if (grp_spawned == -1)
    {
      // If spawn_n fails, restore original thread count.
      inherited::thr_count_ -= n_threads;
      return -1;
    }

  if (inherited::grp_id_ == -1)
    inherited::grp_id_ = grp_spawned;

#if defined (ACE_THREAD_T_IS_A_STRUCT)
  ACE_OS::memcpy( &inherited::last_thread_id_, '\0', sizeof(inherited::last_thread_id_));
#else
  inherited::last_thread_id_ = 0;    // Reset to prevent inadvertant match on ID
#endif /* ACE_THREAD_T_IS_A_STRUCT */

  return 0;
}

//////////////////////////////////////////

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::Common_TaskBase_T (const std::string& threadName_in,
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
 , deadline_ (ACE_Time_Value::zero)
 , stopped_ (false)
 , threadId_ ()
 , condition_ (inherited::lock_, NULL, NULL)
 , finished_ (false)
 //, lock_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::Common_TaskBase_T"));

  ACE_UNUSED_ARG (threadName_in);
  ACE_UNUSED_ARG (threadGroupId_in);
  ACE_UNUSED_ARG (autoStart_in);

  // sanity check(s)
  ACE_ASSERT (!threadCount_in);

  threadId_.handle (ACE_INVALID_HANDLE);
}

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
bool
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::start"));

  deadline_ = (timeout_in ? COMMON_TIME_NOW + *timeout_in : ACE_Time_Value::zero);

  threadId_.id (ACE_OS::thr_self ());
  ACE_hthread_t handle = ACE_INVALID_HANDLE;
  ACE_OS::thr_self (handle);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE process_handle = ::GetCurrentProcess ();
  if (unlikely (!::DuplicateHandle (process_handle,
                                    handle,
                                    process_handle,
                                    &handle,
                                    0,
                                    FALSE,
                                    DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to DuplicateHandle(0x%@): \"%s\", aborting\n"),
                handle,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    return false;
  } // end IF
  // *WARNING*: 'handle' needs CloseHandle()
#endif // ACE_WIN32 || ACE_WIN64
  threadId_.handle (handle);

  int result = this->svc ();

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, inherited::lock_, false);
    finished_ = true;
    int result_2 = condition_.broadcast ();
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Condition::broadcast(): \"%m\"), continuing\n")));
  } // end lock scope

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (!::CloseHandle (handle)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                handle,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
#endif // ACE_WIN32 || ACE_WIN64

  return ((result < 0) ? false : true);
}

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::stop (bool waitForCompletion_in,
                                   bool)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::stop"));

  stopped_ = true;

  if (likely (waitForCompletion_in))
    wait (true);
}

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::wait (bool) const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::wait"));

  int result = -1;
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, inherited::lock_);
    while (!finished_)
    {
      result = condition_.wait (NULL);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Condition::wait(): \"%m\"), continuing\n")));
    } // end WHILE
  } // end lock scope
}

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::pause () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::pause"));

  // sanity check(s)
  if (!isRunning ())
    return;

  int result = ACE_Thread::suspend (threadId_.handle ());
  if (unlikely (result == -1))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread::suspend(0x%@): \"%m\"), continuing\n"),
                threadId_.handle ()));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread::suspend(%d): \"%m\"), continuing\n"),
                threadId_.handle ()));
#endif // ACE_WIN32 || ACE_WIN64
}

template <typename TimePolicyType,
          typename MessageType,
          typename QueueType,
          typename TaskType>
void
Common_TaskBase_T<ACE_NULL_SYNCH,
                  TimePolicyType,
                  MessageType,
                  QueueType,
                  TaskType>::resume () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::resume"));

  int result = ACE_Thread::resume (threadId_.handle ());
  if (unlikely (result == -1))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread::resume(0x%@): \"%m\"), continuing\n"),
                threadId_.handle ()));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread::resume(%d): \"%m\"), continuing\n"),
                threadId_.handle ()));
#endif // ACE_WIN32 || ACE_WIN64
}
