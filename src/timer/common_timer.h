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

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <limits>

#include <time.h>

#include "ace/Global_Macros.h"

#include "common_itimer.h"

class Common_Timer
 : public Common_ITimer
{
 public:
  Common_Timer (bool = true); // start ?
  inline virtual ~Common_Timer () {}

  // implement Common_ITimer
  inline virtual void tic () { start_ = Common_Timer::timeSinceUNIXEpoch (); }
  inline virtual double toc () const { return (Common_Timer::timeSinceUNIXEpoch () - start_) * Common_Timer::toMilliSecondsFactor; }
  virtual double tocTic ();
  inline virtual bool isRunning () const { return start_ != std::numeric_limits<time_t>::min (); }
  inline virtual void reset () { start_ = std::numeric_limits<time_t>::min (); }

  static const double toMilliSecondsFactor;
  static time_t timeSinceUNIXEpoch ();

 protected:
  time_t start_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Timer ())
  ACE_UNIMPLEMENTED_FUNC (Common_Timer (const Common_Timer&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer& operator= (const Common_Timer&))
};

#endif // COMMON_TIMER_H
