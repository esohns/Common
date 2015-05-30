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

#include "common_signalhandler.h"

#include <string>

#include "ace/Assert.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_isignal.h"
#include "common_macros.h"
#include "common_tools.h"

Common_SignalHandler::Common_SignalHandler (Common_ISignal* interfaceHandle_in,
                                            bool useReactor_in)
 : inherited ()
 , inherited2 (ACE_Reactor::instance (),       // default reactor
               ACE_Event_Handler::LO_PRIORITY) // priority
 , interfaceHandle_ (interfaceHandle_in)
 , useReactor_ (useReactor_in)
 , signal_ (-1)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , sigInfo_ (ACE_INVALID_HANDLE)
 , uContext_ (-1)
#else
// , sigInfo_ ()
// , uContext_ ()
#endif
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler::Common_SignalHandler"));

  // sanity check
  ACE_ASSERT (interfaceHandle_);

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ACE_OS::memset (&sigInfo_, 0, sizeof (sigInfo_));
  ACE_OS::memset (&uContext_, 0, sizeof (uContext_));
#endif
}

Common_SignalHandler::~Common_SignalHandler ()
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler::~Common_SignalHandler"));

}

int
Common_SignalHandler::handle_signal (int signal_in,
                                     siginfo_t* info_in,
                                     ucontext_t* context_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler::handle_signal"));

  // initialize return value
  int result = -1;

  // *IMPORTANT NOTE*: in signal context, most actions are forbidden, so save
  // the state and notify the reactor/proactor for callback instead (see below)

  // *TODO*: this doesn't work, future signals will overwrite this state...
  //         --> race condition
  // save state
  signal_ = signal_in;
  ACE_OS::memset (&sigInfo_, 0, sizeof (sigInfo_));
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  sigInfo_.si_handle_ = static_cast<ACE_HANDLE> (info_in);
#else
  if (info_in)
    sigInfo_ = *info_in;
#endif
  if (context_in)
    uContext_ = *context_in;

  // schedule an event (see below)
  if (useReactor_)
  {
    ACE_Reactor* reactor_p = inherited2::reactor ();
    ACE_ASSERT (reactor_p);
    result = reactor_p->notify (this,
                                ACE_Event_Handler::EXCEPT_MASK,
                                NULL);
    // *PORTABILITY*: tracing in a signal handler context is not portable
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_Reactor::notify: \"%m\", aborting\n")));
  } // end IF
  else
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    result =
        proactor_p->schedule_timer (*this,                 // event handler
                                    NULL,                  // act
                                    ACE_Time_Value::zero); // expire immediately
    // *PORTABILITY*: tracing in a signal handler context is not portable
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_Proactor::schedule_timer: \"%m\", aborting\n")));
  } // end ELSE

  return result;
}

void
Common_SignalHandler::handle_time_out (const ACE_Time_Value& time_in,
                                       const void* act_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler::handle_time_out"));

  ACE_UNUSED_ARG (time_in);
  ACE_UNUSED_ARG (act_in);

  int result = handle_exception (ACE_INVALID_HANDLE);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_SignalHandler::handle_exception(): \"%m\", continuing\n")));
}

int
Common_SignalHandler::handle_exception (ACE_HANDLE handle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler::handle_exception"));

  ACE_UNUSED_ARG (handle_in);

  std::string information;
  Common_Tools::retrieveSignalInfo (signal_,
                                    sigInfo_,
                                    &uContext_,
                                    information);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%D: received [%S]: %s\n"),
              signal_,
              ACE_TEXT (information.c_str ())));

  bool success = false;
  try
  {
    success = interfaceHandle_->handleSignal (signal_);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ISignal::handleSignal(%S): \"%m\", continuing\n"),
                signal_));
  }
  if (!success)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_ISignal::handleSignal(%S): \"%m\", continuing\n"),
                signal_));

  signal_ = -1;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  sigInfo_ = ACE_INVALID_HANDLE;
  uContext_ = -1;
#else
  ACE_OS::memset (&sigInfo_, 0, sizeof (sigInfo_));
  ACE_OS::memset (&uContext_, 0, sizeof (uContext_));
#endif

  return (success ? 0 : -1);
}
