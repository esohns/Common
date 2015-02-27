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

#include "common_macros.h"

Common_ReferenceCounterBase::Common_ReferenceCounterBase ()
 : counter_ (1)
 , condition_ (lock_)
 , deleteOnZero_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase (unsigned int initCount_in,
                                                          bool deleteOnZero_in)
 : counter_ (initCount_in)
 , condition_ (lock_)
 , deleteOnZero_ (deleteOnZero_in)
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

Common_ReferenceCounterBase::~Common_ReferenceCounterBase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::~Common_ReferenceCounterBase"));

}

void
Common_ReferenceCounterBase::increase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::increase"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);

  counter_++;
}

void
Common_ReferenceCounterBase::decrease ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::decrease"));

  bool destroy = false;

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);

    counter_--;

    // awaken any waiters...
    if (counter_ == 0)
    {
      // final signal
      condition_.broadcast ();

      destroy = deleteOnZero_;
    } // end IF
  } // end lock scope

  if (destroy)
    delete this;
}

unsigned int
Common_ReferenceCounterBase::count ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::count"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);

  return counter_;
}

void
Common_ReferenceCounterBase::wait_zero ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::wait_zero"));

  {
    // need lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (lock_);

    while (counter_)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("waiting (count: %u)...\n"),
                  counter_));

      condition_.wait ();
    } // end WHILE
  } // end lock scope

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("leaving...\n")));
}
