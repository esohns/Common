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
#include "ace/config-macros.h"
#include "ace/Event_Handler.h"
#include "ace/Global_Macros.h"
#include "ace/os_include/os_ucontext.h"
#include "ace/OS_NS_signal.h"
#include "ace/Synch_Traits.h"

#include "common.h"
#include "common_iinitialize.h"
#include "common_isignal.h"

template <typename ConfigurationType>
class Common_SignalHandler_T
 : public ACE_Handler
 , public ACE_Event_Handler
 , public Common_ISignal
 , public Common_IInitialize_T<ConfigurationType>
{
  typedef ACE_Handler inherited;
  typedef ACE_Event_Handler inherited2;

 public:
  inline virtual ~Common_SignalHandler_T () {}

  // *NOTE*: the signal is dispatched according to dispatchMode_
  virtual int handle_signal (int,                 // signal
                             siginfo_t* = NULL,   // not needed on UNIX
                             ucontext_t* = NULL); // not used

  // *NOTE*: proactor code
  virtual void handle_time_out (const ACE_Time_Value&, // target time
                                const void* = NULL);   // act
  // *NOTE*: reactor code
  virtual int handle_exception (ACE_HANDLE = ACE_INVALID_HANDLE); // handle

  // implement Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&);

 protected:
  Common_SignalHandler_T (enum Common_SignalDispatchType, // dispatch mode
                          ACE_SYNCH_MUTEX*,               // lock handle
                          Common_ISignal* = NULL);        // event handler handle


  ConfigurationType*             configuration_;
  enum Common_SignalDispatchType dispatchMode_;
  bool                           isInitialized_;
  ACE_SYNCH_MUTEX*               lock_;
  Common_Signals_t               signals_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler_T (const Common_SignalHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_SignalHandler_T& operator= (const Common_SignalHandler_T&))

  // implement Common_ISignal
  inline virtual void handle (const struct Common_Signal& signal_in) { ACE_UNUSED_ARG (signal_in); ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  Common_ISignal*                callback_;
};

// include template definition
#include "common_signalhandler.inl"

#endif
