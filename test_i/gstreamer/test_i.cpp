#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "gst/gst.h"

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

#define TEST_I_SOURCE_FILE_NAME "test.mp3"

//////////////////////////////////////////

gboolean
bus_call (GstBus*     bus,
          GstMessage* msg,
          gpointer    data)
{
  GMainLoop* loop = (GMainLoop*)data;

  switch (GST_MESSAGE_TYPE (msg))
  {
    case GST_MESSAGE_EOS:
    {
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("end of stream\n")));

      g_main_loop_quit (loop);
     
      break;
    }
    case GST_MESSAGE_ERROR:
    {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("error: \"%s\", aborting\n"),
                  ACE_TEXT (error->message)));
      g_error_free (error);

      g_main_loop_quit (loop);

      break;
    }
    default:
      break;
  } // end SWITCH

  return TRUE;
}

//void
//on_pad_added (GstElement* element,
//              GstPad*     pad,
//              gpointer    data)
//{
//  GstPad* sinkpad;
//  GstElement* decoder = (GstElement*)data;
//
//  /* We can now link this pad with the vorbis-decoder sink pad */
//  ACE_DEBUG ((LM_INFO,
//              ACE_TEXT ("dynamic pad created, linking demuxer/decoder\n")));
//
//  sinkpad = gst_element_get_static_pad (decoder, "sink");
//
//  gst_pad_link (pad, sinkpad);
//
//  gst_object_unref (sinkpad);
//}

//////////////////////////////////////////

enum Test_I_ModeType
{
  TEST_I_MODE_DEFAULT = 0,
  ////////////////////////////////////////
  TEST_I_MODE_MAX,
  TEST_I_MODE_INVALID
};

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path_root = Common_File_Tools::getWorkingDirectory ();
    //Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
    //                                                  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
    //                                                  false);

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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [PATH]: source MP3 file [")
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
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
    //Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
    //                                                  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
    //                                                  false);

  // initialize results
  sourceFilePath_out = path_root;
  sourceFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  sourceFilePath_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_SOURCE_FILE_NAME);
  mode_out = TEST_I_MODE_DEFAULT;
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
    case TEST_I_MODE_DEFAULT:
    {
      GMainLoop *loop;

      GstElement *pipeline, *source, *demuxer, *decoder, *resample, *convert, *sink;
      GstBus *bus;
      guint bus_watch_id;

      loop = g_main_loop_new (NULL, FALSE);

      /* Create gstreamer elements */
      pipeline = gst_pipeline_new ("audio-player");
      source   = gst_element_factory_make ("filesrc",        "file-source");
      demuxer  = gst_element_factory_make ("mpegaudioparse", "demuxer");
      decoder  = gst_element_factory_make ("mpg123audiodec", "decoder");
      resample = gst_element_factory_make ("audioresample",  "resample");
      convert  = gst_element_factory_make ("audioconvert",   "converter");
      sink     = gst_element_factory_make ("autoaudiosink",  "audio-output");

      if (!pipeline || !source /* || !demuxer*/ || !decoder || !convert || !sink)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to create filter element, returning\n")));
        return;
      } // end IF

      /* Set up the pipeline */

      /* we set the input filename to the source element */
      g_object_set (G_OBJECT (source), ACE_TEXT_ALWAYS_CHAR ("location"), sourceFilePath_in.c_str (), NULL);

      /* we add a message handler */
      bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
      bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
      gst_object_unref (bus);

      /* we add all elements into the pipeline */
      /* file-source | ogg-demuxer | vorbis-decoder | converter | alsa-output */
      gst_bin_add_many (GST_BIN (pipeline),
                        source, demuxer, decoder, resample, convert, sink, NULL);

      /* we link the elements together */
      /* file-source -> demuxer ~> decoder -> resample --> converter -> output */
      gst_element_link (source, demuxer);
      gst_element_link (demuxer, decoder);
      gst_element_link_many (decoder, resample, convert, sink, NULL);
      //g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), decoder);

      /* note that the demuxer will be linked to the decoder dynamically.
         The reason is that Ogg may contain various streams (for example
         audio and video). The source pad(s) will be created at run time,
         by the demuxer when it detects the amount and nature of streams.
         Therefore we connect a callback function which will be executed
         when the "pad-added" is emitted.*/


      /* Set the pipeline to "playing" state*/
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("now playing \"%s\"\n"),
                  ACE_TEXT (sourceFilePath_in.c_str ())));
      gst_element_set_state (pipeline, GST_STATE_PLAYING);

      /* Iterate */
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("running...\n")));
      g_main_loop_run (loop);

      /* Out of the main loop, clean up nicely */
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("running...DONE\n")));
      gst_element_set_state (pipeline, GST_STATE_NULL);

      // clean up
      gst_object_unref (GST_OBJECT (pipeline));
      g_source_remove (bus_watch_id);
      g_main_loop_unref (loop);

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
  gst_init (&argc_in, &argv_in);

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
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
    //Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
    //                                                  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
    //                                                  false);
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_SOURCE_FILE_NAME);
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_DEFAULT;
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
