#include "stdafx.h"

#include "unistd.h"

#include <iostream>
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
#include "common_tools.h"

#include "common_error_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_ui_tools.h"

#include "common_test_i_defines.h"

/////////////////////////////////////////

#define WIDTH 160
#define HEIGHT 44
float A, B, C;
float cubeWidth = 20;
int width = WIDTH, height = HEIGHT;
float zBuffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];
int backgroundASCIICode = '.';
int distanceFromCam = 100;
float horizontalOffset;
float K1 = 40;
float incrementSpeed = 0.6f;

float
calculateX (float i, float j, float k)
{
  return j * sin (A) * sin (B) * cos (C) - k * cos (A) * sin (B) * cos (C) +
         j * cos (A) * sin (C) + k * sin (A) * sin (C) + i * cos (B) * cos (C);
}
float
calculateY (float i, float j, float k)
{
  return j * cos (A) * cos (C) + k * sin (A) * cos (C) -
         j * sin (A) * sin (B) * sin (C) + k * cos (A) * sin (B) * sin (C) -
         i * cos (B) * sin (C);
}
float
calculateZ (float i, float j, float k)
{
  return k * cos (A) * cos (B) - j * sin (A) * cos (B) + i * sin (B);
}

void
calculateForSurface (float cubeX, float cubeY, float cubeZ, int ch)
{
  float x = calculateX (cubeX, cubeY, cubeZ);
  float y = calculateY (cubeX, cubeY, cubeZ);
  float z = calculateZ (cubeX, cubeY, cubeZ) + distanceFromCam;

  float ooz = 1 / z;

  int xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2);
  int yp = (int)(height / 2 + K1 * ooz * y);

  int idx = xp + yp * width;
  if (idx >= 0 && idx < width * height)
  {
    if (ooz > zBuffer[idx])
    {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}

/////////////////////////////////////////

enum Test_I_ModeType
{
  TEST_I_MODE_FONT_SIZE = 0,
  TEST_I_MODE_WINDOW_SIZE,
  TEST_I_MODE_CUBE,
  ////////////////////////////////////////
  TEST_I_MODE_MAX,
  TEST_I_MODE_INVALID
};

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m           : program mode [")
            << 0
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t           : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      enum Test_I_ModeType& mode_out,
                      bool& traceInformation_out)
{
  // initialize results
  mode_out = TEST_I_MODE_FONT_SIZE;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("m:t"),
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
         enum Test_I_ModeType mode_in)
{
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE console_h = GetStdHandle (STD_OUTPUT_HANDLE);
  ACE_ASSERT (console_h != ACE_INVALID_HANDLE);
#endif // ACE_WIN32 || ACE_WIN64

  switch (mode_in)
  {
    case TEST_I_MODE_FONT_SIZE:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct _COORD coord_s;
      ACE_OS::memset (&coord_s, 0, sizeof (struct _COORD));
      coord_s.X = 256;
      coord_s.Y = 256;
      if (!Common_UI_Tools::setConsoleFontSize (coord_s))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_UI_Tools::setConsoleFontSize(%d,%d), returning\n"),
                    coord_s.X, coord_s.Y));
        return;
      } // end IF
#endif // ACE_WIN32 || ACE_WIN64

      std::cout << ACE_TEXT_ALWAYS_CHAR ("Hello World !") << std::endl;

      break;
    }
    case TEST_I_MODE_WINDOW_SIZE:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct _SMALL_RECT small_rect_s = Common_UI_Tools::setConsoleMaxWindowSize ();
#endif // ACE_WIN32 || ACE_WIN64

      std::cout << ACE_TEXT_ALWAYS_CHAR ("Hello World !") << std::endl;

      break;
    }
    case TEST_I_MODE_CUBE:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_Time_Value sleep_interval (0, 16000);
#endif // ACE_WIN32 || ACE_WIN64

      printf ("\x1b[2J");
      while (1)
      {
        ACE_OS::memset (buffer, backgroundASCIICode, width * height * sizeof (char));
        ACE_OS::memset (zBuffer, 0, width * height * sizeof (float));
        //cubeWidth = 20;
        horizontalOffset = -2 * cubeWidth;
        // first cube
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed)
          for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed)
          {
            calculateForSurface (cubeX, cubeY, -cubeWidth, '@');
            calculateForSurface (cubeWidth, cubeY, cubeX, '$');
            calculateForSurface (-cubeWidth, cubeY, -cubeX, '~');
            calculateForSurface (-cubeX, cubeY, cubeWidth, '#');
            calculateForSurface (cubeX, -cubeWidth, -cubeY, ';');
            calculateForSurface (cubeX, cubeWidth, cubeY, '+');
          }
//        cubeWidth = 10;
//        horizontalOffset = 1 * cubeWidth;
//        // second cube
//        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed)
//          for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed)
//          {
//            calculateForSurface (cubeX, cubeY, -cubeWidth, '@');
//            calculateForSurface (cubeWidth, cubeY, cubeX, '$');
//            calculateForSurface (-cubeWidth, cubeY, -cubeX, '~');
//            calculateForSurface (-cubeX, cubeY, cubeWidth, '#');
//            calculateForSurface (cubeX, -cubeWidth, -cubeY, ';');
//            calculateForSurface (cubeX, cubeWidth, cubeY, '+');
//          }
//        cubeWidth = 5;
//        horizontalOffset = 8 * cubeWidth;
//        // third cube
//        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed)
//          for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed)
//          {
//            calculateForSurface (cubeX, cubeY, -cubeWidth, '@');
//            calculateForSurface (cubeWidth, cubeY, cubeX, '$');
//            calculateForSurface (-cubeWidth, cubeY, -cubeX, '~');
//            calculateForSurface (-cubeX, cubeY, cubeWidth, '#');
//            calculateForSurface (cubeX, -cubeWidth, -cubeY, ';');
//            calculateForSurface (cubeX, cubeWidth, cubeY, '+');
//          }
        printf("\x1b[H");
        for (int k = 0; k < width * height; k++)
        {
          putchar (k % width ? buffer[k] : 10);
        }

        A += 0.05f;
        B += 0.05f;
        C += 0.01f;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_OS::sleep (sleep_interval);
#else
        usleep (8000 * 2);
#endif // ACE_WIN32 || ACE_WIN64
      }

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // COM ?
                            false); // RNG ?
#else
  Common_Tools::initialize (false); // RNG ?
#endif // ACE_WIN32 || ACE_WIN64
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_FONT_SIZE;
  bool trace_information = false;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             mode_type_e,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  if (mode_type_e >= TEST_I_MODE_MAX)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                     ACE_TEXT_ALWAYS_CHAR (""),                                           // log file name
                                     false,                                                               // log to syslog ?
                                     false,                                                               // trace messages ?
                                     trace_information,                                                   // debug messages ?
                                     NULL))                                                               // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
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

  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
