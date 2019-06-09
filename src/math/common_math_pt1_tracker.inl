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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <ACE_SYNCH_DECL>
Common_TrackerBase_T<ACE_SYNCH_USE>::Common_TrackerBase_T ()
 : isInitialized_ (false)
 , lock_ ()
 , timer_ (false)
 , x_ (NAN)
{

}

//////////////////////////////////////////

template <ACE_SYNCH_DECL>
Common_PT1Tracker_T<ACE_SYNCH_USE>&
Common_PT1Tracker_T<ACE_SYNCH_USE>::operator= (const Common_PT1Tracker_T<ACE_SYNCH_USE>& rhs_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_PT1Tracker_T::operator="));

  if (likely (this != &rhs_in))
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, inherited::lock_, *this);
    ACE_GUARD_RETURN (typename inherited::LOCK_T, aGuard_2, rhs_in.lock_, *this);
    inherited::isInitialized_ = rhs_in.isInitialized_;
    inherited::timer_ = rhs_in.timer_;
    inherited::x_ = rhs_in.x_;
    tau_ = rhs_in.tau_;
  } // end IF

  return *this;
}

template <ACE_SYNCH_DECL>
void
Common_PT1Tracker_T<ACE_SYNCH_USE>::operator() (double z_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_PT1Tracker_T::operator()(double)"));

  ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, inherited::lock_);

  // compute weighted mean based on time since last update
  if (likely (inherited::isInitialized_))
  {
    const double delta_t_d = inherited::tocTic () / 1000.0 ;
    const double ratio_d  = tau_ / (tau_ + delta_t_d);
    inherited::x_ = inherited::x_ * ratio_d + z_in * (1.0 - ratio_d);

    return;
  } // end IF

  // initialize exactly with measurement
  inherited::isInitialized_ = true;
  inherited::timer_.tic ();
  inherited::x_ = z_in;
}

template <ACE_SYNCH_DECL>
void
Common_PT1Tracker_T<ACE_SYNCH_USE>::reset (double tau_in)
{
  ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, inherited::lock_);

  if (likely (!std::isnan (tau_in)))
  {
    if (unlikely (tau_in < 0.0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid time constant (was: %f), continuing\n"),
                  tau_in));

    tau_ = tau_in;
  } // end IF

  inherited::resetBase ();
}
