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

#ifndef COMMON_TIME_COMMON_H
#define COMMON_TIME_COMMON_H

#include "ace/config-lite.h"
#include "ace/Version.h"

#include "common_defines.h"

#if ((ACE_MAJOR_VERSION > 6) ||   \
     ((ACE_MAJOR_VERSION == 6) && \
      ((ACE_MINOR_VERSION > 1) || \
       ((ACE_MINOR_VERSION == 1) && (ACE_BETA_VERSION > 6)))))
#include "ace/Time_Policy.h"
// *NOTE*: (where possible) use high-resolution timestamps for accuracy and low
//         latency timers
// *IMPORTANT NOTE*: currently, the high resolution time policy is not
//                   compatible with the ACE timer queue implementations.
//                   The thread timer queue adapter uses a timed wait on a
//                   condition to sleep between scheduled timers (see:
//                   Timer_Queue_Adapters.cpp:282). Eventually, this invokes
//                   ACE_OS::cond_timedwait(), which, on WIN32 (!), invokes
//                   ACE_Time_Value::to_relative_time() (see:
//                   Time_Value.cpp:151). This call then subtracts a 'system'
//                   i.e. "wall-clock" time from a 'high-res' i.e. processor-
//                   specific "tick" (counter) time, resulting in garbage (and a
//                   tight loop of the timer queue dispatching thread)
// *TODO*: fix in ACE ASAP
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ACE_System_Time_Policy Common_TimePolicy_t;
#else
typedef ACE_HR_Time_Policy Common_TimePolicy_t;
#endif // ACE_WIN32 || ACE_WIN64
#else
#error "ACE version > 6.1.6 required"
typedef ACE_System_Time_Policy Common_TimePolicy_t;
#endif // ACE_VERSION (> 6.1.6)

#if ((ACE_MAJOR_VERSION > 6) ||   \
     ((ACE_MAJOR_VERSION == 6) && \
      ((ACE_MINOR_VERSION > 1) || \
       ((ACE_MINOR_VERSION == 1) && (ACE_BETA_VERSION > 6)))))
// *NOTE*: global time policy (supplies gettimeofday())
#if defined (__GNUG__)
//#pragma GCC diagnostic ignored "-Wunused-variable"
#endif // __GNUG__
COMMON_COMPILER_UNUSED_SYMBOL_PREFIX static Common_TimePolicy_t COMMON_TIME_POLICY;
COMMON_COMPILER_UNUSED_SYMBOL_PREFIX static ACE_Time_Value_T<ACE_HR_Time_Policy> COMMON_TIME_POLICY_HR;
#if defined (__GNUG__)
//#pragma GCC diagnostic pop
#endif // __GNUG__
#define COMMON_TIME_NOW COMMON_TIME_POLICY ()
#else
#define COMMON_TIME_NOW ACE_OS::gettimeofday ()
#endif // ACE_VERSION (> 6.1.6)

#define COMMON_TIME_NOW_UTC Common_Timer_Tools::localToUTC (COMMON_TIME_NOW)

// high-resolution time
#define COMMON_TIME_NOW_HR COMMON_TIME_POLICY_HR.now ()

#define COMMON_DATE_NOW ACE_Date_Time (COMMON_TIME_NOW)
#define COMMON_DATE_NOW_UTC ACE_Date_Time (Common_Timer_Tools::localToUTC (COMMON_TIME_NOW))

#endif
