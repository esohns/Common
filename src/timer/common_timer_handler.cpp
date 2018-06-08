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

#include "common_timer_handler.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"
#include "common_itimerhandler.h"

Common_TimerHandler::Common_TimerHandler (Common_ITimerHandler* handler_in,
                                          bool isOneShot_in)
 : inherited (NULL,                           // --> default reactor
              ACE_Event_Handler::LO_PRIORITY) // priority
 , inherited2 (NULL) // no proactor
 , isOneShot_ (isOneShot_in)
 , handler_ (handler_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::Common_TimerHandler"));

  // sanity check(s)
  ACE_ASSERT (handler_);
}

int
Common_TimerHandler::handle_close (ACE_HANDLE handle_in,
                                   ACE_Reactor_Mask mask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::handle_close"));

  // sanity check(s)
  ACE_ASSERT (handle_in == ACE_INVALID_HANDLE);
  ACE_ASSERT (mask_in == ACE_Event_Handler::TIMER_MASK);

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("cancelled timer (handle was: %@, mask: %u), continuing\n"),
              handle_in,
              mask_in));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("cancelled timer (handle was: %d, mask: %u), continuing\n"),
              handle_in,
              mask_in));
#endif // ACE_WIN32 || ACE_WIN64
#endif // _DEBUG

  return 0;
}

int
Common_TimerHandler::handle_timeout (const ACE_Time_Value& dipatchTime_in,
                                     const void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::handle_timeout"));

  ACE_UNUSED_ARG (dipatchTime_in);

  try {
    handler_->handle (arg_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught an exception in Common_ITimerHandler::handle(), continuing\n")));
  }

  return (isOneShot_ ? -1 : 0);
}

void
Common_TimerHandler::handle_time_out (const ACE_Time_Value& requestedTime_in,
                                      const void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::handle_time_out"));

  ACE_UNUSED_ARG (requestedTime_in);

  try {
    handler_->handle (arg_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught an exception in Common_ITimerHandler::handle(), continuing\n")));
  }
}
