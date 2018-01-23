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

#include "ace/Time_Value.h"

#include "common_iget.h"
#include "common_iinitialize.h"

// forward declarations
//class ACE_Event_Handler;
//class ACE_Handler;
class Common_TimerHandler;

class Common_ITimerBase
{
 public:
  virtual long schedule_timer (Common_TimerHandler*,                              // event handler handle
                               const void*,                                       // asynchronous completion token
                               const ACE_Time_Value&,                             // expiration time (absolute)
                               const ACE_Time_Value& = ACE_Time_Value::zero) = 0; // set periodic interval ?
  virtual int cancel_timer (long,             // timer id
                            const void** = 0, // return value: asynchronous completion token
                            int = 1) = 0;     // do not (!) call handle_close() ?
  virtual int reset_timer_interval (long,                       // timer id
                                    const ACE_Time_Value&) = 0; // interval

//  virtual bool useReactor () const = 0; // ? : uses proactor
};

//////////////////////////////////////////

//// *NOTE*: see also: ACE_Reactor_Timer_Interface.h
//class Common_IReactorTimer
// : virtual public Common_ITimerBase
//{
// public:
//  virtual long schedule_timer (ACE_Event_Handler*,                                // event handler handle
//                               const void*,                                       // asynchronous completion token
//                               const ACE_Time_Value&,                             // delay
//                               const ACE_Time_Value& = ACE_Time_Value::zero) = 0; // interval
//};

//// *NOTE*: see also: ACE_Proactor.h
//class Common_IProactorTimer
// : virtual public Common_ITimerBase
//{
// public:
//  virtual long schedule_timer (ACE_Handler*,                                      // event handler handle
//                               const void*,                                       // asynchronous completion token
//                               const ACE_Time_Value&,                             // delay
//                               const ACE_Time_Value& = ACE_Time_Value::zero) = 0; // interval
//};

//////////////////////////////////////////

template <typename ConfigurationType>
class Common_ITimer_T
// : public Common_IReactorTimer
// , public Common_IProactorTimer
 : public Common_ITimerBase
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IGetR_2_T<ConfigurationType>
{};

#endif
