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

#include "common_log_tools.h"

#include <fstream>

#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"
#include "common_file_tools.h"

bool
Common_Log_Tools::initializeLogging (const std::string& programName_in,
                                     const std::string& logFile_in,
                                     bool logToSyslog_in,
                                     bool enableTracing_in,
                                     bool enableDebug_in,
                                     ACE_Log_Msg_Backend* backend_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Log_Tools::initializeLogging"));

  int result = -1;

  // *NOTE*: default log target is stderr
  u_long options_flags = ACE_Log_Msg::STDERR;
  if (logToSyslog_in)
    options_flags |= ACE_Log_Msg::SYSLOG;
  if (backend_in)
  {
    options_flags |= ACE_Log_Msg::CUSTOM;
    ACE_LOG_MSG->msg_backend (backend_in);
  } // end IF
  if (!logFile_in.empty ())
  {
    options_flags |= ACE_Log_Msg::OSTREAM;

    ACE_OSTREAM_TYPE* log_stream_p = NULL;
    std::ios_base::openmode open_mode = (std::ios_base::out   |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN (log_stream_p,
                      std::ofstream (logFile_in.c_str (),
                                     open_mode));
//    log_stream_p = ACE_OS::fopen (logFile_in.c_str (),
//                                  ACE_TEXT_ALWAYS_CHAR ("w"));
    if (unlikely (!log_stream_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
//                  ACE_TEXT (logFile_in.c_str ())));
      return false;
    } // end IF
    if (unlikely (log_stream_p->fail ()))
//    if (log_stream_p->open (logFile_in.c_str (),
//                            open_mode))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create log file (was: \"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (logFile_in.c_str ())));
      delete log_stream_p; log_stream_p = NULL;
      return false;
    } // end IF

    // *NOTE*: the logger singleton assumes ownership of the stream object
    // *BUG*: doesn't work on Linux
    ACE_LOG_MSG->msg_ostream (log_stream_p, true);
  } // end IF
  result = ACE_LOG_MSG->open (ACE_TEXT (programName_in.c_str ()),
                              options_flags,
                              NULL); // logger key
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Msg::open(\"%s\", %u): \"%m\", aborting\n"),
                ACE_TEXT (programName_in.c_str ()),
                options_flags));
    return false;
  } // end IF

  // set new mask...
  u_long process_priority_mask = (LM_SHUTDOWN |
                                  LM_TRACE    |
                                  LM_DEBUG    |
                                  LM_INFO     |
                                  LM_NOTICE   |
                                  LM_WARNING  |
                                  LM_STARTUP  |
                                  LM_ERROR    |
                                  LM_CRITICAL |
                                  LM_ALERT    |
                                  LM_EMERGENCY);
  if (!enableTracing_in)
    process_priority_mask &= ~LM_TRACE;
  if (!enableDebug_in)
    process_priority_mask &= ~LM_DEBUG;
  ACE_LOG_MSG->priority_mask (process_priority_mask,
                              ACE_Log_Msg::PROCESS);

  return true;
}

void
Common_Log_Tools::finalizeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Log_Tools::finalizeLogging"));

  // *NOTE*: this may be necessary in case the backend sits on the stack.
  //         In that case, ACE::fini() closes the backend too late
  ACE_LOG_MSG->msg_backend (NULL);
}

std::string
Common_Log_Tools::getLogFilename (const std::string& packageName_in,
                                  const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Log_Tools::getLogFilename"));

  // sanity check(s)
  ACE_ASSERT (!programName_in.empty ());

  unsigned int fallback_level = 0;
  std::string result;
fallback:
  result = Common_Log_Tools::getLogDirectory (packageName_in,
                                              fallback_level);

  if (unlikely (result.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to Common_Log_Tools::getLogDirectory(\"%s\",%d), falling back\n"),
                ACE_TEXT (packageName_in.c_str ()),
                fallback_level));

    result = Common_File_Tools::getWorkingDirectory ();
    if (unlikely (result.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::getWorkingDirectory(), aborting\n")));
      return result;
    } // end IF
    result += ACE_DIRECTORY_SEPARATOR_STR;
    std::string filename = Common_File_Tools::getTempFilename (programName_in);
    if (unlikely (filename.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::getTempFilename(\"%s\"), aborting\n"),
                  ACE_TEXT (programName_in.c_str ())));
      return result;
    } // end IF
    result +=
        ACE_TEXT_ALWAYS_CHAR (ACE::basename (filename.c_str (),
                                             ACE_DIRECTORY_SEPARATOR_CHAR));
    result += COMMON_LOG_FILENAME_SUFFIX;

    return result;
  } // end IF
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += programName_in;
  result += COMMON_LOG_FILENAME_SUFFIX;

  // sanity check(s)
  // *TODO*: replace this with a permission check
  if (unlikely (!Common_File_Tools::create (result)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to Common_File_Tools::create(\"%s\"), falling back\n"),
                ACE_TEXT (result.c_str ())));

    ++fallback_level;

    goto fallback;
  } // end IF
  if (unlikely (!Common_File_Tools::deleteFile (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                ACE_TEXT (result.c_str ())));

    ++fallback_level;

    goto fallback;
  } // end IF

  return result;
}

std::string
Common_Log_Tools::getLogDirectory (const std::string& packageName_in,
                                   unsigned int fallbackLevel_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Log_Tools::getLogDirectory"));

  // initialize return value(s)
  std::string result;

  unsigned int fallback_level = fallbackLevel_in;
  std::string environment_variable;
  const ACE_TCHAR* string_p = NULL;

  if (fallback_level)
  {
    --fallback_level;
    goto fallback;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = Common_File_Tools::getTempDirectory ();
#else
  result = ACE_TEXT_ALWAYS_CHAR (COMMON_LOG_DEFAULT_DIRECTORY);
#endif // ACE_WIN32 || ACE_WIN64
  goto use_path;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
use_environment:
#endif // ACE_WIN32 || ACE_WIN64
  string_p =
      ACE_OS::getenv (ACE_TEXT (environment_variable.c_str ()));
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", falling back\n"),
                ACE_TEXT (environment_variable.c_str ())));
    goto fallback;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

use_path:
  if (unlikely (!packageName_in.empty ()))
  {
    result += ACE_DIRECTORY_SEPARATOR_STR_A;
    result += packageName_in;
  } // end IF

  // sanity check(s): directory exists ?
  // --> (try to) create it
  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));
      goto fallback;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("created log directory: \"%s\"\n"),
                ACE_TEXT (result.c_str ())));
  } // end IF

  return result;

fallback:
  ++fallback_level;
  switch (fallback_level)
  {
    case 1:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      environment_variable =
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE);
      goto use_environment;
#else
      result =
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY);
      goto use_path;
#endif // ACE_WIN32 || ACE_WIN64
    }
    case 2:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      environment_variable =
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE_2);
      goto use_environment;
#else
      result =
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY_2);
      goto use_path;
#endif // ACE_WIN32 || ACE_WIN64
    }
    case 3:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      result = Common_File_Tools::getWorkingDirectory ();
      goto use_path;
#endif // ACE_WIN32 || ACE_WIN64
    }
    default:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (false);
      // *TODO*: implement fallback levels dependent on host Windows (TM) version
      //         see e.g.: http://en.wikipedia.org/wiki/Environment_variable#Windows
#else
      ACE_ASSERT (false);
      // *TODO*: implement fallback levels dependent on host platform/version
      //         see e.g. http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
#endif // ACE_WIN32 || ACE_WIN64
      break;
    }
  } // end SWITCH

  return result;
}
