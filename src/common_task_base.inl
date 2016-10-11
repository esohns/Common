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
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_macros.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_tools.h"
#endif

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::Common_TaskBase_T (const std::string& threadName_in,
                                                      int threadGroupID_in,
                                                      unsigned int threadCount_in,
                                                      bool autoStart_in,
                                                      ACE_Message_Queue<ACE_SYNCH_USE,
                                                                        TimePolicyType>* queue_in)
 : inherited (NULL,     // thread manager instance
              queue_in) // message queue handle
 , threadCount_ (threadCount_in)
 , threadName_ (threadName_in)
 , threadIDs_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::Common_TaskBase_T"));

  // set group ID for worker thread(s)
  inherited::grp_id (threadGroupID_in);

  // auto-start ?
  if (autoStart_in)
  {
    int result = open (NULL);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::open(): \"%m\", continuing\n")));
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::~Common_TaskBase_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::~Common_TaskBase_T"));

  int result = -1;

  // sanity check(s)
  if (inherited::thr_count_ > 0)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%d active thread(s) in dtor --> check implementation\n"),
                inherited::thr_count_));

    // *WARNING*: there already may or may not be a message queue at this stage
    result = close (1);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::close(1): \"%m\", continuing\n")));

    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::wait(): \"%m\", continuing\n")));
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_hthread_t handle = ACE_INVALID_HANDLE;
  for (THREAD_IDS_ITERATOR_T iterator = threadIDs_.begin ();
       iterator != threadIDs_.end ();
       ++iterator)
  {
    handle = (*iterator).handle ();
    if (handle != ACE_INVALID_HANDLE)
      if (!::CloseHandle (handle))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                    handle,
                    ACE_TEXT (Common_Tools::error2String (::GetLastError ()).c_str ())));
  } // end FOR
#endif
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::close"));

  // *NOTE*: this method may be invoked
  //         - by an external thread closing down the active object (1)
  //         - by the worker thread which calls this after returning from
  //           svc() (0)
  //           --> in this case, this should be a NOP
  switch (arg_in)
  {
    case 0:
    { // check specifically for the second case
      if (ACE_OS::thr_equal (ACE_OS::thr_self (),
                             inherited::last_thread ()))
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_hthread_t handle = ACE_INVALID_HANDLE;
        for (THREAD_IDS_ITERATOR_T iterator = threadIDs_.begin ();
             iterator != threadIDs_.end ();
             ++iterator)
        {
          handle = (*iterator).handle ();
          if (handle != ACE_INVALID_HANDLE)
            if (!::CloseHandle (handle))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                          handle,
                          ACE_TEXT (Common_Tools::error2String (::GetLastError ()).c_str ())));
        } // end FOR
#endif
        threadIDs_.clear ();

        break;
      } // end IF

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count_ == 0)
        break; // nothing to do

      // *NOTE*: make sure there is a message queue
      if (inherited::msg_queue_)
        stop (false);
      else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("cannot signal %d worker thread(s) (no message queue) --> check implementation\n"),
                    inherited::thr_count_));

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
          typename TimePolicyType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::start"));

  // sanity check(s)
  if (inherited::thr_count_ > 0)
  {
    if (inherited::mod_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: object already active (current thread pool size: %u), returning\n"),
                  inherited::mod_->name (),
                  inherited::thr_count_));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("object already active (current thread pool size: %u), returning\n"),
                  inherited::thr_count_));
    return;
  } // end IF

  // spawn a worker thread
  ACE_thread_t* thread_ids_p = NULL;
  ACE_NEW_NORETURN (thread_ids_p,
                    ACE_thread_t[1]);
  if (!thread_ids_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), returning\n"),
                sizeof (ACE_thread_t)));
    return;
  } // end IF
  ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
  ACE_hthread_t* thread_handles_p = NULL;
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[1]);
  if (!thread_handles_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), returning\n"),
                sizeof (ACE_hthread_t)));

    // clean up
    delete [] thread_ids_p;

    return;
  } // end IF
  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  const char** thread_names_p = NULL;
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[1]);
  if (!thread_names_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), returning\n"),
                sizeof (const char*)));

    // clean up
    delete [] thread_ids_p;
    delete [] thread_handles_p;

    return;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  char* thread_name_p = NULL;
  ACE_NEW_NORETURN (thread_name_p,
                    char[BUFSIZ]);
  if (!thread_name_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), returning\n"),
                sizeof (char) * BUFSIZ));

    // clean up
    delete [] thread_ids_p;
    delete [] thread_handles_p;
    delete [] thread_names_p;

    return;
  } // end IF
  ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
  ACE_OS::strcpy (thread_name_p,
                  threadName_.c_str ());
  thread_names_p[0] = thread_name_p;

  int result =
    inherited::activate ((THR_NEW_LWP      |
                          THR_JOINABLE     |
                          THR_INHERIT_SCHED),         // flags
                         1,                           // #threads
                         0,                           // force active ?
                         ACE_DEFAULT_THREAD_PRIORITY, // priority
                         inherited::grp_id (),        // group id (see above)
                         NULL,                        // task base
                         thread_handles_p,            // thread handle(s)
                         NULL,                        // stack(s)
                         NULL,                        // stack size(s)
                         thread_ids_p,                // thread id(s)
                         thread_names_p);             // thread name(s)
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::activate(%u): \"%m\", returning\n"),
                threadCount_));

    // clean up
    delete [] thread_ids_p;
    delete [] thread_handles_p;
    delete [] thread_names_p[0];
    delete [] thread_names_p;

    return;
  } // end IF

  //std::ostringstream string_stream;
  //string_stream << thread_ids_p[0];
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("spawned a worker thread (\"%s\", id: %s, group: %d)\n"),
  //            ACE_TEXT (threadName_.c_str ()),
  //            ACE_TEXT (string_stream.str ().c_str ()),
  //            inherited::grp_id ()));
  ACE_Thread_ID thread_id;
  thread_id.handle (thread_handles_p[0]);
  thread_id.id (thread_ids_p[0]);
  threadIDs_.push_back (thread_id);

  // clean up
  delete [] thread_ids_p;
  delete [] thread_handles_p;
  delete [] thread_names_p[0];
  delete [] thread_names_p;
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::stop (bool waitForCompletion_in,
                                         bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  int result = -1;

  control (ACE_Message_Block::MB_STOP,
           true);                      // high-priority

  if (waitForCompletion_in)
  {
    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
  } // end IF
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType>
bool
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::isRunning () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::isRunning"));

  return (inherited::thr_count_ > 0);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::svc"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (ID: %t) starting...\n"),
              ACE_TEXT (threadName_.c_str ())));

  ACE_Message_Block* message_block_p = NULL;
  ACE_Message_Block::ACE_Message_Type message_type;
  int result = 1;
  int error = -1;
  int result_2 = -1;

  do
  {
    result = inherited::getq (message_block_p, NULL);
    if (result == -1)
    {
      error = ACE_OS::last_error ();
      if (error != ESHUTDOWN)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("(%s): worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
                    ACE_TEXT (threadName_.c_str ())));
      break;
    } // end IF
    ACE_ASSERT (message_block_p);

    message_type = message_block_p->msg_type ();
    if (message_type == ACE_Message_Block::MB_STOP)
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

  return result;
}

// *** BEGIN dummy stub methods ***
template <ACE_SYNCH_DECL,
          typename TimePolicyType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::module_closed (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::module_closed"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (-1);

  ACE_NOTREACHED (return -1;)
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::dump_state"));

}
// *** END dummy stub methods ***

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
int
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::open (void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::open"));

  ACE_UNUSED_ARG (arg_in);

  // sanity check(s)
  if (inherited::thr_count_ > 0) return 0; // nothing to do

  // spawn the worker thread(s)
  ACE_thread_t* thread_ids_p = NULL;
  ACE_NEW_NORETURN (thread_ids_p,
                    ACE_thread_t[threadCount_]);
  if (!thread_ids_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (ACE_thread_t) * threadCount_)));
    return -1;
  } // end IF
  ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
  ACE_hthread_t* thread_handles_p = NULL;
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[threadCount_]);
  if (!thread_handles_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (ACE_hthread_t) * threadCount_)));

    // clean up
    delete [] thread_ids_p;

    return -1;
  } // end IF
  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  const char** thread_names_p = NULL;
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[threadCount_]);
  if (!thread_names_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                (sizeof (const char*) * threadCount_)));

    // clean up
    delete [] thread_ids_p;
    delete [] thread_handles_p;

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
    if (!thread_name_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                  (sizeof (char) * BUFSIZ)));

      // clean up
      delete [] thread_ids_p;
      delete [] thread_handles_p;
      for (unsigned int j = 0; j < i; j++)
        delete [] thread_names_p[j];
      delete [] thread_names_p;

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
  int result =
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
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::activate(%u): \"%m\", aborting\n"),
                threadCount_));

    // clean up
    delete [] thread_ids_p;
    delete [] thread_handles_p;
    for (unsigned int i = 0; i < threadCount_; i++)
      delete [] thread_names_p[i];
    delete [] thread_names_p;

    return result;
  } // end IF

  std::ostringstream string_stream;
  ACE_Thread_ID thread_id;
  for (unsigned int i = 0;
       i < threadCount_;
       i++)
  {
    string_stream << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
                  << ACE_TEXT_ALWAYS_CHAR (" ")
                  << thread_ids_p[i]
                  << ACE_TEXT_ALWAYS_CHAR ("\n");

    // clean up
    delete [] thread_names_p[i];

    thread_id.handle (thread_handles_p[i]);
    thread_id.id (thread_ids_p[i]);
    threadIDs_.push_back (thread_id);
  } // end FOR
  //std::string thread_ids_string = string_stream.str ();
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("(%s) spawned %u worker thread(s) (group: %d):\n%s"),
  //            ACE_TEXT (threadName_.c_str ()),
  //            threadCount_,
  //            inherited::grp_id (),
  //            ACE_TEXT (thread_ids_string.c_str ())));

  // clean up
  delete [] thread_ids_p;
  delete [] thread_handles_p;
  delete [] thread_names_p;

  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_TaskBase_T<ACE_SYNCH_USE,
                  TimePolicyType>::control (int messageType_in,
                                            bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::control"));

  int result = -1;

  // drop a control message into the queue
  ACE_Message_Block* message_block_p = NULL;
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
  if (!message_block_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate ACE_Message_Block: \"%m\", returning\n")));
    return;
  } // end IF

  if (highPriority_in)
    result = inherited::ungetq (message_block_p, NULL);
  else
    result = inherited::putq (message_block_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));

    // clean up
    message_block_p->release ();
  } // end IF
}
