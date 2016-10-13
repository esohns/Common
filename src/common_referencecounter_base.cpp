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

#include <ace/Guard_T.h>
#include <ace/Log_Msg.h>

#include "common_macros.h"

Common_ReferenceCounterBase::Common_ReferenceCounterBase (long initialCount_in)
 : inherited (initialCount_in)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase (const Common_ReferenceCounterBase& counter_in)
 : inherited (counter_in)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (counter_in.lock_, // *NOTE*: use the same lock
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (counter_in.deleteOnZero_)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::~Common_ReferenceCounterBase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::~Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase ()
 : inherited (1)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase::Common_ReferenceCounterBase (long initialCount_in,
                                                          bool deleteOnZero_in)
 : inherited (initialCount_in)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (deleteOnZero_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::Common_ReferenceCounterBase"));

}

Common_ReferenceCounterBase&
Common_ReferenceCounterBase::operator= (const Common_ReferenceCounterBase& rhs_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::operator="));

  inherited::operator= (rhs_in);

  // *TODO*: this will not work; using cl.exe there is a compiler problem with
  //         private member access however - fix this (check usecases first)
  //condition_.mutex_ = rhs_in.lock_;
  deleteOnZero_ = rhs_in.deleteOnZero_;

  return *this;
}

unsigned int
Common_ReferenceCounterBase::increase ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::increase"));

  //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

  return static_cast<unsigned int> (inherited::increment ());
}

unsigned int
Common_ReferenceCounterBase::decrease ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::decrease"));

  long result = inherited::decrement ();
  int result_2 = -1;

  // synch access
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    // awaken any waiter(s)
    result_2 = condition_.broadcast ();
    if (result_2 == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\", continuing\n")));
  } // end lock scope

  if ((result == 0) && deleteOnZero_)
    delete this;

  return static_cast<unsigned int> (result);
}

unsigned int
Common_ReferenceCounterBase::count () const
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::count"));

  //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

  return static_cast<unsigned int> (inherited::refcount_.value ());
}

void
Common_ReferenceCounterBase::wait (unsigned int count_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounterBase::wait"));

  int result = -1;

  // synch access
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    while (inherited::refcount_.value () != static_cast<long> (count_in))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("waiting (count: %u)...\n"),
                  inherited::refcount_.value ()));

      result = condition_.wait ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Condition::wait(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope
}
