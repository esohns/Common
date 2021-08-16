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

#include "ace/Assert.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <ACE_SYNCH_DECL>
Common_ReferenceCounter_T<ACE_SYNCH_USE>::Common_ReferenceCounter_T (unsigned int initialCount_in)
 : inherited (static_cast<long> (initialCount_in))
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::Common_ReferenceCounter_T"));

  // sanity check(s)
  ACE_ASSERT (initialCount_in);
}

template <ACE_SYNCH_DECL>
Common_ReferenceCounter_T<ACE_SYNCH_USE>::Common_ReferenceCounter_T (const Common_ReferenceCounter_T& counter_in)
 : inherited (counter_in)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (counter_in.lock_, // *NOTE*: use the same lock
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (counter_in.deleteOnZero_)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::Common_ReferenceCounter_T"));

}

template <ACE_SYNCH_DECL>
Common_ReferenceCounter_T<ACE_SYNCH_USE>::Common_ReferenceCounter_T ()
 : inherited (1)
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::Common_ReferenceCounter_T"));

}

template <ACE_SYNCH_DECL>
Common_ReferenceCounter_T<ACE_SYNCH_USE>::Common_ReferenceCounter_T (unsigned int initialCount_in,
                                                                     bool deleteOnZero_in)
 : inherited (static_cast<long> (initialCount_in))
 , lock_ (NULL, // name
          NULL) // attributes
 , condition_ (lock_,
               USYNC_THREAD,
               NULL,
               NULL)
 , deleteOnZero_ (deleteOnZero_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::Common_ReferenceCounter_T"));

}

template <ACE_SYNCH_DECL>
Common_ReferenceCounter_T<ACE_SYNCH_USE>&
Common_ReferenceCounter_T<ACE_SYNCH_USE>::operator= (const Common_ReferenceCounter_T& rhs_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::operator="));

  inherited::operator= (rhs_in);

  // *TODO*: this will not work; using cl.exe there is a compiler problem with
  //         private member access however - fix this (check usecases first)
  //condition_.mutex_ = rhs_in.lock_;
  deleteOnZero_ = rhs_in.deleteOnZero_;

  return *this;
}

template <ACE_SYNCH_DECL>
unsigned int
Common_ReferenceCounter_T<ACE_SYNCH_USE>::decrease ()
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::decrease"));

  // sanity check(s)
  ACE_ASSERT (inherited::refcount_.value () > 0);

  long result = inherited::decrement ();
  int result_2 = -1;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, lock_, 0);
    // awaken any waiter(s)
    result_2 = condition_.broadcast ();
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_CONDITION::broadcast(): \"%m\", continuing\n")));
  } // end lock scope

  if ((result == 0) && deleteOnZero_)
    delete this;

  return static_cast<unsigned int> (result);
}

template <ACE_SYNCH_DECL>
void
Common_ReferenceCounter_T<ACE_SYNCH_USE>::wait (unsigned int count_in) const
{
  COMMON_TRACE (ACE_TEXT ("Common_ReferenceCounter_T::wait"));

  int result = -1;

  { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, lock_);
    while (inherited::refcount_.value () != static_cast<long> (count_in))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("waiting (reference count: %d)...\n"),
                  inherited::refcount_.value ()));

      result = condition_.wait ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_CONDITION::wait(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope
}
