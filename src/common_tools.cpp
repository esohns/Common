﻿/***************************************************************************
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
#include "stdafx.h"

#include "ace/Synch.h"
#include "common_tools.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// *WORKAROUND*
using namespace std;
// *IMPORTANT NOTE*: several ACE headers inclue ace/iosfwd.h, which introduces
//                   a problem in conjunction with the standard include headers
//                   when ACE_USES_OLD_IOSTREAMS is defined
//                   --> include the necessary headers manually (see above), and
//                       prevent ace/iosfwd.h from causing any harm
#define ACE_IOSFWD_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <combaseapi.h>
#include <dxerr.h>
#include <errors.h>
#include <strmif.h>
#include <Security.h>
#elif defined (ACE_LINUX)
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/utsname.h>

#include <linux/capability.h>
#include <linux/prctl.h>
#include <linux/securebits.h>

#include <grp.h>
#elif defined (__sun) && defined (__SVR4)
// *NOTE*: Solaris (11)-specific
#include <rctl.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/OS.h"
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
#include "ace/OS_Memory.h"
#endif // __sun && __SVR4
#include "ace/POSIX_CB_Proactor.h"
#include "ace/POSIX_Proactor.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
#include "ace/SUN_Proactor.h"
#endif // ACE_HAS_AIO_CALLS && sun
#include "ace/Time_Value.h"
#include "ace/TP_Reactor.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/WIN32_Proactor.h"
#include "ace/WFMO_Reactor.h"
#else
#include "ace/Dev_Poll_Reactor.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_macros.h"
#include "common_time_common.h"

#if defined (DBUS_SUPPORT)
#include "common_dbus_tools.h"
#endif // DBUS_SUPPORT

// initialize statics
#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ACE_HANDLE Common_Tools::debugHeapLogFileHandle_ = ACE_INVALID_HANDLE;

int
common_tools_win32_debugheap_hook (int reportType_in,
                                   char* message_in,
                                   int* returnValue_out)
{
  COMMON_TRACE (ACE_TEXT ("::common_tools_win32_debugheap_hook"));

  // translate loglevel
  ACE_Log_Priority log_priority = LM_ERROR;
  switch (reportType_in)
  {
    case _CRT_ASSERT:
    case _CRT_ERROR:
    case _CRT_ERRCNT:
      break;
    case _CRT_WARN:
      log_priority = LM_WARNING;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown debug heap log level (was: %d), continuing"),
                  reportType_in));
      break;
    }
  } // end SWITCH

  // *WARNING*: this call holds the CRT lock. The logger waits for its lock,
  //            causing deadlock if another thread has the lock and tries to
  //            free objects to the CRT...
  ACE_DEBUG ((log_priority,
              ACE_TEXT ("debug heap: %s"),
              ACE_TEXT (message_in)));

  if (likely (returnValue_out))
    *returnValue_out = 0;

  return FALSE; // <-- do not stop
}
#endif
#endif

unsigned int Common_Tools::randomSeed_ = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
char Common_Tools::randomStateBuffer_[BUFSIZ];
#endif

//////////////////////////////////////////

void
Common_Tools::initialize (bool initializeRandomNumberGenerator_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initialize"));

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  int previous_heap_flags = -1;
  ACE_FILE_Addr file_address;
  ACE_FILE_Connector file_connector;
  ACE_FILE_IO file_IO;
  int result = -1;
  std::string package_name, file_name;
  ACE_HANDLE previous_file_handle = ACE_INVALID_HANDLE;

  if (!COMMON_DEBUG_DEBUGHEAP_DEFAULT_ENABLE)
    goto continue_;

  ACE_ASSERT (debugHeapLogFileHandle_ == ACE_INVALID_HANDLE);

  //int current_debug_heap_flags = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
  //int debug_heap_flags = current_debug_heap_flags;
  int debug_heap_flags = (_CRTDBG_ALLOC_MEM_DF      |
                          _CRTDBG_CHECK_ALWAYS_DF   |
                          _CRTDBG_CHECK_CRT_DF      |
                          _CRTDBG_DELAY_FREE_MEM_DF |
                          _CRTDBG_LEAK_CHECK_DF);
  debug_heap_flags =
    (debug_heap_flags | _CRTDBG_CHECK_EVERY_16_DF);
  // Turn off CRT block checking bit
  //debug_heap_flags &= ~_CRTDBG_CHECK_CRT_DF;
  previous_heap_flags = _CrtSetDbgFlag (debug_heap_flags);
  // output to debug window
  file_name = Common_File_Tools::getLogDirectory (package_name, 0);
  file_name += ACE_DIRECTORY_SEPARATOR_STR;
  file_name += ACE_TEXT_ALWAYS_CHAR (COMMON_DEBUG_DEBUGHEAP_LOG_FILE);
  result = file_address.set (ACE_TEXT (file_name.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (file_name.c_str ())));
    return;
  } // end IF
  result =
    file_connector.connect (file_IO,                 // stream
                            file_address,            // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            (O_RDWR  |
                             O_TEXT  |
                             O_CREAT |
                             O_TRUNC),               // flags --> open
                            ACE_DEFAULT_FILE_PERMS); // permissions --> open
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (file_name.c_str ())));
    return;
  } // end IF
  debugHeapLogFileHandle_ = file_IO.get_handle ();
  ACE_ASSERT (debugHeapLogFileHandle_ != ACE_INVALID_HANDLE);

  //result = _CrtSetReportHook2 (_CRT_RPTHOOK_INSTALL,
  //                             common_tools_win32_debugheap_hook);
  //if (result == -1)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to _CrtSetReportHook2(): \"%m\", aborting\n")));
  //  return;
  //} // end IF
  //ACE_ASSERT (result == 1);
  _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportMode (_CRT_ERROR,  _CRTDBG_MODE_FILE);
  _CrtSetReportMode (_CRT_WARN,   _CRTDBG_MODE_FILE);
  previous_file_handle =
    _CrtSetReportFile (_CRT_ASSERT, debugHeapLogFileHandle_);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
  previous_file_handle =
    _CrtSetReportFile (_CRT_ERROR,  debugHeapLogFileHandle_);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
  previous_file_handle =
    _CrtSetReportFile (_CRT_WARN,   debugHeapLogFileHandle_);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("debug heap configuration:\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n\n"),
              ACE_TEXT ("_CRTDBG_ALLOC_MEM_DF"),
              ((debug_heap_flags & _CRTDBG_ALLOC_MEM_DF) ? ACE_TEXT ("on")
                                                         : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_DELAY_FREE_MEM_DF"),
              ((debug_heap_flags & _CRTDBG_DELAY_FREE_MEM_DF) ? ACE_TEXT ("on")
                                                              : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_ALWAYS_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_ALWAYS_DF) ? ACE_TEXT ("on")
                                                            : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_RESERVED_DF"),
              ((debug_heap_flags & _CRTDBG_RESERVED_DF) ? ACE_TEXT ("on")
                                                        : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_CRT_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_CRT_DF) ? ACE_TEXT ("on")
                                                         : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_LEAK_CHECK_DF"),
              ((debug_heap_flags & _CRTDBG_LEAK_CHECK_DF) ? ACE_TEXT ("on")
                                                          : ACE_TEXT ("off"))));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("debug heap checking-frequency configuration:\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n\n"),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_16_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_16_DF) ? ACE_TEXT ("on")
                                                              : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_128_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_128_DF) ? ACE_TEXT ("on")
                                                               : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_1024_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_1024_DF) ? ACE_TEXT ("on")
                                                                : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_DEFAULT_DF"),
              (((debug_heap_flags & 0xFFFF0000) == _CRTDBG_CHECK_DEFAULT_DF) ? ACE_TEXT ("on")
                                                                             : ACE_TEXT ("off"))));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("configured debug heap (%x, log file: \"%s\")\n"),
              debug_heap_flags,
              ACE_TEXT (file_name.c_str ())));
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("configured debug heap (%x)\n"),
  //            debug_heap_flags));

continue_:
#endif /* ACE_WIN32 || ACE_WIN64 */
#endif /* _DEBUG */

#if defined (LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("running on valgrind\n")));
#endif /* LIBCOMMON_ENABLE_VALGRIND_SUPPORT */

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...\n")));
  //ACE_High_Res_Timer::calibrate (500000, 10);
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...done\n")));

  if (initializeRandomNumberGenerator_in)
  {
    // *TODO*: use STL functionality here
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing random seed (RAND_MAX: %d)...\n"),
                RAND_MAX));
    Common_Tools::randomSeed_ = COMMON_TIME_NOW.usec ();
    // *PORTABILITY*: outside glibc, this is not very portable
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_OS::srand (static_cast<u_int> (randomSeed_));
#else
    ACE_OS::memset (&Common_Tools::randomStateBuffer_,
                    0,
                    sizeof (char[BUFSIZ]));
    struct random_data random_data_s;
    ACE_OS::memset (&random_data_s, 0, sizeof (struct random_data));
    int result = ::initstate_r (Common_Tools::randomSeed_,
                                Common_Tools::randomStateBuffer_, sizeof (char[BUFSIZ]),
                                &random_data_s);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initstate_r(): \"%s\", returning\n")));
      return;
    } // end IF
    result = ::srandom_r (Common_Tools::randomSeed_, &random_data_s);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize random seed: \"%s\", returning\n")));
      return;
    } // end IF
#endif // ACE_WIN32 || ACE_WIN64
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing random seed...DONE\n")));
  } // end IF

#if defined (DBUS_SUPPORT)
  Common_DBus_Tools::initialize ();
#endif // DBUS_SUPPORT
}
void
Common_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalize"));

#if defined (DBUS_SUPPORT)
  Common_DBus_Tools::finalize ();
#endif // DBUS_SUPPORT

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!COMMON_DEBUG_DEBUGHEAP_DEFAULT_ENABLE) goto continue_;

  int result = _CrtSetReportHook2 (_CRT_RPTHOOK_REMOVE,
                                   common_tools_win32_debugheap_hook);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to _CrtSetReportHook2(): \"%m\", continuing\n")));
  else
    ACE_ASSERT (result == 0);

  if (debugHeapLogFileHandle_ != ACE_INVALID_HANDLE)
  {
    result = ACE_OS::close (debugHeapLogFileHandle_);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(%@): \"%m\", continuing\n"),
                  debugHeapLogFileHandle_));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("closed debug heap log file\n")));
    debugHeapLogFileHandle_ = ACE_INVALID_HANDLE;
  } // end IF
continue_:
#endif /* ACE_WIN32 || ACE_WIN64 */
#endif /* _DEBUG */

  return;
}

//Common_ITimer*
//Common_Tools::getTimerManager ()
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Tools::getTimerManager"));
//
//  return COMMON_TIMERMANAGER_SINGLETON::instance ();
//}

bool
Common_Tools::inDebugSession ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::inDebugSession"));

  bool result = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = IsDebuggerPresent ();
#elif defined (ACE_LINUX)
  int status_fd = ACE_INVALID_HANDLE;
  ACE_TCHAR buffer_a[BUFSIZ];
  ssize_t bytes_read = -1;
  int result_2 = -1;
  static const ACE_TCHAR tracer_pid_string[] = ACE_TEXT ("TracerPid:");
  ACE_TCHAR* tracer_pid_p = NULL;

  status_fd = ACE_OS::open (ACE_TEXT_ALWAYS_CHAR ("/proc/self/status"),
                            O_RDONLY);
  if (unlikely (status_fd == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::open(%s): \"%m\", aborting\n"),
                ACE_TEXT ("/proc/self/status")));
    return false; // *WARNING*: potentially false negative
  } // end IF
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
  bytes_read = ACE_OS::read (status_fd,
                             buffer_a,
                             sizeof (ACE_TCHAR[BUFSIZ]) - 1);
  if (unlikely (bytes_read == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::read(%s): \"%m\", aborting\n"),
                ACE_TEXT ("/proc/self/status")));
    goto clean;
  } // end IF
//  buffer_a[bytes_read] = 0;
  tracer_pid_p = ACE_OS::strstr (buffer_a, tracer_pid_string);
  if (unlikely (!tracer_pid_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strstr(%s): \"%m\", aborting\n"),
                tracer_pid_string));
    goto clean;
  } // end IF
  result = !!ACE_OS::atoi (tracer_pid_p + sizeof (tracer_pid_string) - 1);

clean:
  if (likely (status_fd != ACE_INVALID_HANDLE))
  {
    result_2 = ACE_OS::close (status_fd);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(%s): \"%m\", continuing\n"),
                  ACE_TEXT ("/proc/self/status")));
  } // end IF
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif

  return result;
}

std::string
Common_Tools::sanitizeURI (const std::string& uri_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::sanitizeURI"));

  std::string result = uri_in;

  std::replace (result.begin (),
                result.end (),
                '\\', '/');
  size_t position;
  do
  {
    position = result.find (' ', 0);
    if (position == std::string::npos)
      break;

    result.replace (position, 1, ACE_TEXT_ALWAYS_CHAR ("%20"));
  } while (true);
  //XMLCh* transcoded_string =
  //	XMLString::transcode (result.c_str (),
  //	                      XMLPlatformUtils::fgMemoryManager);
  //XMLURL url;
  //if (!XMLURL::parse (transcoded_string,
  //	                  url))
  // {
  //   ACE_DEBUG ((LM_ERROR,
  //               ACE_TEXT ("failed to XMLURL::parse(\"%s\"), aborting\n"),
  //               ACE_TEXT (result.c_str ())));

  //   return result;
  // } // end IF
  //XMLUri uri (transcoded_string,
  //	          XMLPlatformUtils::fgMemoryManager);
  //XMLString::release (&transcoded_string,
  //	                  XMLPlatformUtils::fgMemoryManager);
  //char* translated_string =
  //	XMLString::transcode (uri.getUriText(),
  //	                      XMLPlatformUtils::fgMemoryManager);
  //result = translated_string;
  //XMLString::release (&translated_string,
  //	                  XMLPlatformUtils::fgMemoryManager);

  return result;
}

std::string
Common_Tools::sanitize (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::sanitize"));

  std::string result = string_in;

  std::replace (result.begin (),
                result.end (),
                ' ', '_');

  return result;
}

std::string
Common_Tools::strip (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::strip"));

  std::string result = string_in;

  std::string::size_type position = std::string::npos;
  position =
    result.find_first_not_of (ACE_TEXT_ALWAYS_CHAR (" \t\f\v\n\r"),
                              0);
  if (unlikely (position == std::string::npos))
    result.clear (); // all whitespace
  else if (position)
    result.erase (0, position);
  position =
    result.find_last_not_of (ACE_TEXT_ALWAYS_CHAR (" \t\f\v\n\r"),
                             std::string::npos);
  if (unlikely (position == std::string::npos))
    result.clear (); // all whitespace
  else
    result.erase (position + 1, std::string::npos);

  return result;
}

bool
Common_Tools::isspace (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isspace"));

  for (unsigned int i = 0;
       i < string_in.size ();
       ++i)
    if (!::isspace (static_cast<int> (string_in[i])))
      return false;

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
BOOL CALLBACK locale_cb_function (LPWSTR name_in,
                                  DWORD flags_in,
                                  LPARAM parameter_in)
{
  COMMON_TRACE (ACE_TEXT ("::locale_cb_function"));

  // sanity check(s)
  ACE_ASSERT (parameter_in);

  std::vector<std::string>* locales_p =
    reinterpret_cast<std::vector<std::string>*> (parameter_in);

  // sanity check(s)
  ACE_ASSERT (locales_p);

  locales_p->push_back (ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (name_in)));

  return TRUE;
}
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
void
Common_Tools::setThreadName (const std::string& name_in,
                             DWORD threadId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::setThreadName"));

  // sanity check(s)
  ACE_ASSERT (!name_in.empty ());

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
  HANDLE handle_h =
    (threadId_in ? reinterpret_cast<HANDLE> (threadId_in)
                 : ::GetCurrentThread ());
  HRESULT result =
    ::SetThreadDescription (handle_h,
                            ACE_TEXT_ALWAYS_WCHAR (name_in.c_str ()));
  if (unlikely (FAILED (result)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetThreadDescription(%@): \"%s\", returning\n"),
                handle_h,
                ACE_TEXT (Common_Tools::errorToString (result, false).c_str ())));
#else
#if defined (DEBUG_DEBUGGER)
  // *NOTE*: code based on MSDN article (see:
  //         https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx)
  const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack (push, 8)
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // Must be 0x1000.
    LPCSTR szName;    // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags;    // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack (pop)
  THREADNAME_INFO info_s;
  info_s.dwType = 0x1000;
  info_s.szName = ACE_TEXT_ALWAYS_CHAR (name_in.c_str ());
  info_s.dwThreadID = (threadId_in ? threadId_in : -1);
  info_s.dwFlags = 0;
#pragma warning (push)
#pragma warning (disable: 6320 6322)
  __try {
    ::RaiseException (MS_VC_EXCEPTION,
                      0,
                      sizeof (struct tagTHREADNAME_INFO) / sizeof (ULONG_PTR),
                      (ULONG_PTR*)&info_s);
  } __except (EXCEPTION_EXECUTE_HANDLER) {}
#pragma warning (pop)
#endif // DEBUG_DEBUGGER
#endif // _WIN32_WINNT >= _WIN32_WINNT_WIN10
}
#endif // ACE_WIN32 || ACE_WIN64

unsigned int
Common_Tools::getNumberOfCPUs (bool logicalProcessors_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getNumberOfCPUs"));

  unsigned int result = 1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (logicalProcessors_in)
  {
    struct _SYSTEM_INFO system_info_s;
    GetSystemInfo (&system_info_s);
    result = system_info_s.dwNumberOfProcessors;
  } // end IF
  else
  {
    DWORD size = 0;
    DWORD error = 0;
    BYTE* byte_p = NULL;
    struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* processor_information_p =
      NULL;
    GetLogicalProcessorInformationEx (RelationProcessorCore,
                                      NULL,
                                      &size);
    error = GetLastError ();
    if (unlikely (error != ERROR_INSUFFICIENT_BUFFER))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetLogicalProcessorInformationEx(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Tools::errorToString (error).c_str ())));
      return 1;
    } // end IF
    ACE_NEW_NORETURN (byte_p,
                      BYTE[size]);
    if (unlikely (!byte_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));
      return 1;
    } // end IF
    processor_information_p =
      reinterpret_cast<struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*> (byte_p);
    if (unlikely (!GetLogicalProcessorInformationEx (RelationProcessorCore,
                                                     processor_information_p,
                                                     &size)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetLogicalProcessorInformationEx(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Tools::errorToString (GetLastError ()).c_str ())));

      // clean up
      delete [] byte_p;

      return 1;
    } // end IF
    for (DWORD offset = 0;
         offset < (size / sizeof (struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX));
         ++offset, ++processor_information_p)
    { ACE_ASSERT (processor_information_p);
      switch (processor_information_p->Relationship)
      {
        case RelationProcessorCore:
          ++result; break;
        default:
          break;
      } // end SWITCH
    } // end FOR
    delete [] byte_p;
  } // end ELSE
#else
  long result_2 = ACE_OS::sysconf (_SC_NPROCESSORS_ONLN);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sysconf(_SC_NPROCESSORS_ONLN): \"%m\", returning\n")));
    return 1;
  } // end IF
  result = static_cast<unsigned int> (result_2);
#endif

  return result;
}

void
Common_Tools::printLocales ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::printLocales"));

  std::vector<std::string> locales;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (!EnumSystemLocalesEx (locale_cb_function,
                                      LOCALE_ALL,
                                      reinterpret_cast<LPARAM> (&locales),
                                      NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumSystemLocalesEx(): \"%s\", returning\n"),
                ACE_TEXT (Common_Tools::errorToString (GetLastError ()).c_str ())));
    return;
  } // end IF
#else
  // *TODO*: this should work on most Linux systems, but is really a bad idea:
  //         - relies on local 'locale'
  //         - temporary files
  //         - system(3) call
  //         --> extremely inefficient; remove ASAP
  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR ("locale -a >> ");
  command_line_string += filename_string;

  int result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
  if (unlikely ((result == -1)      ||
                !WIFEXITED (result) ||
                WEXITSTATUS (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (command_line_string.c_str ()),
                WEXITSTATUS (result)));
    return;
  } // end IF
  unsigned char* data_p = NULL;
  unsigned int file_size_i = 0;
  if (unlikely (!Common_File_Tools::load (filename_string,
                                          data_p,
                                          file_size_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), returning\n"),
                ACE_TEXT (filename_string.c_str ())));
    return;
  } // end IF
  if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                ACE_TEXT (filename_string.c_str ())));

  std::string locales_string = reinterpret_cast<char*> (data_p);
  delete [] data_p;

  char buffer[BUFSIZ];
  std::istringstream converter;
  converter.str (locales_string);
  do {
    converter.getline (buffer, sizeof (buffer));
    locales.push_back (buffer);
  } while (!converter.fail ());
#endif

  int index = 1;
  for (std::vector<std::string>::const_iterator iterator = locales.begin ();
       iterator != locales.end ();
       ++iterator, ++index)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d: \"%s\"\n"),
                index,
                ACE_TEXT ((*iterator).c_str ())));
}

bool
Common_Tools::isOperatingSystem (enum Common_OperatingSystemType operatingSystem_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isOperatingSystem"));

  // sanity check(s)
  ACE_ASSERT (operatingSystem_in != COMMON_OPERATINGSYSTEM_INVALID);

  // get system information
  int result_2 = -1;
  ACE_utsname utsname_s;
  result_2 = ACE_OS::uname (&utsname_s);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::uname(): \"%m\", aborting\n")));
    return false;
  } // end IF
#if (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("'uname' output: %s (%s), %s %s, %s\n"),
              ACE_TEXT (utsname_s.nodename),
              ACE_TEXT (utsname_s.machine),
              ACE_TEXT (utsname_s.sysname),
              ACE_TEXT (utsname_s.release),
              ACE_TEXT (utsname_s.version)));
#endif // _DEBUG

#if defined (ACE_LINUX)
  std::string sysname_string (utsname_s.sysname);
  if (sysname_string.find (ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LINUX_UNAME_STRING),
                           0))
    return false;
#else
  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;)
#endif

  return true;
}

#if defined (ACE_LINUX)
enum Common_OperatingSystemDistributionType
Common_Tools::getDistribution (unsigned int& majorVersion_out,
                               unsigned int& minorVersion_out,
                               unsigned int& microVersion_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getDistribution"));

  // initialize return value(s)
  enum Common_OperatingSystemDistributionType result =
      COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID;
  majorVersion_out = 0;
  minorVersion_out = 0;
  microVersion_out = 0;

  // sanity check(s)
  if (unlikely (!Common_Tools::isOperatingSystem (COMMON_OPERATINGSYSTEM_GNU_LINUX)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("this is not a (GNU-) Linux system, aborting\n")));
    return result;
  } // end IF

  // step1: retrieve distributor id
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LSB_RELEASE_STRING);
  COMMON_COMMAND_ADD_SWITCH (command_line_string,COMMON_COMMAND_SWITCH_LSB_RELEASE_DISTRIBUTOR_STRING)
  int exit_status_i = 0;
  std::string command_output_string, distribution_id_string, release_string;
  std::string buffer_string;
  std::istringstream converter;
  char buffer_a [BUFSIZ];
  std::string regex_string = ACE_TEXT_ALWAYS_CHAR ("^(Distributor ID:\t)(.+)$");
  std::regex regex (regex_string);
  std::smatch match_results;
  if (unlikely (!Common_Tools::command (command_line_string,
                                        exit_status_i,
                                        command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (!command_output_string.empty ());
  converter.str (command_output_string);
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    buffer_string = buffer_a;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[2].matched);

    distribution_id_string = match_results[2];
    break;
  } while (!converter.fail ());
  if (unlikely (distribution_id_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve (GNU-) Linux distributor id (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end IF

  if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                       ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_DEBIAN_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN;
  else if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_OPENSUSE_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE;
  else if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_UBUNTU_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU;
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to determine Linux distribution (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end ELSE

  // step2: retrieve release version
  command_line_string =
      ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LSB_RELEASE_STRING);
  COMMON_COMMAND_ADD_SWITCH (command_line_string,COMMON_COMMAND_SWITCH_LSB_RELEASE_RELEASE_STRING)
  std::string regex_string_2 = ACE_TEXT_ALWAYS_CHAR ("^(Release:\t)(.+)$");
  std::regex regex_2 (regex_string_2);
  if (unlikely (!Common_Tools::command (command_line_string,
                                        exit_status_i,
                                        command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (!command_output_string.empty ());
  converter.str (command_output_string);
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    buffer_string = buffer_a;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[2].matched);

    release_string = match_results[2];
    break;
  } while (!converter.fail ());
  if (unlikely (release_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve (GNU-) Linux release version (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end IF
  converter.str (release_string);
  converter >> majorVersion_out;
  converter >> minorVersion_out;
  converter >> microVersion_out;

  return result;
}
#endif // ACE_LINUX

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
std::string
Common_Tools::capabilityToString (unsigned long capability_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::capabilityToString"));

  std::string return_value;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %u) not supported: \"%m\", aborting\n"),
                capability_in));
    return return_value;
  } // end IF

  char* capability_name_string_p = ::cap_to_name (capability_in);
  if (!capability_name_string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_to_name(%d): \"%m\", aborting\n"),
                capability_in));
    return return_value;
  } // end IF
  return_value = capability_name_string_p;

  // clean up
  int result = ::cap_free (capability_name_string_p);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return return_value;
}

bool
Common_Tools::hasCapability (unsigned long capability_in,
                             cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::hasCapability"));

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  cap_flag_value_t in_set = CAP_CLEAR;
  int result_2 = ::cap_get_flag (capabilities_p, capability_in,
                                 set_in, &in_set);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_get_flag(%s,%d): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return (in_set == CAP_SET);
}

void
Common_Tools::printCapabilities ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::printCapabilities"));

  int result = -1;

  // step1: read 'securebits' flags of the calling thread
  result = ::prctl (PR_GET_SECUREBITS);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_GET_SECUREBITS): \"%m\", continuing\n")));
  else
    ACE_DEBUG ((LM_INFO,
//                ACE_TEXT ("%P/%t: securebits set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t%s/%s\n"),
                ACE_TEXT ("%P/%t: securebits set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t\t%s/%s\nSECURE_NO_CAP_AMBIENT_RAISE:\t%s/%s\n"),
                ((result & SECBIT_NOROOT) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NOROOT_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_SETUID_FIXUP) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_SETUID_FIXUP_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_KEEP_CAPS) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_KEEP_CAPS_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),//));
                ((result & SECBIT_NO_CAP_AMBIENT_RAISE) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_CAP_AMBIENT_RAISE_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

  cap_t capabilities_p = NULL;
  bool in_bounding_set, in_ambient_set;
  cap_flag_value_t in_effective_set, in_inheritable_set, in_permitted_set;
  capabilities_p = cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", returning\n")));
    return;
  } // end IF

//#if defined (_DEBUG)
//  ssize_t length_i = 0;
//  char* string_p = cap_to_text (capabilities_p, &length_i);
//  if (unlikely (!string_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_to_text(): \"%m\", returning\n")));
//    goto clean;
//  } // end IF
//  ACE_DEBUG ((LM_INFO,
//              ACE_TEXT ("%P:%t: capabilities \"%s\"\n"),
//              ACE_TEXT (string_p)));
//#endif // _DEBUG

  // step2: in 'ambient'/'bounding' set of the calling thread ?
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P/%t: capability ambient/bounding\teffective/inheritable/permitted\n")));
  for (unsigned long capability = 0;
       capability <= CAP_LAST_CAP;
       ++capability)
  {
    // *TODO*: currently, this fails on Linux (Debian)...
//    result = 0;
    result = ::prctl (PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET,
                      capability,
                      0, 0);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::prctl(PR_CAP_AMBIENT,PR_CAP_AMBIENT_IS_SET,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    in_ambient_set = (result == 1);

    result = ::prctl (PR_CAPBSET_READ,
                      capability);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::prctl(PR_CAPBSET_READ,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    in_bounding_set = (result == 1);

    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_EFFECTIVE, &in_effective_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_EFFECTIVE,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_INHERITABLE, &in_inheritable_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_INHERITABLE,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_PERMITTED, &in_permitted_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_PERMITTED,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%s (%d):\t\t%s/%s\t\t%s/%s/%s\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ()), capability,
                (in_ambient_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (in_bounding_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_effective_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_inheritable_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_permitted_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));
  } // end FOR

  // clean up
//#if defined (_DEBUG)
//  if (likely (string_p))
//  {
//    result = cap_free (string_p);
//    if (unlikely (result == -1))
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
//  } // end IF
//#endif
  if (likely (capabilities_p))
  {
    result = cap_free (capabilities_p);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
  } // end IF
}

bool
Common_Tools::setCapability (unsigned long capability_in,
                             cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::setCapability"));

  bool result = false;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  int result_2 = -1;
  if (likely (set_in == CAP_EFFECTIVE))
  {
    // verify that the capability is in the 'permitted' set
    if (!Common_Tools::hasCapability (capability_in, CAP_PERMITTED))
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%P/%t: capability (was: %s (%u)) not in 'permitted' set: cannot enable, continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
  } // end IF

  cap_value_t capabilities_a[CAP_LAST_CAP + 1];
  ACE_OS::memset (&capabilities_a, 0, sizeof (cap_value_t[CAP_LAST_CAP + 1]));
  capabilities_a[0] = capability_in;
  result_2 = ::cap_set_flag (capabilities_p, set_in,
                             1, capabilities_a,
                             CAP_SET);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_flag(%s,%d,CAP_SET): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF
  if (likely (Common_Tools::hasCapability (capability_in, set_in)))
    result = true;

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return result;
}
bool
Common_Tools::dropCapability (unsigned long capability_in,
                              cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::dropCapability"));

  bool result = false;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  cap_value_t capabilities_a[CAP_LAST_CAP + 1];
  capabilities_a[0] = capability_in;
  int result_2 = ::cap_set_flag (capabilities_p, set_in,
                                 1, capabilities_a,
                                 CAP_CLEAR);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_flag(%s,%d,CAP_CLEAR): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF
  result = true;

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return result;
}
#endif

std::string
Common_Tools::commandLineToString (int argc_in,
                                   ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::commandLineToString"));

  // initialize return value(s)
  std::string return_value;

  for (int i = 0;
       i < argc_in;
       ++i)
  {
    ACE_ASSERT (argv_in[i]);
    return_value += ACE_TEXT_ALWAYS_CHAR (argv_in[i]);
    return_value += ACE_TEXT_ALWAYS_CHAR (" ");
  } // end FOR

  return return_value;
}

bool
Common_Tools::command (const std::string& commandLine_in,
                       int& exitStatus_out,
                       std::string& stdOut_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::command"));

  // initialize return value(s)
  exitStatus_out = -1;
  stdOut_out.clear ();

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (!commandLine_in.empty ());

  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string = commandLine_in;
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" >> ");
  command_line_string += filename_string;

  result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
//  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
  if (unlikely ((result == -1)      ||
                !WIFEXITED (result) ||
                WEXITSTATUS (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (commandLine_in.c_str ()),
                WEXITSTATUS (result)));
    exitStatus_out = WEXITSTATUS (result);
    return false;
  } // end IF
  exitStatus_out = WEXITSTATUS (result);

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (Common_File_Tools::canRead (filename_string, ACE_TEXT_ALWAYS_CHAR ("")));
#else
  ACE_ASSERT (Common_File_Tools::canRead (filename_string, static_cast<uid_t> (-1)));
#endif // ACE_WIN32 || ACE_WIN64

  unsigned char* data_p = NULL;
  unsigned int file_size_i = 0;
  if (unlikely (!Common_File_Tools::load (filename_string,
                                          data_p,
                                          file_size_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename_string.c_str ())));
    return false;
  } // end IF
  if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                ACE_TEXT (filename_string.c_str ())));
  stdOut_out.assign (reinterpret_cast<char* >(data_p), file_size_i);

  // clean up
  delete [] data_p;

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
pid_t
Common_Tools::getProcessId (const std::string& executableName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getProcessId"));

  pid_t result = 0;

  // sanity check(s)
  ACE_ASSERT (!executableName_in.empty ());

#if defined (ACE_LINUX)
  // sanity check(s)
  unsigned int major_i = 0, minor_i = 0, micro_i = 0;
  enum Common_OperatingSystemDistributionType linux_distribution_e =
      Common_Tools::getDistribution (major_i, minor_i, micro_i);
  ACE_UNUSED_ARG (major_i); ACE_UNUSED_ARG (minor_i); ACE_UNUSED_ARG (micro_i);
  if (unlikely (linux_distribution_e == COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::getDistribution(), aborting\n")));
    return result;
  } // end IF

  std::string command_line_string;
  switch (linux_distribution_e)
  {
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE:
    {
      // sanity check(s)
      if (unlikely (!Common_Tools::isInstalled (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_PIDOFPROC_STRING),
                                                command_line_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("command (was: \"%s\") is not installed: cannot proceed, aborting\n"),
                    ACE_TEXT (COMMON_COMMAND_PIDOFPROC_STRING)));
        return result;
      } // end IF
//      ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));
      break;
    }
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU:
    {
      // sanity check(s)
      if (unlikely (!Common_Tools::isInstalled (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_PIDOF_STRING),
                                                command_line_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("command (was: \"%s\") is not installed: cannot proceed, aborting\n"),
                    ACE_TEXT (COMMON_COMMAND_PIDOF_STRING)));
        return result;
      } // end IF
//      ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unsupported linux distribution (was: %d), aborting\n"),
                  linux_distribution_e));
      return result;
    }
  } // end SWITCH
  ACE_ASSERT (!command_line_string.empty ());
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
  command_line_string +=
      ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (executableName_in.c_str ()),
                                           ACE_DIRECTORY_SEPARATOR_CHAR));

  ACE_TCHAR buffer_a[BUFSIZ];
  char* pid_p = NULL;
  int i = 0;
  pid_t process_ids_a[64];
  ACE_OS::memset (&process_ids_a, 0, sizeof (process_ids_a));
  FILE* stream_p = ::popen (command_line_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR ("r"));
  if (unlikely (!stream_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::popen(\"%s\",r): \"%m\", aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  if (unlikely (!ACE_OS::fgets (buffer_a,
                                sizeof (char[BUFSIZ]),
                                stream_p)))
  {
    if (!::feof (stream_p)) // no output ? --> process not found
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fgets(%d,%@): \"%m\", aborting\n"),
                  sizeof (char[BUFSIZ]), stream_p));
    goto clean;
  } // end IF

  pid_p = ACE_OS::strtok (buffer_a, ACE_TEXT_ALWAYS_CHAR (" "));
  while (pid_p)
  {
    process_ids_a[i] = static_cast<pid_t> (ACE_OS::atoi (pid_p));
    ++i;
    pid_p = ACE_OS::strtok (NULL , ACE_TEXT_ALWAYS_CHAR (" "));
  } // end WHILE
  if (unlikely (i > 1))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("found more than one process for executable (was: \"%s\"), returning the lowest PID\n"),
                ACE_TEXT (executableName_in.c_str ())));
    std::sort (process_ids_a, process_ids_a + 64);
  } // end IF
  if (likely (i))
    result = process_ids_a[0];

clean:
  if (likely (stream_p))
  { // (on success) pclose() returns the commands' exit status
    int result_2 = ::pclose (stream_p);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::pclose(%@): \"%m\", continuing\n"),
                  stream_p));
  } // end IF
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);

  ACE_NOTREACHED (return result;)
#endif // ACE_LINUX

  return result;
}

uint64_t
Common_Tools::getStartTime ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getStartTime"));

  // initialize return value(s)
  uint64_t result = 0;

  // sanity check(s)
#if defined (ACE_LINUX)
  std::string proc_self_stat_path = ACE_TEXT_ALWAYS_CHAR ("/proc/self/stat");
  ACE_ASSERT (Common_File_Tools::isReadable (proc_self_stat_path));

  FILE* file_p = NULL;
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  size_t length_i = 0;
  char* string_p = NULL;
  int index_i = 19;
  int result_2 = -1;

  file_p = ACE_OS::fopen (proc_self_stat_path.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("rb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (proc_self_stat_path.c_str ())));
    return result;
  } // end IF

  // *TODO* use readline() instead ?
  if (unlikely (!ACE_OS::fgets (buffer_a,
                                sizeof (char[BUFSIZ]),
                                file_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fgets(%d,%@): \"%m\", aborting\n"),
                sizeof (char[BUFSIZ]),
                file_p));
    goto clean;
  } // end IF
  // *NOTE*: only the second invocation of fgets would set eof
  // ACE_ASSERT (::feof (file_p));
  result_2 = ACE_OS::fclose (file_p);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (proc_self_stat_path.c_str ())));
    goto clean;
  } // end IF
  file_p = NULL;
  length_i = ACE_OS::strlen (buffer_a);

  /* *NOTE*: 'starttime' is the field at index 22 (see: man proc(5)) (i.e. 19
   *         after the 'comm' entry - this is currently the only field
   *         containing the ')' character, so the search should be unambiguous;
   *         search backwards to further avoid trouble */
  // *TODO*: this code is brittle
  string_p = ACE_OS::strrchr (buffer_a, ')');
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strrchr(%s): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (")")));
    goto clean;
  } // end IF
  string_p += 2; // skip ') '
  if (unlikely (static_cast<size_t> (string_p - buffer_a) >= length_i))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strrchr(%s): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (")")));
    goto clean;
  } // end IF
  string_p = ACE_OS::strtok (string_p, ACE_TEXT_ALWAYS_CHAR (" "));
  while ((--index_i + 1) &&
         string_p)
    string_p = ACE_OS::strtok (NULL , ACE_TEXT_ALWAYS_CHAR (" "));
  if (unlikely (!string_p))
  {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::strtok(%s,19): \"%m\", aborting\n"),
                  ACE_TEXT (buffer_a)));
      goto clean;
  } // end IF

  result = ACE_OS::atoi (string_p);

clean:
  if (file_p)
  {
    result_2 = ACE_OS::fclose (file_p);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (proc_self_stat_path.c_str ())));
  } // end IF
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);

  ACE_NOTREACHED (return result;)
#endif // ACE_LINUX

  return result;
}
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Common_Tools::switchUser (uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::switchUser"));

  uid_t  user_id =
      (userId_in == static_cast<uid_t> (-1) ? ACE_OS::getuid () : userId_in);
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  int result = ACE_OS::seteuid (user_id);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::seteuid(%u): \"%m\", aborting\n"),
                user_id));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%sset effective user id (is: %u)\n"),
                (userId_in == static_cast<uid_t> (-1) ? ACE_TEXT ("re") : ACE_TEXT ("")),
                user_id));

  return (result == 0);
}

Common_UserGroups_t
Common_Tools::getUserGroups (uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getUserGroups"));

  // initialize return value(s)
  Common_UserGroups_t return_value;

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);
  struct passwd passwd_s, *passwd_p = NULL;
  ACE_OS::memset (&passwd_s, 0, sizeof (struct passwd));
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  int result =
      ::getpwuid_r (user_id,               // user id
                    &passwd_s,             // passwd entry
                    buffer_a,              // buffer
                    sizeof (char[BUFSIZ]), // buffer size
                    &passwd_p);            // result (handle)
  if (unlikely ((result == -1) || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u): \"%m\", aborting\n"),
                user_id));
    return return_value;
  } // end IF
  return_value.push_back (Common_Tools::groupIdToString (passwd_s.pw_gid));

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF
    ACE_ASSERT (group_s.gr_mem);
    for (char** string_pp = group_s.gr_mem;
         *string_pp;
         ++string_pp)
      if (!ACE_OS::strcmp (username_string.c_str (),
                           *string_pp))
        return_value.push_back (group_s.gr_name);
  } while (true);
  ::endgrent ();

  return return_value;
}

bool
Common_Tools::addGroupMember (uid_t userId_in,
                              gid_t groupId_in,
                              bool persist_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::addGroupMember"));

#if defined (_DEBUG)
  Common_Tools::printCapabilities ();
#endif // _DEBUG

  bool result = false;

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);

  // sanity check(s)
  if (unlikely (Common_Tools::isGroupMember (userId_in, groupId_in)))
    return true; // nothing to do

  if (likely (persist_in))
  {
    // *TODO*: use putgrent(3)
    std::string command_output_string;
    std::string command_line_string =
        ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_GPASSWD_STRING);
//        ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_USERMOD_STRING);
    command_line_string += ACE_TEXT_ALWAYS_CHAR (" -a ");
    command_line_string += username_string;
//    command_line_string += ACE_TEXT_ALWAYS_CHAR (" -a -G ");
    command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
    command_line_string += Common_Tools::groupIdToString (groupId_in);
//    command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
//    command_line_string += username_string;
    int exit_status_i = 0;
    if (unlikely(!Common_Tools::command (command_line_string,
                                         exit_status_i,
                                         command_output_string)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                  ACE_TEXT (command_line_string.c_str ())));
      return false;
    } // end IF

    result = true;
  } // end IF
  else
  {
    // *TODO* use setgroups(2)
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (false);

    ACE_NOTREACHED (return false;)
  } // end ELSE

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("added user \"%s\" (uid: %u) to group \"%s\" (gid: %u)...\n"),
              ACE_TEXT (username_string.c_str ()), user_id,
              ACE_TEXT (Common_Tools::groupIdToString (groupId_in).c_str ()), groupId_in));
#endif // _DEBUG

  return result;
}

bool
Common_Tools::isGroupMember (uid_t userId_in,
                             gid_t groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isGroupMember"));

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct passwd passwd_s;
  struct passwd* passwd_p = NULL;
  int result = -1;
  // step1: check whether the group happens to be the users' 'primary' group
  //        (the user would not appear in the member list of the 'group'
  //        database in this case)
  result = ::getpwuid_r (user_id,               // user id
                         &passwd_s,             // passwd entry
                         buffer_a,              // buffer
                         sizeof (char[BUFSIZ]), // buffer size
                         &passwd_p);            // result (handle)
  if (unlikely ((result == -1) || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u): \"%m\", aborting\n"),
                user_id));
    return false; // *TODO*: avoid false negatives
  } // end IF
  if (passwd_s.pw_gid == groupId_in)
    return true;

  // step2: check the 'secondary' member list of the 'group' database
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct group group_s;
  struct group* group_p = NULL;
  result = ::getgrgid_r (groupId_in,
                         &group_s,
                         buffer_a,
                         sizeof (char[BUFSIZ]),
                         &group_p);
  if (unlikely ((result == -1) || !group_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getgrgid_r(%u): \"%m\", aborting\n"),
                groupId_in));
    return false; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (group_s.gr_mem);
//  if (!*group_s.gr_mem) // groupId_in is a 'primary group', i.e. a group with
//                        // only members that have it set as primary group in
//                        // the corresponding 'passwd' file entry (see: man
//                        // getpwnam(3))
//    return false;
  for (char** string_pp = group_s.gr_mem;
       *string_pp;
       ++string_pp)
    if (!ACE_OS::strcmp (username_string.c_str (),
                         *string_pp))
      return true;

  return false;
}

std::string
Common_Tools::groupIdToString (gid_t groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::groupIdToString"));

  // initialize return value(s)
  std::string return_value;

  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  char buffer_a[BUFSIZ];
  int result = -1;

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF

    if (group_p->gr_gid == groupId_in)
    {
      return_value = group_p->gr_name;
      break;
    } // end IF
  } while (true);
  ::endgrent ();

  return return_value;
}
gid_t
Common_Tools::stringToGroupId (const std::string& groupName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::stringToGroupId"));

  // initialize return value(s)
  gid_t return_value = 0;

  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  char buffer_a[BUFSIZ];
  int result = -1;

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF

    if (!ACE_OS::strcmp (groupName_in.c_str (),
                         group_p->gr_name))
    {
      return_value = group_p->gr_gid;
      break;
    } // end IF
  } while (true);
  ::endgrent ();

  return return_value;
}
#endif // ACE_WIN32 || ACE_WIN64

void
Common_Tools::printUserIds ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::printUserIds"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
#else
  uid_t user_id = ACE_OS::getuid ();
  uid_t effective_user_id = ACE_OS::geteuid ();
  gid_t user_group = ACE_OS::getgid ();
  gid_t effective_user_group = ACE_OS::getegid ();

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P:%t: real/effective user id/group: %u/%u\t%u/%u\n"),
              user_id, effective_user_id,
              user_group, effective_user_group));
#endif // ACE_WIN32 || ACE_WIN64
}

bool
Common_Tools::enableCoreDump (bool enable_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::enableCoreDump"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#else
  if (unlikely (!Common_Tools::setResourceLimits (false,
                                                  enable_in,
                                                  false)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));
    return false;
  } // end IF

  int result =
      ::prctl (PR_SET_DUMPABLE,
               (enable_in ? 1 : 0), 0, 0, 0);
//               (enable_in ? SUID_DUMP_USER : SUID_DUMP_DISABLE), 0, 0, 0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_SET_DUMPABLE): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s core dump\n"),
              (enable_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled"))));

  return true;
}

bool
Common_Tools::setResourceLimits (bool fileDescriptors_in,
                                 bool stackTraces_in,
                                 bool pendingSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::setResourceLimits"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  int result = -1;
  rlimit resource_limit;
#endif

  if (fileDescriptors_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("file descriptor limits are not available on this platform, continuing\n")));
#else
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    //      ACE_DEBUG ((LM_DEBUG,
    //                  ACE_TEXT ("file descriptor limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //                  resource_limit.rlim_cur,
    //                  resource_limit.rlim_max));

    // *TODO*: really unset these limits; note that this probably requires
    // patching/recompiling the kernel...
    // *NOTE*: setting/raising the max limit probably requires CAP_SYS_RESOURCE
    resource_limit.rlim_cur = resource_limit.rlim_max;
    //      resource_limit.rlim_cur = RLIM_INFINITY;
    //      resource_limit.rlim_max = RLIM_INFINITY;
    result = ACE_OS::setrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset file descriptor limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#endif
  } // end IF

// -----------------------------------------------------------------------------

  if (stackTraces_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("core file limits/stack trace dumps are not available on this platform, continuing\n")));
#else
    //  result = ACE_OS::getrlimit (RLIMIT_CORE,
    //                              &resource_limit);
    //  if (result == -1)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
    //    return false;
    //  } // end IF
    //   ACE_DEBUG ((LM_DEBUG,
    //               ACE_TEXT ("corefile limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //               core_limit.rlim_cur,
    //               core_limit.rlim_max));

    // set soft/hard limits to unlimited...
    resource_limit.rlim_cur = RLIM_INFINITY;
    resource_limit.rlim_max = RLIM_INFINITY;
    result = ACE_OS::setrlimit (RLIMIT_CORE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_CORE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset corefile limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#endif
  } // end IF

  if (pendingSignals_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("pending signal limits are not available on this platform, continuing\n")));
#else
    //  result = ACE_OS::getrlimit (RLIMIT_SIGPENDING,
    //                              &resource_limit);
    //  if (result == -1)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
    //    return false;
    //  } // end IF

    //   ACE_DEBUG ((LM_DEBUG,
    //               ACE_TEXT ("pending signals limit (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //               signal_pending_limit.rlim_cur,
    //               signal_pending_limit.rlim_max));

    // set soft/hard limits to unlimited...
    // *NOTE*: setting/raising the max limit probably requires CAP_SYS_RESOURCE
//    resource_limit.rlim_cur = RLIM_INFINITY;
//    resource_limit.rlim_max = RLIM_INFINITY;
//    resource_limit.rlim_cur = resource_limit.rlim_max;
//    result = ACE_OS::setrlimit (RLIMIT_SIGPENDING,
//                                &resource_limit);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
//      return false;
//    } // end IF

    // verify...
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
    rctlblk_t* block_p =
        static_cast<rctlblk_t*> (ACE_CALLOC_FUNC (1, rctlblk_size ()));
    if (unlikely (!block_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      return false;
    } // end IF
    result = ::getrctl (ACE_TEXT_ALWAYS_CHAR ("process.max-sigqueue-size"),
                        NULL, block_p,
                        RCTL_USAGE);
    if (result == 0)
    {
      resource_limit.rlim_cur = rctlblk_get_value (block_p);
      resource_limit.rlim_max = rctlblk_get_value (block_p);
    } // end IF
    ACE_FREE_FUNC (block_p);
#else
    result = ACE_OS::getrlimit (RLIMIT_SIGPENDING,
                                &resource_limit);
#endif
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset pending signal limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#endif
  } // end IF

  return true;
}

void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Common_Tools::getUserName (
#else
Common_Tools::getUserName (uid_t userId_in,
#endif // ACE_WIN32 || ACE_WIN64
                           std::string& username_out,
                           std::string& realname_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getUserName"));

  // initialize return value(s)
  username_out.clear ();
  realname_out.clear ();

//  char user_name[ACE_MAX_USERID];
//  ACE_OS::memset (user_name, 0, sizeof (user_name));
//  if (unlikely (!ACE_OS::cuserid (user_name,
//                                  ACE_MAX_USERID)))
//  {
//    ACE_DEBUG ((LM_WARNING,
//                ACE_TEXT ("failed to ACE_OS::cuserid(): \"%m\", falling back\n")));

//    username_out =
//      ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_DEFAULT_USER_LOGIN_BASE)));

//    return;
//  } // end IF
//  username_out = user_name;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  DWORD buffer_size = sizeof (buffer);
  if (unlikely (!ACE_TEXT_GetUserNameEx (NameDisplay, // EXTENDED_NAME_FORMAT
                                         buffer,
                                         &buffer_size)))
  {
    DWORD error = GetLastError ();
    if (error != ERROR_NONE_MAPPED)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to %s(): \"%m\", continuing\n"),
                  ACE_TEXT (ACE_TEXT_GetUserNameEx)));
  } // end IF
  else
    realname_out = ACE_TEXT_ALWAYS_CHAR (buffer);
#else
  int            result = -1;
  struct passwd  passwd_s;
  struct passwd* passwd_p = NULL;
  char           buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
//  size_t         bufsize = 0;
//  bufsize = sysconf (_SC_GETPW_R_SIZE_MAX);
//  if (bufsize == -1)        /* Value was indeterminate */
//    bufsize = 16384;        /* Should be more than enough */

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  result = ::getpwuid_r (user_id,               // user id
                         &passwd_s,             // passwd entry
                         buffer_a,              // buffer
                         sizeof (char[BUFSIZ]), // buffer size
                         &passwd_p);            // result (handle)
  if (unlikely (!passwd_p))
  {
    if (result == 0)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("user \"%u\" not found, %s\n"),
                  user_id,
                  ((user_id == ACE_OS::geteuid ()) ? ACE_TEXT_ALWAYS_CHAR ("falling back") : ACE_TEXT_ALWAYS_CHAR ("returning"))));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getpwuid_r(%u): \"%m\", %s\n"),
                  ((user_id == ACE_OS::geteuid ()) ? ACE_TEXT_ALWAYS_CHAR ("falling back") : ACE_TEXT_ALWAYS_CHAR ("returning"))));
    if (user_id == ACE_OS::geteuid ())
      username_out =
          ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_DEFAULT_USER_LOGIN_BASE)));
  } // end IF
  else
  {
    username_out = passwd_s.pw_name;
    if (ACE_OS::strlen (passwd_s.pw_gecos))
    {
      realname_out = passwd_s.pw_gecos;
      std::string::size_type position = realname_out.find (',');
      if (position != std::string::npos)
        realname_out.substr (0, position);
    } // end IF
  } // end ELSE
#endif // ACE_WIN32 || ACE_WIN64
}

std::string
Common_Tools::getHostName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getHostName"));

  // initialize return value(s)
  std::string return_value;

  int result = -1;
  ACE_TCHAR host_name[MAXHOSTNAMELEN + 1];
  ACE_OS::memset (host_name, 0, sizeof (host_name));
  result = ACE_OS::hostname (host_name, sizeof (host_name));
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::hostname(): \"%m\", aborting\n")));
  else
    return_value = ACE_TEXT_ALWAYS_CHAR (host_name);

  return return_value;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
std::string
Common_Tools::GUIDToString (REFGUID GUID_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::GUIDToString"));

  std::string result;

  OLECHAR GUID_string[CHARS_IN_GUID];
  ACE_OS::memset (GUID_string, 0, sizeof (GUID_string));
  int result_2 = StringFromGUID2 (GUID_in,
                                  GUID_string, CHARS_IN_GUID);
  ACE_ASSERT (result_2 == CHARS_IN_GUID);

#if defined (OLE2ANSI)
  result = GUID_string;
#else
  result = ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (GUID_string));
#endif

  return result;
}
struct _GUID
Common_Tools::StringToGUID (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::StringToGUID"));

  struct _GUID result = GUID_NULL;

  HRESULT result_2 = E_FAIL;
#if defined (OLE2ANSI)
  result_2 = CLSIDFromString (string_in.c_str (), &result);
#else
  result_2 =
    CLSIDFromString (ACE_TEXT_ALWAYS_WCHAR (string_in.c_str ()), &result);
#endif
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CLSIDFromString(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in.c_str ()),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return GUID_NULL;
  } // end IF

  return result;
}

std::string
Common_Tools::errorToString (DWORD error_in,
                             bool useAMGetErrorText_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::errorToString"));

  std::string result;

  DWORD result_2 = 0;
  if (useAMGetErrorText_in)
  {
    TCHAR buffer[MAX_ERROR_TEXT_LEN];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    result_2 = AMGetErrorText (static_cast<HRESULT> (error_in),
                               buffer,
                               MAX_ERROR_TEXT_LEN);
    if (!result_2)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to AMGetErrorText(0x%x): \"%s\", falling back\n"),
                  error_in,
                  ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
      goto fallback;
    } // end IF
    result = ACE_TEXT_ALWAYS_CHAR (buffer);

    goto continue_;
  } // end IF

  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  result_2 =
    ACE_TEXT_FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,                 // dwFlags
                            NULL,                                       // lpSource
                            error_in,                                   // dwMessageId
                            MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                            buffer,                                     // lpBuffer
                            sizeof (buffer),                            // nSize
                            NULL);                                      // Arguments
  if (!result_2)
  {
    DWORD error = ::GetLastError ();
    if (unlikely (error != ERROR_MR_MID_NOT_FOUND))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to FormatMessage(0x%x): \"%s\", continuing\n"),
                  error_in,
                  ACE_TEXT (Common_Tools::errorToString (error).c_str ())));
      return result;
    } // end IF

fallback:
    // try DirectX error messages
    // *TODO*: implement ascii variants of DXGetErrorString
    //ACE_TCHAR* string_p = ACE_TEXT_WCHAR_TO_TCHAR (DXGetErrorString (error_in));
    //if (!string_p)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to DXGetErrorString(0x%x): \"%s\", aborting\n"),
    //              error_in,
    //              ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
    //  return result;
    //} // end IF
    //result = ACE_TEXT_ALWAYS_CHAR (string_p);
    //result += ACE_TEXT_ALWAYS_CHAR (": ");

    WCHAR buffer_2[BUFSIZ];
    ACE_OS::memset (buffer_2, 0, sizeof (buffer_2));
    DXGetErrorDescription (error_in, buffer_2, BUFSIZ);
    ACE_Wide_To_Ascii converter (buffer_2);
    if (unlikely (!ACE_OS::strcpy (buffer,
                                   ACE_TEXT_CHAR_TO_TCHAR (converter.char_rep ()))))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::strcpy(): \"%m\", aborting\n")));
      return result;
    } // end IF
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer);

  // strip trailing newline
  if (result[result.size () - 1] == '\n')
  {
    result.erase (--result.end ());
    result.erase (--result.end ());
  } // end IF

continue_:
  return result;
}
#endif

bool
Common_Tools::initializeLogging (const std::string& programName_in,
                                 const std::string& logFile_in,
                                 bool logToSyslog_in,
                                 bool enableTracing_in,
                                 bool enableDebug_in,
                                 ACE_Log_Msg_Backend* backend_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeLogging"));

  int result = -1;

  // *NOTE*: default log target is stderr
  u_long options_flags = ACE_Log_Msg::STDERR;
  if (logToSyslog_in)
    options_flags |= ACE_Log_Msg::SYSLOG;
  if (backend_in)
  {
    options_flags |= ACE_Log_Msg::CUSTOM;
    ACE_LOG_MSG->msg_backend (backend_in);
  } // end IF
  if (!logFile_in.empty ())
  {
    options_flags |= ACE_Log_Msg::OSTREAM;

    ACE_OSTREAM_TYPE* log_stream_p = NULL;
    std::ios_base::openmode open_mode = (std::ios_base::out   |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN (log_stream_p,
                      std::ofstream (logFile_in.c_str (),
                                     open_mode));
//    log_stream_p = ACE_OS::fopen (logFile_in.c_str (),
//                                  ACE_TEXT_ALWAYS_CHAR ("w"));
    if (unlikely (!log_stream_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
//                  ACE_TEXT (logFile_in.c_str ())));
      return false;
    } // end IF
    if (unlikely (log_stream_p->fail ()))
//    if (log_stream_p->open (logFile_in.c_str (),
//                            open_mode))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create log file (was: \"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (logFile_in.c_str ())));

      // clean up
      delete log_stream_p;

      return false;
    } // end IF

    // *NOTE*: the logger singleton assumes ownership of the stream object
    // *BUG*: doesn't work on Linux
    ACE_LOG_MSG->msg_ostream (log_stream_p, true);
  } // end IF
  result = ACE_LOG_MSG->open (ACE_TEXT (programName_in.c_str ()),
                              options_flags,
                              NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Msg::open(\"%s\", %u): \"%m\", aborting\n"),
                ACE_TEXT (programName_in.c_str ()),
                options_flags));
    return false;
  } // end IF

  // set new mask...
  u_long process_priority_mask = (LM_SHUTDOWN |
                                  LM_TRACE    |
                                  LM_DEBUG    |
                                  LM_INFO     |
                                  LM_NOTICE   |
                                  LM_WARNING  |
                                  LM_STARTUP  |
                                  LM_ERROR    |
                                  LM_CRITICAL |
                                  LM_ALERT    |
                                  LM_EMERGENCY);
  if (!enableTracing_in)
    process_priority_mask &= ~LM_TRACE;
  if (!enableDebug_in)
    process_priority_mask &= ~LM_DEBUG;
  ACE_LOG_MSG->priority_mask (process_priority_mask,
                              ACE_Log_Msg::PROCESS);

  return true;
}

void
Common_Tools::finalizeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeLogging"));

  // *NOTE*: this may be necessary in case the backend sits on the stack.
  //         In that case, ACE::fini() closes the backend too late...
  ACE_LOG_MSG->msg_backend (NULL);
}

bool
Common_Tools::initializeEventDispatch (struct Common_EventDispatchConfiguration& configuration_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeEventDispatch"));

  // initialize return value(s)
  configuration_inout.handlersRequireSerialization = false;

  // step0: initialize reactor/proactor implementation
  // *NOTE*: the appropriate type of event dispatch mechanism may depend on
  //         several factors:
  //         - targeted platform(s) (i.e. supported mechanisms, (software) environment)
  //         - envisioned application type (i.e. library, plugin)
  // *TODO*: the chosen event dispatch implementation should be based on
  //         #defines only (!, see above, common.h)
  configuration_inout.reactorType = COMMON_EVENT_REACTOR_TYPE;
  configuration_inout.proactorType = COMMON_EVENT_PROACTOR_TYPE;
  if (configuration_inout.numberOfReactorThreads)
    configuration_inout.reactorType =
        (configuration_inout.useThreadPoolReactor ? COMMON_REACTOR_THREAD_POOL
                                                  : configuration_inout.reactorType);

  // step1: initialize reactor
  if (configuration_inout.numberOfReactorThreads)
  {
    ACE_Reactor_Impl* reactor_impl_p = NULL;
    switch (configuration_inout.reactorType)
    {
      case COMMON_REACTOR_ACE_DEFAULT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using ACE default (platform-specific) reactor\n")));
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      case COMMON_REACTOR_DEV_POLL:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using /dev/poll reactor\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_Dev_Poll_Reactor (COMMON_EVENT_MAXIMUM_HANDLES,    // max num handles
                                                true,                            // restart after EINTR ?
                                                NULL,                            // signal handler handle
                                                NULL,                            // timer queue handle
                                                ACE_DISABLE_NOTIFY_PIPE_DEFAULT, // disable notify pipe ?
                                                NULL,                            // notification handler handle
                                                1,                               // mask signals ?
                                                ACE_DEV_POLL_TOKEN::FIFO));      // signal queue

        configuration_inout.handlersRequireSerialization = true;

        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case COMMON_REACTOR_SELECT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using select reactor\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_Select_Reactor (static_cast<size_t> (COMMON_EVENT_MAXIMUM_HANDLES), // max num handles
                                              true,                                               // restart after EINTR ?
                                              NULL,                                               // signal handler handle
                                              NULL,                                               // timer queue handle
                                              ACE_DISABLE_NOTIFY_PIPE_DEFAULT,                    // disable notification pipe ?
                                              NULL,                                               // notification handler handle
                                              true,                                               // mask signals ?
                                              ACE_SELECT_TOKEN::FIFO));                           // signal queue

        break;
      }
      case COMMON_REACTOR_THREAD_POOL:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using thread-pool reactor\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_TP_Reactor (static_cast<size_t> (COMMON_EVENT_MAXIMUM_HANDLES), // max num handles
                                          true,                                               // restart after EINTR ?
                                          NULL,                                               // signal handler handle
                                          NULL,                                               // timer queue handle
                                          true,                                               // mask signals ?
                                          ACE_Select_Reactor_Token::FIFO));                   // signal queue

        configuration_inout.handlersRequireSerialization = true;

        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case COMMON_REACTOR_WFMO:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using WFMO reactor\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_WFMO_Reactor (ACE_WFMO_Reactor::DEFAULT_SIZE, // max num handles (62 [+ 2])
                                            0,                              // unused
                                            NULL,                           // signal handler handle
                                            NULL,                           // timer queue handle
                                            NULL));                         // notification handler handle

        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      ///////////////////////////////////
      case COMMON_REACTOR_INVALID:
      case COMMON_REACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown reactor type (was: %d), aborting\n"),
                    configuration_inout.reactorType));
        return false;
      }
    } // end SWITCH
    ACE_Reactor* reactor_p = NULL;
    if (likely (reactor_impl_p))
    {
      ACE_NEW_NORETURN (reactor_p,
                        ACE_Reactor (reactor_impl_p, // implementation handle
                                     1));            // delete in dtor ?
      if (unlikely (!reactor_p))
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

        // clean up
        delete reactor_impl_p;

        return false;
      } // end IF
    } // end IF

    // make this the "default" reactor
    ACE_Reactor* previous_reactor_p =
      ACE_Reactor::instance (reactor_p, // reactor handle
                             1);        // delete in dtor ?
    if (previous_reactor_p)
      delete previous_reactor_p;
  } // end IF

  // step2: initialize proactor
  if (configuration_inout.numberOfProactorThreads)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    struct aioinit aioinit_s;
    ACE_OS::memset (&aioinit_s, 0, sizeof (aioinit_s));
    aioinit_s.aio_threads = configuration_inout.numberOfProactorThreads; // default: 20
    aioinit_s.aio_num =
      COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS;                        // default: 64
//    aioinit_s.aio_locks = 0;
//    aioinit_s.aio_usedba = 0;
//    aioinit_s.aio_debug = 0;
//    aioinit_s.aio_numusers = 0;
    aioinit_s.aio_idle_time = 1;                                         // default: 1
//    aioinit_s.aio_reserved = 0;
    aio_init (&aioinit_s);
#endif // ACE_WIN32 || ACE_WIN64

    ACE_Proactor_Impl* proactor_impl_p = NULL;
    switch (configuration_inout.proactorType)
    {
      case COMMON_PROACTOR_ACE_DEFAULT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using ACE default (platform-specific) proactor\n")));
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      case COMMON_PROACTOR_POSIX_AIOCB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX AIOCB proactor\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_AIOCB_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
      case COMMON_PROACTOR_POSIX_CB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX CB proactor\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_CB_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
      case COMMON_PROACTOR_POSIX_SIG:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX RT-signal proactor\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_SIG_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
      case COMMON_PROACTOR_POSIX_SUN:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using SunOS proactor\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_SUN_Proactor (COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS)); // parallel operations

        break;
      }
#endif // ACE_HAS_AIO_CALLS && sun
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case COMMON_PROACTOR_WIN32:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using Win32 proactor\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_WIN32_Proactor (1,       // #concurrent thread(s)/I/O completion port [0: #processors]
                                              false)); // N/A

        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      ///////////////////////////////////
      case COMMON_PROACTOR_INVALID:
      case COMMON_PROACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown proactor type (was: %d), aborting\n"),
                    configuration_inout.proactorType));
        return false;
      }
    } // end SWITCH
    ACE_Proactor* proactor_p = NULL;
    if (likely (proactor_impl_p))
    {
      ACE_NEW_NORETURN (proactor_p,
                        ACE_Proactor (proactor_impl_p, // implementation handle --> create new ?
//                                      false,  // *NOTE*: --> call close() manually
//                                              // (see finalizeEventDispatch() below)
                                      true,            // delete in dtor ?
                                      NULL));          // timer queue handle --> create new
      if (unlikely (!proactor_p))
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        return false;
      } // end IF

      // make this the "default" proactor
      ACE_Proactor* previous_proactor_p =
          ACE_Proactor::instance (proactor_p, // proactor handle
                                  1);         // delete in dtor ?
      if (previous_proactor_p)
        delete previous_proactor_p;
    } // end IF
    else
      proactor_p = ACE_Proactor::instance ();
  } // end IF

  return true;
}

ACE_THR_FUNC_RETURN
common_event_dispatch_function (void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_event_dispatch_function"));

  ACE_THR_FUNC_RETURN result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = std::numeric_limits<unsigned long>::max ();
#else
  result = arg_in;
#endif // ACE_WIN32 || ACE_WIN64

  struct Common_EventDispatchState* state_p =
    reinterpret_cast<struct Common_EventDispatchState*> (arg_in);
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();

  // sanity check(s)
  ACE_ASSERT (state_p);
  ACE_ASSERT (state_p->configuration);
  ACE_ASSERT (thread_manager_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME),
                               0);
#else
//  pid_t result_2 = syscall (SYS_gettid);
//  if (result_2 == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to syscall(SYS_gettid): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): thread (id: %t) starting\n"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME)));
#endif // _DEBUG

  int result_2 = -1;
  int group_id_i = -1;
  result_2 = thread_manager_p->get_grp (ACE_OS::thr_self (),
                                        group_id_i);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::get_grp(%t): \"%m\", returning\n")));
    return result;
  } // end IF

  // dispatch events
  if (group_id_i == state_p->reactorGroupId)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);

    // *NOTE*: transfer 'ownership' of the (select) reactor ?
    // *TODO*: determining the default ACE reactor type is not specified
    //         (platform-specific)
    if ((state_p->configuration->numberOfReactorThreads == 1) &&
        ((state_p->configuration->reactorType == COMMON_REACTOR_ACE_DEFAULT) ||
         (state_p->configuration->reactorType == COMMON_REACTOR_SELECT)))
    {
      ACE_thread_t thread_2;
      result_2 = reactor_p->owner (ACE_OS::thr_self (),
                                   &thread_2);
      if (unlikely (result_2 == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::owner(%t): \"%m\", continuing\n")));
    } // end IF

    result_2 = reactor_p->run_reactor_event_loop (NULL);
  } // end IF
  else
  {
    // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
    //                   multithreaded application, the disposition of a
    //                   particular signal is the same for all threads."
    //                   (see man 7 signal)

//    // unblock [SIGRTMIN,SIGRTMAX] IFF on POSIX AND using the
//    // ACE_POSIX_SIG_Proactor (the default)
//    // *IMPORTANT NOTE*: the proactor implementation dispatches the signals in
//    //                   worker thread(s)
//    //                   (see also: Asynch_Pseudo_Task.cpp:56)
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#else
//    ACE_POSIX_Proactor* proactor_impl_p =
//        dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
//    ACE_ASSERT (proactor_impl_p);
//    ACE_POSIX_Proactor::Proactor_Type proactor_type =
//        proactor_impl_p->get_impl_type ();
//    if (!use_reactor &&
//        (proactor_type == ACE_POSIX_Proactor::PROACTOR_SIG))
//    {
//      sigset_t original_mask;
//      Common_Tools::unblockRealtimeSignals (original_mask);
//    } // end IF
#endif // ACE_WIN32 || ACE_WIN64
    result_2 = proactor_p->proactor_run_event_loop (NULL);
  } // end ELSE
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%t) failed to handle events: \"%m\", aborting\n")));

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): thread (id: %t) leaving\n"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME)));
#endif // _DEBUG

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (result_2 != -1)
    result = static_cast<ACE_THR_FUNC_RETURN> (result_2);
#else
  result = ((result_2 == 0) ? NULL : result);
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

bool
Common_Tools::startEventDispatch (struct Common_EventDispatchState& dispatchState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::startEventDispatch"));

  // sanity check(s)
  ACE_ASSERT (dispatchState_inout.configuration);
  ACE_ASSERT (dispatchState_inout.proactorGroupId == -1);
  ACE_ASSERT (dispatchState_inout.reactorGroupId == -1);

  // reset event dispatch
  if (dispatchState_inout.configuration->numberOfReactorThreads)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
    reactor_p->reset_reactor_event_loop ();
  } // end IF

  int result = -1;
  if (dispatchState_inout.configuration->numberOfProactorThreads)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    result = proactor_p->proactor_reset_event_loop ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::proactor_reset_event_loop: \"%m\", continuing\n")));
  } // end ELSE

  // spawn worker thread(s) ?
  int loop_i = 0, group_id_i = -1;
  unsigned int number_of_threads_i = 0;
  ACE_hthread_t* thread_handles_p = NULL;
  const char** thread_names_p = NULL;
  char* thread_name_p;
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  ACE_THR_FUNC function_p =
    static_cast<ACE_THR_FUNC> (::common_event_dispatch_function);
  void* arg_p = &dispatchState_inout;
  std::string buffer;
  std::ostringstream converter;

spawn:
  number_of_threads_i =
      (!!loop_i ? dispatchState_inout.configuration->numberOfProactorThreads
                : dispatchState_inout.configuration->numberOfReactorThreads);
  // *NOTE*: if #dispatch threads == 0, event dispatch takes place in the main
  //         thread --> do not spawn any dedicated thread(s)
  if (!number_of_threads_i)
    goto continue_;

  // spawn (a group of-) thread(s)
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[number_of_threads_i]);
  if (unlikely (!thread_handles_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (ACE_hthread_t) * number_of_threads_i)));
    return false;
  } // end IF
//  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[number_of_threads_i]);
  if (unlikely (!thread_names_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (char*) * number_of_threads_i)));

    // clean up
    delete [] thread_handles_p;

    return false;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  for (unsigned int i = 0;
       i < number_of_threads_i;
       ++i)
  {
    thread_name_p = NULL;
    // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
    //         available)
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (unlikely (!thread_name_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                  sizeof (char[BUFSIZ])));

      // clean up
      delete [] thread_handles_p;
      for (unsigned int j = 0; j < i; ++j)
        delete [] thread_names_p[j];
      delete [] thread_names_p;

      return false;
    } // end IF
    ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (i + 1);
    buffer = (!loop_i ? ACE_TEXT_ALWAYS_CHAR ("proactor ")
                      : ACE_TEXT_ALWAYS_CHAR ("reactor "));
    buffer += ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME);
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::strcpy (thread_name_p, buffer.c_str ());
    thread_names_p[i] = thread_name_p;
  } // end FOR
  group_id_i =
    thread_manager_p->spawn_n (number_of_threads_i,          // # threads
                               function_p,                   // function
                               arg_p,                        // argument
                               (THR_NEW_LWP      |
                                THR_JOINABLE     |
                                THR_INHERIT_SCHED),          // flags
                               ACE_DEFAULT_THREAD_PRIORITY,  // priority
                               (!!loop_i ? COMMON_EVENT_PROACTOR_THREAD_GROUP_ID
                                         : COMMON_EVENT_REACTOR_THREAD_GROUP_ID), // group id
                               NULL,                         // task
                               thread_handles_p,             // handle(s)
                               NULL,                         // stack(s)
                               NULL,                         // stack size(s)
                               thread_names_p);              // name(s)
  if (unlikely (group_id_i == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                number_of_threads_i));

    // clean up
    delete [] thread_handles_p;
    for (unsigned int i = 0; i < number_of_threads_i; ++i)
      delete [] thread_names_p[i];
    delete [] thread_names_p;

    return false;
  } // end IF
  if (!!loop_i)
    dispatchState_inout.proactorGroupId = group_id_i;
  else
    dispatchState_inout.reactorGroupId = group_id_i;

  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//    __uint64_t thread_id = 0;
#endif
  for (unsigned int i = 0;
       i < number_of_threads_i;
       ++i)
  {
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//    ::pthread_getthreadid_np (&thread_handles_p[i], &thread_id);
#endif
    converter << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
              << ACE_TEXT_ALWAYS_CHAR (" ")
#if defined (ACE_WIN32) || defined (ACE_WIN64)
              << ::GetThreadId (thread_handles_p[i])
#else
              << thread_handles_p[i]
//              << thread_id
#endif
              << ACE_TEXT_ALWAYS_CHAR ("\n");

    // also: clean up
    delete [] thread_names_p[i];
  } // end FOR

#if defined (_DEBUG)
  buffer = converter.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): spawned %u dispatch thread(s) (group id: %d):\n%s"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME),
              number_of_threads_i,
              group_id_i,
              ACE_TEXT (buffer.c_str ())));
#endif // _DEBUG

  // clean up
  delete [] thread_handles_p;
  thread_handles_p = NULL;
  delete [] thread_names_p;
  thread_names_p = NULL;

continue_:
  if (!loop_i)
  {
    ++loop_i;
    group_id_i = -1;
    goto spawn;
  } // end IF

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch\n")));
#endif // _DEBUG

  return true;
}

void
Common_Tools::finalizeEventDispatch (int proactorGroupId_in,
                                     int reactorGroupId_in,
                                     bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeEventDispatch"));

  int result = -1;
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);

  // step1: stop default reactor/proactor
  if (proactorGroupId_in != -1)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    result = proactor_p->end_event_loop ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::end_event_loop: \"%m\", continuing\n")));

//    // *WARNING*: on UNIX; this could prevent proper signal reactivation (see
//    //            finalizeSignals())
//    result = proactor_p->close ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_Proactor::close: \"%m\", continuing\n")));
  } // end IF

  if (reactorGroupId_in != -1)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
    result = reactor_p->end_event_loop ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Reactor::end_event_loop: \"%m\", continuing\n")));
  } // end IF

  // step2: wait for any worker thread(s) ?
  if (likely (!waitForCompletion_in))
    goto continue_;

  if (proactorGroupId_in != -1)
  {
    result = thread_manager_p->wait_grp (proactorGroupId_in);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  proactorGroupId_in));
  } // end IF

  if (reactorGroupId_in != -1)
  {
    result = thread_manager_p->wait_grp (reactorGroupId_in);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  reactorGroupId_in));
  } // end IF

continue_:
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopped event dispatch\n")));
#endif // _DEBUG
}

void
Common_Tools::dispatchEvents (bool useReactor_in,
                              int groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::dispatchEvents"));

  int result = -1;

  // *NOTE*: when using a thread pool, handle things differently
  if (groupId_in != -1)
  {
    ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
    ACE_ASSERT (thread_manager_p);
    result = thread_manager_p->wait_grp (groupId_in);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  groupId_in));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("joined dispatch thread group (id was: %d)\n"),
                  groupId_in));
  } // end IF
  else
  {
    if (useReactor_in)
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      //// *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      //reactor_p->restart (1);
      result = reactor_p->run_reactor_event_loop (0);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", continuing\n")));
    } // end IF
    else
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->proactor_run_event_loop (0);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Proactor::proactor_run_event_loop(): \"%m\", continuing\n")));
    } // end ELSE
  } // end ELSE

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("event dispatch complete\n")));
#endif // _DEBUG
}

bool
Common_Tools::isInstalled (const std::string& executableName_in,
                           std::string& executablePath_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isInstalled"));

  // initialize return value(s)
  executablePath_out.clear ();
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!executableName_in.empty ());

#if defined (ACE_LINUX)
  // sanity check(s)
  unsigned int major_i = 0, minor_i = 0, micro_i = 0;
  enum Common_OperatingSystemDistributionType linux_distribution_e =
      Common_Tools::getDistribution (major_i, minor_i, micro_i);
  ACE_UNUSED_ARG (major_i); ACE_UNUSED_ARG (minor_i); ACE_UNUSED_ARG (micro_i);
  if (unlikely (linux_distribution_e == COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::getDistribution(), aborting\n")));
    return result; // *TODO*: avoid false negatives
  } // end IF

  std::string command_line_string;
  std::string command_output_string;
  switch (linux_distribution_e)
  {
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU:
    {
      // sanity check(s)
      command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_WHICH_STRING);
      command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
      command_line_string +=
          ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LOCATE_STRING);
      int exit_status_i = 0;
      if (unlikely(!Common_Tools::command (command_line_string,
                                           exit_status_i,
                                           command_output_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                    ACE_TEXT (command_line_string.c_str ())));
        return result; // *TODO*: avoid false negatives
      } // end IF
      if (unlikely(command_output_string.empty ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to 'which' \"%s\", aborting\n"),
                    ACE_TEXT (COMMON_COMMAND_LOCATE_STRING)));
        return result; // *TODO*: avoid false negatives
      } // end IF
      command_line_string = Common_Tools::strip (command_output_string);
//      ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unsupported linux distribution (was: %d), aborting\n"),
                  linux_distribution_e));
      return result; // *TODO*: avoid false negatives
    }
  } // end SWITCH
  ACE_ASSERT (!command_line_string.empty ());
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" -b '\\");
  command_line_string +=
      ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (executableName_in.c_str ()),
                                           ACE_DIRECTORY_SEPARATOR_CHAR));
  command_line_string += ACE_TEXT_ALWAYS_CHAR ("' -e -l 1");
  int exit_status_i = 0;
  if (unlikely (!Common_Tools::command (command_line_string,
                                        exit_status_i,
                                        command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (!command_output_string.empty ());

  std::istringstream converter;
  converter.str (command_output_string);
  char buffer_a [BUFSIZ];
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    if (converter.eof ())
      break; // done
    if (executablePath_out.empty ())
      executablePath_out = buffer_a;
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found executable (was: \"%s\"): \"%s\"\n"),
                ACE_TEXT (executableName_in.c_str ()),
                ACE_TEXT (buffer_a)));
#endif // _DEBUG
  } while (true);

  result = !executablePath_out.empty ();
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)
#endif // ACE_LINUX

  return result;
}
