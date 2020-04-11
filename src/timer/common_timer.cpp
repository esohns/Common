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

#include "common_timer.h"

#include <chrono>

#include "common_macros.h"

//#if defined (__GNUC__) && defined (__GNUC_MINOR__) && (((__GNUC__) * 100) + (__GNUC_MINOR__)) < 407
//  typedef std::chrono::monotonic_clock Common_Clock_t;
//#else
  typedef std::chrono::steady_clock    Common_Clock_t;
//#endif // __GNUC__

const double
Common_Timer::toMilliSecondsFactor =
    1000.0 * static_cast<double> (Common_Clock_t::period::num) / static_cast<double> (Common_Clock_t::period::den);

time_t
Common_Timer::timeSinceUNIXEpoch ()
{
  return Common_Clock_t::now ().time_since_epoch ().count ();
}

//////////////////////////////////////////

Common_Timer::Common_Timer (bool start_in)
 : start_ (start_in ? Common_Timer::timeSinceUNIXEpoch ()
                    : std::numeric_limits<time_t>::min ())
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer::Common_Timer"));

}

double
Common_Timer::tocTic ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer::tocTic"));

  time_t previous, next;

  do {
    next = Common_Timer::timeSinceUNIXEpoch ();
    previous = start_;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  } while (previous != InterlockedCompareExchange64 (&start_, next, previous));
#else
  } while (previous != __sync_val_compare_and_swap (&start_, previous, next));
#endif

  return (next - previous) * Common_Timer::toMilliSecondsFactor;
}
