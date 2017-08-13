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

#include "ace/config-lite.h"

/////////////////////////////////////////

// +++ platform-specifics +++

// ----------- hardware -----------------

// *NOTE*: address any platform specifications affecting 'user-land' (aka
//         application-) software.
//         From a C/C++ perspective, this currently includes:
//         - CPU memory layout (endianness)
//         - .../8/16/32/64/... address ('int') size
//         - (floating point-/GPU/...) co-processors ((still) not specified --> compiler flags)
//         - [not to mention: asynch/io, 'shared' memory access, 'signals', ... --> [register/memory] locking]
// [Win32 / (g)libc API]
// *NOTE*: specifically, it does not, at this time, involve:
//         - (C/G/...)PU register access

// // hardware

// ----------- software -----------------

// *** C/C++ locale ***
// *NOTE*: on UNIX, try 'locale -a', or 'localectl list-locales' to list
//         supported values
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_LOCALE_EN_US_STRING                             "en-US" // ASCII
#else
#define COMMON_LOCALE_EN_US_STRING                             "en_US.utf8" // ASCII
#endif

// *** default (system) "locations" ***

// *HISTORY*: Contrary to UNIX-based systems (?), Windows (TM) systems have not
//            (yet) implemented a consistent concept of a (system-wide)
//            (file)system storage tree (as in/see also: LSB)
// [forward/backward slash path discriminator, mount points/drive letters]

// *NOTE*: given the industrial rift (?) between hardware manufacturers and the
//         software industry, there is no apparent concept or method to exactly
//         specify a (storage) location within a computer system

// *NOTE*: the concept of 'environment variables' (see e.g. wikipedia article)
//         has been adopted by all currently supported platforms of this
//         library.
// *PORTABILIY*: this fact is used here to portably identify the default
//               location for temporary (log)files
#if defined (ACE_WIN32) || defined (ACE_WIN64)
// *TODO*: differentiate between different Windows (TM) versions
#define COMMON_LOCATION_APPLICATION_STORAGE_VARIABLE           "LOCALAPPDATA"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE             "TMP"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE_2           "TEMP"
#define COMMON_LOCATION_LOG_STORAGE_VARIABLE                   COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE
#else
#define COMMON_LOCATION_APPLICATION_STORAGE_DIRECTORY          "/usr/share"
#define COMMON_LOCATION_LOG_STORAGE_DIRECTORY                  "/var/log"
#define COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY            "/var/tmp"
#define COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY_2          "/tmp"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE             "TEMP"
#endif // defined (ACE_WIN32) || defined (ACE_WIN64)

#define COMMON_LOCATION_CONFIGURATION_DIRECTORY                "etc"
#define COMMON_LOCATION_DATA_DIRECTORY                         "data"

// // software

/////////////////////////////////////////

// +++ compiler-specifics +++

#if defined (__GNUG__)
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX                   __attribute__ ((unused))
#else
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX
#endif // defined (__GNUG__)

/////////////////////////////////////////

// *** debug ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_DEBUG_DEBUGHEAP_DEFAULT_ENABLE                  false
#define COMMON_DEBUG_DEBUGHEAP_LOG_FILE                        "debugheap.log"
#endif

// *** log ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#define COMMON_LOG_DEFAULT_DIRECTORY                 ACE_OS::getenv (COMMON_LOCATION_LOG_STORAGE_VARIABLE)
#else
#define COMMON_LOG_DEFAULT_DIRECTORY                           COMMON_LOCATION_LOG_STORAGE_DIRECTORY
#endif
#define COMMON_LOG_FILENAME_SUFFIX                             ".log"
#define COMMON_LOG_VERBOSE                                     false

// *** timers ***
#define COMMON_TIMER_DEFAULT_MODE                              COMMON_TIMER_MODE_QUEUE
#define COMMON_TIMER_DEFAULT_QUEUE                             COMMON_TIMER_QUEUE_HEAP

// *IMPORTANT NOTE*: make sure group IDs are consistent across the entire (!)
//                   application
#define COMMON_TIMER_THREAD_GROUP_ID                           100
#define COMMON_TIMER_THREAD_NAME                               "timer dispatch"
// *IMPORTANT NOTE*: currently used for (initial !) slot pre-allocation only;
//                   ultimately, the total number of available concurrent slots
//                   depends on the actual implementation
//                   --> check the code, don't rely on ACE_DEFAULT_TIMERS
//#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS    ACE_DEFAULT_TIMERS
#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS                   32768
#define COMMON_TIMER_PREALLOCATE_TIMER_SLOTS                   true

// *** event loop ***
#define COMMON_EVENT_MAXIMUM_HANDLES                           ACE::max_handles ()

#define COMMON_EVENT_THREAD_GROUP_ID                           101
#define COMMON_EVENT_THREAD_NAME                               "event dispatch"

// "proactor"-based
#define COMMON_EVENT_PROACTOR_TYPE                             COMMON_PROACTOR_ACE_DEFAULT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#undef COMMON_EVENT_PROACTOR_TYPE
#define COMMON_EVENT_PROACTOR_TYPE                             COMMON_PROACTOR_WIN32
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (__GNUC__)
// *NOTE*: currently, on Linux systems, the ACE default proactor implementation
//         is COMMON_PROACTOR_POSIX_SIG
//#undef COMMON_EVENT_PROACTOR_TYPE
//#define COMMON_EVENT_PROACTOR_TYPE                           COMMON_PROACTOR_POSIX_CB

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
//#define COMMON_EVENT_PROACTOR_TYPE                           COMMON_PROACTOR_POSIX_AIOCB
#endif // __GNUC__
#endif // defined (ACE_WIN32) || defined (ACE_WIN64)

// proactor options
// *NOTE*: parallel (!) (in-flight) operations
#define COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS             ACE_AIO_MAX_SIZE
// *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
//                   signals (see also signal(7)); these signals cannot be
//                   used in applications. ..." (see 'man 7 pthreads')
// --> on POSIX platforms, ensure that ACE_SIGRTMIN == 34 (!?)
#define COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL                    ACE_SIGRTMIN

// "reactor"-based
#define COMMON_EVENT_REACTOR_TYPE                              COMMON_REACTOR_ACE_DEFAULT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
// *CHECK*: the default reactor would be COMMON_REACTOR_SELECT on these systems
#else // --> currently: UNIX-based
// *CHECK*: the default reactor would be COMMON_REACTOR_SELECT on these systems
#undef COMMON_EVENT_REACTOR_TYPE
#define COMMON_EVENT_REACTOR_TYPE                              COMMON_REACTOR_DEV_POLL
#endif // defined (ACE_WIN32) || defined (ACE_WIN64)

// *** environment ***
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#define COMMON_DEF_USER_LOGIN_BASE                             "LOGNAME" // environment
#else
#define COMMON_DEF_USER_LOGIN_BASE                             "USERNAME" // environment
#endif // !defined (ACE_WIN32) && !defined (ACE_WIN64)

// *** parser ***
// output more debugging information ?
#define COMMON_PARSER_DEFAULT_LEX_TRACE                        false
#define COMMON_PARSER_DEFAULT_YACC_TRACE                       false

#endif
