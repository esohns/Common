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

#ifndef COMMON_MESSAGE_QUEUE_ITERATOR_H
#define COMMON_MESSAGE_QUEUE_ITERATOR_H

#include "ace/Global_Macros.h"
#include "ace/Message_Queue_T.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Policy.h"

// forward declarations
class ACE_Message_Block;

// *IMPORTANT NOTE*: this implementation does not grab the queues' lock in every
//                   member function call; should be faster, but needs outside
//                   lock management
template <ACE_SYNCH_DECL,
          class TIME_POLICY = ACE_System_Time_Policy>
class Common_MessageQueueIterator_T
 : public ACE_Message_Queue_Iterator<ACE_SYNCH_USE,
                                     TIME_POLICY>
{
 typedef ACE_Message_Queue_Iterator<ACE_SYNCH_USE,
                                    TIME_POLICY> inherited;

 public:
  // convenient types
  typedef ACE_Message_Queue<ACE_SYNCH_USE,
                            TIME_POLICY> MESSAGE_QUEUE_T;

  Common_MessageQueueIterator_T (MESSAGE_QUEUE_T&);
  inline virtual ~Common_MessageQueueIterator_T () {}

  int next (ACE_Message_Block*&);
  inline int done (void) const { return (!inherited::curr_ ? 1 : 0); }
  int advance (void);
//  void dump (void) const;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueIterator_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueIterator_T (const Common_MessageQueueIterator_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueIterator_T& operator= (const Common_MessageQueueIterator_T&))
};

//////////////////////////////////////////

template <typename MessageType,
          ACE_SYNCH_DECL,
          class TIME_POLICY = ACE_System_Time_Policy>
class Common_MessageQueueExIterator_T
 : public ACE_Message_Queue_Iterator<ACE_SYNCH_USE,
                                     TIME_POLICY>
{
 typedef ACE_Message_Queue_Iterator<ACE_SYNCH_USE,
                                    TIME_POLICY> inherited;

 public:
  // convenient types
  typedef ACE_Message_Queue_Ex<MessageType,
                               ACE_SYNCH_USE,
                               TIME_POLICY> MESSAGE_QUEUE_T;

  Common_MessageQueueExIterator_T (MESSAGE_QUEUE_T&);
  inline virtual ~Common_MessageQueueExIterator_T () {}

  int next (MessageType*&);
  inline int done (void) const { return (!inherited::curr_ ? 1 : 0); }
  int advance (void);
//  void dump (void) const;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueExIterator_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueExIterator_T (const Common_MessageQueueExIterator_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_MessageQueueExIterator_T& operator= (const Common_MessageQueueExIterator_T&))
};

// include template definition
#include "common_message_queue_iterator.inl"

#endif
