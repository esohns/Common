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

#ifndef COMMON_UI_COMMON_H
#define COMMON_UI_COMMON_H

//#include <vector>

#include "ace/Containers_T.h"
#include "ace/Synch_Traits.h"

#include "common.h"

//enum Common_UI_EventType : int
enum Common_UI_EventType
{
  COMMON_UI_EVENT_INVALID              = -1,

  // message & data
  COMMON_UI_EVENT_MESSAGE_DATA_BASE    = 0x100,
  COMMON_UI_EVENT_CONTROL,
  COMMON_UI_EVENT_DATA,
  COMMON_UI_EVENT_SESSION,

  // task
  COMMON_UI_EVENT_TASK_BASE            = 0x200,
  COMMON_UI_EVENT_FINISHED,
  COMMON_UI_EVENT_PAUSED,
  COMMON_UI_EVENT_RESET,
  COMMON_UI_EVENT_STARTED,
  COMMON_UI_EVENT_STOPPED,

  // network & session
  COMMON_UI_EVENT_NETWORK_SESSION_BASE = 0x400,
  COMMON_UI_EVENT_CONNECT,
  COMMON_UI_EVENT_DISCONNECT,
  ////////////////////////////////////////
  COMMON_UI_EVENT_STATISTIC,

  // other & user
  COMMON_UI_EVENT_OTHER_USER_BASE      = 0x800,

  // -------------------------------------
  COMMON_UI_EVENT_MAX
};
typedef ACE_Unbounded_Stack<enum Common_UI_EventType> Common_UI_Events_t;
typedef typename ACE_Unbounded_Stack<enum Common_UI_EventType>::ITERATOR Common_UI_EventsIterator_t;

struct Common_UI_State
{
  Common_UI_State ()
   : eventStack (NULL)
   , lock ()
   , logStack ()
   , logStackLock ()
   , subscribersLock ()
  {};

  Common_UI_Events_t        eventStack;
  ACE_SYNCH_MUTEX           lock;
  Common_MessageStack_t     logStack;
  ACE_SYNCH_MUTEX           logStackLock;
  ACE_SYNCH_RECURSIVE_MUTEX subscribersLock;
};

#endif
