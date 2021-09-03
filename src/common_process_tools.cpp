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

#include "common_process_tools.h"

#include <regex>
#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_defines.h"
#if defined (ACE_LINUX)
#include "common_tools.h"
#endif // ACE_LINUX
#include "common_file_tools.h"

std::string
Common_Process_Tools::toString (int argc_in,
                                ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Process_Tools::toString"));

  // initialize return value(s)
  std::string return_value;

  for (int i = 0;
       i < argc_in;
       ++i)
  {
    ACE_ASSERT (argv_in[i]);
    return_value += ACE_TEXT_ALWAYS_CHAR (argv_in[i]);
    return_value += ACE_TEXT_ALWAYS_CHAR (" ");
  } // end FOR

  return return_value;
}

bool
Common_Process_Tools::command (const std::string& commandLine_in,
                               int& exitStatus_out,
                               std::string& stdOut_out,
                               bool returnStdOut_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Process_Tools::command"));

  // initialize return value(s)
  exitStatus_out = -1;
  stdOut_out.clear ();

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (!commandLine_in.empty ());

  std::string filename_string;
  std::string command_line_string = commandLine_in;
  if (likely (returnStdOut_in))
  {
    filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME));
    command_line_string += ACE_TEXT_ALWAYS_CHAR (" >> ");
    command_line_string += filename_string;
  } // end IF
  result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
//  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
  if (unlikely ((result == -1)      ||
                !WIFEXITED (result) ||
                WEXITSTATUS (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (commandLine_in.c_str ()),
                WEXITSTATUS (result)));
    exitStatus_out = WEXITSTATUS (result);
    return false;
  } // end IF
  exitStatus_out = WEXITSTATUS (result);

  // sanity check(s)
  if (likely (returnStdOut_in))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_ASSERT (Common_File_Tools::canRead (filename_string, ACE_TEXT_ALWAYS_CHAR ("")));
#else
    ACE_ASSERT (Common_File_Tools::canRead (filename_string, static_cast<uid_t> (-1)));
#endif // ACE_WIN32 || ACE_WIN64

    unsigned char* data_p = NULL;
    unsigned int file_size_i = 0;
    if (unlikely (!Common_File_Tools::load (filename_string,
                                            data_p,
                                            file_size_i)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                  ACE_TEXT (filename_string.c_str ())));
      return false;
    } // end IF
    if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_string.c_str ())));
    stdOut_out.assign (reinterpret_cast<char* >(data_p), file_size_i);

    // clean up
    delete [] data_p;
  } // end IF

  return true;
}

pid_t
Common_Process_Tools::id (const std::string& executableName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Process_Tools::id"));

  pid_t result = 0;

  // sanity check(s)
  ACE_ASSERT (!executableName_in.empty ());

#if defined (ACE_LINUX)
  // sanity check(s)
  unsigned int major_i = 0, minor_i = 0, micro_i = 0;
  enum Common_OperatingSystemDistributionType linux_distribution_e =
      Common_Tools::getDistribution (major_i, minor_i, micro_i);
  ACE_UNUSED_ARG (major_i); ACE_UNUSED_ARG (minor_i); ACE_UNUSED_ARG (micro_i);
  if (unlikely (linux_distribution_e == COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::getDistribution(), aborting\n")));
    return result;
  } // end IF

  std::string command_line_string;
  switch (linux_distribution_e)
  {
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE:
    {
      // sanity check(s)
      if (unlikely (!Common_Tools::isInstalled (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_PIDOFPROC),
                                                command_line_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("command (was: \"%s\") is not installed: cannot proceed, aborting\n"),
                    ACE_TEXT (COMMON_COMMAND_PIDOFPROC)));
        return result;
      } // end IF
//      ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));
      break;
    }
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_REDHAT:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU:
    {
      // sanity check(s)
      if (unlikely (!Common_Tools::isInstalled (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_PIDOF),
                                                command_line_string)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("command (was: \"%s\") is not installed: cannot proceed, aborting\n"),
                    ACE_TEXT (COMMON_COMMAND_PIDOF)));
        return result;
      } // end IF
//      ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unsupported linux distribution (was: %d), aborting\n"),
                  linux_distribution_e));
      return result;
    }
  } // end SWITCH
  ACE_ASSERT (!command_line_string.empty ());
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
  command_line_string +=
      ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (executableName_in.c_str ()),
                                           ACE_DIRECTORY_SEPARATOR_CHAR));

  ACE_TCHAR buffer_a[BUFSIZ];
  char* pid_p = NULL;
  int i = 0;
  pid_t process_ids_a[64];
  ACE_OS::memset (&process_ids_a, 0, sizeof (pid_t[64]));
  FILE* stream_p = ::popen (command_line_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR ("r"));
  if (unlikely (!stream_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::popen(\"%s\",r): \"%m\", aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  if (unlikely (!ACE_OS::fgets (buffer_a,
                                sizeof (ACE_TCHAR[BUFSIZ]),
                                stream_p)))
  {
    if (!::feof (stream_p)) // no output ? --> process not found
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fgets(%d,%@): \"%m\", aborting\n"),
                  sizeof (char[BUFSIZ]), stream_p));
    goto clean;
  } // end IF

  pid_p = ACE_OS::strtok (buffer_a, ACE_TEXT_ALWAYS_CHAR (" "));
  while (pid_p)
  {
    process_ids_a[i] = static_cast<pid_t> (ACE_OS::atoi (pid_p));
    ++i;
    pid_p = ACE_OS::strtok (NULL , ACE_TEXT_ALWAYS_CHAR (" "));
  } // end WHILE
  if (unlikely (i > 1))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("found more than one process for executable (was: \"%s\"), returning the lowest PID\n"),
                ACE_TEXT (executableName_in.c_str ())));
    std::sort (process_ids_a, process_ids_a + 64);
  } // end IF
  if (likely (i))
    result = process_ids_a[0];

clean:
  if (likely (stream_p))
  { // (on success) pclose() returns the commands' exit status
    int result_2 = ::pclose (stream_p);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::pclose(%@): \"%m\", continuing\n"),
                  stream_p));
  } // end IF
#else
  std::string command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_TASKLIST);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" /NH /FI \"imagename eq ");
  command_line_string += executableName_in;
  command_line_string += ACE_TEXT_ALWAYS_CHAR ("\"");
  int exit_status = 0;
  std::string stdout_string;
  if (!Common_Process_Tools::command (command_line_string,
                                      exit_status,
                                      stdout_string,
                                      true))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n")));
    return result;
  } // end IF

  // parse result
  char buffer_a[BUFSIZ];
  std::istringstream converter, converter_2;
  converter.str (stdout_string);
  std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^([^ ]+)(?:[[:space:]]+)([[:digit:]]+)(?:[[:space:]]+)([[:alpha:]]+)(?:[[:space:]]+)([[:digit:]]+)(?:[[:space:]]+)(.+)(?:\r)$"));
  std::smatch match_results;
  std::string line_string;
  do {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    line_string = buffer_a;
    if (!std::regex_match (line_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
    {
      //ACE_DEBUG ((LM_ERROR,
      //            ACE_TEXT ("failed to match string (was: \"%s\"), continue\n"),
      //            ACE_TEXT (line_string.c_str ())));
      continue;
    } // end IF
    ACE_ASSERT (match_results[2].matched);
    converter_2.str (match_results[2].str ());
    converter_2 >> result;
  } while (!converter.fail ());
#endif // ACE_LINUX

  return result;
}

bool
Common_Process_Tools::kill (pid_t processId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Process_Tools::kill"));

  bool result = true;

  // sanity check(s)
  ACE_ASSERT (processId_in != 0);

#if defined (ACE_LINUX)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#else
  std::string command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_TASKKILL);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" /PID ");
  std::ostringstream converter;
  converter << processId_in;
  command_line_string += converter.str ();
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" /F");
  int exit_status = 0;
  std::string stdout_string;
  if (!Common_Process_Tools::command (command_line_string,
                                      exit_status,
                                      stdout_string,
                                      false))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Process_Tools::command(\"%s\"), aborting\n")));
    return false;
  } // end IF
#endif // ACE_LINUX

  return result;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
uint64_t
Common_Process_Tools::getStartTime ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Process_Tools::getStartTime"));

  // initialize return value(s)
  uint64_t result = 0;

  // sanity check(s)
#if defined (ACE_LINUX)
  std::string proc_self_stat_path = ACE_TEXT_ALWAYS_CHAR ("/proc/self/stat");
  ACE_ASSERT (Common_File_Tools::isReadable (proc_self_stat_path));

  FILE* file_p = NULL;
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  size_t length_i = 0;
  char* string_p = NULL;
  int index_i = 19;
  int result_2 = -1;

  file_p = ACE_OS::fopen (proc_self_stat_path.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("rb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (proc_self_stat_path.c_str ())));
    return result;
  } // end IF

  // *TODO* use readline() instead ?
  if (unlikely (!ACE_OS::fgets (buffer_a,
                                sizeof (char[BUFSIZ]),
                                file_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fgets(%d,%@): \"%m\", aborting\n"),
                sizeof (char[BUFSIZ]),
                file_p));
    goto clean;
  } // end IF
  // *NOTE*: only the second invocation of fgets would set eof
  // ACE_ASSERT (::feof (file_p));
  result_2 = ACE_OS::fclose (file_p);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (proc_self_stat_path.c_str ())));
    goto clean;
  } // end IF
  file_p = NULL;
  length_i = ACE_OS::strlen (buffer_a);

  /* *NOTE*: 'starttime' is the field at index 22 (see: man proc(5)) (i.e. 19
   *         after the 'comm' entry - this is currently the only field
   *         containing the ')' character, so the search should be unambiguous;
   *         search backwards to further avoid trouble */
  // *TODO*: this code is brittle
  string_p = ACE_OS::strrchr (buffer_a, ')');
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strrchr(%s): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (")")));
    goto clean;
  } // end IF
  string_p += 2; // skip ') '
  if (unlikely (static_cast<size_t> (string_p - buffer_a) >= length_i))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strrchr(%s): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (")")));
    goto clean;
  } // end IF
  string_p = ACE_OS::strtok (string_p, ACE_TEXT_ALWAYS_CHAR (" "));
  while ((--index_i + 1) &&
         string_p)
    string_p = ACE_OS::strtok (NULL , ACE_TEXT_ALWAYS_CHAR (" "));
  if (unlikely (!string_p))
  {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::strtok(%s,19): \"%m\", aborting\n"),
                  ACE_TEXT (buffer_a)));
      goto clean;
  } // end IF

  result = ACE_OS::atoi (string_p);

clean:
  if (file_p)
  {
    result_2 = ACE_OS::fclose (file_p);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (proc_self_stat_path.c_str ())));
  } // end IF
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);

  ACE_NOTREACHED (return result;)
#endif // ACE_LINUX

  return result;
}
#endif // ACE_WIN32 || ACE_WIN64
