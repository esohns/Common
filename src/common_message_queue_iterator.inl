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
          class TIME_POLICY>
Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                              TIME_POLICY>::Common_MessageQueueIterator_T (MESSAGE_QUEUE_T& queue_in)
 : inherited (queue_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::Common_MessageQueueIterator_T"));

}

template <ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                              TIME_POLICY>::next (ACE_Message_Block*& message_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::next"));

  if (likely (inherited::curr_))
  {
    message_out = inherited::curr_;
    return 1;
  } // end IF

  return 0;
}

template <ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                              TIME_POLICY>::advance (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::advance"));

  if (likely (inherited::curr_))
    inherited::curr_ = inherited::curr_->next ();

  return (inherited::curr_ ? 1 : 0);
}

//////////////////////////////////////////

template <typename MessageType,
          ACE_SYNCH_DECL,
          class TIME_POLICY>
Common_MessageQueueExIterator_T<MessageType,
                                ACE_SYNCH_USE,
                                TIME_POLICY>::Common_MessageQueueExIterator_T (MESSAGE_QUEUE_T& queue_in)
 : inherited (queue_in.queue ())
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueExIterator_T::Common_MessageQueueExIterator_T"));

}

template <typename MessageType,
          ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueExIterator_T<MessageType,
                                ACE_SYNCH_USE,
                                TIME_POLICY>::next (MessageType*& message_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueExIterator_T::next"));

  if (likely (inherited::curr_))
  {
    message_out = reinterpret_cast<MessageType*> (inherited::curr_->base ());
    return 1;
  } // end IF

  return 0;
}

template <typename MessageType,
          ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueExIterator_T<MessageType,
                                ACE_SYNCH_USE,
                                TIME_POLICY>::advance (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueExIterator_T::advance"));

  if (likely (inherited::curr_))
    inherited::curr_ = inherited::curr_->next ();

  return (inherited::curr_ ? 1 : 0);
}
