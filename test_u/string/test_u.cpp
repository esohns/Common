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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Profile_Timer.h"

#include "common_defines.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

//////////////////////////////////////////

typedef std::vector <std::string> Test_U_Words_t;
typedef Test_U_Words_t::iterator Test_U_WordsIterator_t;
typedef std::iterator_traits<Test_U_WordsIterator_t>::difference_type Test_U_WordsDifference_t;
typedef std::pair<Test_U_Words_t, Test_U_Words_t> Test_U_TestCase_t;
typedef std::vector <Test_U_TestCase_t> Test_U_TestCases_t;
typedef Test_U_TestCases_t::iterator Test_U_TestCasesIterator_t;

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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : program mode [")
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
                     enum Test_U_ModeType& mode_out,
                     bool& logToFile_out,
                     bool& traceInformation_out)
{
  COMMON_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  mode_out = TEST_U_MODE_DEFAULT;
  logToFile_out = false;
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
        std::istringstream converter (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()),
                                      std::ios_base::in);
        int i = 0;
        converter >> i;
        mode_out = static_cast<enum Test_U_ModeType> (i);
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
do_work (enum Test_U_ModeType mode_in)
{
  COMMON_TRACE (ACE_TEXT ("::do_work"));

  switch (mode_in)
  {
    case TEST_U_MODE_DEFAULT:
    {
      // step1: specify testcases
      // #1
      Test_U_Words_t words_a, words_2;
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("one"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("two"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("three"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("four"));

      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("three"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("four"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("five"));
      Test_U_TestCases_t test_cases_a;
      test_cases_a.push_back (std::make_pair (words_a, words_2));
      words_a.clear (); words_2.clear ();

      // #2
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("four"));

      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("three"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("four"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("five"));
      test_cases_a.push_back (std::make_pair (words_a, words_2));
      words_a.clear (); words_2.clear ();

      // #3
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("one"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("two"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("three"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("five"));

      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("five"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("six"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("seven"));
      test_cases_a.push_back (std::make_pair (words_a, words_2));
      words_a.clear (); words_2.clear ();

      // #4
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("one"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("two"));
      words_a.push_back (ACE_TEXT_ALWAYS_CHAR ("three"));

      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("four"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("five"));
      words_2.push_back (ACE_TEXT_ALWAYS_CHAR ("six"));
      test_cases_a.push_back (std::make_pair (words_a, words_2));
      words_a.clear (); words_2.clear ();

      int index_i = 1;
      std::ostringstream converter;
      for (Test_U_TestCasesIterator_t iterator = test_cases_a.begin ();
           iterator != test_cases_a.end ();
           ++iterator, ++index_i)
      {
        // algorithm START
        Test_U_WordsIterator_t start_iterator = (*iterator).first.begin ();
        Test_U_WordsDifference_t index_2 = 0, index_3 = 0;
        Test_U_WordsIterator_t iterator_2;
continue_:
        for (Test_U_WordsIterator_t iterator_3 = start_iterator;
             iterator_3 != (*iterator).first.end ();
             ++iterator_3)
        {
continue_2:
          index_2 = std::distance (start_iterator, iterator_3);
          iterator_2 = std::find ((*iterator).second.begin (),
                                  (*iterator).second.end (),
                                  *iterator_3);
          if (iterator_2 == (*iterator).second.end ())
          { // no overlap at this index --> increment start_iterator and continue
            std::advance (start_iterator, 1);
            if (start_iterator == (*iterator).first.end ())
              goto continue_3; // done; append
            goto continue_;
          } // end IF
          index_3 = std::distance ((*iterator).second.begin (), iterator_2);
          if (index_2 != index_3)
          { // no overlap at this index --> increment start_iterator and continue
            std::advance (start_iterator, 1);
            if (start_iterator == (*iterator).first.end ())
              goto continue_3; // done; append
            goto continue_;
          } // end IF
          std::advance (iterator_3, 1);
          if (iterator_3 == (*iterator).first.end ())
          { // found matching trailing sequence: it starts at start_iterator
            (*iterator).first.erase (start_iterator, (*iterator).first.end ());
continue_3:
            (*iterator).first.insert ((*iterator).first.end (),
                                      (*iterator).second.begin (),
                                      (*iterator).second.end ());
            break;
          } // end IF
          goto continue_2;
        } // end FOR
        // algorithm END

        // print result(s)
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << index_i;
        std::cout << converter.str () << ACE_TEXT_ALWAYS_CHAR (": ");
        for (iterator_2 = (*iterator).first.begin ();
             iterator_2 != (*iterator).first.end ();
             ++iterator_2)
          std::cout << *iterator_2 << ACE_TEXT_ALWAYS_CHAR (" ");
        std::cout << std::endl;
      } // end FOR

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

//error:
//  ;
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
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  ACE_Profile_Timer process_profile;
  process_profile.start ();

  enum Test_U_ModeType mode_type_e = TEST_U_MODE_DEFAULT;
  bool trace_information_b = false;
  bool log_to_file = false;
  std::string log_file_name;
  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;

  if (!do_processArguments (argc_in,
                            argv_in,
                            mode_type_e,
                            log_to_file,
                            trace_information_b))
  {
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
    goto error;
  } // end IF

  // step1d: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                        ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                     log_file_name,                                     // log file name
                                     false,                                             // log to syslog ?
                                     false,                                             // trace messages ?
                                     trace_information_b,                               // debug messages ?
                                     NULL))                                             // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    goto error;
  } // end IF

  timer.start ();
  do_work (mode_type_e);
  timer.stop ();

  timer.elapsed_time (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (Common_Timer_Tools::periodToString (working_time).c_str ())));

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
