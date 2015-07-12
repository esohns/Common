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

#include <iostream>
#include <fstream>
#include <sstream>

// *WORKAROUND*
using namespace std;
// *IMPORTANT NOTE*: several ACE headers inclue ace/iosfwd.h, which introduces
//                   a problem in conjunction with the standard include headers
//                   when ACE_USES_OLD_IOSTREAMS is defined
//                   --> include the necessary headers manually (see above), and
//                       prevent ace/iosfwd.h from causing any harm
#define ACE_IOSFWD_H

#if defined (_MSC_VER)
#include <Security.h>
// *NOTE*: Solaris (11)-specific
#elif defined (__sun) && defined (__SVR4)
#include <rctl.h>
#else
//#include <syscall.h>
#endif

#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Backend.h"
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
#include "ace/WFMO_Reactor.h"
#else
#include "ace/Dev_Poll_Reactor.h"
#endif

#if defined (LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
#include "valgrind/memcheck.h"
#endif

#include "common_tools.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_timer_manager_common.h"

void
Common_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initialize"));

#if defined (LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("running on valgrind...\n")));
#endif

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...\n")));
  //ACE_High_Res_Timer::calibrate (500000, 10);
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...done\n")));
}

Common_Timer_Manager_t*
Common_Tools::getTimerManager ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getTimerManager"));

  return COMMON_TIMERMANAGER_SINGLETON::instance ();
}

bool
Common_Tools::period2String (const ACE_Time_Value& period_in,
                             std::string& timeString_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::period2String"));

  // init return value(s)
  timeString_out.clear ();

  // extract hours and minutes...
  ACE_Time_Value temp = period_in;
  int hours = static_cast<int> (temp.sec()) / (60 * 60);
  temp.sec (temp.sec () % (60 * 60));

  int minutes = static_cast<int> (temp.sec ()) / 60;
  temp.sec (temp.sec () % 60);

  char time_string[BUFSIZ];
  // *TODO*: rewrite this in C++...
  if (ACE_OS::snprintf (time_string,
                        sizeof (time_string),
                        ACE_TEXT_ALWAYS_CHAR ("%d:%d:%d.%d"),
                        hours,
                        minutes,
                        static_cast<int> (temp.sec ()),
                        static_cast<int> (temp.usec ())) < 0)
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

  // *TODO*: remove tabs & other non-printable characters
  std::string::size_type current_space = std::string::npos;
  while ((current_space = result.find (' ',
                                       0)) == 0)
    result.erase (current_space, 1);
  while ((current_space = result.rfind (' ',
                                        std::string::npos)) == (result.size () - 1))
    result.erase (current_space, 1);

  return result;
}

bool
Common_Tools::isLinux ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::isLinux"));

  int result = -1;

  // get system information
  ACE_utsname name;
  result = ACE_OS::uname (&name);
  if (result == -1)
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

  std::string kernel (name.sysname);
  return (kernel.find (ACE_TEXT_ALWAYS_CHAR ("Linux"), 0) == 0);
}

bool
Common_Tools::setResourceLimits (bool fileDescriptors_in,
                                 bool stackTraces_in,
                                 bool pendingSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::setResourceLimits"));

  int result = -1;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    rlimit resource_limit;
#endif

  if (fileDescriptors_in)
  {
// *PORTABILITY*: this is almost entirely non-portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (result == -1)
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
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset file descriptor limits, now: [soft: %u, hard: %u]...\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#else
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("file descriptor limits are not available on this platform, continuing\n")));
#endif
  } // end IF

// -----------------------------------------------------------------------------

  if (stackTraces_in)
  {
    // *PORTABILITY*: this is almost entirely non-portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_CORE,
                                &resource_limit);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset corefile limits, now: [soft: %u, hard: %u]...\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#else
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("corefile limits are not available on this platform, continuing\n")));
#endif
  } // end IF

  if (pendingSignals_in)
  {
    // *PORTABILITY*: this is almost entirely non-portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
    if (!block_p)
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
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
      return false;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset pending signal limits, now: [soft: %u, hard: %u]...\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
#else
    // *TODO*: really ?
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("pending signal limits are not available on this platform, continuing\n")));
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
  if (!ACE_OS::cuserid (user_name, ACE_MAX_USERID))
  {
    ACE_DEBUG ((LM_ERROR,
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
  if (pwd_result == NULL)
  {
    if (success == 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("user \"%u\" not found, falling back\n"),
                  user_id));
    else
      ACE_DEBUG ((LM_ERROR,
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
  if (!ACE_TEXT_GetUserNameEx (NameDisplay, // EXTENDED_NAME_FORMAT
                               buffer,
                               &buffer_size))
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
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::hostname(): \"%m\", aborting\n")));
  else
    return_value = ACE_TEXT_ALWAYS_CHAR (host_name);

  return return_value;
}

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
    std::ios_base::openmode open_mode = (std::ios_base::out |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN (log_stream_p,
                      std::ofstream (logFile_in.c_str (),
                                     open_mode));
//    log_stream_p = ACE_OS::fopen (logFile_in.c_str (),
//                                  ACE_TEXT_ALWAYS_CHAR ("w"));
    if (!log_stream_p)
    {
      //ACE_DEBUG ((LM_CRITICAL,
      //            ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fopen(): \"%m\", aborting\n")));
      return false;
    } // end IF
//    if (log_stream->open (logFile_in.c_str (),
//                          open_mode))
    if (log_stream_p->fail ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize logfile (was: \"%s\"): \"%m\", aborting\n"),
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
  if (result == -1)
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
  if (result == - 1)
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
  // *NOTE*: there is no need to keep this around after registration
  // *IMPORTANT NOTE*: do NOT restart system calls in this case (see manual)
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                0);                                       // flags
  ACE_Sig_Action previous_action;
  result = ignore_action.register_action (SIGPIPE,
                                          &previous_action);
  if (result == -1)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                SIGPIPE));
  else
    previousActions_out[SIGPIPE] = previous_action;

  // step2: block certain signals
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *IMPORTANT NOTE*: child threads inherit the signal mask of their parent...
  // --> make sure this is the main thread and invoked ASAP !

  // step2a: --> block [SIGRTMIN,SIGRTMAX] iff the default/current (!) proactor
  //             implementation happens to be ACE_POSIX_Proactor::PROACTOR_SIG
  //             (i.e. Linux, ...)

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
  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34

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
  if (!useReactor_in &&
      (proactor_type == ACE_POSIX_Proactor::PROACTOR_SIG))
  {
    sigset_t rt_signal_set;
    result = ACE_OS::sigemptyset (&rt_signal_set);
    if (result == - 1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
      return false;
    } // end IF
    unsigned int number = 0;
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         i++, number++)
    {
      result = ACE_OS::sigaddset (&rt_signal_set, i);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sigaddset(%S): \"%m\", continuing\n"),
                    i));
        number--;
      } // end IF

      // remove any RT signals from the signal set handled by the application
      // itself
      if (signals_inout.is_member (i))
      {
        result = signals_inout.sig_del (i); // <-- let the event dispatch handle
                                            //     all realtime signals
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%S): \"%m\", continuing\n"),
                      i));
//        else
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("removed %S from handled signals...\n"),
//                      i));
      } // end IF
    } // end IF
    result = ACE_OS::thr_sigsetmask (SIG_BLOCK,
                                     &rt_signal_set,
                                     &originalMask_out);
    if (result == -1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("[%d-%d]: blocked %d RT signals...\n"),
                ACE_SIGRTMIN, ACE_SIGRTMAX,
                number));
  } // end IF
_continue:
#endif

  return true;
}

bool
Common_Tools::initializeSignals (const ACE_Sig_Set& signals_in,
                                 const ACE_Sig_Set& ignoreSignals_in,
                                 ACE_Event_Handler* eventHandler_in,
                                 Common_SignalActions_t& previousActions_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeSignals"));

  // initialize return value(s)
  previousActions_out.clear ();

  // *NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  // step1: backup previous actions
  ACE_Sig_Action previous_action;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (signals_in.is_member (i))
    {
      previous_action.retrieve_action (i);
      previousActions_out[i] = previous_action;
    } // end IF

  // step2: ignore/block certain signals

  // step2a: ignore certain signals
  // *NOTE*: there is no need to keep this around after registration
  // *IMPORTANT NOTE*: do NOT restart system calls in this case (see manual)
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                (SA_RESTART | SA_SIGINFO));               // flags
  int result = -1;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (ignoreSignals_in.is_member (i))
    {
      result = ignore_action.register_action (i,
                                              &previous_action);
      if (result == -1)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                    i));
      else
      {
        previousActions_out[i] = previous_action;

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("ignoring signal \"%S\"...\n"),
                    i));
      } // end IF
    } // end IF

//  // step2b: block certain signals
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//  // *IMPORTANT NOTE*: child threads inherit the signal mask of their parent...
//  // --> make sure this is the main thread and invoked ASAP !

//  // step2a: --> block [SIGRTMIN,SIGRTMAX] iff the default/current (!) proactor
//  //             implementation happens to be ACE_POSIX_Proactor::PROACTOR_SIG
//  //             (i.e. Linux, ...)

//  // *NOTE*: the ACE_POSIX_Proactor::PROACTOR_SIG implementation also blocks the
//  //         RT signal(s) in its ctor
//  // --> see also: POSIX_Proactor.cpp:1604,1653
//  //         ...and later dispatches the signals in worker thread(s) running the
//  //         event loop
//  // --> see also: POSIX_Proactor.cpp:1864
//  // --> see also: man sigwaitinfo, man 7 signal

//  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
//  //                   signals (see also signal(7)); these signals cannot be
//  //                   used in applications. ..." (see 'man 7 pthreads')
//  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34

//  ACE_POSIX_Proactor::Proactor_Type proactor_type;
//  ACE_Proactor* proactor_p = ACE_Proactor::instance ();
//  ACE_ASSERT (proactor_p);
//  ACE_POSIX_Proactor* proactor_impl_p =
//      dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
//  if (!proactor_impl_p)
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("dynamic_cast<ACE_POSIX_Proactor> failed, continuing\n")));
//    goto _continue;
//  } // end IF
//  proactor_type = proactor_impl_p->get_impl_type ();
//  if (!useReactor_in &&
//      (proactor_type == ACE_POSIX_Proactor::PROACTOR_SIG))
//  {
//    sigset_t rt_signal_set;
//    result = ACE_OS::sigemptyset (&rt_signal_set);
//    if (result == - 1)
//    {
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
//      return false;
//    } // end IF
//    for (int i = ACE_SIGRTMIN;
//         i <= ACE_SIGRTMAX;
//         i++)
//    {
//      result = ACE_OS::sigaddset (&rt_signal_set, i);
//      if (result == -1)
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to ACE_OS::sigaddset(%S): \"%m\", continuing\n"),
//                    i));

//      // remove any RT signals from the signal set handled by the application
//      // itself
//      if (signals_inout.is_member (i))
//      {
//        result = signals_inout.sig_del (i); // <-- let the event dispatch handle
//        //     all realtime signals
//        if (result == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%S): \"%m\", continuing\n"),
//                      i));
//      } // end IF
//    } // end IF
//    result = ACE_OS::thr_sigsetmask (SIG_BLOCK,
//                                     &rt_signal_set,
//                                     &originalMask_out);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));
//      return false;
//    } // end IF
//  } // end IF
//_continue:
//#endif

  // step3: register (process-wide) signal handler
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action new_action (static_cast<ACE_SignalHandler> (SIG_DFL), // default action
                             ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                       // --> block them all (bar KILL/STOP; see manual)
                             (SA_RESTART | SA_SIGINFO));               // flags
  ACE_Reactor* reactor_p = ACE_Reactor::instance ();
  ACE_ASSERT (reactor_p);
  result = reactor_p->register_handler (signals_in,
                                        eventHandler_in,
                                        &new_action);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));
    return false;
  } // end IF

  // debug info
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (signals_in.is_member (i))
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("handling signal \"%S\"...\n"),
                  i));

  return true;
}

void
Common_Tools::finalizeSignals (const ACE_Sig_Set& signals_in,
                               const Common_SignalActions_t& previousActions_in,
                               const sigset_t& previousMask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeSignals"));

  int result = -1;

  // step1: restore previous signal handlers
  ACE_Reactor* reactor_p = ACE_Reactor::instance ();
  ACE_ASSERT (reactor_p);
  result = reactor_p->remove_handler (signals_in);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Reactor::remove_handler(): \"%m\", returning\n")));
    return;
  } // end IF

  for (Common_SignalActionsIterator_t iterator = previousActions_in.begin ();
       iterator != previousActions_in.end ();
       iterator++)
  {
    result =
        const_cast<ACE_Sig_Action&> ((*iterator).second).register_action ((*iterator).first,
                                                                          NULL);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                  (*iterator).first));
  } // end FOR

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // step2: restore previous signal mask
  result = ACE_OS::thr_sigsetmask (SIG_SETMASK,
                                   &previousMask_in,
                                   NULL);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", continuing\n")));
#endif
}

void
Common_Tools::retrieveSignalInfo (int signal_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                  const struct siginfo_t& info_in,
#else
                                  const siginfo_t& info_in,
#endif
                                  const ucontext_t* context_in,
                                  std::string& information_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::retrieveSignalInfo"));

  // initialize return value
  information_out.clear ();

  int result = -1;
  std::ostringstream information;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // step0: common information (on POSIX.1b)
  information << ACE_TEXT_ALWAYS_CHAR ("PID/UID: ");
  information << info_in.si_pid;
  information << ACE_TEXT_ALWAYS_CHAR ("/");
  information << info_in.si_uid;

  // (try to) get user name
  char buffer[BUFSIZ];
  struct passwd passwd;
  struct passwd* passwd_p = NULL;
// *PORTABILITY*: this isn't completely portable... (man getpwuid_r)
  result = ::getpwuid_r (info_in.si_uid,
                         &passwd,
                         buffer,
                         sizeof (buffer),
                         &passwd_p);
  if (result || !passwd_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%d) : \"%m\", continuing\n"),
                info_in.si_uid));
  } // end IF
  else
  {
    information << ACE_TEXT_ALWAYS_CHAR ("[\"");
    information << passwd.pw_name;
    information << ACE_TEXT_ALWAYS_CHAR ("\"]");
  } // end ELSE

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  information << ACE_TEXT_ALWAYS_CHAR (", errno: ");
  information << info_in.si_errno;
  information << ACE_TEXT_ALWAYS_CHAR ("[\"");
  information << ACE_OS::strerror (info_in.si_errno);
  information << ACE_TEXT_ALWAYS_CHAR ("\"], code: ");

  // step1: retrieve signal code...
  switch (info_in.si_code)
  {
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_NOINFO:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_NOINFO"); break;
  case SI_DTRACE:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_DTRACE"); break;
  case SI_RCTL:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_RCTL"); break;
/////////////////////////////////////////
#endif
  case SI_USER:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_USER"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_LWP:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_LWP"); break;
#else
  case SI_KERNEL:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_KERNEL"); break;
#endif
    case SI_QUEUE:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_QUEUE"); break;
    case SI_TIMER:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_TIMER"); break;
    case SI_ASYNCIO:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_ASYNCIO"); break;
    case SI_MESGQ:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_MESGQ"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_LWP_QUEUE:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_LWP_QUEUE"); break;
#else
  case SI_SIGIO:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_SIGIO"); break;
    case SI_TKILL:
      information << ACE_TEXT_ALWAYS_CHAR ("SI_TKILL"); break;
#endif
  default:
    { // (signal-dependant) codes...
      switch (signal_in)
      {
        case SIGILL:
        {
          switch (info_in.si_code)
          {
            case ILL_ILLOPC:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPC"); break;
            case ILL_ILLOPN:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPN"); break;
            case ILL_ILLADR:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_ILLADR"); break;
            case ILL_ILLTRP:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_ILLTRP"); break;
            case ILL_PRVOPC:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_PRVOPC"); break;
            case ILL_PRVREG:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_PRVREG"); break;
            case ILL_COPROC:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_COPROC"); break;
            case ILL_BADSTK:
              information << ACE_TEXT_ALWAYS_CHAR ("ILL_BADSTK"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGFPE:
        {
          switch (info_in.si_code)
          {
            case FPE_INTDIV:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_INTDIV"); break;
            case FPE_INTOVF:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_INTOVF"); break;
            case FPE_FLTDIV:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTDIV"); break;
            case FPE_FLTOVF:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTOVF"); break;
            case FPE_FLTUND:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTUND"); break;
            case FPE_FLTRES:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTRES"); break;
            case FPE_FLTINV:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTINV"); break;
            case FPE_FLTSUB:
              information << ACE_TEXT_ALWAYS_CHAR ("FPE_FLTSUB"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGSEGV:
        {
          switch (info_in.si_code)
          {
            case SEGV_MAPERR:
              information << ACE_TEXT_ALWAYS_CHAR ("SEGV_MAPERR"); break;
            case SEGV_ACCERR:
              information << ACE_TEXT_ALWAYS_CHAR ("SEGV_ACCERR"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGBUS:
        {
          switch (info_in.si_code)
          {
            case BUS_ADRALN:
              information << ACE_TEXT_ALWAYS_CHAR ("BUS_ADRALN"); break;
            case BUS_ADRERR:
              information << ACE_TEXT_ALWAYS_CHAR ("BUS_ADRERR"); break;
            case BUS_OBJERR:
              information << ACE_TEXT_ALWAYS_CHAR ("BUS_OBJERR"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGTRAP:
        {
          switch (info_in.si_code)
          {
            case TRAP_BRKPT:
              information << ACE_TEXT_ALWAYS_CHAR ("TRAP_BRKPT"); break;
            case TRAP_TRACE:
              information << ACE_TEXT_ALWAYS_CHAR ("TRAP_TRACE"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGCHLD:
        {
          switch (info_in.si_code)
          {
            case CLD_EXITED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_EXITED"); break;
            case CLD_KILLED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_KILLED"); break;
            case CLD_DUMPED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_DUMPED"); break;
            case CLD_TRAPPED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_TRAPPED"); break;
            case CLD_STOPPED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_STOPPED"); break;
            case CLD_CONTINUED:
              information << ACE_TEXT_ALWAYS_CHAR ("CLD_CONTINUED"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGPOLL:
        {
          switch (info_in.si_code)
          {
            case POLL_IN:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_IN"); break;
            case POLL_OUT:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_OUT"); break;
            case POLL_MSG:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_MSG"); break;
            case POLL_ERR:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_ERR"); break;
            case POLL_PRI:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_PRI"); break;
            case POLL_HUP:
              information << ACE_TEXT_ALWAYS_CHAR ("POLL_HUP"); break;
            default:
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          info_in.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                      info_in.si_code));
          break;
        }
      } // end SWITCH

      break;
    }
  } // end SWITCH

  // step2: retrieve more (signal-dependant) information
  switch (signal_in)
  {
    case SIGALRM:
    {
#if defined (__linux__)
      information << ACE_TEXT_ALWAYS_CHAR (", overrun: ");
      information << info_in.si_overrun;
      information << ACE_TEXT_ALWAYS_CHAR (", (internal) id: ");
      information << info_in.si_timerid;
#endif
      break;
    }
    case SIGCHLD:
    {
      information << ACE_TEXT_ALWAYS_CHAR (", (exit) status: ");
      information << info_in.si_status;
      information << ACE_TEXT_ALWAYS_CHAR (", time consumed (user): ");
      information << info_in.si_utime;
      information << ACE_TEXT_ALWAYS_CHAR (" / (system): ");
      information << info_in.si_stime;
      break;
    }
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    {
      // *TODO*: more data ?
      information << ACE_TEXT_ALWAYS_CHAR (", fault at address: ");
      information << info_in.si_addr;
      break;
    }
    case SIGPOLL:
    {
      information << ACE_TEXT_ALWAYS_CHAR (", band event: ");
      information << info_in.si_band;
      information << ACE_TEXT_ALWAYS_CHAR (", (file) descriptor: ");
      information << info_in.si_fd;
      break;
    }
    default:
    {
      // *TODO*: handle more signals here ?
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("no additional information for signal: \"%S\"...\n"),
//                   signal_in));
      break;
    }
  } // end SWITCH
#else
  switch (signal_in)
  {
    case SIGINT:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGINT"); break;
    case SIGILL:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGILL"); break;
    case SIGFPE:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGFPE"); break;
    case SIGSEGV:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGSEGV"); break;
    case SIGTERM:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGTERM"); break;
    case SIGBREAK:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGBREAK"); break;
    case SIGABRT:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGABRT"); break;
    case SIGABRT_COMPAT:
      information << ACE_TEXT_ALWAYS_CHAR ("SIGABRT_COMPAT"); break;
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid/unknown signal: %S, continuing\n"),
                  signal_in));
      break;
    }
  } // end SWITCH

  information << ACE_TEXT_ALWAYS_CHAR (", signalled handle: ");
  information << info_in.si_handle_;
  //information << ACE_TEXT_ALWAYS_CHAR (", array of signalled handle(s): ");
  //information << info_in.si_handles_;
#endif

  // OK: set return value
  information_out = information.str ();
}

bool
Common_Tools::initializeEventDispatch (bool useReactor_in,
                                       bool useThreadPool_in,
                                       bool& serializeOutput_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeEventDispatch"));

  // initialize return value(s)
  serializeOutput_out = false;

  // step0: select reactor/proactor implementation
  Common_ReactorType reactor_type = COMMON_REACTOR_DEFAULT;
  Common_ProactorType proactor_type = COMMON_PROACTOR_DEFAULT;
  if (useReactor_in)
  {
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    if (COMMON_EVENT_POSIX_USE_DEV_POLL_REACTOR)
      reactor_type = COMMON_REACTOR_DEV_POLL;
    else
#else
    if (COMMON_EVENT_WINXX_USE_WFMO_REACTOR)
      reactor_type = COMMON_REACTOR_WFMO;
    else
#endif
      reactor_type = (useThreadPool_in ? COMMON_REACTOR_TP
                                       : COMMON_REACTOR_SELECT);
  } // end IF
  else
  {
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    if (COMMON_EVENT_PROACTOR_USE_SIG)
      proactor_type = COMMON_PROACTOR_POSIX_SIG;
    else if (COMMON_EVENT_PROACTOR_USE_AIOCB)
      proactor_type = COMMON_PROACTOR_POSIX_AIOCB;
#endif
  } // end ELSE

  // step1: initialize reactor/proactor
  if (useReactor_in)
  {
    ACE_Reactor_Impl* reactor_impl_p = NULL;
    switch (reactor_type)
    {
      case COMMON_REACTOR_DEFAULT:
      {
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("using default (platform-specific) reactor...\n")));
        break;
      }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      case COMMON_REACTOR_DEV_POLL:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using dev/poll reactor...\n")));

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
                    ACE_TEXT ("using select reactor...\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_Select_Reactor (COMMON_EVENT_MAXIMUM_HANDLES,    // max num handles
                                              true,                            // restart after EINTR ?
                                              NULL,                            // signal handler handle
                                              NULL,                            // timer queue handle
                                              ACE_DISABLE_NOTIFY_PIPE_DEFAULT, // disable notification pipe ?
                                              NULL,                            // notification handler handle
                                              true,                            // mask signals ?
                                              ACE_SELECT_TOKEN::FIFO));        // signal queue

        break;
      }
      case COMMON_REACTOR_TP:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using thread-pool reactor...\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_TP_Reactor (COMMON_EVENT_MAXIMUM_HANDLES,     // max num handles
                                          true,                             // restart after EINTR ?
                                          NULL,                             // signal handler handle
                                          NULL,                             // timer queue handle
                                          true,                             // mask signals ?
                                          ACE_Select_Reactor_Token::FIFO)); // signal queue

        serializeOutput_out = true;

        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case COMMON_REACTOR_WFMO:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using WFMO reactor...\n")));

        ACE_NEW_NORETURN (reactor_impl_p,
                          ACE_WFMO_Reactor (ACE_WFMO_Reactor::DEFAULT_SIZE, // max num handles (62 [+ 2])
                                            0,                              // unused
                                            NULL,                           // signal handler handle
                                            NULL,                           // timer queue handle
                                            NULL));                         // notification handler handle

        break;
      }
#endif
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown reactor type (was: %d), aborting\n"),
                    reactor_type));
        return false;
      }
    } // end SWITCH
    ACE_Reactor* reactor_p = NULL;
    if (reactor_impl_p)
    {
      ACE_NEW_NORETURN (reactor_p,
                        ACE_Reactor (reactor_impl_p, // implementation handle
                                     1));            // delete in dtor ?
      if (!reactor_p)
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
    ACE_Proactor_Impl* proactor_impl_p = NULL;
    switch (proactor_type)
    {
      case COMMON_PROACTOR_DEFAULT:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using default (platform-specific) proactor...\n")));
        break;
      }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      case COMMON_PROACTOR_POSIX_AIOCB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX AIOCB proactor...\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_AIOCB_Proactor (COMMON_EVENT_PROACTOR_NUM_AIO_OPERATIONS)); // parallel operations

        break;
      }
      case COMMON_PROACTOR_POSIX_SIG:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX RT-signal proactor...\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_SIG_Proactor (COMMON_EVENT_PROACTOR_NUM_AIO_OPERATIONS)); // parallel operations

        break;
      }
#if defined (ACE_HAS_AIO_CALLS) && defined (sun)
      case COMMON_PROACTOR_POSIX_SUN:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using SunOS proactor...\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_SUN_Proactor (COMMON_EVENT_PROACTOR_NUM_AIO_OPERATIONS)); // parallel operations

        break;
      }
#endif
      case COMMON_PROACTOR_POSIX_CB:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("using POSIX CB proactor...\n")));

        ACE_NEW_NORETURN (proactor_impl_p,
                          ACE_POSIX_CB_Proactor (COMMON_EVENT_PROACTOR_NUM_AIO_OPERATIONS)); // parallel operations

        break;
      }
#endif
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown proactor type (was: %d), aborting\n"),
                    proactor_type));
        return false;
      }
    } // end SWITCH
    ACE_Proactor* proactor_p = NULL;
    if (proactor_impl_p)
    {
      ACE_NEW_NORETURN (proactor_p,
                        ACE_Proactor (proactor_impl_p, // implementation handle --> create new ?
                                      //                                    false,  // *NOTE*: call close() manually
                                      //                                            // (see finalizeEventDispatch() below)
                                      true,   // delete in dtor ?
                                      NULL)); // timer queue handle --> create new
      if (!proactor_p)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        return false;
      } // end IF
    } // end IF

    // make this the "default" proactor...
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

  bool use_reactor = *reinterpret_cast<bool*> (arg_in);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_THR_FUNC_RETURN result = -1;
#else
  ACE_THR_FUNC_RETURN result = arg_in;
#endif

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

  // handle any events...
  int result_2 = -1;
  if (use_reactor)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
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
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
//#endif
    result_2 = proactor_p->proactor_run_event_loop (NULL);
  } // end ELSE
  if (result_2 == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(%t) failed to handle events: \"%m\", leaving\n")));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker leaving...\n")));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = result_2;
#else
  result = ((result_2 == 0) ? NULL : result);
#endif

  return result;
}

bool
Common_Tools::startEventDispatch (bool useReactor_in,
                                  unsigned int numDispatchThreads_in,
                                  int& groupID_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::startEventDispatch"));

  int result = -1;

  // initialize return value(s)
  groupID_out = -1;

  // reset event dispatch
  if (useReactor_in)
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
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::proactor_reset_event_loop: \"%m\", continuing\n")));
  } // end ELSE

  // spawn worker(s) ?
  // *NOTE*: if #dispatch threads == 1, event dispatch takes place in the main
  //         thread --> do NOT spawn any workers here...
  if (numDispatchThreads_in <= 1)
    return true;

  // start a (group of) worker thread(s)...
  ACE_hthread_t* thread_handles_p = NULL;
  ACE_NEW_NORETURN (thread_handles_p,
                    ACE_hthread_t[numDispatchThreads_in]);
  if (!thread_handles_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (ACE_hthread_t) * numDispatchThreads_in)));
    return false;
  } // end IF
//  ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
  const char** thread_names_p = NULL;
  ACE_NEW_NORETURN (thread_names_p,
                    const char*[numDispatchThreads_in]);
  if (!thread_names_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                (sizeof (char*) * numDispatchThreads_in)));

    // clean up
    delete [] thread_handles_p;

    return false;
  } // end IF
  ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
  char* thread_name_p;
  std::string buffer;
  std::ostringstream converter;
  for (unsigned int i = 0;
       i < numDispatchThreads_in;
       i++)
  {
    thread_name_p = NULL;
    ACE_NEW_NORETURN (thread_name_p,
                      char[BUFSIZ]);
    if (!thread_name_p)
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
    buffer = ACE_TEXT_ALWAYS_CHAR (COMMON_EVENT_DISPATCH_THREAD_NAME);
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::strcpy (thread_name_p,
                    buffer.c_str ());
    thread_names_p[i] = thread_name_p;
  } // end FOR
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  groupID_out =
    thread_manager_p->spawn_n (numDispatchThreads_in,                  // # threads
                               ::threadpool_event_dispatcher_function, // function
                               &const_cast<bool&> (useReactor_in),     // argument
                               (THR_NEW_LWP     |
                               THR_JOINABLE     |
                               THR_INHERIT_SCHED),                     // flags
                               ACE_DEFAULT_THREAD_PRIORITY,            // priority
                               COMMON_EVENT_DISPATCH_THREAD_GROUP_ID,  // group id
                               NULL,                                   // task
                               thread_handles_p,                       // handle(s)
                               NULL,                                   // stack(s)
                               NULL,                                   // stack size(s)
                               thread_names_p);                        // name(s)
  if (groupID_out == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                numDispatchThreads_in));

    // clean up
    delete [] thread_handles_p;
    for (unsigned int i = 0; i < numDispatchThreads_in; i++)
      delete [] thread_names_p[i];
    delete [] thread_names_p;

    return false;
  } // end IF

  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//    __uint64_t thread_id = 0;
#endif
  for (unsigned int i = 0; i < numDispatchThreads_in; i++)
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
  } // end IF

  buffer = converter.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) spawned %u worker thread(s) (group: %d):\n%s"),
              ACE_TEXT (COMMON_EVENT_DISPATCH_THREAD_NAME),
              numDispatchThreads_in,
              groupID_out,
              ACE_TEXT (buffer.c_str ())));

  // clean up
  delete [] thread_handles_p;
  delete [] thread_names_p;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  return true;
}

void
Common_Tools::finalizeEventDispatch (bool stopReactor_in,
                                     bool stopProactor_in,
                                     int groupID_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalizeEventDispatch"));

  int result = -1;

  // step1: stop reactor/proactor
  // *IMPORTANT NOTE*: current proactor implementations start a pseudo-task
  //                   that runs the reactor --> stop that as well
  if (stopReactor_in || stopProactor_in)
  {
    ACE_Reactor* reactor_p = ACE_Reactor::instance ();
    ACE_ASSERT (reactor_p);
    result = reactor_p->end_event_loop ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Reactor::end_event_loop: \"%m\", continuing\n")));
  } // end IF

  if (stopProactor_in)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    result = proactor_p->end_event_loop ();
    if (result == -1)
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
  if (groupID_in != -1)
  {
    ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
    ACE_ASSERT (thread_manager_p);
    result = thread_manager_p->wait_grp (groupID_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  groupID_in));
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopped event dispatch...\n")));
}

void
Common_Tools::dispatchEvents (bool stopReactor_in,
                              bool stopProactor_in,
                              int groupID_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::dispatchEvents"));

  int result = -1;

  // *NOTE*: when using a thread pool, handle things differently...
  if (groupID_in != -1)
  {
    ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
    ACE_ASSERT (thread_manager_p);
    result = thread_manager_p->wait_grp (groupID_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  groupID_in));
  } // end IF
  else
  {
    if (stopReactor_in)
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      //// *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      //reactor_p->restart (1);
      result = reactor_p->run_reactor_event_loop (0);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", continuing\n")));
    } // end IF
    else
    {
      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result = proactor_p->proactor_run_event_loop (0);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Proactor::proactor_run_event_loop(): \"%m\", continuing\n")));
    } // end ELSE
  } // end ELSE

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("event dispatch complete...\n")));
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
