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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
// *NOTE*: see also: OS_NS_sys_utsname.cpp:24
#define COMMON_OS_WIN32_UNAME_STRING                           "Win32"
#elif defined (ACE_LINUX)
// *NOTE*: as returned by ::uname(2)
#define COMMON_OS_LINUX_UNAME_STRING                           "Linux"

// Linux distributions
// *NOTE*: as returned by ::lsb_release -i
#define COMMON_OS_LSB_DEBIAN_STRING                            "Debian"
#define COMMON_OS_LSB_FEDORA_STRING                            "Fedora"
#define COMMON_OS_LSB_OPENSUSE_STRING                          "openSUSE"
#define COMMON_OS_LSB_UBUNTU_STRING                            "Ubuntu"
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX

// *** C/C++ locale ***
// *NOTE*: on UNIX, try 'locale -a', or 'localectl list-locales' to list
//         supported values
#if defined (ACE_WIN32) || defined (ACE_WIN64)
// *TODO*: this is probably wrong, but "en-US" isn't ASCII, and "en-US.037"
//         throws...
#define COMMON_LOCALE_EN_US_ASCII_STRING                       "C"
#else
// *TODO*: this is probably wrong, but "en-US" isn't ASCII, and "en-US.US-ASCII"
//         throws...
#define COMMON_LOCALE_EN_US_ASCII_STRING                       "C"
#endif // ACE_WIN32 || ACE_WIN64

// *** default (storage) 'locations' ***
// *NOTE*: given the industrial collaboration between hardware manufacturers and
//         the emerging software industry, there may be no generic concept or
//         standardized method to specify a (storage) location within a
//         computer system, so this remains a paradigmatic rift
// *NOTE*: traditionally, storage paths derive of 'local' file systems (or
//         'directories') (i.e. from 'archive' (magnetic) tape|floppy|hard- disk
//         drives or (optical) disk media) organized side-by-side (e.g. Windows
//         (TM)) or in one logical tree (e.g. UNIX (see also: LSB)).
//         For reasons yet unknown, there appears to be a trend toward more
//         distributed (as in 'remote') data storage concepts, so nowadays these
//         might be either (physical) (hot-)plug-in ((CPU) cache) RAM|'flash'
//         memory cards, or (logical) database-, LAN- or even internet-URLs,
//         to be interpreted by some (networked-) generic abstract data access
//         layer
// [forward/backward slash path discriminator, mount points/drive letters]

// *NOTE*: the concept of 'environment variables' (see e.g. wikipedia article)
//         has been adopted by all currently supported platforms of this
//         library
// *PORTABILIY*: this fact is used here to portably identify the default
//               location for temporary (log)files
#if defined (ACE_WIN32) || defined (ACE_WIN64)
// *TODO*: differentiate between different Windows (TM) versions
#define COMMON_LOCATION_APPLICATION_STORAGE_VARIABLE           "LOCALAPPDATA"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE             "TMP"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE_2           "TEMP"
#define COMMON_LOCATION_LOG_STORAGE_VARIABLE                   COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE
#else
//#define COMMON_LOCATION_APPLICATION_STORAGE_ROOT_DIRECTORY     "/var"
#define COMMON_LOCATION_APPLICATION_STORAGE_ROOT_DIRECTORY     "/usr"
#define COMMON_LOCATION_APPLICATION_STORAGE_LOCAL_SUBDIRECTORY "local"
#define COMMON_LOCATION_APPLICATION_STORAGE_SUBDIRECTORY       "share"
#define COMMON_LOCATION_LOG_STORAGE_DIRECTORY                  "/var/log"
#define COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY            "/var/tmp"
#define COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY_2          "/tmp"
#define COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE             "TEMP"
#define COMMON_LOCATION_DOWNLOAD_STORAGE_SUBDIRECTORY          "Downloads" // *TODO*: Fedora-specific ?
#endif // ACE_WIN32 || ACE_WIN64

#define COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY             "etc"
#define COMMON_LOCATION_DATA_SUBDIRECTORY                      "share"
#define COMMON_LOCATION_PARENT_SUBDIRECTORY                    ".."
#define COMMON_LOCATION_SCRIPTS_SUBDIRECTORY                   "scripts"
#define COMMON_LOCATION_SOURCE_SUBDIRECTORY                    "src"
#define COMMON_LOCATION_TEST_I_SUBDIRECTORY                    "test_i"
#define COMMON_LOCATION_TEST_U_SUBDIRECTORY                    "test_u"

// // software

/////////////////////////////////////////

// +++ compiler-specifics +++

#if defined (__GNUG__)
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX                   __attribute__ ((unused))
#else
#define COMMON_COMPILER_UNUSED_SYMBOL_PREFIX
#endif // __GNUG__

/////////////////////////////////////////

// *** file ***
#define COMMON_FILE_FILENAME_BACKUP_SUFFIX                     ".bak"

// *** log ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#define COMMON_LOG_DEFAULT_DIRECTORY                           ACE_OS::getenv (COMMON_LOCATION_LOG_STORAGE_VARIABLE)
#else
#define COMMON_LOG_DEFAULT_DIRECTORY                           COMMON_LOCATION_LOG_STORAGE_DIRECTORY
#endif // ACE_WIN32 || ACE_WIN64
#define COMMON_LOG_FILENAME_SUFFIX                             ".log"
#define COMMON_LOG_VERBOSE                                     false

// *** threads ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#define COMMON_THREAD_PTHREAD_NAME_MAX_LENGTH                  16 // includes '\0' (!)
#endif // ACE_WIN32 || ACE_WIN64

// *** timeouts ***
#if defined (_DEBUG)
#define COMMON_TIMEOUT_DEFAULT_THREAD_SPAWN                    200 // ms
#else
#define COMMON_TIMEOUT_DEFAULT_THREAD_SPAWN                    100 // ms
#endif // _DEBUG

// *** (miscellaneous) environment ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_ENVIRONMENT_USER_LOGIN_BASE                     "USERNAME" // environment
#else
#define COMMON_ENVIRONMENT_USER_LOGIN_BASE                     "LOGNAME" // environment
#endif // ACE_WIN32 || ACE_WIN64
#define COMMON_ENVIRONMENT_DIRECTORY_ROOT_ACE                  "ACE_ROOT"
#define COMMON_ENVIRONMENT_DIRECTORY_ROOT_LIB                  "LIB_ROOT"
#define COMMON_ENVIRONMENT_DIRECTORY_ROOT_PROJECTS             "PRJ_ROOT"

// *** state machine ***
#define COMMON_STATEMACHINE_THREAD_GROUP_ID                    110
#define COMMON_STATEMACHINE_THREAD_NAME                        "state machine"

// *** (locally installed-) (UNIX) commands / programs ***

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_COMMAND_TASKKILL                                "taskkill.exe"
#define COMMON_COMMAND_TASKLIST                                "tasklist.exe"
#else
#define COMMON_COMMAND_FIND                                    "find"
#define COMMON_COMMAND_GPASSWD                                 "gpasswd"
#define COMMON_COMMAND_HWINFO                                  "hwinfo"
#define COMMON_COMMAND_SWITCH_HWINFO_DISPLAY                   "display"
#define COMMON_COMMAND_SWITCH_HWINFO_MONITOR                   "monitor"
#define COMMON_COMMAND_SWITCH_HWINFO_SHORT                     "short"
#define COMMON_COMMAND_KILL                                    "kill"
#define COMMON_COMMAND_LOCATE                                  "locate"
#define COMMON_COMMAND_LSB_RELEASE                             "lsb_release"
#define COMMON_COMMAND_SWITCH_LSB_RELEASE_DISTRIBUTOR          "i"
#define COMMON_COMMAND_SWITCH_LSB_RELEASE_RELEASE              "r"
#define COMMON_COMMAND_PIDOF                                   "pidof"
#define COMMON_COMMAND_PIDOFPROC                               "pidofproc"
#define COMMON_COMMAND_RESIZE                                  "resize"
#define COMMON_COMMAND_SWITCH_RESIZE_SUN_SET                   "s"
#define COMMON_COMMAND_USERMOD                                 "usermod"
#define COMMON_COMMAND_WHICH                                   "which"
#define COMMON_COMMAND_XARGS                                   "xargs"
#define COMMON_COMMAND_XDOTOOL                                 "xdotool"
#define COMMON_COMMAND_XRANDR                                  "xrandr"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
// registry
#define COMMON_WIN32_REGISTRY_RESOLVE_RETRIES                  3

// COM
#define COMMON_WIN32_COM_INITIALIZATION_DEFAULT_FLAGS          COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY
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

#define COMMON_APPLICATION_THREAD_GROUP_ID                     1000

// random numbers
//#define COMMON_APPLICATION_RNG_ENGINE                          std::mt19937_64
#define COMMON_APPLICATION_RNG_ENGINE                          std::default_random_engine
#define COMMON_APPLICATION_RNG_ENGINE_DEFAULT_STATE_SIZE       200

#endif
