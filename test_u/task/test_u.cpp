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
#include <sstream>
#include <string>

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_process_tools.h"

#include "common_log_tools.h"

#include "common_signal_common.h"
#include "common_signal_handler.h"
#include "common_signal_tools.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"
#include "common_timer_tools.h"

#include "common_task.h"
#include "common_task_common.h"
#include "common_task_defines.h"

#include "common_test_u_common.h"

//////////////////////////////////////////

class Test_U_SignalHandler
 : public Common_SignalHandler_T<struct Common_SignalHandlerConfiguration>
{
 typedef Common_SignalHandler_T<struct Common_SignalHandlerConfiguration> inherited;

 public:
  Test_U_SignalHandler ()
   : inherited (NULL)
  {}

  inline virtual void handle (const struct Common_Signal& signal_in) {}
};

//////////////////////////////////////////

class Test_U_Task
 : public Common_Task_2<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        struct Common_Task_Statistic>
{
  typedef Common_Task_2<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        struct Common_Task_Statistic> inherited;

 public:
  Test_U_Task (Common_ITaskManager* manager_in)
   : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_TASK_THREAD_NAME),
                COMMON_TASK_THREAD_GROUP_ID,
                1U,
                false,
                NULL,
                manager_in)
  {}
  inline virtual ~Test_U_Task () {}

  virtual void handle (ACE_Message_Block*& message_inout)
  {
    // sanity check(s)
    ACE_ASSERT (!inherited::threadIds_.empty ());
    ACE_ASSERT (message_inout);

    // *TODO*: do some meaningful work here
    ACE_OS::sleep (ACE_Time_Value (3, 0)); // sleep some seconds

    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
      inherited::statistic_.bytes += message_inout->total_length ();
      inherited::statistic_.messages += 1;
    } // end lock scope
    message_inout->release (); message_inout = NULL;

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s):[%t]: work done, shutting down...\n"),
                ACE_TEXT (Common_Process_Tools::name (inherited::threadIds_[0]).c_str ())));
    inherited::finished ();
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task ())
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task (const Test_U_Task&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task& operator= (const Test_U_Task&))
};

class Test_U_Task_2
 : public Common_Task_2<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        struct Common_Task_Statistic>
{
  typedef Common_Task_2<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        struct Common_Task_Statistic> inherited;

 public:
  Test_U_Task_2 (Common_ITaskManager* manager_in)
   : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_TASK_THREAD_NAME),
                COMMON_TASK_THREAD_GROUP_ID,
                1U,
                false,
                NULL,
                manager_in)
  {}
  inline virtual ~Test_U_Task_2 () {}

  virtual void handle (ACE_Message_Block*& message_inout)
  {
    // sanity check(s)
    ACE_ASSERT (!inherited::threadIds_.empty ());
    ACE_ASSERT (message_inout);

    // *TODO*: do some meaningful work here
    ACE_OS::sleep (ACE_Time_Value (5, 0)); // sleep some seconds

    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
      inherited::statistic_.bytes += message_inout->total_length ();
      inherited::statistic_.messages += 1;
    } // end lock scope
    message_inout->release (); message_inout = NULL;

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s):[%t]: work done, shutting down...\n"),
                ACE_TEXT (Common_Process_Tools::name (inherited::threadIds_[0]).c_str ())));
    inherited::finished ();
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_2 ())
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_2 (const Test_U_Task_2&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_2& operator= (const Test_U_Task_2&))
};

class Test_U_Task_3
 : public Common_Task_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t>
{
  typedef Common_Task_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t> inherited;

 public:
  Test_U_Task_3 (Common_ITaskManager* manager_in)
   : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_TASK_THREAD_NAME),
                COMMON_TASK_THREAD_GROUP_ID,
                1U,
                false,
                NULL)
   , manager_ (manager_in)
  { ACE_ASSERT (manager_);
    manager_->register_ (this);
  }
  inline virtual ~Test_U_Task_3 () {}

  virtual void finished ()
  {
    inherited::finished_ = true;
    inherited::stop (false,  // *WARNING*: (probably) cannot wait here
                     false); // high priority ?

    if (likely (manager_))
      manager_->finished (this);
  }

  virtual void handle (ACE_Message_Block*& message_inout)
  {
    // sanity check(s)
    ACE_ASSERT (!inherited::threadIds_.empty ());
    ACE_ASSERT (message_inout);

    // *TODO*: do some meaningful work here
    ACE_OS::sleep (ACE_Time_Value (7, 0)); // sleep some seconds

    //{ ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
    //  inherited::statistic_.bytes += message_inout->total_length ();
    //  inherited::statistic_.messages += 1;
    //} // end lock scope
    message_inout->release (); message_inout = NULL;

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s):[%t]: work done, shutting down...\n"),
                ACE_TEXT (Common_Process_Tools::name (inherited::threadIds_[0]).c_str ())));
    finished ();
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_3 ())
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_3 (const Test_U_Task_3&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Task_3& operator= (const Test_U_Task_3&))

  Common_ITaskManager* manager_;
};

//////////////////////////////////////////

enum Test_U_ModeType
{
  TEST_U_MODE_DEFAULT = 0,
  ////////////////////////////////////////
  TEST_U_MODE_MAX,
  TEST_U_MODE_INVALID
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m [INTEGER]: mode [")
            << TEST_U_MODE_DEFAULT
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
                     bool& logToFile_out,
                     enum Test_U_ModeType& mode_out,
                     bool& traceInformation_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  mode_out = TEST_U_MODE_DEFAULT;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("lm:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option_i = 0;
  while ((option_i = argument_parser ()) != EOF)
  {
    switch (option_i)
    {
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        std::istringstream converter;
        converter.str (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()));
        int mode_i = 0;
        converter >> mode_i;
        mode_out = static_cast<enum Test_U_ModeType> (mode_i);
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
do_work (enum Test_U_ModeType mode_in,
         ACE_Sig_Set& signals_in,
         ACE_Sig_Set& ignoredSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  ACE_Sig_Set previous_signal_mask (false); // fill ?
  Common_SignalActions_t previous_signal_actions;
  struct Common_TimerConfiguration timer_configuration;
  struct Common_Task_ManagerConfiguration task_configuration;

  struct Common_SignalHandlerConfiguration signal_configuration;
  Test_U_SignalHandler signal_handler;
  if (!signal_handler.initialize (signal_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_SignalHandler_T::initialize(), returning\n")));
    goto error;
  } // end IF

  switch (mode_in)
  {
    case TEST_U_MODE_DEFAULT:
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

      Common_Timer_Manager_t* timer_manager_p =
        COMMON_TIMERMANAGER_SINGLETON::instance ();
      ACE_ASSERT (timer_manager_p);
      bool result = timer_manager_p->initialize (timer_configuration);
      ACE_ASSERT (result);
      result = timer_manager_p->start (NULL);
      ACE_ASSERT (result);

      Common_Task_Manager_t* task_manager_p =
        COMMON_TASK_MANAGER_SINGLETON::instance ();
      ACE_ASSERT (task_manager_p);
      result = task_manager_p->initialize (task_configuration);
      ACE_ASSERT (result);
      result = task_manager_p->start (NULL);
      ACE_ASSERT (result);
      ACE_UNUSED_ARG (result);

      Test_U_Task task (task_manager_p);
      Test_U_Task_2 task_2 (task_manager_p);
      Test_U_Task_3 task_3 (task_manager_p);

      ACE_Message_Block* message_block_p = NULL;
      ACE_NEW_NORETURN (message_block_p,
                        ACE_Message_Block (100,
                                           ACE_Message_Block::MB_DATA,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                           ACE_Time_Value::zero,
                                           ACE_Time_Value::max_time,
                                           NULL,
                                           NULL));
      ACE_ASSERT (message_block_p);
      int result_2 = task.put (message_block_p, NULL);
      ACE_ASSERT (result_2 > 0);
      ACE_UNUSED_ARG (result_2);

      message_block_p = NULL;
      ACE_NEW_NORETURN (message_block_p,
                        ACE_Message_Block (200,
                                           ACE_Message_Block::MB_DATA,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                           ACE_Time_Value::zero,
                                           ACE_Time_Value::max_time,
                                           NULL,
                                           NULL));
      ACE_ASSERT (message_block_p);
      result_2 = task_2.put (message_block_p, NULL);
      ACE_ASSERT (result_2 > 0);
      message_block_p = NULL;

      ACE_NEW_NORETURN (message_block_p,
                        ACE_Message_Block (300,
                                           ACE_Message_Block::MB_DATA,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                           ACE_Time_Value::zero,
                                           ACE_Time_Value::max_time,
                                           NULL,
                                           NULL));
      ACE_ASSERT (message_block_p);
      result_2 = task_3.put (message_block_p, NULL);
      ACE_ASSERT (result_2 > 0);
      message_block_p = NULL;

      task_manager_p->wait (true); // N/A
      task_manager_p->stop (true,   // wait for completion
                            false); // high priority ?

      timer_manager_p->stop (true,   // wait for completion
                             false); // high priority ?

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
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));

  bool log_to_file = false;
  enum Test_U_ModeType mode_type_e = TEST_U_MODE_DEFAULT;
  bool trace_information_b = false;
  std::string log_file_name;
  ACE_High_Res_Timer timer;
  std::string working_time_string;
  ACE_Time_Value working_time;
  ACE_Sig_Set signals, ignored_signals;

  if (!do_processArguments (argc_in,
                            argv_in,
                            log_to_file,
                            mode_type_e,
                            trace_information_b))
  {
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));
    goto error;
  } // end IF

  // step1d: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                        Common_File_Tools::executable);
  if (!Common_Log_Tools::initialize (Common_File_Tools::executable, // program name
                                     log_file_name,                 // log file name
                                     false,                         // log to syslog ?
                                     false,                         // trace messages ?
                                     trace_information_b,           // debug messages ?
                                     NULL))                         // logger ?
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
  Common_Log_Tools::finalize ();
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
