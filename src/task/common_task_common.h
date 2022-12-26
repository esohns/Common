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

#ifndef COMMON_TASK_COMMON_H
#define COMMON_TASK_COMMON_H

#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common_time_common.h"

#include "common_task_defines.h"
#include "common_task_manager.h"

struct Common_Task_ManagerConfiguration
{
  Common_Task_ManagerConfiguration ()
   : maximumNumberOfConcurrentTasks (COMMON_TASK_MANAGER_DEFAULT_NUMBER_OF_CONCURRENT_TASKS)
   , visitInterval (0, COMMON_TASK_MANAGER_DEFAULT_VISIT_INTERVAL_MS * 1000)
  {};

  unsigned int   maximumNumberOfConcurrentTasks;
  ACE_Time_Value visitInterval;
};

struct Common_Task_Statistic
{
  Common_Task_Statistic ()
   : bytes (0)
   , messages (0)
   , bytesPerSecond (0.0F)
   , messagesPerSecond (0.0F)
   , timeStamp (ACE_Time_Value::zero)
  {}

  struct Common_Task_Statistic operator+= (const struct Common_Task_Statistic& rhs_in)
  {
    bytes += rhs_in.bytes;
    messages += rhs_in.messages;

    bytesPerSecond += rhs_in.bytesPerSecond;
    messagesPerSecond += rhs_in.messagesPerSecond;

    return *this;
  }

  ACE_UINT64     bytes;           // amount of processed data
  ACE_UINT32     messages;        // messages

  // (current) runtime performance
  float          bytesPerSecond;
  float          messagesPerSecond;

  ACE_Time_Value timeStamp;
};

//////////////////////////////////////////

typedef Common_Task_Manager_T<ACE_MT_SYNCH,
                              Common_TimePolicy_t,
                              struct Common_Task_Statistic,
                              struct Common_Task_ManagerConfiguration,
                              void*> Common_Task_Manager_t;
typedef typename Common_Task_Manager_t::SINGLETON_T COMMON_TASK_MANAGER_SINGLETON;

#endif
