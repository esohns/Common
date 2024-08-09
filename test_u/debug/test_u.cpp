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
#include <string>

#include "sys/capability.h"
#include "sys/prctl.h"

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_process_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

void
do_printUsage (const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [PATH]     : executable")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s            : do not halt the program (SIGSTOP) before exec [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t            : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-w [DIRECTORY]: working directory [current]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     std::string& executableFilePath_out,
                     std::string& arguments_out,
                     bool& haltBeforeExec_out,
                     std::string& workingDirectory_out,
                     bool& traceInformation_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  executableFilePath_out.clear ();
  arguments_out.clear ();
  haltBeforeExec_out = true;
  workingDirectory_out = Common_File_Tools::getWorkingDirectory();
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("f:stw:"),
                               1,                            // skip command name
                               1,                            // report parsing errors
                               ACE_Get_Opt::RETURN_IN_ORDER, // ordering
                               0);                           // for now, don't use long options

  int option_i = 0;
  bool done = false;
  while ((option_i = argument_parser ()) != EOF)
  {
    switch (option_i)
    {
      case 'f':
      {
        executableFilePath_out =
            ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        done = true;
        break;
      }
      case 's':
      {
        haltBeforeExec_out = false;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'w':
      {
        workingDirectory_out =
            ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
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
    if (done)
      break;
  } // end WHILE
//  ACE_ASSERT (option_i == EOF);
  for (int i = argument_parser.opt_ind ();
       i < argc_in;
       ++i)
    arguments_out +=
        (std::string (ACE_TEXT_ALWAYS_CHAR (argv_in[i])) + ACE_TEXT_ALWAYS_CHAR (" "));
  if (argument_parser.opt_ind ())
    arguments_out.erase (--arguments_out.end ());

  return true;
}

void
do_work (const std::string& executableFilePath_in,
         const std::string& arguments_in,
         bool haltBeforeExec_in,
         const std::string& workingDirectory_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (Common_File_Tools::canExecute (executableFilePath_in, ACE_TEXT_ALWAYS_CHAR ("")));
#else
  ACE_ASSERT (Common_File_Tools::canExecute (executableFilePath_in, static_cast<uid_t> (-1)));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (Common_File_Tools::isDirectory (workingDirectory_in));

  cap_t capabilities_p = cap_get_proc ();
  if (!capabilities_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to cap_get_proc(): \"%m\", returning\n")));
    return;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%P: capabilities: %s\n"),
              ACE_TEXT (cap_to_text (capabilities_p, NULL))));
#endif // _DEBUG
  cap_free (capabilities_p); capabilities_p = NULL;

  pid_t pid_i = 0;
  char* argv_a[BUFSIZ];
  ACE_OS::memset (argv_a, 0, sizeof (char*[BUFSIZ]));
  argv_a[0] = const_cast<char*> (executableFilePath_in.c_str ());
  int i = 1;
  const char* white_space_p = ACE_TEXT_ALWAYS_CHAR (" \t\v\n\r");
  char* current_position_p = NULL;
  char* token_p = ACE_OS::strtok_r (const_cast<char*> (arguments_in.data ()),
                                    white_space_p,
                                    &current_position_p);
  while (token_p)
  {
    argv_a[i++] = token_p;
    token_p = ACE_OS::strtok_r (NULL,
                                white_space_p,
                                &current_position_p);
  } // end WHILE

  int result = -1;
  if (likely (!ACE_OS::strcmp (Common_File_Tools::getWorkingDirectory ().c_str (),
                               workingDirectory_in.c_str ())))
      goto continue_;
  result = ACE_OS::chdir (workingDirectory_in.c_str ());
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::chdir(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (workingDirectory_in.c_str ())));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%P: changed working directory (is: \"%s\")...\n"),
              ACE_TEXT (workingDirectory_in.c_str ())));

continue_:
  if (likely (!haltBeforeExec_in))
    goto continue_2;
  pid_i = ACE_OS::getpid ();
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopping %u...\n"),
              pid_i));
#endif // _DEBUG
  result = ACE_OS::kill (pid_i, SIGSTOP);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::kill(%u,%S): \"%m\", returning\n"),
                pid_i, SIGSTOP));
    return;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("resuming %u...\n"),
              pid_i));
#endif // _DEBUG

continue_2:
  result = ACE_OS::execv (executableFilePath_in.c_str (),
                          argv_a);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::execv(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (executableFilePath_in.c_str ())));
    return;
  } // end IF
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("::main"));

  std::string executable_file_path_string;
  std::string arguments_string;
  bool halt_before_exec_b = true;
  std::string working_directory_string =
      Common_File_Tools::getWorkingDirectory();
  bool trace_information_b = false;

  if (!do_processArguments (argc_in,
                            argv_in,
                            executable_file_path_string,
                            arguments_string,
                            halt_before_exec_b,
                            working_directory_string,
                            trace_information_b))
  {
    do_printUsage (ACE::basename (argv_in[0]));
    return EXIT_FAILURE;
  } // end IF

  // step1d: initialize logging and/or tracing
  std::string log_file_name;
  if (!Common_Log_Tools::initialize (ACE::basename (argv_in[0]), // program name
                                     log_file_name,              // log file name
                                     false,                      // log to syslog ?
                                     false,                      // trace messages ?
                                     trace_information_b,        // debug messages ?
                                     NULL))                      // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    return EXIT_FAILURE;
  } // end IF

  if (
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      !Common_File_Tools::canRead (executable_file_path_string, ACE_TEXT_ALWAYS_CHAR ("")) ||
#else
      !Common_File_Tools::canRead (executable_file_path_string, static_cast<uid_t> (-1)) ||
#endif // ACE_WIN32 || ACE_WIN64
      !Common_File_Tools::isDirectory (working_directory_string))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments (was: \"%s\"), aborting\n"),
                ACE_TEXT (Common_Process_Tools::toString (argc_in, argv_in).c_str ())));
    do_printUsage (ACE::basename (argv_in[0]));
    return EXIT_FAILURE;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (executable_file_path_string,
           arguments_string,
           halt_before_exec_b,
           working_directory_string);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  Common_Log_Tools::finalize ();

  return EXIT_SUCCESS;
}
