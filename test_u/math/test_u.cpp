#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <vector>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/Profile_Timer.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_math_tools.h"

template <typename T>
void
print_vector (std::vector<T>& vector_in)
{
  for (int i = 0;
       i < static_cast<int> (vector_in.size ());
       ++i)
    std::cout << vector_in[i];
  std::cout << std::endl;
}

template <typename T>
void
print_vectors (std::vector<std::vector<T>>& vectors_in)
{
  std::cout << vectors_in.size () << ACE_TEXT_ALWAYS_CHAR (" result(s)") << std::endl;
  for (int i = 0;
       i < static_cast<int> (vectors_in.size ());
       ++i)
    print_vector (vectors_in[i]);
}

enum Test_U_ModeType
{
  TEST_U_MODE_DEFAULT = 0,
  TEST_U_MODE_MAP,
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l: log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m: program mode [")
            << TEST_U_MODE_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t: trace information [")
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
      std::vector<int> test_vector_a = {1, 2, 3, 4, 5}, result;
      std::vector<std::vector<int>> results;
      results.push_back (test_vector_a);
      do
      {
        result = Common_Math_Tools::permute (test_vector_a);
        if (result.empty ())
          break;
        results.push_back (result);
      } while (true);
      print_vectors (results);

      std::vector<int> test_vector_2 = {1, 2, 3, 4, 5, 6};
      results.clear ();
      std::vector<int> positions_a (test_vector_2.size (), 0);
      Common_Math_Tools::combine (test_vector_2,
                                  positions_a,
                                  0,
                                  results);
      print_vectors (results);
      break;
    }
    case TEST_U_MODE_MAP:
    {
      float result = Common_Math_Tools::map (5.0F, 0.0F, 10.0F, 0.0F, 1.0F, -1);
      std::cout << result << std::endl;
      result = Common_Math_Tools::map (-5.0F, -100.0F, 1.0F, -10.0F, 10.0F, -1);
      std::cout << result << std::endl;
      result = Common_Math_Tools::map (-5.0F, -100.0F, -5.0F, -10.0F, 10.0F, -1);
      std::cout << result << std::endl;
      result = Common_Math_Tools::map (5.0F, -10.0F, 10.0F, 0.0F, 1.0F, 2);
      std::cout << result << std::endl;

      double result_2 = Common_Math_Tools::map (0.0, -10.0, 10.0, 0.0, 1.0, 10);
      std::cout << result_2 << std::endl;

      int result_3 = Common_Math_Tools::map (0, -10, 10, 0, 10, 0);
      std::cout << result_3 << std::endl;
      result_3 = Common_Math_Tools::map (15, 10, 100, -10, 10, 0);
      std::cout << result_3 << std::endl;
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

  // debug info
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
