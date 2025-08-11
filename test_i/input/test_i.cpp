#include "stdafx.h"

#include <iostream>
#include <string>

#include "ace/ACE.h"
#include "ace/Assert.h"
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
#include "common_tools.h"

#include "common_input_tools.h"

#include "common_log_tools.h"

#include "common_signal_common.h"
#include "common_signal_tools.h"

#include "common_timer_common.h"
#include "common_timer_second_publisher.h"
#include "common_timer_tools.h"

#include "common_ui_common.h"

#include "test_i_common.h"
#include "test_i_inputhandler.h"
#include "test_i_signalhandler.h"
#include "test_i_timerhandler.h"

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : line mode [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s          : simulate key presses [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      bool& logToFile_out,
                      bool& lineMode_out,
                      bool& simulateKeyPresses_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  logToFile_out = false;
  lineMode_out = false;
  simulateKeyPresses_out = false;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("lmst"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
//  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        lineMode_out = true;
        break;
      }
      case 's':
      {
        simulateKeyPresses_out = true;
        break;
      }
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

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         bool lineMode_in,
         bool simulateKeyPresses_in)
{
  // step1: initialize signals
  ACE_Sig_Set signals_a (true);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_a.empty_set ();
  signals_a.sig_add (SIGINT);            // 2       /* interrupt */
  signals_a.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_a.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_a.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_a.sig_add (SIGBREAK);          // 21      /* Ctrl-Break sequence */
  signals_a.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_a.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Sig_Set previous_signal_mask_a (false);
  Common_SignalActions_t previous_signal_actions_a;
  if (unlikely (!Common_Signal_Tools::preInitialize (signals_a,
                                                     COMMON_SIGNAL_DISPATCH_SIGNAL,
                                                     false, // using networking ?
                                                     false, // using asynch timers ?
                                                     previous_signal_actions_a,
                                                     previous_signal_mask_a)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), returning\n")));
    return;
  } // end IF
  ACE_Sig_Set ignored_signals_a (false);
  struct Common_EventDispatchState event_dispatch_state;
  struct Common_SignalHandlerConfiguration signal_configuration;
  signal_configuration.dispatchState = &event_dispatch_state;
  Test_I_SignalHandler signal_handler;
  if (unlikely (!signal_handler.initialize (signal_configuration)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_SignalHandler_T::initialize(), returning\n")));
    return;
  } // end IF
  if (unlikely (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                                  signals_a,
                                                  ignored_signals_a,
                                                  &signal_handler,
                                                  previous_signal_actions_a)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
    return;
  } // end IF

  // step2: initialize timer ?
  Test_I_TimerHandler timer_handler;
  if (simulateKeyPresses_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    Common_Tools::initialize (false, true); // initialize RNG
#else
    Common_Tools::initialize (true) // initialize RNG
#endif // ACE_WIN32 || ACE_WIN64
    Common_Timer_Tools::configuration_.publishSeconds = true;
    Common_Timer_Tools::initialize ();

    COMMON_TIMER_SECONDPUBLISHER_SINGLETON::instance ()->subscribe (&timer_handler);
  } // end IF

  struct Common_AllocatorConfiguration allocator_configuration;
  struct Common_EventDispatchConfiguration event_dispatch_configuration;
  event_dispatch_configuration.dispatch = COMMON_EVENT_DISPATCH_REACTOR;
  event_dispatch_state.configuration = &event_dispatch_configuration;
  Test_I_Input_Manager_t* input_manager_p = NULL;
  struct Common_UI_CBData cbdata_s;
  struct Common_Input_Configuration input_configuration;
  struct Common_Input_Manager_Configuration configuration;
  
  // step3a: initialize input handling
  if (unlikely (!Common_Input_Tools::initialize (lineMode_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Tools::initialize(), returning\n")));
    goto error;
  } // end IF

  // step3b: initialize input manager
  input_manager_p = INPUT_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (input_manager_p);
  input_configuration.allocatorConfiguration = &allocator_configuration;
  input_configuration.eventDispatchConfiguration =
      &event_dispatch_configuration;
  input_configuration.lineMode = lineMode_in;
  input_configuration.manager = input_manager_p;
  input_configuration.CBData = &cbdata_s;
  configuration.eventDispatchConfiguration = &event_dispatch_configuration;
  configuration.eventDispatchState = &event_dispatch_state;
  configuration.handlerConfiguration = &input_configuration;
  configuration.manageEventDispatch = true;
  if (unlikely (!input_manager_p->initialize (configuration)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Manager_T::initialize(): \"%m\", returning\n")));
    goto error;
  } // end IF
  if (unlikely (!input_manager_p->start (NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Manager_T::start(): \"%m\", returning\n")));
    goto error;
  } // end IF

  input_manager_p->wait (false); // N/A

error:
  // step4: finalize input
  if (unlikely (!Common_Input_Tools::finalize ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Tools::finalize(), returning\n")));
    return;
  } // end IF

  // step5: finalize timer ?
  if (simulateKeyPresses_in)
  {
    Common_Timer_Tools::finalize ();
    Common_Tools::finalize ();
  } // end IF

  // step6: finalize signal handling
  Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                 previous_signal_actions_a,
                                 previous_signal_mask_a);
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

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  bool log_to_file = false;
  std::string log_file_name;
  bool line_mode = false;
  bool simulate_key_presses = false;
  bool trace_information = false;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             log_to_file,
                             line_mode,
                             simulate_key_presses,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

//  if (!Common_File_Tools::isReadable (source_file_path))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("invalid argument(s), aborting\n")));
//    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
//    goto clean;
//  } // end IF

  // step1c: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                        ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Log_Tools::initialize (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR), // program name
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
           argv_in,
           line_mode,
           simulate_key_presses);
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
