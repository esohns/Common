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

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::Common_TaskBase_T (const std::string& threadName_in,
                                                      int threadGroupID_in,
                                                      unsigned int threadCount_in,
                                                      bool autoStart_in,

                                                      ACE_Message_Queue<TaskSynchStrategyType,
                                                                        TimePolicyType>* queue_in)
 : inherited (NULL,     // thread manager instance
              queue_in) // message queue handle
 , threadCount_ (threadCount_in)
 , threadName_ (threadName_in)
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

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::~Common_TaskBase_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::~Common_TaskBase_T"));

  int result = -1;

  // sanity check(s)
  if (inherited::thr_count_ > 0)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%d active threads in dtor --> check implementation\n"),
                inherited::thr_count_));

    result = close (1);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_TaskBase_T::close(1): \"%m\", continuing\n")));

    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::wait(): \"%m\", continuing\n")));
  } // end IF
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
Common_TaskBase_T<TaskSynchStrategyType,
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
  } // end FOR
  std::string thread_ids_string = string_stream.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) spawned %u worker thread(s) (group: %d):\n%s"),
              ACE_TEXT (threadName_.c_str ()),
              threadCount_,
              inherited::grp_id (),
              ACE_TEXT (thread_ids_string.c_str ())));

  // clean up
  delete [] thread_ids_p;
  delete [] thread_handles_p;
  delete [] thread_names_p;

  return result;
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
Common_TaskBase_T<TaskSynchStrategyType,
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
    { // check specifically for the second case...
      if (ACE_OS::thr_equal (ACE_Thread::self (),
                             inherited::last_thread ()))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count_ == 0)
        break; // nothing to do

      shutdown ();

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

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::put (ACE_Message_Block* messageBlock_in,
                                        ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::put"));

  return inherited::putq (messageBlock_in, timeout_in);
}

// *** dummy stub methods ***
template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::svc"));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker starting...\n")));

  ACE_Message_Block* message_block_p = NULL;
  int result = 0;
  while (inherited::getq (message_block_p, NULL) != -1) // blocking wait
  {
    if (!message_block_p)
      break; // ? *TODO*

    if (message_block_p->msg_type () == ACE_Message_Block::MB_STOP)
    {
      if (inherited::thr_count_ > 0)
      {
        result = inherited::putq (message_block_p, NULL);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", aborting\n")));

          // clean up
          message_block_p->release ();
        } // end IF
      } // end IF
      else
      {
        // clean up
        message_block_p->release ();
      } // end ELSE

      goto done;
    } // end IF

    // clean up
    message_block_p->release ();
    message_block_p = NULL;
  } // end WHILE
  result = -1;

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

done:
  return result;
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::module_closed (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::module_closed"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (-1);

  ACE_NOTREACHED (return -1;)
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::dump_state"));

}
// *** END dummy stub methods ***

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
Common_TaskBase_T<TaskSynchStrategyType,
                  TimePolicyType>::shutdown ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TaskBase_T::shutdown"));

  int result = -1;

  // drop a control message into the queue...
  ACE_Message_Block* message_block_p = NULL;
  ACE_NEW_NORETURN (message_block_p,
                    ACE_Message_Block (0,                                  // size
                                       ACE_Message_Block::MB_STOP,         // type
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

  result = inherited::putq (message_block_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));

    // clean up
    message_block_p->release ();
  } // end IF
}
