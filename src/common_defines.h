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

// *** OS ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_OS_WIN32_SDK_VERSION(version)                   (_WIN32_WINNT && (_WIN32_WINNT >= version))
#define COMMON_OS_WIN32_TARGET_PLATFORM(version)               (WINVER && (WINVER >= version))
#elif defined (ACE_LINUX)
#define COMMON_OS_LINUX_IF_DISTRIBUTION_AT_LEAST(distribution, major,minor,micro)       \
  unsigned int major_i, minor_i, micro_i;                                               \
  if ((distribution == Common_Tools::getDistribution (major_i, minor_i, micro_i)) &&    \
      ((major_i > major)                                             ||                 \
       ((major_i == major) && minor_i > minor)                       ||                 \
       ((major_i == major) && (minor_i == minor) && (micro_i >= micro))))

// *NOTE*: as returned by ::uname(2)
#define COMMON_OS_LINUX_UNAME_STRING                           "Linux"

// Linux distributions
// *NOTE*: as returned by ::lsb_release -i
#define COMMON_OS_LSB_DEBIAN_STRING                            "Debian"
#define COMMON_OS_LSB_OPENSUSE_STRING                          "openSUSE"
#define COMMON_OS_LSB_UBUNTU_STRING                            "Ubuntu"
#endif // ACE_WIN32 || ACE_WIN64

// *** C/C++ locale ***
// *NOTE*: on UNIX, try 'locale -a', or 'localectl list-locales' to list
//         supported values
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_LOCALE_EN_US_STRING                             "en-US" // ASCII
#else
#define COMMON_LOCALE_EN_US_STRING                             "en_US.utf8" // ASCII
#endif // ACE_WIN32 || ACE_WIN64

// *** default (system-) 'locations' ***

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
#endif // ACE_WIN32 || ACE_WIN64

#define COMMON_LOCATION_CONFIGURATION_DIRECTORY                "etc"
#define COMMON_LOCATION_DATA_DIRECTORY                         "data"

// // software

/////////////////////////////////////////

// +++ compiler-specifics +++

#if defined (__GNUG__)
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX                   __attribute__ ((unused))
#else
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX
#endif // __GNUG__

/////////////////////////////////////////

// *** debug ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_DEBUG_DEBUGHEAP_DEFAULT_ENABLE                  false
#define COMMON_DEBUG_DEBUGHEAP_LOG_FILE                        "debugheap.log"
#endif // ACE_WIN32 || ACE_WIN64

// *** file ***
#define COMMON_FILE_FILENAME_BACKUP_SUFFIX                     ".bak"

// *** log ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#define COMMON_LOG_DEFAULT_DIRECTORY                 ACE_OS::getenv (COMMON_LOCATION_LOG_STORAGE_VARIABLE)
#else
#define COMMON_LOG_DEFAULT_DIRECTORY                           COMMON_LOCATION_LOG_STORAGE_DIRECTORY
#endif
#define COMMON_LOG_FILENAME_SUFFIX                             ".log"
#define COMMON_LOG_VERBOSE                                     false

// *** signals ***
#define COMMON_SIGNAL_DEFAULT_DISPATCH_MODE                    COMMON_SIGNAL_DISPATCH_SIGNAL

// *** timeouts ***
#if defined (_DEBUG)
#define COMMON_TIMEOUT_DEFAULT_THREAD_SPAWN                    200 // ms
#else
#define COMMON_TIMEOUT_DEFAULT_THREAD_SPAWN                    100 // ms
#endif // _DEBUG

// *** event loop ***
#define COMMON_EVENT_MAXIMUM_HANDLES                           ACE::max_handles ()

// "proactor"-based
#define COMMON_EVENT_PROACTOR_TYPE                             COMMON_PROACTOR_ACE_DEFAULT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#undef COMMON_EVENT_PROACTOR_TYPE
#define COMMON_EVENT_PROACTOR_TYPE                             COMMON_PROACTOR_WIN32
#endif // ACE_WIN32 || ACE_WIN64
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
#endif // ACE_WIN32 || ACE_WIN64
#define COMMON_EVENT_PROACTOR_THREAD_GROUP_ID                  101

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
#endif // ACE_WIN32 || ACE_WIN64
#define COMMON_EVENT_REACTOR_THREAD_GROUP_ID                   102

// reactor options
#define COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL            false

#define COMMON_EVENT_THREAD_NAME                               "event dispatch"
#define COMMON_EVENT_DEFAULT_DISPATCH                          COMMON_EVENT_DISPATCH_PROACTOR

// *** environment ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_DEFAULT_USER_LOGIN_BASE                         "USERNAME" // environment
#else
#define COMMON_DEFAULT_USER_LOGIN_BASE                         "LOGNAME" // environment
#endif // ACE_WIN32 || ACE_WIN64

// *** parser ***

// (f)lex/yacc/bison
#define YY_END_OF_BUFFER_CHAR                                  0 // "\0\0"
#define COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE                2
// *IMPORTANT NOTE*: scans buffers in-place (avoids a copy,
//         see: http://flex.sourceforge.net/manual/Multiple-Input-Buffers.html)
//         --> in order to use yy_scan_buffer(), the buffer needs to have been
//             prepared for usage by flex: buffers need two trailing '\0's
//             BEYOND their datas' tail byte (i.e. at positions length() + 1 and
//             length() + 2)
#define COMMON_PARSER_DEFAULT_FLEX_USE_YY_SCAN_BUFFER          true

// output more debugging information ?
#define COMMON_PARSER_DEFAULT_LEX_TRACE                        false
#define COMMON_PARSER_DEFAULT_YACC_TRACE                       false

// *** state machine ***
#define COMMON_STATEMACHINE_THREAD_GROUP_ID                    110
#define COMMON_STATEMACHINE_THREAD_NAME                        "state machine"

// *** (locally installed-) (UNIX) commands / programs ***

//#define COMMON_COMMAND_ADDUSER_STRING                          "adduser"
#define COMMON_COMMAND_GPASSWD_STRING                          "gpasswd"
#define COMMON_COMMAND_LOCATE_STRING                           "locate"
#define COMMON_COMMAND_LSB_RELEASE_STRING                      "lsb_release"
#define COMMON_COMMAND_SWITCH_LSB_RELEASE_DISTRIBUTOR_STRING   "i"
#define COMMON_COMMAND_SWITCH_LSB_RELEASE_RELEASE_STRING       "r"
#define COMMON_COMMAND_PIDOF_STRING                            "pidof"
#define COMMON_COMMAND_PIDOFPROC_STRING                        "pidofproc"
#define COMMON_COMMAND_USERMOD_STRING                          "usermod"
#define COMMON_COMMAND_WHICH_STRING                            "which"
#define COMMON_STRING_APPEND(string,string_2) \
  string += ACE_TEXT_ALWAYS_CHAR (#string_2);
#define COMMON_COMMAND_ADD_SWITCH(command,switch_) \
  command += COMMON_STRING_APPEND (command,-);     \
  command += ACE_TEXT_ALWAYS_CHAR (switch_);
#define COMMON_COMMAND_START_IN_BACKGROUND(command) \
  command += COMMON_STRING_APPEND (command, &);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
// systemd
#define COMMON_SYSTEMD_UNIT_COMMAND_RESTART                    "restart"
#define COMMON_SYSTEMD_UNIT_COMMAND_START                      "start"
#define COMMON_SYSTEMD_UNIT_COMMAND_STATUS                     "status"
#define COMMON_SYSTEMD_UNIT_COMMAND_STOP                       "stop"

#define COMMON_SYSTEMD_UNIT_IFUPDOWN                           "ifupdown.service"
#define COMMON_SYSTEMD_UNIT_NETWORKMANAGER                     "NetworkManager.service"
#define COMMON_SYSTEMD_UNIT_RESOLVED                           "systemd-resolved.service"
#define COMMON_SYSTEMD_UNIT_WPASUPPLICANT                      "wpa_supplicant.service"
#endif // ACE_WIN32 || ACE_WIN64

// *** application ***

#define COMMON_CHECK_VERSION(major,minor,micro)                                                                        \
    ((libCommon_VERSION_MAJOR > major)                                                                              || \
     ((libCommon_VERSION_MAJOR == major) && (libCommon_VERSION_MINOR > minor))                                      || \
     ((libCommon_VERSION_MAJOR == major) && (libCommon_VERSION_MINOR == minor) && (libCommon_VERSION_MICRO >= micro)))

#define COMMON_APPLICATION_THREAD_GROUP_ID                     1000

#endif
