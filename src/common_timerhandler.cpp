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

#include "common_timerhandler.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"
#include "common_itimer.h"

Common_TimerHandler::Common_TimerHandler (Common_ITimer* interfaceHandle_in,
                                          bool isOneShot_in)
 : inherited (NULL,                           // no reactor
              ACE_Event_Handler::LO_PRIORITY) // priority
 , interfaceHandle_ (interfaceHandle_in)
 , isOneShot_ (isOneShot_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::Common_TimerHandler"));

}

Common_TimerHandler::~Common_TimerHandler ()
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::~Common_TimerHandler"));

}

int
Common_TimerHandler::handle_timeout (const ACE_Time_Value& tv_in,
                                     const void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_TimerHandler::handle_timeout"));

  ACE_UNUSED_ARG (tv_in);
  ACE_UNUSED_ARG (arg_in);

  try
  {
    interfaceHandle_->handleTimeout (arg_in);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught an exception in Common_ITimer::handleTimeout(), continuing\n")));
  }

  return (isOneShot_ ? -1 : 0);
}
