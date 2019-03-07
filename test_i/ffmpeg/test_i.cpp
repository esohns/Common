#include "stdafx.h"

#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#endif /* __cplusplus */

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

#include "common_image_tools.h"

#include "common_timer_tools.h"

#include "common_log_tools.h"

enum AVPixelFormat
common_ffmpeg_get_format_cb (struct AVCodecContext* context_in,
                             const enum AVPixelFormat* formats_in)
{
  // sanity check(s)
  ACE_ASSERT (context_in);
  ACE_ASSERT (formats_in);
  ACE_ASSERT (context_in->opaque);

  enum AVPixelFormat* preferred_format_p =
    reinterpret_cast<enum AVPixelFormat*> (context_in->opaque);

  // try to find the preferred format first
  for (const enum AVPixelFormat* iterator = formats_in;
       *iterator != -1;
       ++iterator)
    if (*iterator == *preferred_format_p)
      return *iterator;
  ACE_DEBUG ((LM_WARNING,
              ACE_TEXT ("%s: preferred format (was: %s) not supported, falling back\n"),
              ACE_TEXT (avcodec_get_name (context_in->codec_id)),
              ACE_TEXT (Common_Image_Tools::pixelFormatToString (*preferred_format_p).c_str ())));

  // *TODO*: set context_in->hw_frames_ctx here as well

  // accept first format (if any) as a fallback
  for (const enum AVPixelFormat* iterator = formats_in;
       *iterator != -1;
       ++iterator)
    return *iterator;

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("%s: does not support any format, aborting\n"),
              ACE_TEXT (avcodec_get_name (context_in->codec_id))));

  return AV_PIX_FMT_NONE;
}

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
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR ("test.png");
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [PATH]   : source PNG file [")
            << source_file_path
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
                      std::string& sourceFilePath_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  sourceFilePath_out = path_root;
  sourceFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  sourceFilePath_out += ACE_TEXT_ALWAYS_CHAR ("test.png");
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("f:t"),
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
      case 'f':
      {
        sourceFilePath_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
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
         const std::string& sourceFilePath_in)
{
  std::string out_filename = ACE_TEXT_ALWAYS_CHAR ("outfile.rgb");
  const AVCodec* codec_p = NULL;
  AVCodecContext* context_p = NULL;
  FILE* file_p = NULL, *file_2 = NULL;
  AVFrame* frame_p = NULL;
  uint8_t buffer_a[4096 + AV_INPUT_BUFFER_PADDING_SIZE];
  /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
  ACE_OS::memset (buffer_a + 4096, 0, AV_INPUT_BUFFER_PADDING_SIZE);
  size_t   data_size = 0;
  int result = -1;
  AVPacket packet_s;
  av_init_packet (&packet_s);
  enum AVPixelFormat pixel_format_e = AV_PIX_FMT_RGB24;

  codec_p = avcodec_find_decoder (AV_CODEC_ID_PNG);
  if  (!codec_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_find_decoder(AV_CODEC_ID_PNG), aborting\n")));
    return;
  } // end IF
  context_p = avcodec_alloc_context3 (codec_p);
  if (!context_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_alloc_context3(), aborting\n")));
    goto error;
  }

  context_p->codec_type = AVMEDIA_TYPE_VIDEO;
//  context_p->flags = 0;
//  context_p->flags2 = 0;
  context_p->opaque = &pixel_format_e;
  context_p->get_format = common_ffmpeg_get_format_cb;
//  context_p->request_sample_fmt = AV_PIX_FMT_RGB24;
//  context_p->refcounted_frames = 1; // *NOTE*: caller 'owns' the frame buffer
  context_p->workaround_bugs = 0xFFFFFFFF;
  context_p->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;
  context_p->error_concealment = 0xFFFFFFFF;
#if defined (_DEBUG)
  context_p->debug = 0xFFFFFFFF;
#endif // _DEBUG
//  context_p->err_recognition = 0xFFFFFFFF;
//  hwaccel_context
//  idct_algo
//  bits_per_coded_sample
//  thread_count
//  thread_type
//  pkt_timebase
//  hw_frames_ctx
//  hw_device_ctx
//  hwaccel_flags
  if (avcodec_open2 (context_p, codec_p, NULL) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(), aborting\n")));
    goto error;
  }

  frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_frame_alloc(), aborting\n")));
    goto error;
  }

  file_p = ACE_OS::fopen (sourceFilePath_in.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("rb"));
  ACE_ASSERT (file_p);

  while (!feof (file_p))
  {
    /* read raw data from the input file */
    data_size = ACE_OS::fread (buffer_a, 1, 4096, file_p);
    if (!data_size)
      break;
    packet_s.data = buffer_a;
    packet_s.size = data_size;

    result = avcodec_send_packet (context_p, &packet_s);
    if (result < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to avcodec_send_packet(), aborting\n")));
      goto error;
    }
    while (result >= 0)
    {
      result = avcodec_receive_frame (context_p, frame_p);
        if (result == AVERROR (EAGAIN) ||
            result == AVERROR_EOF)
          continue;
        else if (result < 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to avcodec_receive_frame(), aborting\n")));
          goto error;
        } // end ELSE IF
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("decoded image\n")));
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
    } // end WHILEs
  } // end WHILE

  file_2 = ACE_OS::fopen (out_filename.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("w"));
  ACE_ASSERT (file_2);
  ACE_OS::fwrite (frame_p->data[0],
                  frame_p->linesize[0] * frame_p->height,
                  1,
                  file_2);
  ACE_OS::fclose (file_2);

error:
  ACE_OS::fclose (file_p);
  av_frame_free (&frame_p);
  avcodec_free_context (&context_p);
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

  Common_Tools::initialize ();

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  std::string source_file_path = path_root;
  source_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  source_file_path += ACE_TEXT_ALWAYS_CHAR ("test.png");
  bool trace_information = false;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             source_file_path,
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
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]),           // program name
                                            ACE_TEXT_ALWAYS_CHAR (""),            // log file name
                                            false,                                // log to syslog ?
                                            false,                                // trace messages ?
                                            trace_information,                    // debug messages ?
                                            NULL))                                // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
           source_file_path);
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
  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
