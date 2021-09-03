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

#ifndef COMMON_PT1_TRACKER_H
#define COMMON_PT1_TRACKER_H

#include <cmath>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_timer.h"

template <ACE_SYNCH_DECL>
class Common_TrackerBase_T
{
 public:
  // convenient types
  typedef ACE_SYNCH_MUTEX_T LOCK_T;

  inline virtual ~Common_TrackerBase_T () {}

 protected:
  Common_TrackerBase_T ();

  inline void resetBase () { isInitialized_ = false; timer_.reset (); x_ = NAN; }

  mutable bool              isInitialized_;
  mutable ACE_SYNCH_MUTEX_T lock_;
  mutable Common_Timer      timer_;
  mutable double            x_;
};

template <ACE_SYNCH_DECL>
class Common_PT1Tracker_T
 : public Common_TrackerBase_T<ACE_SYNCH_USE>
{
 typedef Common_TrackerBase_T<ACE_SYNCH_USE> inherited;

 public:
  // convenient types
//  typedef Common_PT1Tracker_T<ACE_SYNCH_USE> OWN_TYPE_T;

  inline Common_PT1Tracker_T (double tau_in = 1.0) { reset (tau_in); }
  inline Common_PT1Tracker_T (const Common_PT1Tracker_T& that_in) { *this = that_in; }
  inline virtual ~Common_PT1Tracker_T () {}

  Common_PT1Tracker_T<ACE_SYNCH_USE>& operator= (const Common_PT1Tracker_T<ACE_SYNCH_USE>&);
  void operator() (double); // z

  void reset (double = NAN); // tau

 protected:
  double tau_; // PT1 time constant

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_PT1Tracker_T ())
};

// include template definitions
#include "common_math_pt1_tracker.inl"

#endif // COMMON_PT1_TRACKER_H
