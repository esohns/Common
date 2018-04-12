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
          typename TimePolicyType,
          typename LockType,
          typename MessageType>
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType,
              LockType,
              MessageType>::Common_Task_T (const std::string& threadName_in,
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

// *** BEGIN dummy stub methods ***
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType,
          typename MessageType>
void
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType,
              LockType,
              MessageType>::handle (MessageType*& message_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::handle"));

  // sanity check(s)
  ACE_ASSERT (message_inout);

  message_inout->release ();
  message_inout = NULL;
}
// *** END dummy stub methods ***

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType,
          typename MessageType>
int
Common_Task_T<ACE_SYNCH_USE,
              TimePolicyType,
              LockType,
              MessageType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_T::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::setThreadName (inherited::threadName_,
                               -1);
#endif // ACE_WIN32 || ACE_WIN64
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
              ACE_TEXT (inherited::threadName_.c_str ()),
              inherited::grp_id_));
#endif // _DEBUG

  MessageType* message_p = NULL;
  ACE_Message_Block::ACE_Message_Type message_type;
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
      break;
    } // end IF
    ACE_ASSERT (message_p);

    message_type = message_p->msg_type ();
    if (unlikely (message_type == ACE_Message_Block::MB_STOP))
    {
      if (inherited::thr_count_ > 1)
      {
        result_2 = inherited::putq (message_p, NULL);
        if (result_2 == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("(%s): failed to ACE_Task::putq(): \"%m\", aborting\n"),
                      ACE_TEXT (inherited::threadName_.c_str ())));
      } // end IF
      else
        message_p->release ();
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
      message_p->release ();
      message_p = NULL;
    } // end IF
  } while (true);

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (inherited::threadName_.c_str ())));
#endif

  return result;
}
