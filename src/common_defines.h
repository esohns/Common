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

#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H

#include "ace/ACE.h"
#include "ace/Default_Constants.h"
#include "ace/POSIX_Proactor.h"

//#include "common.h"

// *** compiler ***
#define COMMON_GCC_UNUSED_GUARD                  __attribute__ ((unused))

// *** trace log ***
// *PORTABILITY*: pathnames are not portable --> (try to) use %TEMP% on Windows
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#define COMMON_DEF_DUMP_DIR                      "/var/tmp"
#else
#define COMMON_DEF_DUMP_DIR                      "TEMP" // environment
#endif
#define COMMON_DEF_LOG_DIRECTORY                 COMMON_DEF_DUMP_DIR
#define COMMON_LOG_FILENAME_SUFFIX               ".log"
#define COMMON_LOG_VERBOSE                       false

// *** timers ***
#define COMMON_TIMER_MANAGER_DEFAULT_MODE        COMMON_TIMER_MODE_QUEUE
#define COMMON_TIMER_MANAGER_DEFAULT_QUEUE       COMMON_TIMER_QUEUE_WHEEL

// *IMPORTANT NOTE*: make sure group IDs are consistent across the entire (!)
//                   application
#define COMMON_TIMER_THREAD_GROUP_ID             100
#define COMMON_TIMER_THREAD_NAME                 "timer dispatch"
// *IMPORTANT NOTE*: currently used for (initial !) slot pre-allocation only;
//                   ultimately, the total number of available concurrent slots
//                   depends on the actual implementation
//                   --> check the code, don't rely on ACE_DEFAULT_TIMERS
//#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS    ACE_DEFAULT_TIMERS
#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS     32768
#define COMMON_TIMER_PREALLOCATE_TIMER_SLOTS     true

// *** event loop ***
#define COMMON_EVENT_DISPATCH_THREAD_GROUP_ID    101
#define COMMON_EVENT_DISPATCH_THREAD_NAME        "event dispatch"

#define COMMON_EVENT_MAXIMUM_HANDLES             ACE::max_handles ()

// reactor
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_EVENT_WINXX_USE_WFMO_REACTOR      false
#else
#define COMMON_EVENT_POSIX_USE_DEV_POLL_REACTOR  true
#endif

// proactor
// *NOTE*: the current SIG proactor does not work very well on recent Linux
//         systems (see also POSIX_Proactor.cpp:1878, flg_aio is never set)
//         This is slightly problematic, as it is the default proactor on that
//         platform (ACE_Proactor::instance ())
#define COMMON_EVENT_PROACTOR_USE_SIG            false
#define COMMON_EVENT_PROACTOR_USE_AIOCB          true

// *NOTE*: parallel (!) operations
#define COMMON_EVENT_PROACTOR_NUM_AIO_OPERATIONS ACE_AIO_MAX_SIZE
// *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
//                   signals (see also signal(7)); these signals cannot be
//                   used in applications. ..." (see 'man 7 pthreads')
// --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34
#define COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL      ACE_SIGRTMIN

// *** environment ***
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#define COMMON_DEF_USER_LOGIN_BASE               "LOGNAME" // environment
#else
#define COMMON_DEF_USER_LOGIN_BASE               "USERNAME" // environment
#endif

#endif
