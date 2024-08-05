#include "stdafx.h"

#include <iostream>
#include <string>

#include "ace/config-lite.h"
#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Init_ACE.h"
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
#include "ace/Synch.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_parser_m3u_parser_driver.h"

enum Test_I_ModeType
{
  TEST_I_MODE_DEFAULT = 0,
  TEST_I_MODE_WRAP,
////////////////////////////////////////
  TEST_I_MODE_MAX,
  TEST_I_MODE_INVALID
};

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d          : debug scanner [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-e          : debug parser [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR ("test_2.txt");
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [PATH]   : source (m3u) file [")
            << source_file_path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : program mode [")
            << TEST_I_MODE_DEFAULT
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
                      bool& debugScanner_out,
                      bool& debugParser_out,
                      std::string& sourceFilePath_out,
                      bool& logToFile_out,
                     enum Test_I_ModeType& mode_out,
                     bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  debugScanner_out = false;
  debugParser_out = false;
  sourceFilePath_out = path_root;
  sourceFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  sourceFilePath_out += ACE_TEXT_ALWAYS_CHAR ("test_3.txt");
  logToFile_out = false;
  mode_out = TEST_I_MODE_DEFAULT;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("def:lm:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        debugScanner_out = true;
        break;
      }
      case 'e':
      {
        debugParser_out = true;
        break;
      }
      case 'f':
      {
        sourceFilePath_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
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
        mode_out = static_cast<enum Test_I_ModeType> (i);
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
         bool debugScanner_in,
         bool debugParser_in,
         const std::string& sourceFilePath_in,
         enum Test_I_ModeType mode_in)
{
  // step1: load data
  ACE_UINT64 file_size_i = Common_File_Tools::size (sourceFilePath_in);
  uint8_t* data_p = NULL, *data_2 = NULL;
  ACE_Message_Block* message_block_p = NULL, *message_block_2 = NULL, *message_block_3 = NULL;
  struct Common_FlexBisonParserConfiguration configuration;
  Common_Parser_M3U_ParserDriver parser_driver;

  if (!Common_File_Tools::load (sourceFilePath_in,
                                data_p,
                                file_size_i,
                                COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), returning\n"),
                ACE_TEXT (sourceFilePath_in.c_str ())));
    return;
  } // end IF
  ACE_ASSERT (data_p);

  // step2: initialize parser
  configuration.block = true;
#if defined (_DEBUG)
  configuration.debugParser = debugParser_in;
  configuration.debugScanner = debugScanner_in;
#endif // _DEBUG
  configuration.messageQueue = NULL;
  configuration.useYYScanBuffer = COMMON_PARSER_DEFAULT_FLEX_USE_YY_SCAN_BUFFER;

  if (!parser_driver.initialize (configuration))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to initialize parser, returning\n")));
    goto clean;
  } // end IF

  switch (mode_in)
  {
    case TEST_I_MODE_DEFAULT:
    {
      ACE_NEW_NORETURN (message_block_p,
                        ACE_Message_Block (reinterpret_cast<char*> (data_p),
                                           file_size_i + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                                           ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY));
      ACE_ASSERT (message_block_p);
      message_block_p->size (file_size_i);
      message_block_p->wr_ptr (file_size_i);

      // step3: parse data
      if (!parser_driver.parse (message_block_p))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to parse data, returning\n")));
        goto clean;
      } // end IF

      break;
    }
    case TEST_I_MODE_WRAP:
    {
#define FRAGMENT_SIZE 16384
      ACE_NEW_NORETURN (message_block_p,
                        ACE_Message_Block (FRAGMENT_SIZE + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE));
      ACE_ASSERT (message_block_p);
      message_block_p->copy (reinterpret_cast<char*> (data_p),
                             FRAGMENT_SIZE);
      data_2 = data_p;
      data_2 += FRAGMENT_SIZE;
      file_size_i -= FRAGMENT_SIZE;
      message_block_2 = message_block_p;
      while (file_size_i)
      {
        ACE_UINT64 size_i =
            std::min (static_cast<ACE_UINT64> (FRAGMENT_SIZE), file_size_i);
        ACE_NEW_NORETURN (message_block_3,
                          ACE_Message_Block (size_i + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE));
        ACE_ASSERT (message_block_3);
        message_block_3->copy (reinterpret_cast<char*> (data_2),
                               size_i);
        data_2 += size_i;
        file_size_i -= size_i;

        message_block_2->cont (message_block_3);
        message_block_2 = message_block_3;
      } // end WHILE
      data_2 = NULL;
      message_block_2 = message_block_3 = NULL;

      // step3: parse data
      if (!parser_driver.parse (message_block_p))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to parse data, returning\n")));
        goto clean;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), returning\n"),
                  mode_in));
      goto clean;
    }
  } // end SWITCH

clean:
  if (message_block_p)
  {
    message_block_p->release (); message_block_p = NULL;
  } // end IF
  if (data_p)
  {
    delete [] data_p; data_p = NULL;
  } // end IF
  if (message_block_2)
  {
    message_block_2->release (); message_block_2 = NULL;
  } // end IF
  if (data_2)
  {
    delete [] data_2; data_2 = NULL;
  } // end IF
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // COM ?
                            false); // RNG ?
#else
  Common_Tools::initialize (false); // RNG ?
#endif // ACE_WIN32 || ACE_WIN64

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  bool debug_scanner = false;
  bool debug_parser = false;
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR ("test_3.txt");
  bool log_to_file = false;
  std::string log_file_name;
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_DEFAULT;
  bool trace_information = false;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             debug_scanner,
                             debug_parser,
                             source_file_path,
                             log_to_file,
                             mode_type_e,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (source_file_path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                        ACE::basename (argv_in[0]));
  if (!Common_Log_Tools::initialize (ACE::basename (argv_in[0]), // program name
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
           debug_scanner,
           debug_parser,
           source_file_path,
           mode_type_e);
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
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
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
  Common_Tools::finalize ();

  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
