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

#ifndef COMMON_SIGNALHANDLER_H
#define COMMON_SIGNALHANDLER_H

#include "ace/Asynch_IO.h"
#include "ace/Event_Handler.h"
#include "ace/Global_Macros.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"

#include "common_exports.h"

// forward declaration(s)
class Common_ISignal;

class Common_Export Common_SignalHandler
 : public ACE_Handler
 , public ACE_Event_Handler
{
 public:
  virtual ~Common_SignalHandler ();

//  // *NOTE*: proactor code: invoke handle_exception
//  virtual void handle_time_out (const ACE_Time_Value&, // target time
//                                const void* = NULL);   // act
  // *NOTE*: notifies the proactor/reactor
  virtual int handle_signal (int,                 // signal
                             siginfo_t* = NULL,   // not needed on UNIX
                             ucontext_t* = NULL); // not used

 protected:
  Common_SignalHandler (Common_ISignal*, // event handler handle
                        bool = true);    // use reactor ?

 private:
  typedef ACE_Handler inherited;
  typedef ACE_Event_Handler inherited2;

  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler ())
  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler (const Common_SignalHandler&))
  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler& operator= (const Common_SignalHandler&))

  // *NOTE*: implement specific behaviour
  virtual int handle_exception (ACE_HANDLE = ACE_INVALID_HANDLE); // handle

  Common_ISignal* interfaceHandle_;
  bool            useReactor_;
};

#endif
