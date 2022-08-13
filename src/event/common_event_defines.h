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

#ifndef COMMON_EVENT_DEFINES_H
#define COMMON_EVENT_DEFINES_H

#include "ace/config-lite.h"

#define COMMON_EVENT_MAXIMUM_HANDLES                           ACE::max_handles ()
#define COMMON_EVENT_THREAD_NAME                               "event dispatch"

#define COMMON_EVENT_DEFAULT_DISPATCH                          COMMON_EVENT_DISPATCH_PROACTOR

// "proactor"-based
#define COMMON_EVENT_PROACTOR_THREAD_GROUP_ID                  101
#define COMMON_EVENT_PROACTOR_TYPE                             COMMON_PROACTOR_ACE_DEFAULT

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_EVENT_PROACTOR_DEFAULT_TYPE                     COMMON_PROACTOR_WIN32
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#elif defined (ACE_LINUX)
// *NOTE*: currently, on Linux systems, the ACE default proactor implementation
//         is COMMON_PROACTOR_POSIX_SIG
//#define COMMON_EVENT_PROACTOR_DEFAULT_TYPE                   COMMON_PROACTOR_POSIX_CB

// *IMPORTANT NOTE*: on Linux, this proactor implementation has a
//                   (long-standing, see below) issue when used in multi-
//                   threaded applications. Apparently, current (2.19) versions
//                   of glibcs' implementation of aio_suspend are not (very)
//                   reentrant.
// "../sysdeps/pthread/aio_suspend.c:218: aio_suspend: Assertion `requestlist[cnt] != ((void *)0)' failed."
//                   This is a known issue (see also:
// https://groups.yahoo.com/neo/groups/ace-users/conversations/topics/37756).
//                   A (proper) solution would involve attention and changes to
//                   either (both) glibc (and) or ACE
//#define COMMON_EVENT_PROACTOR_DEFAULT_TYPE                   COMMON_PROACTOR_POSIX_AIOCB
#endif // (ACE_WIN32 || ACE_WIN64) || ACE_LINUX

// proactor options
// *NOTE*: parallel (!) (in-flight) operations
#define COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS             ACE_AIO_MAX_SIZE
// *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
//                   signals (see also signal(7)); these signals cannot be
//                   used in applications. ..." (see 'man 7 pthreads')
// --> on POSIX platforms, ensure that ACE_SIGRTMIN == 34 (!?)
#define COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL                    ACE_SIGRTMIN

// "reactor"-based
#define COMMON_EVENT_REACTOR_THREAD_GROUP_ID                   102
#define COMMON_EVENT_REACTOR_TYPE                              COMMON_REACTOR_ACE_DEFAULT

// reactor options
//#define COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL            false
#define COMMON_EVENT_REACTOR_DEFAULT_THREADPOOL_THREADS        3

#endif
