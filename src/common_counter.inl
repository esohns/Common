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

#include "common_macros.h"

template <ACE_SYNCH_DECL>
Common_Counter_T<ACE_SYNCH_USE>::Common_Counter_T (unsigned int initialCount_in)
 : inherited (initialCount_in, false)
 , initialCount_ (initialCount_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter_T::Common_Counter_T"));

}

template <ACE_SYNCH_DECL>
void
Common_Counter_T<ACE_SYNCH_USE>::Common_Counter_T::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter_T::reset"));

  { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, inherited::lock_);
    unsigned int count_i = inherited::count ();
    unsigned int iterations_i =
      ((initialCount_ > count_i) ? initialCount_ - count_i
                                 : count_i - initialCount_);

    if (count_i > initialCount_)
      for (unsigned int i = 0;
           i < iterations_i;
           ++i)
        inherited::decrement ();
    else
      for (unsigned int i = 0;
           i < iterations_i;
           ++i)
        inherited::increment ();
  } // end lock scope
}
