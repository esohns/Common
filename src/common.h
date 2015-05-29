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

#ifndef COMMON_H
#define COMMON_H

#include <deque>
#include <map>
#include <string>

#include "ace/Signal.h"

#include "common_defines.h"

enum Common_TimerMode_t
{
  COMMON_TIMER_MODE_PROACTOR = 0,
  COMMON_TIMER_MODE_QUEUE,
  COMMON_TIMER_MODE_REACTOR,
  COMMON_TIMER_MODE_SIGNAL,
  /////////////////////////////////////
  COMMON_TIMER_MODE_MAX,
  COMMON_TIMER_MODE_INVALID
};

enum Common_TimerQueueType_t
{
  COMMON_TIMER_QUEUE_HEAP = 0,
  COMMON_TIMER_QUEUE_LIST,
  COMMON_TIMER_QUEUE_WHEEL,
  ///////////////////////////////////////
  COMMON_TIMER_QUEUE_MAX,
  COMMON_TIMER_QUEUE_INVALID
};

struct Common_TimerConfiguration_t
{
  inline Common_TimerConfiguration_t ()
   : mode (COMMON_TIMER_MANAGER_DEFAULT_MODE)
   , queueType (COMMON_TIMER_MANAGER_DEFAULT_QUEUE)
  {};

  Common_TimerMode_t      mode;
  Common_TimerQueueType_t queueType;
};

// *** signals ***
typedef std::map<int, ACE_Sig_Action> Common_SignalActions_t;
typedef Common_SignalActions_t::const_iterator Common_SignalActionsIterator_t;

// *** event dispatch
enum Common_Proactor_t
{
  COMMON_EVENT_PROACTOR_DEFAULT = 0, // platform-specific
  COMMON_EVENT_PROACTOR_POSIX_AIOCB, // POSIX only !
  COMMON_EVENT_PROACTOR_POSIX_SIG,   // POSIX only !
  COMMON_EVENT_PROACTOR_POSIX_SUN,   // POSIX only !
  COMMON_EVENT_PROACTOR_POSIX_CB,    // POSIX only !
  ///////////////////////////////////////
  COMMON_EVENT_PROACTOR_MAX,
  COMMON_EVENT_PROACTOR_INVALID
};

enum Common_Reactor_t
{
  COMMON_EVENT_REACTOR_DEFAULT = 0, // platform-specific
  COMMON_EVENT_REACTOR_DEV_POLL,    // POSIX only !
  COMMON_EVENT_REACTOR_SELECT,
  COMMON_EVENT_REACTOR_TP,
  COMMON_EVENT_REACTOR_WFMO,        // Win32 only !
  ///////////////////////////////////////
  COMMON_EVENT_REACTOR_MAX,
  COMMON_EVENT_REACTOR_INVALID
};

// *** log ***
typedef std::deque<std::string> Common_MessageStack_t;
typedef Common_MessageStack_t::const_iterator Common_MessageStackConstIterator_t;
//typedef std::deque<ACE_Log_Record> Common_LogRecordStack_t;

#endif
