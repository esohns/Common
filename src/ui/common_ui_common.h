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

#include <deque>

#include <ace/Synch_Traits.h>

#include "common.h"

enum Common_UI_Event
{
  COMMON_UI_EVENT_INVALID = -1,

  // network & data
  COMMON_UI_EVENT_NETWORK_DATA_BASE = 0x100,
  COMMON_UI_EVENT_CONNECT,
  COMMON_UI_EVENT_DATA,
  COMMON_UI_EVENT_DISCONNECT,

  // task
  COMMON_UI_EVENT_TASK_BASE         = 0x200,
  COMMON_UI_EVENT_FINISHED,
  COMMON_UI_EVENT_PAUSED,
  COMMON_UI_EVENT_STARTED,
  COMMON_UI_EVENT_STOPPED,

  // other
  COMMON_UI_EVENT_OTHER_BASE        = 0x400,
  COMMON_UI_EVENT_STATISTIC,

  // -------------------------------------
  COMMON_UI_EVENT_MAX
};

typedef std::deque<Common_UI_Event> Common_UI_Events_t;
typedef Common_UI_Events_t::const_iterator Common_UI_EventsIterator_t;

struct Common_UI_State
{
  inline Common_UI_State ()
   : eventStack ()
   , lock ()
   , logStack ()
   , logStackLock ()
  {};

  Common_UI_Events_t    eventStack;
  ACE_SYNCH_MUTEX       lock;
  Common_MessageStack_t logStack;
  ACE_SYNCH_MUTEX       logStackLock;
};

#endif
