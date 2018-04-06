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

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_tools.h"

#include "common_timer_tools.h"

enum Test_U_Common_File_ModeType
{
  TEST_U_COMMON_FILE_MODE_SIZE,    // <-- determine file size using f[open|seek|tell|close] vs fstat
  ////////////////////////////////////////
  TEST_U_COMMON_FILE_MODE_MAX,
  TEST_U_COMMON_FILE_MODE_INVALID
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
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [PATH]   : determine file size")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     enum Test_U_Common_File_ModeType& mode_out,
                     std::string& filePath_out,
                     bool& traceInformation_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  mode_out = TEST_U_COMMON_FILE_MODE_INVALID;
  filePath_out.clear ();
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("s:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option_i = 0;
  while ((option_i = argument_parser ()) != EOF)
  {
    switch (option_i)
    {
      case 's':
      {
        mode_out = TEST_U_COMMON_FILE_MODE_SIZE;
        filePath_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
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
do_work (enum Test_U_Common_File_ModeType mode_in,
         const std::string& filePath_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  switch (mode_in)
  {
    case TEST_U_COMMON_FILE_MODE_SIZE:
    {
      // sanity check(s)
      ACE_ASSERT (Common_File_Tools::canRead (filePath_in));

      unsigned char* data_p = NULL;
      unsigned int file_size_i = 0, file_size_2 = 0;
      if (unlikely (!Common_File_Tools::load (filePath_in,
                                              data_p,
                                              file_size_i)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), returning\n"),
                    ACE_TEXT (filePath_in.c_str ())));
        return;
      } // end IF
      file_size_2 = Common_File_Tools::size (filePath_in);
      ACE_ASSERT (file_size_i == file_size_2);

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
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("::main"));

  enum Test_U_Common_File_ModeType mode_type_e =
    TEST_U_COMMON_FILE_MODE_INVALID;
  std::string file_path_string;
  bool trace_information_b = false;

  if (!do_processArguments (argc_in,
                            argv_in,
                            mode_type_e,
                            file_path_string,
                            trace_information_b))
  {
    do_printUsage (ACE::basename (argv_in[0]));
    return EXIT_FAILURE;
  } // end IF

  // step1d: initialize logging and/or tracing
  std::string log_file_name;
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                        log_file_name,              // log file name
                                        false,                      // log to syslog ?
                                        false,                      // trace messages ?
                                        trace_information_b,        // debug messages ?
                                        NULL))                      // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));
    return EXIT_FAILURE;
  } // end IF

  if ((mode_type_e == TEST_U_COMMON_FILE_MODE_SIZE) && !Common_File_Tools::canRead (file_path_string))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments (was: \"%s\"), aborting\n"),
                ACE_TEXT (Common_Tools::commandLineToString (argc_in, argv_in).c_str ())));

    do_printUsage (ACE::basename (argv_in[0]));

    return EXIT_FAILURE;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (mode_type_e,
           file_path_string);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  Common_Timer_Tools::periodToString (working_time,
                                      working_time_string);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  return EXIT_SUCCESS;
}
