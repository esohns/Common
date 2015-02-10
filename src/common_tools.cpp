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

#include "common_tools.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <locale>

#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/Proactor.h"
#include "ace/POSIX_Proactor.h"
#include "ace/Reactor.h"

#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include <Security.h>
#endif

#include "common_macros.h"
#include "common_defines.h"

bool
Common_Tools::period2String(const ACE_Time_Value& period_in,
                            std::string& timeString_out)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::period2String"));

  // init return value(s)
  timeString_out.resize(0);

  // extract hours and minutes...
  ACE_Time_Value temp = period_in;
  int hours = static_cast<int>(temp.sec()) / (60 * 60);
  temp.sec(temp.sec() % (60 * 60));

  int minutes = static_cast<int>(temp.sec()) / 60;
  temp.sec(temp.sec() % 60);

  char time_string[RPG_COMMON_BUFSIZE];
  // *TODO*: rewrite this in C++...
  if (ACE_OS::snprintf(time_string,
                       sizeof(time_string),
                       ACE_TEXT_ALWAYS_CHAR("%d:%d:%d.%d"),
                       hours,
                       minutes,
                       static_cast<int>(temp.sec()),
                       static_cast<int>(temp.usec())) < 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));

    return false;
  } // end IF

  timeString_out = time_string;

  return true;
}

std::string
Common_Tools::sanitizeURI(const std::string& uri_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::isLinux"));

  std::string result = uri_in;

  std::replace(result.begin(),
               result.end(),
               '\\', '/');
  size_t position;
  do
  {
    position = result.find(' ', 0);
    if (position == std::string::npos)
      break;

    result.replace(position, 1, ACE_TEXT_ALWAYS_CHAR("%20"));
  } while (true);
  //XMLCh* transcoded_string =
  //	XMLString::transcode(result.c_str(),
  //	                     XMLPlatformUtils::fgMemoryManager);
  //XMLURL url;
  //if (!XMLURL::parse(transcoded_string,
  //	                 url))
  // {
  //   ACE_DEBUG((LM_ERROR,
  //              ACE_TEXT("failed to XMLURL::parse(\"%s\"), aborting\n"),
  //              ACE_TEXT(result.c_str())));

  //   return result;
  // } // end IF
  //XMLUri uri(transcoded_string,
  //	         XMLPlatformUtils::fgMemoryManager);
  //XMLString::release(&transcoded_string,
  //	                 XMLPlatformUtils::fgMemoryManager);
  //char* translated_string =
  //	XMLString::transcode(uri.getUriText(),
  //	                     XMLPlatformUtils::fgMemoryManager);
  //result = translated_string;
  //XMLString::release(&translated_string,
  //	                 XMLPlatformUtils::fgMemoryManager);

  return result;
}

std::string
Common_Tools::sanitize(const std::string& string_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::sanitize"));

  std::string result = string_in;

  std::replace(result.begin(),
               result.end(),
               ' ', '_');

  return result;
}

std::string
Common_Tools::strip(const std::string& string_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::strip"));

  std::string result = string_in;

  // *TODO*: remove tabs & other non-printable characters
  std::string::size_type current_space = std::string::npos;
  while ((current_space = result.find(' ',
                                      0)) == 0)
    result.erase(current_space, 1);
  while ((current_space = result.rfind(' ',
                                       std::string::npos)) == (result.size() - 1))
    result.erase(current_space, 1);

  return result;
}

bool
Common_Tools::isLinux()
{
  RPG_TRACE(ACE_TEXT("Common_Tools::isLinux"));

  // get system information
  ACE_utsname name;
  if (ACE_OS::uname(&name) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::uname(): \"%m\", aborting\n")));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("local system info: %s (%s), %s %s, %s\n"),
//              name.nodename,
//              name.machine,
//              name.sysname,
//              name.release,
//              name.version));

  std::string kernel(name.sysname);
  return (kernel.find(ACE_TEXT_ALWAYS_CHAR("Linux"), 0) == 0);
}

bool
Common_Tools::initResourceLimits(const bool& fileDescriptors_in,
                                 const bool& stackTraces_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::initResourceLimits"));

  if (fileDescriptors_in)
  {
// *PORTABILITY*: this is almost entirely non-portable...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    rlimit fd_limit;

      if (ACE_OS::getrlimit(RLIMIT_NOFILE,
                            &fd_limit) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));

        return false;
      } // end IF

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("file descriptor limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
//                  fd_limit.rlim_cur,
//                  fd_limit.rlim_max));

      // *TODO*: really unset these limits; note that this probably requires
      // patching/recompiling the kernel...
      // *NOTE*: setting/raising the max limit probably requires CAP_SYS_RESOURCE
      fd_limit.rlim_cur = fd_limit.rlim_max;
//      fd_limit.rlim_cur = RLIM_INFINITY;
//      fd_limit.rlim_max = RLIM_INFINITY;
      if (ACE_OS::setrlimit(RLIMIT_NOFILE,
                            &fd_limit) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::setrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));

        return false;
      } // end IF

      // verify...
      if (ACE_OS::getrlimit(RLIMIT_NOFILE,
                            &fd_limit) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));

        return false;
      } // end IF

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unset file descriptor limits, now: [soft: %u, hard: %u]...\n"),
                 fd_limit.rlim_cur,
                 fd_limit.rlim_max));
#else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("file descriptor limits are not available on this platform, continuing\n")));
#endif
  } // end IF

// -----------------------------------------------------------------------------

  if (!stackTraces_in)
    return true;

// *PORTABILITY*: this is almost entirely non-portable...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  rlimit core_limit;

//  // debug info
//  if (ACE_OS::getrlimit(RLIMIT_CORE,
//                        &core_limit) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

//    return false;
//  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("corefile limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
//              core_limit.rlim_cur,
//              core_limit.rlim_max));

  // set soft/hard limits to unlimited...
  core_limit.rlim_cur = RLIM_INFINITY;
  core_limit.rlim_max = RLIM_INFINITY;
  if (ACE_OS::setrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

    return false;
  } // end IF

  // verify...
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

    return false;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("unset corefile limits, now: [soft: %u, hard: %u]...\n"),
             core_limit.rlim_cur,
             core_limit.rlim_max));
#else
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("corefile limits are not available on this platform, continuing\n")));
#endif

  return true;
}

void
Common_Tools::getCurrentUserName(std::string& username_out,
                                 std::string& realname_out)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::getCurrentUserName"));

  // init return value(s)
  username_out.clear();
  realname_out.clear();

  char user_name[ACE_MAX_USERID];
  ACE_OS::memset(user_name, 0, ACE_MAX_USERID);
  if (ACE_OS::cuserid(user_name, ACE_MAX_USERID) == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::cuserid(): \"%m\", falling back\n")));

    username_out = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_USER_LOGIN_BASE)));

    return;
  } // end IF
  username_out = user_name;

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  int            success = -1;
  struct passwd  pwd;
  struct passwd* pwd_result = NULL;
  char           buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, sizeof(RPG_COMMON_BUFSIZE));
//  size_t         bufsize = 0;
//  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
//  if (bufsize == -1)        /* Value was indeterminate */
//    bufsize = 16384;        /* Should be more than enough */

  uid_t user_id = ACE_OS::geteuid();
  success = ::getpwuid_r(user_id,            // user id
                         &pwd,               // passwd entry
                         buffer,             // buffer
                         RPG_COMMON_BUFSIZE, // buffer size
                         &pwd_result);       // result (handle)
  if (pwd_result == NULL)
  {
    if (success == 0)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("user \"%u\" not found, falling back\n"),
                 user_id));
    else
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::getpwuid_r(%u): \"%m\", falling back\n"),
                 user_id));
    username_out = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_USER_LOGIN_BASE)));
  } // end IF
  else
    realname_out = pwd.pw_gecos;
#else
  TCHAR buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, RPG_COMMON_BUFSIZE * sizeof(TCHAR));
  DWORD buffer_size = 0;
  if (GetUserNameEx(NameDisplay, // EXTENDED_NAME_FORMAT
                    buffer,
                    &buffer_size) == 0)
  {
    if (GetLastError() != ERROR_NONE_MAPPED)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to GetUserNameEx(): \"%m\", continuing\n")));
  } // end IF
  else
    realname_out = buffer;
#endif
}

std::string
Common_Tools::getHostName()
{
  RPG_TRACE(ACE_TEXT("Common_Tools::getHostName"));

  std::string result;

  ACE_TCHAR host_name[MAXHOSTNAMELEN];
  ACE_OS::memset(host_name, 0, sizeof(host_name));
  if (ACE_OS::hostname(host_name, sizeof(host_name)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::hostname(): \"%m\", aborting\n")));
  else
    result = host_name;

  return result;
}

bool
Common_Tools::initLogging(const std::string& programName_in,
                          const std::string& logFile_in,
                          const bool& logToSyslog_in,
                          const bool& enableTracing_in,
                          const bool& enableDebug_in,
                          ACE_Log_Msg_Backend* backend_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::initLogging"));

  // *NOTE*: default log target is stderr
  u_long options_flags = ACE_Log_Msg::STDERR;
  if (logToSyslog_in)
    options_flags |= ACE_Log_Msg::SYSLOG;
  if (backend_in)
  {
    options_flags |= ACE_Log_Msg::CUSTOM;
    ACE_LOG_MSG->msg_backend(backend_in);
  } // end IF
  if (!logFile_in.empty())
  {
    options_flags |= ACE_Log_Msg::OSTREAM;

    ACE_OSTREAM_TYPE* log_stream;
    std::ios_base::openmode open_mode = (std::ios_base::out |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN(log_stream,
                     std::ofstream(logFile_in.c_str(),
                                   open_mode));
    if (!log_stream)
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

      return false;
    } // end IF
//    if (log_stream->open(logFile_in.c_str(),
//                         open_mode))
    if (log_stream->fail())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to initialize logfile (was: \"%s\"): \"%m\", aborting\n"),
                 logFile_in.c_str()));

      // clean up
      delete log_stream;

      return false;
    } // end IF

    // *NOTE*: the logger singleton assumes ownership of the stream lifecycle
    ACE_LOG_MSG->msg_ostream(log_stream, 1);
  } // end IF
  if (ACE_LOG_MSG->open(ACE_TEXT(programName_in.c_str()),
                        options_flags,
                        NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Log_Msg::open(\"%s\", %u): \"%m\", aborting\n"),
               programName_in.c_str(),
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
  ACE_LOG_MSG->priority_mask(process_priority_mask,
                             ACE_Log_Msg::PROCESS);

  return true;
}

bool
Common_Tools::preInitSignals(ACE_Sig_Set& signals_inout,
                             const bool& useReactor_in,
                             Common_SignalActions_t& previousActions_out)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::preInitSignals"));

  // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  // step1: ignore SIGPIPE: continue gracefully after a client suddenly
  // disconnects (i.e. application/system crash, etc...)
  // --> specify ignore action
  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
  // *NOTE*: do NOT restart system calls in this case (see manual)
  ACE_Sig_Action ignore_action(static_cast<ACE_SignalHandler>(SIG_IGN), // ignore action
                               ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                        // --> block them all (bar KILL/STOP; see manual)
                               0);                                      // flags
  ACE_Sig_Action previous_action;
  if (ignore_action.register_action(SIGPIPE,
                                    &previous_action) == -1)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
               SIGPIPE));
  else
    previousActions_out[SIGPIPE] = previous_action;

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  // step2: block [SIGRTMIN,SIGRTMAX] IFF on UNIX AND using the
  // ACE_POSIX_SIG_Proactor (the default)
  // *IMPORTANT NOTE*: proactor implementation dispatches the signals in worker
  // thread(s) and re-enables them there automatically (see code)
  if (!useReactor_in)
  {
    ACE_POSIX_Proactor* proactor_impl = dynamic_cast<ACE_POSIX_Proactor*>(ACE_Proactor::instance()->implementation());
    ACE_ASSERT(proactor_impl);
    if (proactor_impl->get_impl_type() == ACE_POSIX_Proactor::PROACTOR_SIG)
    {
      sigset_t signal_set;
      if (ACE_OS::sigemptyset(&signal_set) == - 1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

        return false;
      } // end IF
      for (int i = ACE_SIGRTMIN;
           i <= ACE_SIGRTMAX;
           i++)
      {
        if (ACE_OS::sigaddset(&signal_set, i) == -1)
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to ACE_OS::sigaddset(): \"%m\", aborting\n")));

          return false;
        } // end IF
        if (signals_inout.is_member(i))
          if (signals_inout.sig_del(i) == -1)
          {
            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("failed to ACE_Sig_Set::sig_del(%S): \"%m\", aborting\n"),
                       i));

            return false;
          } // end IF
      } // end IF
      sigset_t original_mask;
      if (ACE_OS::thr_sigsetmask(SIG_BLOCK,
                                 &signal_set,
                                 &original_mask) == -1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));

        return false;
      } // end IF
    } // end IF
  } // end IF
#endif

  return true;
}

bool
Common_Tools::initSignals(ACE_Sig_Set& signals_inout,
                          ACE_Event_Handler* eventHandler_in,
                          Common_SignalActions_t& previousActions_out)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::initSignals"));

  // init return value(s)
  previousActions_out.clear();

  // *NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  // step1: backup previous actions
  ACE_Sig_Action previous_action;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (signals_inout.is_member(i))
    {
      previous_action.retrieve_action(i);
      previousActions_out[i] = previous_action;
    } // end IF

  // step2: register (process-wide) signal handler
  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
  ACE_Sig_Action new_action(static_cast<ACE_SignalHandler>(SIG_DFL), // default action
                            ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                     // --> block them all (bar KILL/STOP; see manual)
                            (SA_RESTART | SA_SIGINFO));              // flags
  if (ACE_Reactor::instance()->register_handler(signals_inout,
                                                eventHandler_in,
                                                &new_action) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));

    return false;
  } // end IF

  return true;
}

void
Common_Tools::finiSignals(const ACE_Sig_Set& signals_in,
                          const bool& useReactor_in,
                          const Common_SignalActions_t& previousActions_in)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::finiSignals"));

  // step1: unblock [SIGRTMIN,SIGRTMAX] IFF on Linux AND using the
  // ACE_POSIX_SIG_Proactor (the default)
  // *IMPORTANT NOTE*: proactor implementation dispatches the signals in worker
  // thread(s) and enabled them there automatically (see code)
  if (Common_Tools::isLinux() &&
      !useReactor_in)
  {
    sigset_t signal_set;
    if (ACE_OS::sigemptyset(&signal_set) == - 1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

      return;
    } // end IF
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         i++)
      if (ACE_OS::sigaddset(&signal_set,
                            i) == -1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_OS::sigaddset(): \"%m\", aborting\n")));

        return;
      } // end IF
    if (ACE_OS::thr_sigsetmask(SIG_UNBLOCK,
                               &signal_set,
                               NULL) == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));

      return;
    } // end IF
  } // end IF

  // step2: restore previous signal handlers
  if (ACE_Reactor::instance()->remove_handler(signals_in) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::remove_handler(): \"%m\", aborting\n")));

    return;
  } // end IF

  for (Common_SignalActionsIterator_t iterator = previousActions_in.begin();
       iterator != previousActions_in.end();
       iterator++)
    if (const_cast<ACE_Sig_Action&>((*iterator).second).register_action((*iterator).first,
                                                                        NULL) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                 (*iterator).first));
}

void
Common_Tools::retrieveSignalInfo(const int& signal_in,
                                 const siginfo_t& info_in,
                                 const ucontext_t* context_in,
                                 std::string& information_out)
{
  RPG_TRACE(ACE_TEXT("Common_Tools::retrieveSignalInfo"));

  // init return value
  information_out.resize(0);

  std::ostringstream information;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  // step0: common information (on POSIX.1b)
  information << ACE_TEXT("PID/UID: ");
  information << info_in.si_pid;
  information << ACE_TEXT("/");
  information << info_in.si_uid;

  // (try to) get user name
  char pw_buf[BUFSIZ];
  passwd pw_struct;
  passwd* pw_ptr = NULL;
// *PORTABILITY*: this isn't completely portable... (man getpwuid_r)
  if (::getpwuid_r(info_in.si_uid,
                   &pw_struct,
                   pw_buf,
                   sizeof(pw_buf),
                   &pw_ptr))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::getpwuid_r(%d) : \"%m\", continuing\n"),
               info_in.si_uid));
  } // end IF
  else
  {
    information << ACE_TEXT("[\"");
    information << pw_struct.pw_name;
    information << ACE_TEXT("\"]");
  } // end ELSE

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  information << ACE_TEXT(", errno: ");
  information << info_in.si_errno;
  information << ACE_TEXT("[\"");
  information << ACE_OS::strerror(info_in.si_errno);
  information << ACE_TEXT("\"], code: ");

  // step1: retrieve signal code...
  switch (info_in.si_code)
  {
    case SI_USER:
      information << ACE_TEXT("SI_USER"); break;
    case SI_KERNEL:
      information << ACE_TEXT("SI_KERNEL"); break;
    case SI_QUEUE:
      information << ACE_TEXT("SI_QUEUE"); break;
    case SI_TIMER:
      information << ACE_TEXT("SI_TIMER"); break;
    case SI_MESGQ:
      information << ACE_TEXT("SI_MESGQ"); break;
    case SI_ASYNCIO:
      information << ACE_TEXT("SI_ASYNCIO"); break;
    case SI_SIGIO:
      information << ACE_TEXT("SI_SIGIO"); break;
    case SI_TKILL:
      information << ACE_TEXT("SI_TKILL"); break;
    default:
    { // (signal-dependant) codes...
      switch (signal_in)
      {
        case SIGILL:
        {
          switch (info_in.si_code)
          {
            case ILL_ILLOPC:
              information << ACE_TEXT("ILL_ILLOPC"); break;
            case ILL_ILLOPN:
              information << ACE_TEXT("ILL_ILLOPN"); break;
            case ILL_ILLADR:
              information << ACE_TEXT("ILL_ILLADR"); break;
            case ILL_ILLTRP:
              information << ACE_TEXT("ILL_ILLTRP"); break;
            case ILL_PRVOPC:
              information << ACE_TEXT("ILL_PRVOPC"); break;
            case ILL_PRVREG:
              information << ACE_TEXT("ILL_PRVREG"); break;
            case ILL_COPROC:
              information << ACE_TEXT("ILL_COPROC"); break;
            case ILL_BADSTK:
              information << ACE_TEXT("ILL_BADSTK"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("FPE_INTDIV"); break;
            case FPE_INTOVF:
              information << ACE_TEXT("FPE_INTOVF"); break;
            case FPE_FLTDIV:
              information << ACE_TEXT("FPE_FLTDIV"); break;
            case FPE_FLTOVF:
              information << ACE_TEXT("FPE_FLTOVF"); break;
            case FPE_FLTUND:
              information << ACE_TEXT("FPE_FLTUND"); break;
            case FPE_FLTRES:
              information << ACE_TEXT("FPE_FLTRES"); break;
            case FPE_FLTINV:
              information << ACE_TEXT("FPE_FLTINV"); break;
            case FPE_FLTSUB:
              information << ACE_TEXT("FPE_FLTSUB"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("SEGV_MAPERR"); break;
            case SEGV_ACCERR:
              information << ACE_TEXT("SEGV_ACCERR"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("BUS_ADRALN"); break;
            case BUS_ADRERR:
              information << ACE_TEXT("BUS_ADRERR"); break;
            case BUS_OBJERR:
              information << ACE_TEXT("BUS_OBJERR"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("TRAP_BRKPT"); break;
            case TRAP_TRACE:
              information << ACE_TEXT("TRAP_TRACE"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("CLD_EXITED"); break;
            case CLD_KILLED:
              information << ACE_TEXT("CLD_KILLED"); break;
            case CLD_DUMPED:
              information << ACE_TEXT("CLD_DUMPED"); break;
            case CLD_TRAPPED:
              information << ACE_TEXT("CLD_TRAPPED"); break;
            case CLD_STOPPED:
              information << ACE_TEXT("CLD_STOPPED"); break;
            case CLD_CONTINUED:
              information << ACE_TEXT("CLD_CONTINUED"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
              information << ACE_TEXT("POLL_IN"); break;
            case POLL_OUT:
              information << ACE_TEXT("POLL_OUT"); break;
            case POLL_MSG:
              information << ACE_TEXT("POLL_MSG"); break;
            case POLL_ERR:
              information << ACE_TEXT("POLL_ERR"); break;
            case POLL_PRI:
              information << ACE_TEXT("POLL_PRI"); break;
            case POLL_HUP:
              information << ACE_TEXT("POLL_HUP"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
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
      information << ACE_TEXT(", overrun: ");
      information << info_in.si_overrun;
      information << ACE_TEXT(", (internal) id: ");
      information << info_in.si_timerid;

      break;
    }
    case SIGCHLD:
    {
      information << ACE_TEXT(", (exit) status: ");
      information << info_in.si_status;
      information << ACE_TEXT(", time consumed (user): ");
      information << info_in.si_utime;
      information << ACE_TEXT(" / (system): ");
      information << info_in.si_stime;

      break;
    }
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    {
      // *TODO*: more data ?
      information << ACE_TEXT(", fault at address: ");
      information << info_in.si_addr;

      break;
    }
    case SIGPOLL:
    {
      information << ACE_TEXT(", band event: ");
      information << info_in.si_band;
      information << ACE_TEXT(", (file) descriptor: ");
      information << info_in.si_fd;

      break;
    }
    default:
    {
      // *TODO*: handle more signals here ?
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("no additional information for signal: \"%S\"...\n"),
//                  signal_in));

      break;
    }
  } // end SWITCH
#else
  switch (signal_in)
  {
    case SIGINT:
      information << ACE_TEXT("SIGINT"); break;
    case SIGILL:
      information << ACE_TEXT("SIGILL"); break;
    case SIGFPE:
      information << ACE_TEXT("SIGFPE"); break;
    case SIGSEGV:
      information << ACE_TEXT("SIGSEGV"); break;
    case SIGTERM:
      information << ACE_TEXT("SIGTERM"); break;
    case SIGBREAK:
      information << ACE_TEXT("SIGBREAK"); break;
    case SIGABRT:
      information << ACE_TEXT("SIGABRT"); break;
    case SIGABRT_COMPAT:
      information << ACE_TEXT("SIGABRT_COMPAT"); break;
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid/unknown signal: %S, continuing\n"),
                 signal_in));

      break;
    }
  } // end SWITCH

  information << ACE_TEXT(", signalled handle: ");
  information << info_in.si_handle_;
  //information << ACE_TEXT(", array of signalled handle(s): ");
  //information << info_in.si_handles_;
#endif

  // OK: set return value
  information_out = information.str();
}
