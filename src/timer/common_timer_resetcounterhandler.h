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

#ifndef COMMON_TIMER_RESETCOUNTERHANDLER_H
#define COMMON_TIMER_RESETCOUNTERHANDLER_H

#include "ace/Global_Macros.h"

#include "common_timer_handler.h"

// forward declaration(s)
class Common_ICounter;

class Common_Timer_ResetCounterHandler
 : public Common_Timer_Handler
{
  typedef Common_Timer_Handler inherited;

 public:
  Common_Timer_ResetCounterHandler (Common_ICounter*); // interface handle
  inline virtual ~Common_Timer_ResetCounterHandler () {}

  // implement Common_ITimerHandler
  virtual void handle (const void*); // asynchronous completion token

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_ResetCounterHandler ())
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_ResetCounterHandler (const Common_Timer_ResetCounterHandler&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_ResetCounterHandler& operator= (const Common_Timer_ResetCounterHandler&))

  Common_ICounter* counter_;
};

#endif
