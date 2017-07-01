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

#ifndef COMMON_ITIMER_H
#define COMMON_ITIMER_H

#include "ace/Asynch_IO.h"
#include "ace/Time_Value.h"

// forward declarations
class ACE_Event_Handler;

class Common_ITimer
{
 public:
  virtual ~Common_ITimer () {}

  // exposed interface
  // proactor version
  virtual long schedule_timer (ACE_Handler*,                                      // event handler
                               const void*,                                       // act
                               const ACE_Time_Value&,                             // delay
                               const ACE_Time_Value& = ACE_Time_Value::zero) = 0; // interval
  // *NOTE*: API adopted from ACE_Reactor_Timer_Interface
  virtual long schedule_timer (ACE_Event_Handler*,                                // event handler
                               const void*,                                       // act
                               const ACE_Time_Value&,                             // delay
                               const ACE_Time_Value& = ACE_Time_Value::zero) = 0; // interval
  virtual int reset_timer_interval (long,                       // timer id
                                    const ACE_Time_Value&) = 0; // interval
  virtual int cancel_timer (long,             // timer id
                            const void** = 0, // return value: act
                            int = 1) = 0;     // don't call handle_close()
};

#endif
