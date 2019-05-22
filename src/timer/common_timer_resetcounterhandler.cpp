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

#include "common_timer_resetcounterhandler.h"

#include "ace/Log_Msg.h"

#include "common_icounter.h"

#include "common_macros.h"

Common_Timer_ResetCounterHandler::Common_Timer_ResetCounterHandler (Common_ICounter* counter_in)
 : inherited (this,  // dispatch interface
              false) // invoke only once ?
 , counter_ (counter_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_ResetCounterHandler::Common_Timer_ResetCounterHandler"));

  // sanity check(s)
  ACE_ASSERT (counter_);
}

void
Common_Timer_ResetCounterHandler::handle (const void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_ResetCounterHandler::handle"));

  ACE_UNUSED_ARG (arg_in);

  // sanity check(s)
  ACE_ASSERT (counter_);

  try {
    counter_->reset ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught an exception in Common_ICounter::reset(), returning\n")));
    return;
  }
}
