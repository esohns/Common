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

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include  "sys/inotify.h"
#endif // ACE_WIN32 || ACE_WIN64

#include <iostream>
#include <string>

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "ace/Time_Value.h"

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_process_tools.h"
#include "common_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
class INotify_Event_Handler
 : public ACE_Event_Handler
{
 typedef ACE_Event_Handler inherited;

 public:
  inline INotify_Event_Handler (ACE_Reactor* reactor_in) : inherited (reactor_in, ACE_Event_Handler::LO_PRIORITY) {}
  inline virtual ~INotify_Event_Handler () {}
  virtual int handle_input (ACE_HANDLE fd_in = ACE_INVALID_HANDLE)
  { // sanity check(s)
    ACE_ASSERT (fd_in != ACE_INVALID_HANDLE);

    uint8_t buffer_a[16 * sizeof (struct inotify_event)];
    ssize_t bytes_read_i = 0;
    unsigned int offset_i = 0;

    bytes_read_i = ACE_OS::read (fd_in,
                                 static_cast<void*> (&buffer_a),
                                 sizeof (buffer_a));
    switch (bytes_read_i)
    {
      case -1:
        return -1;
      case 0:
        return 0;
      default:
      {
        struct inotify_event* event_p = NULL;
        do
        {
          event_p = reinterpret_cast<struct inotify_event*> (&(buffer_a[0]) + offset_i);
          if (event_p->mask & IN_MODIFY)
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("directory \"%s\" modified, continuing\n"),
                        ACE_TEXT (event_p->name)));
          offset_i += sizeof (struct inotify_event) + event_p->len;
        } while (offset_i < bytes_read_i);

        break;
      }
    } // end SWITCH

    return 0;
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (INotify_Event_Handler (const INotify_Event_Handler&))
  ACE_UNIMPLEMENTED_FUNC (INotify_Event_Handler& operator= (const INotify_Event_Handler&))
};
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

enum Test_U_Common_File_ModeType
{
  TEST_U_COMMON_FILE_MODE_PATH,    // <-- verify path (absolute/relative)
  TEST_U_COMMON_FILE_MODE_SIZE,    // <-- determine file size using f[open|seek|tell|close] vs fstat
  TEST_U_COMMON_FILE_MODE_WATCH,   // <-- watch directory (inotify)
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d [PATH]   : watch directory")
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
  mode_out = TEST_U_COMMON_FILE_MODE_PATH;
  filePath_out.clear ();
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("f:m:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option_i = 0;
  while ((option_i = argument_parser ()) != EOF)
  {
    switch (option_i)
    {
      case 'f':
      {
        filePath_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'm':
      {
        std::istringstream converter (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()),
                                      std::ios_base::in);
        int i = 0;
        converter >> i;
        mode_out = static_cast<enum Test_U_Common_File_ModeType> (i);
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
    case TEST_U_COMMON_FILE_MODE_PATH:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\" is a%s path\n"),
                  ACE_TEXT (filePath_in.c_str ()),
                  Common_File_Tools::isAbsolutePath (filePath_in) ? ACE_TEXT ("n absolute") : ACE_TEXT (" relative")));
      break;
    }
    case TEST_U_COMMON_FILE_MODE_SIZE:
    {
      // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (Common_File_Tools::canRead (filePath_in, ACE_TEXT_ALWAYS_CHAR ("")));
#else
      ACE_ASSERT (Common_File_Tools::canRead (filePath_in, static_cast<uid_t> (-1)));
#endif // ACE_WIN32 || ACE_WIN64

      unsigned char* data_p = NULL;
      ACE_UINT64 file_size_i = 0, file_size_2 = 0;
      if (unlikely (!Common_File_Tools::load (filePath_in,
                                              data_p,
                                              file_size_i,
                                              0)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), returning\n"),
                    ACE_TEXT (filePath_in.c_str ())));
        return;
      } // end IF
      file_size_2 = Common_File_Tools::size (filePath_in);
      if (unlikely (file_size_i != file_size_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("test failed: %u != %u, returning\n"),
                    file_size_i, file_size_2));
        ACE_ASSERT (false);
      } // end IF

      break;
    }
    case TEST_U_COMMON_FILE_MODE_WATCH:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (false); // *TODO*
#else
      int flags_i = 0;
//      IN_CLOEXEC | IN_NONBLOCK;
      int result = inotify_init1 (flags_i);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to inotify_init1(%x): \"%m\", returning\n"),
                    flags_i));
        return;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("initialized inotify...\n")));
      uint32_t mask_i = /*IN_ACCESS |
                        IN_ATTRIB |
                        IN_CLOSE_WRITE |
                        IN_CLOSE_NOWRITE |
                        IN_CREATE |
                        IN_DELETE |
                        IN_DELETE_SELF |
                        IN_MODIFY |
                        IN_MOVE_SELF |
                        IN_MOVED_FROM |
                        IN_MOVED_TO |
                        IN_OPEN;*/
                        IN_ALL_EVENTS;
      int result_2 = inotify_add_watch (result,
                                        filePath_in.c_str (),
                                        mask_i);
      if (result_2 == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to inotify_add_watch(%d,%s,%d): \"%m\", returning\n"),
                    result,
                    ACE_TEXT (filePath_in.c_str ()),
                    mask_i));
        ACE_OS::close (result);
        return;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("watching \"%s\"...\n"),
                  ACE_TEXT (filePath_in.c_str ())));

      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      INotify_Event_Handler event_handler (reactor_p);
//      event_handler.set_handle (result);
      result_2 = reactor_p->register_handler (result,
                                              &event_handler,
                                              ACE_Event_Handler::READ_MASK);
      if (result_2 == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::register_handler(%d,%d): \"%m\", returning\n"),
                    result,
                    ACE_Event_Handler::READ_MASK));
        ACE_OS::close (result);
        return;
      } // end IF

//      fd_set rfds;
//      FD_ZERO (&rfds);
//      FD_SET (result, &rfds);
//      int res=select (FD_SETSIZE,&rfds,NULL,NULL,NULL);

      result_2 = reactor_p->run_event_loop ();
      if (result_2 == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::run_event_loop(): \"%m\", returning\n")));
        ACE_OS::close (result);
        return;
      } // end IF
#endif // ACE_WIN32 || ACE_WIN64

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // COM ?
                            false); // RNG ?
#else
  Common_Tools::initialize (false); // RNG ?
#endif // ACE_WIN32 || ACE_WIN64

  enum Test_U_Common_File_ModeType mode_type_e =
    TEST_U_COMMON_FILE_MODE_PATH;
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
  if (file_path_string.empty ())
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

  if (((mode_type_e == TEST_U_COMMON_FILE_MODE_SIZE) ||
       (mode_type_e == TEST_U_COMMON_FILE_MODE_WATCH)) &&
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      !Common_File_Tools::canRead (file_path_string, ACE_TEXT_ALWAYS_CHAR ("")))
#else
      !Common_File_Tools::canRead (file_path_string, static_cast<uid_t> (-1)))
#endif // ACE_WIN32 || ACE_WIN64
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments (was: \"%s\"), aborting\n"),
                ACE_TEXT (Common_Process_Tools::toString (argc_in, argv_in).c_str ())));
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
  working_time_string = Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  Common_Log_Tools::finalize ();

  return EXIT_SUCCESS;
}
