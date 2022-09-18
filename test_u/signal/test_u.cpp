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
#include "ace/ace_wchar.h"
#include "stdafx.h"

#include "ace/config-lite.h"

#include <iostream>
#include <sstream>
#include <string>

#include "ace/ACE.h"
#include "ace/Condition_T.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Signal.h"
#include "ace/Thread_Mutex.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_file_tools.h"

#include "common_log_tools.h"

#include "common_signal_common.h"
#include "common_signal_handler.h"
#include "common_signal_tools.h"

#include "common_timer_tools.h"

#include "common_test_u_common.h"

//////////////////////////////////////////

class Test_U_SignalHandler
 : public Common_SignalHandler_T<struct Test_U_SignalHandlerConfiguration>
{
  typedef Common_SignalHandler_T<struct Test_U_SignalHandlerConfiguration> inherited;

 public:
  Test_U_SignalHandler() : inherited (this) {}
  inline virtual ~Test_U_SignalHandler () {}

  // implement Common_ISignal
  virtual void handle (const struct Common_Signal& signal_in)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received signal: [%d] \"%S\"\n"),
                signal_in.signal, signal_in.signal));

    if (signal_in.signal == SIGINT)
    {
      int result = -1;
      { ACE_GUARD (ACE_Thread_Mutex, aGuard, inherited::configuration_->condition->mutex ());
        result = inherited::configuration_->condition->broadcast ();
      } // end lock scope
      ACE_ASSERT (result == 0);
    } // end IF
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_SignalHandler (const Test_U_SignalHandler&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_SignalHandler& operator= (const Test_U_SignalHandler&))
};

//////////////////////////////////////////

enum Test_U_Common_Signal_ModeType
{
  TEST_U_COMMON_SIGNAL_MODE_DEFAULT = 0,
  TEST_U_COMMON_SIGNAL_MODE_IGNORE_SIGINT,
  TEST_U_COMMON_SIGNAL_MODE_HANDLE_RT,
  ////////////////////////////////////////
  TEST_U_COMMON_SIGNAL_MODE_MAX,
  TEST_U_COMMON_SIGNAL_MODE_INVALID
};

void
do_printUsage (const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m [INTEGER]: mode [")
            << TEST_U_COMMON_SIGNAL_MODE_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     enum Test_U_Common_Signal_ModeType& mode_out,
                     bool& traceInformation_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  mode_out = TEST_U_COMMON_SIGNAL_MODE_DEFAULT;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("m:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option_i = 0;
  while ((option_i = argument_parser ()) != EOF)
  {
    switch (option_i)
    {
      case 'm':
      {
        std::istringstream converter;
        converter.str (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()));
        int mode_i = 0;
        converter >> mode_i;
        mode_out = static_cast<enum Test_U_Common_Signal_ModeType> (mode_i);
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
do_initializeSignals (ACE_Sig_Set& signals_out,
                      ACE_Sig_Set& ignoredSignals_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_initializeSignals"));

  int result = -1;

  // initialize return value(s)
  result = signals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF
  result = ignoredSignals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF

   // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
   //                ACE_Sig_Set::fill_set() doesn't really work as specified
   // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_out.sig_add (SIGBREAK);          // 21      /* Ctrl-Break sequence */
  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  result = signals_out.fill_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
}

void
do_work (enum Test_U_Common_Signal_ModeType mode_in,
         ACE_Sig_Set& signals_in,
         ACE_Sig_Set& ignoredSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  ACE_Sig_Set previous_signal_mask (false); // fill ?
  Common_SignalActions_t previous_signal_actions;

  ACE_Thread_Mutex lock;
  ACE_Thread_Condition<ACE_Thread_Mutex> condition (lock, NULL, NULL);
  struct Test_U_SignalHandlerConfiguration configuration;
  configuration.condition = &condition;
  // *IMPORTANT NOTE*: on UNIX systems, this invokes the Proactor ctor, which
  //                   blocks RT signals (the default proactor on UNIX is
  //                   ACE_POSIX_SIG_Proactor)
  Test_U_SignalHandler signal_handler;
  if (!signal_handler.initialize (configuration))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_SignalHandler_T::initialize(), returning\n")));
    goto error;
  } // end IF

  switch (mode_in)
  {
    case TEST_U_COMMON_SIGNAL_MODE_DEFAULT:
    {
      if (!Common_Signal_Tools::preInitialize (signals_in,
                                               COMMON_SIGNAL_DISPATCH_SIGNAL,
                                               false, // using networking ?
                                               false, // using asynch timers ?
                                               previous_signal_actions,
                                               previous_signal_mask))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), returning\n")));
        return;
      } // end IF

      if (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                            signals_in,
                                            ignoredSignals_in,
                                            &signal_handler,
                                            previous_signal_actions))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
        goto error;
      } // end IF

      break;
    }
    case TEST_U_COMMON_SIGNAL_MODE_IGNORE_SIGINT:
    {
      if (!Common_Signal_Tools::preInitialize (signals_in,
                                               COMMON_SIGNAL_DISPATCH_SIGNAL,
                                               false, // using networking ?
                                               false, // using asynch timers ?
                                               previous_signal_actions,
                                               previous_signal_mask))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), returning\n")));
        return;
      } // end IF

      ignoredSignals_in.sig_add (SIGINT);
      if (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                            signals_in,
                                            ignoredSignals_in,
                                            &signal_handler,
                                            previous_signal_actions))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
        goto error;
      } // end IF

      break;
    }
    case TEST_U_COMMON_SIGNAL_MODE_HANDLE_RT:
    {
      // *IMPORTANT NOTE*: on UNIX systems, unblock RT signals (see above)
      sigset_t original_mask;
      Common_Signal_Tools::unblockRealtimeSignals (original_mask);

      if (!Common_Signal_Tools::preInitialize (signals_in,
                                               COMMON_SIGNAL_DISPATCH_SIGNAL,
                                               true,  // using networking ?
                                               false, // using asynch timers ?
                                               previous_signal_actions,
                                               previous_signal_mask))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), returning\n")));
        return;
      } // end IF

      if (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                            signals_in,
                                            ignoredSignals_in,
                                            &signal_handler,
                                            previous_signal_actions))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
        goto error;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), returning\n"),
                  mode_in));
      break;
    }
  } // end SWITCH

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock);
    int result = configuration.condition->wait (NULL);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Condition::wait(): \"%m\", continuing\n")));
  } // end lock scope

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working\n")));

error:
  Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                 previous_signal_actions,
                                 previous_signal_mask);
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("::main"));

  int result = EXIT_FAILURE;

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  int result_2 = ACE::init ();
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
//  ACE_Profile_Timer process_profile;
//  process_profile.start ();

  enum Test_U_Common_Signal_ModeType mode_type_e =
    TEST_U_COMMON_SIGNAL_MODE_DEFAULT;
  bool trace_information_b = false;
  std::string log_file_name;
  ACE_High_Res_Timer timer;
  std::string working_time_string;
  ACE_Time_Value working_time;
  ACE_Sig_Set signals, ignored_signals;

  if (!do_processArguments (argc_in,
                            argv_in,
                            mode_type_e,
                            trace_information_b))
  {
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));
    goto error;
  } // end IF

  // step1d: initialize logging and/or tracing
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                            log_file_name,                                     // log file name
                                            false,                                             // log to syslog ?
                                            false,                                             // trace messages ?
                                            trace_information_b,                               // debug messages ?
                                            NULL))                                             // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
    goto error;
  } // end IF

  do_initializeSignals (signals,
                        ignored_signals);

  timer.start ();
  do_work (mode_type_e,
           signals,
           ignored_signals);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  result = EXIT_SUCCESS;

error:
  Common_Log_Tools::finalizeLogging ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result_2 = ACE::fini ();
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
