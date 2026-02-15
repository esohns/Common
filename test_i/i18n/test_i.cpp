#include "stdafx.h"

#include "libintl.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Init_ACE.h"
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_string_tools.h"

#include "common_timer_tools.h"

#include "common_log_tools.h"

#define _(STRING) gettext(STRING)

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path_root =
    Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                                                      false);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                                                      false);

  // initialize results
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 't':
      {
        traceInformation_out = true;
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
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
        return false;
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

bool
do_initialize_i18n (const std::string& locale_in)
{
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // LocaleNameToLCID requires a LPCWSTR so we need to convert from char to wchar_t
  const auto wStringSize =
    MultiByteToWideChar (CP_UTF8, 0, locale_in.data(), static_cast<int> (locale_in.length()), nullptr, 0);
  std::wstring localeName;
  localeName.reserve(wStringSize);
  MultiByteToWideChar (CP_UTF8, 0, locale_in.data (), static_cast<int> (locale_in.length ()), localeName.data (), wStringSize);

  _configthreadlocale (_DISABLE_PER_THREAD_LOCALE);
  const auto localeId = LocaleNameToLCID (localeName.c_str (), LOCALE_ALLOW_NEUTRAL_NAMES);
  SetThreadLocale (localeId);
#else
  if (unlikely (!setlocale (LC_MESSAGES, locale_in.c_str ())))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setlocale(%d,\"%s\"): %m, aborting\n"),
                LC_MESSAGES,
                ACE_TEXT (locale_in.c_str ())));
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  bindtextdomain (GETTEXT_DOMAIN, GETTEXT_OUTPUT_DIR);
  bind_textdomain_codeset (GETTEXT_DOMAIN, ACE_TEXT_ALWAYS_CHAR ("UTF-8"));
  textdomain (GETTEXT_DOMAIN);

  return true;
}

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[])
{
  ACE_UNUSED_ARG (argc_in);
  ACE_UNUSED_ARG (argv_in);

  if (unlikely (!do_initialize_i18n (ACE_TEXT_ALWAYS_CHAR ("de_DE"))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to do_initialize_i18n(\"%s\"), returning\n"),
                ACE_TEXT ("de_DE")));
    return;
  } // end IF

  std::cout << _("Hello World!") << std::endl;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working\n")));
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  int result = EXIT_FAILURE, result_2 = -1;

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result_2 = ACE::init ();
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Profile_Timer process_profile;
  process_profile.start ();

//  Common_Tools::initialize ();
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  std::string path_root =
    Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                                                      false);
  bool trace_information = false;
  std::string log_file_name;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

  if (false)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  log_file_name =
    Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                      ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                     log_file_name,              // log file name
                                     false,                      // log to syslog ?
                                     false,                      // trace messages ?
                                     trace_information,          // debug messages ?
                                     NULL))                      // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"\n"),
              ACE_TEXT (Common_Timer_Tools::periodToString (working_time).c_str ())));

  // step3: shut down
  process_profile.stop ();

  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result_2 = process_profile.elapsed_time (elapsed_time);
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));
    goto clean;
  } // end IF
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ()),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#endif // ACE_WIN32 || ACE_WIN64

  result = EXIT_SUCCESS;

clean:
  Common_Log_Tools::finalize ();
  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
