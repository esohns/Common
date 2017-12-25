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
#include "stdafx.h"

#include "ace/Synch.h"
#include "common_tools.h"

#include <fstream>
#include <iostream>
#include <limits>
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
#include <linux/capability.h>
#include <linux/prctl.h>
#include <linux/securebits.h>
#elif defined (__sun) && defined (__SVR4)
// *NOTE*: Solaris (11)-specific
#include <rctl.h>
#endif

#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/OS.h"
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
#include "ace/OS_Memory.h"
#endif
#include "ace/POSIX_CB_Proactor.h"
#include "ace/POSIX_Proactor.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
#include "ace/SUN_Proactor.h"
#endif
#include "ace/Time_Value.h"
#include "ace/TP_Reactor.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/WIN32_Proactor.h"
#include "ace/WFMO_Reactor.h"
#else
#include "ace/Dev_Poll_Reactor.h"
#endif

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_macros.h"
#include "common_time_common.h"

#include "common_timer_manager_common.h"

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
char Common_Tools::randomStateBuffer_[BUFSIZ];

ACE_Sig_Handler Common_Tools::signalHandler_;

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

  if (!COMMON_DEBUG_DEBUGHEAP_DEFAULT_ENABLE) goto continue_;

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
#endif
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing random seed...DONE\n")));
  } // end IF
}
void
Common_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalize"));

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
#else
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
  if (status_fd != ACE_INVALID_HANDLE)
  {
    result_2 = ACE_OS::close (status_fd);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(%s): \"%m\", continuing\n"),
                  ACE_TEXT ("/proc/self/status")));
  } // end IF
#endif

  return result;
}

bool
Common_Tools::periodToString (const ACE_Time_Value& period_in,
                              std::string& timeString_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::periodToString"));

  // initialize return value(s)
  timeString_out.clear ();

  // extract hours and minutes
  ACE_Time_Value temp = period_in;
  int hours = static_cast<int> (temp.sec()) / (60 * 60);
  temp.sec (temp.sec () % (60 * 60));

  int minutes = static_cast<int> (temp.sec ()) / 60;
  temp.sec (temp.sec () % 60);

  char time_string[BUFSIZ];
  // *TODO*: rewrite this in C++...
  if (unlikely (ACE_OS::snprintf (time_string,
                                  sizeof (time_string),
                                  ACE_TEXT_ALWAYS_CHAR ("%d:%d:%d.%d"),
                                  hours,
                                  minutes,
                                  static_cast<int> (temp.sec ()),
                                  static_cast<int> (temp.usec ())) < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));
    return false;
  } // end IF

  timeString_out = time_string;

  return true;
}
bool
Common_Tools::timestampToString (const ACE_Time_Value& timeStamp_in,
                                 bool UTC_in,
                                 std::string& timeString_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::timestampToString"));

  // initialize return value(s)
  timeString_out.clear ();

  time_t timestamp = timeStamp_in.sec ();
  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  if (UTC_in)
  {
    if (unlikely (!ACE_OS::gmtime_r (&timestamp, &tm_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::gmtime_r(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF
  else
    if (unlikely (!ACE_OS::localtime_r (&timestamp, &tm_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
      return false;
    } // end IF

  char time_string[BUFSIZ];
  ACE_OS::memset (time_string, 0, BUFSIZ);
  // *TODO*: rewrite this in C++...
  if (unlikely (ACE_OS::snprintf (time_string,
                                  sizeof (time_string),
                        ACE_TEXT_ALWAYS_CHAR ("%u-%u-%u %u:%u:%u.%ld"),
                                  tm_s.tm_year + 1900,
                                  tm_s.tm_mon + 1,
                                  tm_s.tm_mday,
                                  tm_s.tm_hour,
                                  tm_s.tm_min,
                                  tm_s.tm_sec,
                                  timeStamp_in.usec ()) < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));
    return false;
  } // end IF

  timeString_out = time_string;

  return true;
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
  if (unlikely (!Common_File_Tools::load (filename_string,
                                          data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", returning\n"),
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
Common_Tools::isLinux ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isLinux"));

  int result = -1;

  // get system information
  ACE_utsname name;
  result = ACE_OS::uname (&name);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::uname(): \"%m\", aborting\n")));
    return false;
  } // end IF

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("local system info: %s (%s), %s %s, %s\n"),
//               name.nodename,
//               name.machine,
//               name.sysname,
//               name.release,
//               name.version));

  std::string kernel_name_string (name.sysname);
  return (kernel_name_string.find (ACE_TEXT_ALWAYS_CHAR ("Linux"), 0) == 0);
}

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
                ACE_TEXT ("capability (was: %u) not supported: \"%m\", aborting\n"),
                capability_in));
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

  cap_flag_value_t in_set;
  int result_2 = ::cap_get_flag (capabilities_p, capability_in,
                                 set_in, &in_set);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_get_flag(\"%s\",%d): \"%m\", aborting\n"),
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
  result = ::prctl (PR_GET_SECUREBITS,
                    0, 0, 0, 0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_GET_SECUREBITS): \"%m\", returning\n")));
    return;
  } // end IF
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P:%t: securebit set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t%s/%s...\n"),
//              ACE_TEXT ("%P:%t: securebit set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t%s/%s\nSECURE_NO_CAP_AMBIENT_RAISE:\t%s/%s...\n"),
              ((result & SECBIT_NOROOT) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
              ((result & SECBIT_NOROOT_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
              ((result & SECBIT_NO_SETUID_FIXUP) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
              ((result & SECBIT_NO_SETUID_FIXUP_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
              ((result & SECBIT_KEEP_CAPS) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
              ((result & SECBIT_KEEP_CAPS_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));
//              ((result & SECBIT_NO_CAP_AMBIENT_RAISE) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((result & SECBIT_NO_CAP_AMBIENT_RAISE_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", returning\n")));
    return;
  } // end IF

  // step2: in bounding/ambient set of the calling thread ?
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P:%t: capability bounding/ambient:effective/inheritable/permitted\n")));
  bool in_bounding_set, in_ambient_set;
  cap_flag_value_t in_effective_set, in_inheritable_set, in_permitted_set;
  char* capability_name_string_p = NULL;
  for (unsigned long capability = 0;
       capability <= CAP_LAST_CAP;
       ++capability)
  {
    capability_name_string_p = cap_to_name (capability);
    if (unlikely (!capability_name_string_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_to_name(%d): \"%m\", continuing\n"),
                  capability));
      continue;
    } // end IF

    result = ::prctl (PR_CAPBSET_READ,
                      capability, 0, 0, 0);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::prctl(PR_CAPBSET_READ,\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (capability_name_string_p)));
      goto continue_;
    } // end IF
    in_bounding_set = (result == 1);

    // *TODO*: currently, this fails on Linux (Debian)...
    result = 0;
//    result = ::prctl (PR_CAP_AMBIENT,
//                      PR_CAP_AMBIENT_IS_SET, capability, 0, 0);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ::prctl(PR_CAP_AMBIENT,PR_CAP_AMBIENT_IS_SET,\"%s\"): \"%m\", returning\n"),
//                  capability_name_string_p));
//      goto continue_;
//    } // end IF
    in_ambient_set = (result == 1);

    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_EFFECTIVE, &in_effective_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_EFFECTIVE,\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (capability_name_string_p)));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_INHERITABLE, &in_inheritable_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_INHERITABLE,\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (capability_name_string_p)));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_PERMITTED, &in_permitted_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_PERMITTED,\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (capability_name_string_p)));

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("\"%s\" (%d):\t%s/%s\t%s/%s/%s\n"),
                ACE_TEXT (capability_name_string_p), capability,
                (in_bounding_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (in_ambient_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_effective_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_inheritable_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_permitted_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

    // clean up
continue_:
    result = cap_free (capability_name_string_p);
    if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
  } // end FOR

  // clean up
  result = cap_free (capabilities_p);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
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
                ACE_TEXT ("capability (was: %d) not supported: \"%m\", aborting\n"),
                capability_in));
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
  cap_value_t capabilities_a[CAP_LAST_CAP + 1];
  if (likely (set_in == CAP_EFFECTIVE))
  {
    // verify that the capability is in the 'permitted' set
    cap_flag_value_t in_permitted_set;
    result_2 = ::cap_get_flag (capabilities_p, capability_in,
                               CAP_PERMITTED, &in_permitted_set);
    if (unlikely (result_2 == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(\"%s\",%d): \"%m\", aborting\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                  CAP_PERMITTED));
      goto clean;
    } // end IF
    if (unlikely (in_permitted_set != CAP_SET))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%t: capability \"%s\" not in permitted set: cannot enable, aborting\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ())));
      ACE_OS::last_error (EPERM);
      goto clean;
    } // end IF
  } // end IF

  ACE_OS::memset (&capabilities_a, 0, sizeof (cap_value_t[CAP_LAST_CAP + 1]));
  capabilities_a[0] = capability_in;
  result_2 = ::cap_set_flag (capabilities_p, set_in,
                             1, capabilities_a,
                             CAP_SET);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_flag(\"%s\",%d,CAP_SET): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF
  ACE_ASSERT (Common_Tools::hasCapability (capability_in, set_in));
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
                ACE_TEXT ("capability (was: %d) not supported: \"%m\", aborting\n"),
                capability_in));
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
                ACE_TEXT ("failed to ::cap_set_flag(\"%s\",%d,CAP_CLEAR): \"%m\", aborting\n"),
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Common_Tools::setRootPrivileges ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::setRootPrivileges"));

  uid_t effective_user_id = 0; // <-- root
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  int result = ACE_OS::seteuid (effective_user_id);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::seteuid(%u): \"%m\", aborting\n"),
                effective_user_id));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("set effective user id to: %u\n"),
                effective_user_id));

  return (result == 0);
}
void
Common_Tools::dropRootPrivileges ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::dropRootPrivileges"));

  uid_t real_user_id = ACE_OS::getuid ();
  int result = ACE_OS::seteuid (real_user_id);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::seteuid(%u): \"%m\", continuing\n"),
                real_user_id));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("reset effective user id to: %u\n"),
                real_user_id));
}
void
Common_Tools::printPrivileges ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::printPrivileges"));

  uid_t real_user_id = ACE_OS::getuid ();
  uid_t effective_user_id = ACE_OS::geteuid ();
  gid_t real_user_group = ACE_OS::getgid ();
  gid_t effective_user_group = ACE_OS::getegid ();

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P:%t: real/effective user id/group: %u/%u\t%u/%u\n"),
              real_user_id, effective_user_id,
              real_user_group, effective_user_group));
}
#endif

bool
Common_Tools::enableCoreDump (bool enable_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::enableCoreDump"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#else
  int result =
      ::prctl (PR_SET_DUMPABLE,
               (enable_in ? 1 : 0), 0, 0, 0);
//               (enable_in ? SUID_DUMP_USER : SUID_DUMP_DISABLE), 0, 0, 0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_SET_DUMPABLE): \"%m\", returning\n")));
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
Common_Tools::getCurrentUserName (std::string& username_out,
                                  std::string& realname_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getCurrentUserName"));

  // initialize return value(s)
  username_out.clear ();
  realname_out.clear ();

  char user_name[ACE_MAX_USERID];
  ACE_OS::memset (user_name, 0, sizeof (user_name));
  if (unlikely (!ACE_OS::cuserid (user_name,
                                  ACE_MAX_USERID)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ACE_OS::cuserid(): \"%m\", falling back\n")));

    username_out =
      ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_DEF_USER_LOGIN_BASE)));

    return;
  } // end IF
  username_out = user_name;

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  int            success = -1;
  struct passwd  pwd;
  struct passwd* pwd_result = NULL;
  char           buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
//  size_t         bufsize = 0;
//  bufsize = sysconf (_SC_GETPW_R_SIZE_MAX);
//  if (bufsize == -1)        /* Value was indeterminate */
//    bufsize = 16384;        /* Should be more than enough */

  uid_t user_id = ACE_OS::geteuid ();
  success = ::getpwuid_r (user_id,         // user id
                          &pwd,            // passwd entry
                          buffer,          // buffer
                          sizeof (buffer), // buffer size
                          &pwd_result);    // result (handle)
  if (unlikely (!pwd_result))
  {
    if (success == 0)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("user \"%u\" not found, falling back\n"),
                  user_id));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to ACE_OS::getpwuid_r(%u): \"%m\", falling back\n"),
                  user_id));
    username_out = ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_DEF_USER_LOGIN_BASE)));
  } // end IF
  else
    realname_out = pwd.pw_gecos;
#else
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
#endif
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
Common_Tools::preInitializeSignals (ACE_Sig_Set& signals_inout,
                                    bool useReactor_in,
                                    Common_SignalActions_t& previousActions_out,
                                    sigset_t& originalMask_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::preInitializeSignals"));

  int result = -1;

  // initialize return value(s)
  previousActions_out.clear ();
  result = ACE_OS::sigemptyset (&originalMask_out);
  if (unlikely (result == - 1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
    return false;
  } // end IF

  // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  // step1: ignore SIGPIPE: continue gracefully after a client suddenly
  // disconnects (i.e. application/system crash, etc...)
  // --> specify ignore action
  // *IMPORTANT NOTE*: do NOT restart system calls in this case (see manual)
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                0);                                       // flags
  ACE_Sig_Action previous_action;
  result = ignore_action.register_action (SIGPIPE,
                                          &previous_action);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", continuing\n"),
                SIGPIPE, SIGPIPE));
  else
    previousActions_out[SIGPIPE] = previous_action;

  // step2: block certain signals
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *IMPORTANT NOTE*: child threads inherit the signal mask of their parent
  //                   --> make sure this is the main thread

  // step2a: block [SIGRTMIN,SIGRTMAX] iff the default[/current (!)] proactor
  // implementation happens to be ACE_POSIX_Proactor::PROACTOR_SIG (i.e. Linux,
  // ...)

  // *NOTE*: the ACE_POSIX_Proactor::PROACTOR_SIG implementation also blocks the
  //         RT signal(s) in its ctor
  // --> see also: POSIX_Proactor.cpp:1604,1653
  //         ...and later dispatches the signals in worker thread(s) running the
  //         event loop
  // --> see also: POSIX_Proactor.cpp:1864
  // --> see also: man sigwaitinfo, man 7 signal

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  //                   --> on POSIX platforms, ensure that ACE_SIGRTMIN == 34

  if (!useReactor_in)
  {
    ACE_POSIX_Proactor::Proactor_Type proactor_type;
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    ACE_POSIX_Proactor* proactor_impl_p =
        dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    if (!proactor_impl_p)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("dynamic_cast<ACE_POSIX_Proactor> failed, continuing\n")));
      goto _continue;
    } // end IF
    proactor_type = proactor_impl_p->get_impl_type ();
    if (proactor_type != ACE_POSIX_Proactor::PROACTOR_SIG)
      goto _continue;

    sigset_t rt_signal_set;
    result = ACE_OS::sigemptyset (&rt_signal_set);
    if (unlikely (result == - 1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
      return false;
    } // end IF
    unsigned int number = 0;
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         ++i, number++)
    {
      result = ACE_OS::sigaddset (&rt_signal_set, i);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sigaddset(%d: %S): \"%m\", continuing\n"),
                    i, i));
        number--;
      } // end IF

      // remove any RT signals from the signal set handled by the application
      if (signals_inout.is_member (i))
      {
        result = signals_inout.sig_del (i); // <-- let the event dispatch handle
                                            //     all RT signals
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: %S): \"%m\", continuing\n"),
                      i, i));
//        else
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("removed %d: %S from handled signals\n"),
//                      i, i));
      } // end IF
    } // end IF
    result = ACE_OS::thr_sigsetmask (SIG_BLOCK,
                                     &rt_signal_set,
                                     &originalMask_out);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("[%d: %S - %d: %S]: blocked %d real-time signal(s)\n"),
                ACE_SIGRTMIN, ACE_SIGRTMIN, ACE_SIGRTMAX, ACE_SIGRTMAX,
                number));
  } // end IF
_continue:
#endif

  // *NOTE*: remove SIGSEGV to enable core dumps on non-Win32 systems
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (signals_inout.is_member (SIGSEGV))
  {
    result = signals_inout.sig_del (SIGSEGV);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: %S): \"%m\", continuing\n"),
                  SIGSEGV, SIGSEGV));
//    else
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("removed %d: %S from handled signals\n"),
//                  SIGSEGV, SIGSEGV));
  } // end IF
#endif

  return true;
}

bool
Common_Tools::initializeSignals (enum Common_SignalDispatchType dispatch_in,
                                 const ACE_Sig_Set& signals_in,
                                 const ACE_Sig_Set& ignoreSignals_in,
                                 ACE_Event_Handler* eventHandler_in,
                                 Common_SignalActions_t& previousActions_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeSignals"));

  // initialize return value(s)
  previousActions_out.clear ();

  // *NOTE*: "The signal disposition is a per-process attribute: in a
  //         multithreaded application, the disposition of a particular signal
  //         is the same for all threads." (see man(7) signal)

  // step1: backup previous actions
  ACE_Sig_Action previous_action;
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
    if (signals_in.is_member (i))
    {
      previous_action.retrieve_action (i);
      previousActions_out[i] = previous_action;
    } // end IF

  // step2: ignore[/block] certain signals

  // step2a: ignore certain signals
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                SA_RESTART);                              // flags
  int result = -1;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (ignoreSignals_in.is_member (i))
    {
      result = ignore_action.register_action (i,
                                              &previous_action);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", continuing\n"),
                    i, i));
      else
      {
        previousActions_out[i] = previous_action;
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("ignoring signal %d: %S\n"),
//                    i, i));
      } // end IF
    } // end IF

  // step3: register (process-wide) signal handler
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action default_action (static_cast<ACE_SignalHandler> (SIG_DFL), // default action
                                 ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                           // --> block them all (bar KILL/STOP; see manual)
                                 (SA_RESTART | SA_SIGINFO));               // flags
  switch (dispatch_in)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      ACE_Event_Handler* event_handler_p = NULL;
      ACE_Sig_Action previous_action;
      for (int i = 1;
           i < ACE_NSIG;
           ++i)
        if (signals_in.is_member (i))
        {
          result =
              Common_Tools::signalHandler_.register_handler (i,
                                                             eventHandler_in,
                                                             &default_action,
                                                             &event_handler_p,
                                                             &previous_action);
          if (unlikely (result == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Sig_Handler::register_handler(%d: %S): \"%m\", aborting\n"),
                        i, i));
            return false;
          } // end IF
//          else
//            ACE_DEBUG ((LM_DEBUG,
//                        ACE_TEXT ("handling signal %d: \"%S\"\n"),
//                        i, i));

          // clean up
          if (event_handler_p)
          {
            delete event_handler_p;
            event_handler_p = NULL;
          } // end IF
        } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->register_handler (signals_in,
                                            eventHandler_in,
                                            &default_action);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));
        return false;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown signal dispatch mode (was: %d), aborting\n"),
                  dispatch_in));
      return false;
    }
  } // end SWITCH

#if defined (_DEBUG)
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
    if (signals_in.is_member (i))
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("handling signal %d: \"%S\"\n"),
                  i, i));
#endif

  return true;
}

void
Common_Tools::finalizeSignals (enum Common_SignalDispatchType dispatch_in,
                               const ACE_Sig_Set& signals_in,
                               const Common_SignalActions_t& previousActions_in,
                               const sigset_t& previousMask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeSignals"));

  ACE_UNUSED_ARG (previousMask_in);

  int result = -1;

  // step1: restore previous signal handlers
  switch (dispatch_in)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      ACE_Sig_Action previous_action;
      Common_SignalActionsIterator_t iterator;
      for (int i = 1;
           i < ACE_NSIG;
           ++i)
        if (signals_in.is_member (i))
        {
          iterator = previousActions_in.find (i);
          ACE_ASSERT (iterator != previousActions_in.end ());
          result =
              Common_Tools::signalHandler_.remove_handler (i,
                                                           const_cast<ACE_Sig_Action*> (&(*iterator).second),
                                                           &previous_action,
                                                           -1);
          if (unlikely (result == -1))
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Sig_Handler::remove_handler(%d: %S): \"%m\", continuing\n"),
                        i, i));
        } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->remove_handler (signals_in);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::remove_handler(): \"%m\", continuing\n")));

      for (Common_SignalActionsIterator_t iterator = previousActions_in.begin ();
           iterator != previousActions_in.end ();
           iterator++)
      {
        result =
            const_cast<ACE_Sig_Action&> ((*iterator).second).register_action ((*iterator).first,
                                                                              NULL);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                      (*iterator).first));
      } // end FOR

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown signal dispatch mode (was: %d), continuing\n"),
                  dispatch_in));
      break;
    }
  } // end SWITCH

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // step2: restore previous signal mask
  result = ACE_OS::thr_sigsetmask (SIG_SETMASK,
                                   &previousMask_in,
                                   NULL);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", continuing\n")));
#endif
}

std::string
Common_Tools::signalToString (const Common_Signal& signal_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::signalToString"));

  // initialize return value
  std::string result;

  std::ostringstream converter;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (signal_in.signal)
  {
    case SIGINT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGINT"); break;
    case SIGILL:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGILL"); break;
    case SIGFPE:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGFPE"); break;
    case SIGSEGV:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGSEGV"); break;
    case SIGTERM:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGTERM"); break;
    case SIGBREAK:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGBREAK"); break;
    case SIGABRT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGABRT"); break;
    case SIGABRT_COMPAT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGABRT_COMPAT"); break;
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid/unknown signal: %S, continuing\n"),
                  signal_in.signal));
      break;
    }
  } // end SWITCH

  result += ACE_TEXT_ALWAYS_CHAR (", signalled handle: ");
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result += ACE_TEXT_ALWAYS_CHAR ("0x");
#endif
  converter << signal_in.siginfo.si_handle_;
  result += converter.str ();
  //result += ACE_TEXT_ALWAYS_CHAR (", array of signalled handle(s): ");
  //result += signal_in.siginfo.si_handles_;
#else
  int result_2 = -1;

  // step0: common information (on POSIX.1b)
  result += ACE_TEXT_ALWAYS_CHAR ("PID/UID: ");
  converter << signal_in.siginfo.si_pid;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("/");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_uid;
  result += converter.str ();

  // (try to) get user name
  ACE_TCHAR buffer_a[BUFSIZ];
  //  ACE_TCHAR buffer[L_cuserid];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
//  char* result_p = ACE_OS::cuserid (buffer_a);
//  if (!result_p)
  struct passwd passwd_s;
  struct passwd* passwd_p = NULL;
// *PORTABILITY*: this isn't very portable (man getpwuid_r)
  result_2 = ::getpwuid_r (signal_in.siginfo.si_uid,
                           &passwd_s,
                           buffer_a,
                           sizeof (ACE_TCHAR[BUFSIZ]),
                           &passwd_p);
  if (unlikely (result_2 || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u) : \"%m\", continuing\n"),
                signal_in.siginfo.si_uid));
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::cuserid() : \"%m\", continuing\n")));
  } // end IF
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("[\"");
    result += passwd_s.pw_name;
//    information << buffer;
    result += ACE_TEXT_ALWAYS_CHAR ("\"]");
  } // end ELSE

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  result += ACE_TEXT_ALWAYS_CHAR (", errno: ");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_errno;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("[\"");
  result += ACE_OS::strerror (signal_in.siginfo.si_errno);
  result += ACE_TEXT_ALWAYS_CHAR ("\"], code: ");

  // step1: retrieve signal code
  switch (signal_in.siginfo.si_code)
  {
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_NOINFO:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_NOINFO"); break;
  case SI_DTRACE:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_DTRACE"); break;
  case SI_RCTL:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_RCTL"); break;
/////////////////////////////////////////
#endif
  case SI_USER:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_USER"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_LWP:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_LWP"); break;
#else
  case SI_KERNEL:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_KERNEL"); break;
#endif
    case SI_QUEUE:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_QUEUE"); break;
    case SI_TIMER:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_TIMER"); break;
    case SI_ASYNCIO:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_ASYNCIO"); break;
    case SI_MESGQ:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_MESGQ"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_LWP_QUEUE:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_LWP_QUEUE"); break;
#else
  case SI_SIGIO:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_SIGIO"); break;
    case SI_TKILL:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_TKILL"); break;
#endif
  default:
    { // (signal-dependant) codes...
      switch (signal_in.signal)
      {
        case SIGILL:
        {
          switch (signal_in.siginfo.si_code)
          {
            case ILL_ILLOPC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPC"); break;
            case ILL_ILLOPN:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPN"); break;
            case ILL_ILLADR:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLADR"); break;
            case ILL_ILLTRP:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLTRP"); break;
            case ILL_PRVOPC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_PRVOPC"); break;
            case ILL_PRVREG:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_PRVREG"); break;
            case ILL_COPROC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_COPROC"); break;
            case ILL_BADSTK:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_BADSTK"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGFPE:
        {
          switch (signal_in.siginfo.si_code)
          {
            case FPE_INTDIV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_INTDIV"); break;
            case FPE_INTOVF:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_INTOVF"); break;
            case FPE_FLTDIV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTDIV"); break;
            case FPE_FLTOVF:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTOVF"); break;
            case FPE_FLTUND:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTUND"); break;
            case FPE_FLTRES:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTRES"); break;
            case FPE_FLTINV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTINV"); break;
            case FPE_FLTSUB:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTSUB"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGSEGV:
        {
          switch (signal_in.siginfo.si_code)
          {
            case SEGV_MAPERR:
              result += ACE_TEXT_ALWAYS_CHAR ("SEGV_MAPERR"); break;
            case SEGV_ACCERR:
              result += ACE_TEXT_ALWAYS_CHAR ("SEGV_ACCERR"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGBUS:
        {
          switch (signal_in.siginfo.si_code)
          {
            case BUS_ADRALN:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_ADRALN"); break;
            case BUS_ADRERR:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_ADRERR"); break;
            case BUS_OBJERR:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_OBJERR"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGTRAP:
        {
          switch (signal_in.siginfo.si_code)
          {
            case TRAP_BRKPT:
              result += ACE_TEXT_ALWAYS_CHAR ("TRAP_BRKPT"); break;
            case TRAP_TRACE:
              result += ACE_TEXT_ALWAYS_CHAR ("TRAP_TRACE"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGCHLD:
        {
          switch (signal_in.siginfo.si_code)
          {
            case CLD_EXITED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_EXITED"); break;
            case CLD_KILLED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_KILLED"); break;
            case CLD_DUMPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_DUMPED"); break;
            case CLD_TRAPPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_TRAPPED"); break;
            case CLD_STOPPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_STOPPED"); break;
            case CLD_CONTINUED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_CONTINUED"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGPOLL:
        {
          switch (signal_in.siginfo.si_code)
          {
            case POLL_IN:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_IN"); break;
            case POLL_OUT:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_OUT"); break;
            case POLL_MSG:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_MSG"); break;
            case POLL_ERR:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_ERR"); break;
            case POLL_PRI:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_PRI"); break;
            case POLL_HUP:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_HUP"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                      signal_in.siginfo.si_code));
          break;
        }
      } // end SWITCH

      break;
    }
  } // end SWITCH

  // step2: retrieve more (signal-dependant) information
  switch (signal_in.signal)
  {
    case SIGALRM:
    {
#if defined (__linux__)
      result += ACE_TEXT_ALWAYS_CHAR (", overrun: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_overrun;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", (internal) id: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_timerid;
      result += converter.str ();
#endif
      break;
    }
    case SIGCHLD:
    {
      result += ACE_TEXT_ALWAYS_CHAR (", (exit) status: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_status;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", time consumed (user): ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_utime;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" / (system): ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_stime;
      result += converter.str ();
      break;
    }
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    {
      // *TODO*: more data ?
      result += ACE_TEXT_ALWAYS_CHAR (", fault at address: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_addr;
      result += converter.str ();
      break;
    }
    case SIGPOLL:
    {
      result += ACE_TEXT_ALWAYS_CHAR (", band event: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_band;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", (file) descriptor: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_fd;
      result += converter.str ();
      break;
    }
    default:
    {
      // *TODO*: handle more signals here ?
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("no additional information for signal: \"%S\"\n"),
//                   signal_in.signal));
      break;
    }
  } // end SWITCH
#endif

  return result;
}

bool
Common_Tools::initializeTimers (const struct Common_TimerConfiguration& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeTimers"));

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  //Common_Timer_Manager_Asynch_t* timer_manager_2 =
  //  COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  //ACE_ASSERT (timer_manager_2);

  switch (configuration_in.dispatch)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();

      return true;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();
      if (unlikely (!timer_manager_p->isRunning ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to start timer manager, aborting\n")));
        return false;
      } // end IF

      return true;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();

      return true;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (false);
      
      ACE_NOTREACHED (return false;)
      //if (!timer_manager_2->initialize (configuration_in))
      //{
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to initialize timer manager, aborting\n")));
      //  return false;
      //} // end IF

      //timer_manager_2->start ();

      //return true;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer dispatch (was: %d), returning\n"),
                  configuration_in.dispatch));
      break;
    }
  } // end SWITCH

  return false;
}

void
Common_Tools::finalizeTimers (const struct Common_TimerConfiguration& configuration_in,
                              bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeTimers"));

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  //Common_Timer_Manager_Asynch_t* timer_manager_2 =
  //  COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  //ACE_ASSERT (timer_manager_2);

  switch (configuration_in.dispatch)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP;

      ACE_NOTREACHED (return;)
      //timer_manager_2->stop (waitForCompletion_in,
      //                       true);
      //break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer dispatch (was: %d), returning\n"),
                  configuration_in.dispatch));
      break;
    }
  } // end SWITCH
}

bool
Common_Tools::initializeEventDispatch (bool useReactor_in,
                                       bool useThreadPool_in,
                                       unsigned int numberOfThreads_in,
                                       Common_ProactorType& proactorType_out,
                                       Common_ReactorType& reactorType_out,
                                       bool& serializeOutput_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeEventDispatch"));

  // initialize return value(s)
  serializeOutput_out = false;

  // step0: initialize reactor/proactor implementation
  // *NOTE*: the appropriate type of event dispatch mechanism may depend on
  //         several factors:
  //         - targeted platform(s) (i.e. supported mechanisms, (software) environment)
  //         - envisioned application type (i.e. library, plugin)
  // *TODO*: the chosen event dispatch implementation should be based on
  //         #defines only (!, see above, common.h)
  reactorType_out = COMMON_EVENT_REACTOR_TYPE;
  proactorType_out = COMMON_EVENT_PROACTOR_TYPE;
  if (useReactor_in)
    reactorType_out = (useThreadPool_in ? COMMON_REACTOR_THREAD_POOL
                                        : reactorType_out);

  // step1: initialize reactor/proactor
  if (useReactor_in)
  {
    ACE_Reactor_Impl* reactor_impl_p = NULL;
    switch (reactorType_out)
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

        serializeOutput_out = true;

        break;
      }
#endif
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

        serializeOutput_out = true;

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
#endif
      ///////////////////////////////////
      case COMMON_REACTOR_INVALID:
      case COMMON_REACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown reactor type (was: %d), aborting\n"),
                    reactorType_out));
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

    // make this the "default" reactor...
    ACE_Reactor* previous_reactor_p =
      ACE_Reactor::instance (reactor_p, // reactor handle
                             1);        // delete in dtor ?
    if (previous_reactor_p)
      delete previous_reactor_p;
  } // end IF
  else
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    struct aioinit aioinit_s;
    ACE_OS::memset (&aioinit_s, 0, sizeof (aioinit_s));
    aioinit_s.aio_threads = numberOfThreads_in;   // default: 20
    aioinit_s.aio_num =
      COMMON_EVENT_PROACTOR_POSIX_AIO_OPERATIONS; // default: 64
//    aioinit_s.aio_locks = 0;
//    aioinit_s.aio_usedba = 0;
//    aioinit_s.aio_debug = 0;
//    aioinit_s.aio_numusers = 0;
    aioinit_s.aio_idle_time = 1;                  // default: 1
//    aioinit_s.aio_reserved = 0;
    aio_init (&aioinit_s);
#endif

    ACE_Proactor_Impl* proactor_impl_p = NULL;
    switch (proactorType_out)
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
#endif
#endif
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
#endif
      ///////////////////////////////////
      case COMMON_PROACTOR_INVALID:
      case COMMON_PROACTOR_MAX:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown proactor type (was: %d), aborting\n"),
                    proactorType_out));
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
    } // end IF

    // make this the "default" proactor
    ACE_Proactor* previous_proactor_p =
        ACE_Proactor::instance (proactor_p, // proactor handle
                                1);         // delete in dtor ?
    if (previous_proactor_p)
      delete previous_proactor_p;
  } // end ELSE

  return true;
}

ACE_THR_FUNC_RETURN
threadpool_event_dispatcher_function (void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("::threadpool_event_dispatcher_function"));

  ACE_THR_FUNC_RETURN result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = std::numeric_limits<unsigned long>::max ();
#else
  result = arg_in;
#endif
  struct Common_DispatchThreadData* thread_data_p =
    reinterpret_cast<struct Common_DispatchThreadData*> (arg_in);
  ACE_ASSERT (thread_data_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker starting...\n")));
#else
//  pid_t result_2 = syscall (SYS_gettid);
//  if (result_2 == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to syscall(SYS_gettid): \"%m\", continuing\n")));
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t --> %d) worker starting...\n"),
//              result_2));
#endif

  // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
  //                   multithreaded application, the disposition of a
  //                   particular signal is the same for all threads."
  //                   (see man 7 signal)

  // handle any events
  int result_2 = -1;
  if (thread_data_p->useReactor)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);

    // *NOTE*: transfer 'ownership' of the (select) reactor ?
    // *TODO*: determining the default ACE reactor type is not specified
    //         (platform-specific)
    if ((thread_data_p->numberOfDispatchThreads == 1) &&
        ((thread_data_p->reactorType == COMMON_REACTOR_ACE_DEFAULT) ||
         (thread_data_p->reactorType == COMMON_REACTOR_SELECT)))
    {
      ACE_thread_t thread_2 = -1;
      result_2 = reactor_p->owner (ACE_Thread::self (), &thread_2);
      if (unlikely (result_2 == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::owner(%t): \"%m\", continuing\n")));
    } // end IF

    result_2 = reactor_p->run_reactor_event_loop (NULL);
  } // end IF
  else
  {
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
#endif
    result_2 = proactor_p->proactor_run_event_loop (NULL);
  } // end ELSE
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%t) failed to handle events: \"%m\", aborting\n")));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker leaving\n")));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (result_2 != -1)
    result = static_cast<ACE_THR_FUNC_RETURN> (result_2);
#else
  result = ((result_2 == 0) ? NULL : result);
#endif

  return result;
}

bool
Common_Tools::startEventDispatch (const struct Common_DispatchThreadData& threadData_in,
                                  int& groupId_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::startEventDispatch"));

  int result = -1;

  // initialize return value(s)
  groupId_out = -1;

  // reset event dispatch
  if (threadData_in.useReactor)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
    reactor_p->reset_reactor_event_loop ();
  } // end IF
  else
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    result = proactor_p->proactor_reset_event_loop ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::proactor_reset_event_loop: \"%m\", continuing\n")));
  } // end ELSE

  // spawn worker(s) ?
  // *NOTE*: if #dispatch threads == 0, event dispatch takes place in the main
  //         thread --> do not spawn any workers
  if (threadData_in.numberOfDispatchThreads == 0)
    return true;

  // start a (group of) worker thread(s)...
  ACE_hthread_t* thread_handles_p = NULL;
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[threadData_in.numberOfDispatchThreads]);
  if (unlikely (!thread_handles_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (ACE_hthread_t) * threadData_in.numberOfDispatchThreads)));
    return false;
  } // end IF
//  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  const char** thread_names_p = NULL;
  // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
  //         available)
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[threadData_in.numberOfDispatchThreads]);
  if (unlikely (!thread_names_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (char*) * threadData_in.numberOfDispatchThreads)));

    // clean up
    delete [] thread_handles_p;

    return false;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  char* thread_name_p;
  std::string buffer;
  std::ostringstream converter;
  for (unsigned int i = 0;
       i < threadData_in.numberOfDispatchThreads;
       i++)
  {
    thread_name_p = NULL;
    // *TODO*: use ACE_NEW_MALLOC_ARRAY (as soon as the NORETURN variant becomes
    //         available)
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (unlikely (!thread_name_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));

      // clean up
      delete [] thread_handles_p;
      for (unsigned int j = 0; j < i; j++)
        delete [] thread_names_p[j];
      delete [] thread_names_p;

      return false;
    } // end IF
    ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (i + 1);
    buffer = ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_THREAD_NAME);
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::strcpy (thread_name_p, buffer.c_str ());
    thread_names_p[i] = thread_name_p;
  } // end FOR
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  ACE_THR_FUNC function_p =
    static_cast<ACE_THR_FUNC> (::threadpool_event_dispatcher_function);
  void* arg_p = &const_cast<struct Common_DispatchThreadData&> (threadData_in);
  groupId_out =
    thread_manager_p->spawn_n (threadData_in.numberOfDispatchThreads, // # threads
                               function_p,                            // function
                               arg_p,                                 // argument
                               (THR_NEW_LWP      |
                                THR_JOINABLE     |
                                THR_INHERIT_SCHED),                   // flags
                               ACE_DEFAULT_THREAD_PRIORITY,           // priority
                               COMMON_EVENT_THREAD_GROUP_ID,          // group id
                               NULL,                                  // task
                               thread_handles_p,                      // handle(s)
                               NULL,                                  // stack(s)
                               NULL,                                  // stack size(s)
                               thread_names_p);                       // name(s)
  if (unlikely (groupId_out == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                threadData_in.numberOfDispatchThreads));

    // clean up
    delete [] thread_handles_p;
    for (unsigned int i = 0; i < threadData_in.numberOfDispatchThreads; i++)
      delete [] thread_names_p[i];
    delete [] thread_names_p;

    return false;
  } // end IF

  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//    __uint64_t thread_id = 0;
#endif
  for (unsigned int i = 0; i < threadData_in.numberOfDispatchThreads; i++)
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

  buffer = converter.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): spawned %u dispatch thread(s) (group: %d):\n%s"),
              ACE_TEXT (COMMON_EVENT_THREAD_NAME),
              threadData_in.numberOfDispatchThreads,
              groupId_out,
              ACE_TEXT (buffer.c_str ())));

  // clean up
  delete [] thread_handles_p;
  delete [] thread_names_p;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch\n")));

  return true;
}

void
Common_Tools::finalizeEventDispatch (bool stopReactor_in,
                                     bool stopProactor_in,
                                     int groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeEventDispatch"));

  int result = -1;

  // step1: stop reactor/proactor
  // *IMPORTANT NOTE*: some proactor implementations start a pseudo-task that
  //                   runs the (default) reactor --> stop that as well
  if (stopReactor_in || stopProactor_in)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
    result = reactor_p->end_event_loop ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Reactor::end_event_loop: \"%m\", continuing\n")));
  } // end IF

  if (stopProactor_in)
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

  // step2: wait for any worker(s) ?
  if (groupId_in != -1)
  {
    ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
    ACE_ASSERT (thread_manager_p);
    result = thread_manager_p->wait_grp (groupId_in);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  groupId_in));
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopped event dispatch\n")));
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
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("event dispatch complete\n")));
  } // end ELSE
}

//void
//Common_Tools::unblockRealtimeSignals (sigset_t& originalMask_out)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Tools::unblockRealtimeSignals"));
//
//  int result = -1;
//
//  // initialize return value(s)
//  result = ACE_OS::sigemptyset (&originalMask_out);
//  if (result == - 1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
//    return;
//  } // end IF
//
//  sigset_t signal_set;
//  result = ACE_OS::sigemptyset (&signal_set);
//  if (result == - 1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  for (int i = ACE_SIGRTMIN;
//       i <= ACE_SIGRTMAX;
//       i++)
//  {
//    result = ACE_OS::sigaddset (&signal_set, i);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::sigaddset(): \"%m\", returning\n")));
//      return;
//    } // end IF
//  } // end FOR
//
//  result = ACE_OS::thr_sigsetmask (SIG_UNBLOCK,
//                                   &signal_set,
//                                   &originalMask_out);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(SIG_UNBLOCK): \"%m\", returning\n")));
//    return;
//  } // end IF
//}
