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
#include "stdafx.h"

#include "common_referencecounter_base.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

Common_ReferenceCounterBase::Common_ReferenceCounterBase (unsigned int initialCount_in)
 : counter_ (initialCount_in)
 , condition_ (lock_)
 , deleteOnZero_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase (const Common_ReferenceCounterBase& counter_in)
 : counter_ (counter_in.counter_)
 , condition_ (counter_in.lock_) // *NOTE*: uses the same lock
 , deleteOnZero_ (counter_in.deleteOnZero_)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::~Common_ReferenceCounterBase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::~Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase ()
 : counter_ (1)
 , condition_ (lock_)
 , deleteOnZero_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase (unsigned int initialCount_in,
                                                          bool deleteOnZero_in)
 : counter_ (initialCount_in)
 , condition_ (lock_)
 , deleteOnZero_ (deleteOnZero_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase&
Common_ReferenceCounterBase::operator= (const Common_ReferenceCounterBase& rhs)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::operator="));

  counter_ = rhs.counter_;
  // *TODO*: this will not work; using cl.exe there is a compiler problem with
  //         private member access however - fix this (check usecases first)
  //condition_.mutex_ = rhs.lock_;
  deleteOnZero_ = rhs.deleteOnZero_;

  return *this;
}

unsigned int
Common_ReferenceCounterBase::increase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::increase"));

  //ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);
  //ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);
  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

  return ++counter_;
}

unsigned int
Common_ReferenceCounterBase::decrease ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::decrease"));

  unsigned int result = 0;
  bool destroy = false;

  // synch access
  {
    //ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);
    //ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    result = --counter_;

    // awaken any waiters...
    if (counter_ == 0)
    {
      // (final) signal
      int result_2 = condition_.broadcast ();
      if (result_2 == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\", continuing\n")));

      destroy = deleteOnZero_;
    } // end IF
  } // end lock scope

  if (destroy)
    delete this;

  return result;
}

unsigned int
Common_ReferenceCounterBase::count () const
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::count"));

  //ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);
  //ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);
  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

  return counter_;
}

void
Common_ReferenceCounterBase::wait (unsigned int count_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::wait"));

  int result = -1;

  {
    //ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (lock_);
    //ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    while (counter_ != count_in)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("waiting (count: %u)...\n"),
                  counter_));

      result = condition_.wait ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Condition::wait(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("leaving...\n")));
}
