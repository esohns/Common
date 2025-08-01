#include "stdafx.h"

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/hwcontext.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#endif /* __cplusplus */

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

#include "common_image_tools.h"

#include "common_timer_tools.h"

#include "common_log_tools.h"


#define TEST_I_SOURCE_FILE_NAME "oak-tree.png"
#define TEST_I_TARGET_FILE_NAME "outfile.data"

enum Test_I_ModeType
{
  TEST_I_MODE_IMAGE = 0,
  TEST_I_MODE_HWACCEL,
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
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_SOURCE_FILE_NAME);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [PATH]: source PNG file [")
            << source_file_path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m       : program mode [")
            << 0
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      std::string& sourceFilePath_out,
                      enum Test_I_ModeType& mode_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                                                      false);

  // initialize results
  sourceFilePath_out = path_root;
  sourceFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  sourceFilePath_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_SOURCE_FILE_NAME);
  mode_out = TEST_I_MODE_IMAGE;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("f:m:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'f':
      {
        sourceFilePath_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
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
         const std::string& sourceFilePath_in,
         enum Test_I_ModeType mode_in)
{
  switch (mode_in)
  {
    case TEST_I_MODE_IMAGE:
    {
      Common_Image_Resolution_t resolution_s;
      enum AVPixelFormat pixel_format_e = AV_PIX_FMT_NONE;
      uint8_t *data_a[AV_NUM_DATA_POINTERS], *data_2[AV_NUM_DATA_POINTERS];
      ACE_OS::memset (&data_a, 0, sizeof (uint8_t* [AV_NUM_DATA_POINTERS]));
      ACE_OS::memset (&data_2, 0, sizeof (uint8_t* [AV_NUM_DATA_POINTERS]));
      std::string out_filename = ACE_TEXT_ALWAYS_CHAR (TEST_I_TARGET_FILE_NAME);
      std::ofstream file_stream;
      //std::string file_extension_string =
      //    Common_File_Tools::fileExtension (sourceFilePath_in, false);

      if (!Common_Image_Tools::load (sourceFilePath_in,
                                     //Common_Image_Tools::stringToCodecId (Common_String_Tools::toupper (file_extension_string)),
                                     resolution_s,
                                     pixel_format_e,
                                     data_a))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_Image_Tools::load(\"%s\"), returning\n"),
                    ACE_TEXT (sourceFilePath_in.c_str ())));
        return;
      } // end IF
      ACE_ASSERT (data_a[0]);
      if (pixel_format_e != AV_PIX_FMT_RGBA)
      {
        Common_Image_Tools::convert (resolution_s,
                                     pixel_format_e,
                                     data_a,
                                     resolution_s,
                                     AV_PIX_FMT_RGBA,
                                     data_2);
        ACE_ASSERT (data_2[0]);
        delete [] data_a[0]; data_a[0] = NULL;
        delete [] data_a[1]; data_a[1] = NULL;
        ACE_OS::memcpy (data_a, data_2, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));
        ACE_ASSERT (data_a[0]);
      } // end IF
      file_stream.open (out_filename,
                        std::ios::out | std::ios::binary);
      if (unlikely (file_stream.fail ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to open file (was: \"%s\"): \"%m\", returning\n"),
                    ACE_TEXT (out_filename.c_str ())));
        delete [] data_a[0]; data_a[0] = NULL;
        return;
      } // end IF
      int size_i =
        av_image_get_buffer_size (AV_PIX_FMT_RGBA,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                  resolution_s.cx, resolution_s.cy,
#else
                                  resolution_s.width, resolution_s.height,
#endif // ACE_WIN32 || ACE_WIN64
                                  1); // *TODO*: linesize alignment
      ACE_ASSERT (file_stream.is_open ());
      file_stream.write (reinterpret_cast<char*> (data_a[0]),
                         size_i);
      if (unlikely (file_stream.fail ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to write file (%d byte(s)): \"%m\", returning\n"),
                    size_i));
        delete [] data_a[0]; data_a[0] = NULL;
        return;
      } // end IF
      file_stream.close ();
      if (unlikely (file_stream.fail ()))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to close file (\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT (out_filename.c_str ())));
      delete [] data_a[0]; data_a[0] = NULL;

      break;
    }
    case TEST_I_MODE_HWACCEL:
    {
      enum AVHWDeviceType device_type_e =
        av_hwdevice_iterate_types (AV_HWDEVICE_TYPE_NONE);
      while (device_type_e != AV_HWDEVICE_TYPE_NONE)
      {
        ACE_DEBUG ((LM_INFO,
                    ACE_TEXT ("supported hw device type: \"%s\"\n"),
                    ACE_TEXT (av_hwdevice_get_type_name (device_type_e))));
        device_type_e = av_hwdevice_iterate_types (device_type_e);
      } // end WHILE

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
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_SOURCE_FILE_NAME);
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_IMAGE;
  bool trace_information = false;
  std::string log_file_name;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             source_file_path,
                             mode_type_e,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (source_file_path))
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
           argv_in,
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
