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

#include "ace/Log_Msg.h"

#include "common_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::Common_Task_T (const std::string& threadName_in,
                                              int threadGroupId_in,
                                              unsigned int threadCount_in,
                                              bool autoStart_in,
                                              typename inherited::MESSAGE_QUEUE_T* messageQueue_in)
 : inherited (threadName_in,
              threadGroupId_in,
              threadCount_in,
              autoStart_in,
              messageQueue_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::Common_Task_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::stop (bool waitForCompletion_in,
                                     bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::stop"));

  control (ACE_Message_Block::MB_STOP,
           highPriority_in);

  if (waitForCompletion_in)
    inherited::wait (true);
}

// *** BEGIN dummy stub methods ***
template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::handle (ACE_Message_Block*& message_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::handle"));

  // sanity check(s)
  ACE_ASSERT (message_inout);

  message_inout->release (); message_inout = NULL;
}
// *** END dummy stub methods ***

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
void
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::control (int messageType_in,
                                        bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::control"));

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
          typename TimePolicyType>
bool
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::isShuttingDown ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::isShuttingDown"));

  // sanity check(s)
  ACE_ASSERT (inherited::msg_queue_);

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

template <ACE_SYNCH_DECL,
          typename TimePolicyType>
int
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (inherited::threadName_,
                                     NULL);
#else
  Common_Error_Tools::setThreadName (inherited::threadName_,
                                     0);
#endif // _WIN32_WINNT_WIN10
#endif // ACE_WIN32 || ACE_WIN64
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
              ACE_TEXT (inherited::threadName_.c_str ()),
              inherited::grp_id_));

  ACE_Message_Block* message_p = NULL;
  int result = -1;
  int error = -1;
  int result_2 = -1;

  do
  {
    result = inherited::getq (message_p, NULL);
    if (unlikely (result == -1))
    {
      error = ACE_OS::last_error ();
      if (error != ESHUTDOWN)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("(%s): worker thread (id: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
                    ACE_TEXT (inherited::threadName_.c_str ())));
      else
        result = 0; // OK, queue has been close()d
      break;
    } // end IF
    ACE_ASSERT (message_p);
    if (unlikely (message_p->msg_type () == ACE_Message_Block::MB_STOP))
    {
      result = 0;
      if (inherited::thr_count_ > 1)
      {
        result_2 = inherited::putq (message_p, NULL);
        if (result_2 == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("(%s): worker thread (id: %t) failed to ACE_Task::putq(): \"%m\", aborting\n"),
                      ACE_TEXT (inherited::threadName_.c_str ())));
          result = -1;
        } // end IF
        message_p = NULL;
      } // end IF
      // clean up ?
      if (message_p)
      {
        message_p->release (); message_p = NULL;
      } // end IF
      break; // done
    } // end IF

    try {
      handle (message_p);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("(%s): caught exception in Common_ITaskHandler_T::handle(%@), continuing\n"),
                  ACE_TEXT (inherited::threadName_.c_str ()),
                  message_p));
    }
    // clean up ?
    if (unlikely (message_p))
    {
      message_p->release (); message_p = NULL;
    } // end IF
  } while (true);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (inherited::threadName_.c_str ())));

  return result;
}
