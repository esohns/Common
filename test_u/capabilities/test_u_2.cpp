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

#include <sys/prctl.h>
#include <linux/capability.h>
#include <linux/securebits.h>

#include <iostream>
#include <string>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/Time_Value.h"
#include "ace/Version.h"

#include "common_os_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

//#include "test_u_common.h"
//#include "test_u_defines.h"

void
do_printUsage (const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

//  std::string configuration_path =
//    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-a [STRING/INT]: set process ambient capability")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b [STRING/INT]: set process bounding capability")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-k [0|1]       : set/clear SECBIT_KEEP_CAPS")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l             : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p             : print process capabilities [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t             : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v             : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned long& ambientCapability_out,
                     unsigned long& boundingCapability_out,
                     int& secBitKeepCaps_out,
                     bool& logToFile_out,
                     bool& printCapabilities_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

//  std::string configuration_path =
//    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  ambientCapability_out = 0;
  boundingCapability_out = 0;
  secBitKeepCaps_out = -1;
  logToFile_out = false;
  printCapabilities_out = false;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("a:b:k:lptv"),
                               1,                          // skip command name
                               1,                          // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                               0);                         // for now, don't use long options
//  int result = -1;

//  result = argument_parser.long_option (ACE_TEXT ("sync"),
//                                        'x',
//                                        ACE_Get_Opt::NO_ARG);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Get_Opt::long_option(): \"%m\", aborting\n")));
//    return false;
//  } // end IF

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> ambientCapability_out;
        break;
      }
      case 'b':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> boundingCapability_out;
        break;
      }
      case 'k':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> secBitKeepCaps_out;
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'p':
      {
        printCapabilities_out = true;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argument_parser.last_option ()));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("found long option \"%s\", continuing\n"),
                    argument_parser.long_option ()));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

//void
//do_initializeSignals (bool allowUserRuntimeConnect_in,
//                      ACE_Sig_Set& signals_out,
//                      ACE_Sig_Set& ignoredSignals_out)
//{
//  COMMON_TRACE (ACE_TEXT ("::do_initializeSignals"));

//  int result = -1;

//  // initialize return value(s)
//  result = signals_out.empty_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  result = ignoredSignals_out.empty_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
//    return;
//  } // end IF

//  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
//  //                ACE_Sig_Set::fill_set() doesn't really work as specified
//  // --> add valid signals (see <signal.h>)...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
//  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
//  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
//  //  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
//  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
//  if (allowUserRuntimeConnect_in)
//  {
//    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
//    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
//  } // end IF
//  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
//  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
//#else
//  result = signals_out.fill_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  // *NOTE*: cannot handle some signals --> registration fails for these...
//  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
//  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
//  // ---------------------------------------------------------------------------
//  if (!allowUserRuntimeConnect_in)
//  {
//    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
//    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
//  } // end IF
//  // *NOTE* core dump on SIGSEGV
//  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
//  // *NOTE* don't care about SIGPIPE
//  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */

////  signals_out.sig_del (SIGIO);             // 29      /* I/O now possible */
//  // remove realtime-signals (don't need 'em)

//#ifdef ENABLE_VALGRIND_SUPPORT
//  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
//  // if the application installs its own handler (see documentation)
//  if (RUNNING_ON_VALGRIND)
//    signals_out.sig_del (SIGRTMAX);        // 64
//#endif
//#endif
//}

void
do_printVersion (const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_printVersion"));

  std::ostringstream converter;

  // compiler version string...
  converter << ACE::compiler_major_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::compiler_minor_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::compiler_beta_version ();

  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" compiled on ")
            << ACE_TEXT_ALWAYS_CHAR (ACE::compiler_name ())
            << ACE_TEXT (" ")
            << converter.str ()
            << std::endl << std::endl;

  std::cout << ACE_TEXT ("libraries: ")
            << std::endl
#if defined (HAVE_CONFIG_H)
            << ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME)
            << ACE_TEXT_ALWAYS_CHAR (": ")
//            << ACE_TEXT_ALWAYS_CHAR (LIBCOMMON_VERSION)
            << Common_VERSION_MAJOR << ACE_TEXT_ALWAYS_CHAR (".")
            << Common_VERSION_MINOR << ACE_TEXT_ALWAYS_CHAR (".")
            << Common_VERSION_MICRO
            << (ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (Common_VERSION_DEVEL)) ? ACE_TEXT_ALWAYS_CHAR ("-") : ACE_TEXT_ALWAYS_CHAR (""))
            << (ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (Common_VERSION_DEVEL)) ? ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_ALWAYS_CHAR (Common_VERSION_DEVEL)) : ACE_TEXT_ALWAYS_CHAR (""))
            << std::endl
#endif // HAVE_CONFIG_H
            ;

  converter.str ("");
  converter.clear ();
  // ACE version string...
  converter << ACE::major_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::minor_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::beta_version ();

  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::cout << ACE_TEXT_ALWAYS_CHAR ("ACE: ")
//             << ACE_VERSION
            << converter.str ()
            << std::endl;
}

void
do_work (unsigned long ambientCapability_in,
         unsigned long boundingCapability_in,
         int secBitKeepCaps_in,
         bool printCapabilities_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  int result = -1;

  // retrieve secure bits
  int secure_bits = ::prctl (PR_GET_SECUREBITS);

  if (ambientCapability_in)
  {
    // sanity check(s)
    if (secure_bits & SECBIT_NO_CAP_AMBIENT_RAISE)
    {
      // sanity check(s)
      // *NOTE*: see also: man capabilities(7)
      if (!Common_OS_Tools::hasCapability (CAP_SETPCAP))
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("%P/%t: does not have CAP_SETPCAP capability, cannot clear SECBIT_NO_CAP_AMBIENT_RAISE, continuing\n")));

      secure_bits &= ~SECBIT_NO_CAP_AMBIENT_RAISE;
      result = ::prctl (PR_SET_SECUREBITS,
                        secure_bits);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%P/%t: failed to prctl(PR_SET_SECUREBITS,%x): \"%m\", continuing\n"),
                    secure_bits));
      else
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%P/%t: cleared SECURE_NO_CAP_AMBIENT_RAISE, continuing\n")));
    } // end IF
    // *NOTE*: see also: man prctl(2)
    if (!Common_OS_Tools::hasCapability (ambientCapability_in, CAP_PERMITTED) ||
        !Common_OS_Tools::hasCapability (ambientCapability_in, CAP_INHERITABLE))
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%P/%t: does not have capability (was: %s) in 'permitted' and 'inheritable' sets, cannot add it to 'ambient' set, continuing\n")));

    result = ::prctl (PR_CAP_AMBIENT,
                      PR_CAP_AMBIENT_RAISE,
                      ambientCapability_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%P/%t: failed to prctl(PR_CAP_AMBIENT,PR_CAP_AMBIENT_RAISE,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_OS_Tools::capabilityToString (ambientCapability_in).c_str ())));
  } // end IF

  if (boundingCapability_in)
  {
    result = ::prctl (PR_CAP_AMBIENT,
                      PR_CAP_AMBIENT_RAISE,
                      ambientCapability_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%P/%t: failed to prctl(PR_CAP_AMBIENT,PR_CAP_AMBIENT_RAISE,%s): \"%m\", continuing\n")));
  } // end IF

  if (secBitKeepCaps_in != -1)
  { ACE_ASSERT ((secBitKeepCaps_in == 0) || (secBitKeepCaps_in == 1));
//    if (secBitKeepCaps_in == 0)
//      secure_bits &= ~SECBIT_KEEP_CAPS;
//    else
//      secure_bits |= SECBIT_KEEP_CAPS;
//    result = ::prctl (PR_SET_SECUREBITS,
//                      secure_bits);
    result = ::prctl (PR_SET_KEEPCAPS,
                      secBitKeepCaps_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%P/%t: failed to prctl(PR_SET_KEEPCAPS,%d): \"%m\", continuing\n"),
                  secBitKeepCaps_in));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%P/%t: %s SECBIT_KEEP_CAPS, continuing\n"),
                  (secBitKeepCaps_in ? ACE_TEXT ("set") : ACE_TEXT ("cleared"))));
  } // end IF

  if (printCapabilities_in)
    Common_OS_Tools::printCapabilities ();
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("::main"));

//  int result = -1;

  // process profile
//  ACE_Profile_Timer process_profile;
//  process_profile.start ();

//  std::string configuration_path =
//    Common_File_Tools::getWorkingDirectory ();

  // step1a set defaults
  unsigned long ambient_capability = 0;
  unsigned long bounding_capability = 0;
  bool log_to_file = false;
  int secbit_keep_caps = -1;
  bool print_capabilities = false;
  bool trace_information = false;
  bool print_version_and_exit = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in, argv_in,
                            ambient_capability,
                            bounding_capability,
                            secbit_keep_caps,
                            log_to_file,
                            print_capabilities,
                            trace_information,
                            print_version_and_exit))
  {
    do_printUsage (ACE::basename (argv_in[0]));
    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  bool invalid_arguments_b = false;
  if (ambient_capability &&
      (!Common_OS_Tools::hasCapability (ambient_capability, CAP_PERMITTED) ||
       !Common_OS_Tools::hasCapability (ambient_capability, CAP_INHERITABLE)))
  { // *NOTE*: see also: man prctl(2)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("ambient capability (was: %s) cannot be set unless it is in the 'permitted' and 'inheritable' sets, continuing\n"),
                ACE_TEXT (Common_OS_Tools::capabilityToString (ambient_capability).c_str ())));
//    invalid_arguments_b = true;
  } // end IF
  if (invalid_arguments_b)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));
    return EXIT_FAILURE;
  } // end IF

  // step1d: initialize logging and/or tracing
//  Common_MessageStack_t log_stack;
//  ACE_Null_Mutex log_stack_lock;
//  Common_SynchLogger_t logger (&log_stack,
//                               NULL);
//                               &log_stack_lock);
  std::string log_file_name;
  if (log_to_file)
  {
    log_file_name =
        Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                           ACE::basename (argv_in[0]));
    ACE_ASSERT (!log_file_name.empty ());
  } // end IF
  if (!Common_Log_Tools::initialize (ACE::basename (argv_in[0]),               // program name
                                     log_file_name,                            // log file name
                                     false,                                    // log to syslog ?
                                     false,                                    // trace messages ?
                                     trace_information,                        // debug messages ?
                                     NULL))                                    // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initialize(), aborting\n")));
    return EXIT_FAILURE;
  } // end IF

//  // step1e: pre-initialize signal handling
//  ACE_Sig_Set signal_set (0);
//  ACE_Sig_Set ignored_signal_set (0);
//  do_initializeSignals (true, // allow SIGUSR1/SIGBREAK
//                        signal_set,
//                        ignored_signal_set);
//  Common_SignalActions_t previous_signal_actions;
//  sigset_t previous_signal_mask;
//  result = ACE_OS::sigemptyset (&previous_signal_mask);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

//    Common_Tools::finalizeLogging ();
//    return EXIT_FAILURE;
//  } // end IF
//  if (!Common_Signal_Tools::preInitialize (signal_set,
//                                           use_reactor,
//                                           previous_signal_actions,
//                                           previous_signal_mask))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), aborting\n")));

//    Common_Tools::finalizeLogging ();
//    return EXIT_FAILURE;
//  } // end IF
//  Test_U_AudioEffect_SignalHandler signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                                   &gtk_cb_data_p->lock);

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

//    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                   signal_set,
//                                   previous_signal_actions,
//                                   previous_signal_mask);
    Common_Log_Tools::finalize ();
    return EXIT_SUCCESS;
  } // end IF

//  // step1g: set process resource limits
//  // *NOTE*: settings will be inherited by any child processes
//  if (!Common_Tools::setResourceLimits (false,  // file descriptors
//                                        true,   // stack traces
//                                        false)) // pending signals
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

//    Common_Signal_Tools::finalize ((COMMON_EVENT_USE_REACTOR ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                             : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                   signal_set,
//                                   previous_signal_actions,
//                                   previous_signal_mask);
//    Common_Tools::finalizeLogging ();
//    return EXIT_FAILURE;
//  } // end IF

#if defined (_DEBUG)
  ACE_High_Res_Timer timer;
  timer.start ();
#endif
  // step2: do actual work
  do_work (ambient_capability,
           bounding_capability,
           secbit_keep_caps,
           print_capabilities);
#if defined (_DEBUG)
  timer.stop ();
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  working_time_string =
      Common_Timer_Tools::periodToString (working_time);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));
#endif // _DEBUG

//  process_profile.stop ();
//  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
//  elapsed_time.real_time = 0.0;
//  elapsed_time.user_time = 0.0;
//  elapsed_time.system_time = 0.0;
//  result == process_profile.elapsed_time (elapsed_time);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

//    Common_Signal_Tools::finalize ((COMMON_EVENT_USE_REACTOR ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                             : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                   signal_set,
//                                   previous_signal_actions,
//                                   previous_signal_mask);
//    Common_Tools::finalizeLogging ();
//    return EXIT_FAILURE;
//  } // end IF
//  ACE_Profile_Timer::Rusage elapsed_rusage;
//  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
//  process_profile.elapsed_rusage (elapsed_rusage);
//  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
//  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
//  std::string user_time_string;
//  std::string system_time_string;
//  Common_Timer_Tools::periodToString (user_time,
//                                      user_time_string);
//  Common_Timer_Tools::periodToString (system_time,
//                                      system_time_string);
////#if defined (ACE_WIN32) || defined (ACE_WIN64)
////  ACE_DEBUG ((LM_DEBUG,
////    ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
////              elapsed_time.real_time,
////              elapsed_time.user_time,
////              elapsed_time.system_time,
////              ACE_TEXT (user_time_string.c_str ()),
////              ACE_TEXT (system_time_string.c_str ())));
////#else
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
//              elapsed_time.real_time,
//              elapsed_time.user_time,
//              elapsed_time.system_time,
//              ACE_TEXT (user_time_string.c_str ()),
//              ACE_TEXT (system_time_string.c_str ()),
//              elapsed_rusage.ru_maxrss,
//              elapsed_rusage.ru_ixrss,
//              elapsed_rusage.ru_idrss,
//              elapsed_rusage.ru_isrss,
//              elapsed_rusage.ru_minflt,
//              elapsed_rusage.ru_majflt,
//              elapsed_rusage.ru_nswap,
//              elapsed_rusage.ru_inblock,
//              elapsed_rusage.ru_oublock,
//              elapsed_rusage.ru_msgsnd,
//              elapsed_rusage.ru_msgrcv,
//              elapsed_rusage.ru_nsignals,
//              elapsed_rusage.ru_nvcsw,
//              elapsed_rusage.ru_nivcsw));
////#endif

//  Common_Signal_Tools::finalize ((COMMON_EVENT_USE_REACTOR ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                           : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                 signal_set,
//                                 previous_signal_actions,
//                                 previous_signal_mask);
  Common_Log_Tools::finalize ();

  return EXIT_SUCCESS;
} // end main
