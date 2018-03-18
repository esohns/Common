﻿/***************************************************************************
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

#include "ace/Synch.h"
#include "common_file_tools.h"

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <Shlobj.h>
#include <Userenv.h>
#else
#include <sys/stat.h>
#endif

#include "ace/ACE.h"
#include "ace/Dirent_Selector.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE_IO.h"
#include "ace/OS.h"
#include "ace/OS_NS_sys_sendfile.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_tools.h"

#if defined (HAVE_CONFIG_H)
#include "libCommon_config.h"
#endif

std::string
Common_File_Tools::addressToString (const ACE_FILE_Addr& address_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::addressToString"));

  std::string result;

  ACE_TCHAR buffer[PATH_MAX];
  int result_2 = address_in.addr_to_string (buffer,
                                            sizeof (buffer));
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", aborting\n")));
    return result;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer);

  return result;
}

std::string
Common_File_Tools::fileExtension (const std::string& path_in,
                                  bool returnLeadingDot_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::fileExtension"));

  std::string return_value;

  std::string::size_type position =
      path_in.find_last_of ('.', std::string::npos);
  if (position != std::string::npos)
    return_value = path_in.substr ((returnLeadingDot_in ? position : position + 1),
                                   std::string::npos);

  return return_value;
}

bool
Common_File_Tools::isExecutable (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isExecutable"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT)  // 2  : not found
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return (((stat.st_mode & S_IFMT) & S_IFREG) && // regular file ?
          (stat.st_mode & S_IRUSR) &&            // readable (by owner) ?
          (stat.st_mode & S_IXUSR));             // executable (by owner) ?
}
bool
Common_File_Tools::isReadable (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isReadable"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT)  // 2  : not found
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return ((((stat.st_mode & S_IFMT) & S_IFDIR) ||  // directory ?
           ((stat.st_mode & S_IFMT) & S_IFREG) ||  // regular file ?
           ((stat.st_mode & S_IFMT) & S_IFLNK)) && // (symbolic) link ?
          (stat.st_mode & S_IRUSR));               // readable (by owner) ?
}

bool
Common_File_Tools::isEmpty (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isEmpty"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof(stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return (stat.st_size == 0);
}

bool
Common_File_Tools::isDirectory (const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isDirectory"));

//  // *NOTE*: ACE_OS::stat does not currently work in Win32; this may (!) be
//  //         related to i18n %APPDATA%/%PROGRAMFILES%... path quirkyness
//  // *TODO*: find a portable solution and integrate it into ACE
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  DWORD attributes = GetFileAttributes (directory_in.c_str ());
//
//  return (attributes != INVALID_FILE_ATTRIBUTES &&
//         (attributes & FILE_ATTRIBUTE_DIRECTORY));
//#else
  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (directory_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT) // 2:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (directory_in.c_str ())));

    // URI doesn't even exist --> NOT a directory !
    return false;
  } // end IF

  return ((stat.st_mode & S_IFMT) & S_IFDIR);
//#endif
}

//int
//Common_File_Tools::selector(const dirent* dirEntry_in)
//{
//COMMON_TRACE (ACE_TEXT ("Common_File_Tools::selector"));

//  // *IMPORTANT NOTE*: select all files

//  // sanity check --> ignore dot/double-dot
//  if (ACE_OS::strncmp (dirEntry_in->d_name,
//                       ACE_TEXT_ALWAYS_CHAR (Net_SERVER_LOG_FILENAME_PREFIX),
//                       ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (Net_SERVER_LOG_FILENAME_PREFIX))) != 0)
//  {
////     ACE_DEBUG ((LM_DEBUG,
////                 ACE_TEXT ("ignoring \"%s\"...\n"),
////                 ACE_TEXT (dirEntry_in->d_name)));

//    return 0;
//  } // end IF

//  return 1;
//}

//int
//Common_File_Tools::comparator(const dirent** d1,
//                              const dirent** d2)
//{
//COMMON_TRACE (ACE_TEXT ("Common_File_Tools::comparator"));

//  return ACE_OS::strcmp ((*d1)->d_name,
//                         (*d2)->d_name);
//}

bool
Common_File_Tools::isEmptyDirectory (const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isEmptyDirectory"));

  // initialize return value
  bool return_value = false;

  int result = -1;
  ACE_Dirent_Selector entries;
  result = entries.open (ACE_TEXT (directory_in.c_str ()),
//                    &Common_File_Tools::dirent_selector,
//                    &Common_File_Tools::dirent_comparator) == -1)
                         NULL,
                         NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (directory_in.c_str ())));
    return false;
  } // end IF
  return_value = (entries.length () == 0);

  // clean up
  result = entries.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (directory_in.c_str ())));

  return return_value;
}

bool
Common_File_Tools::isValidFilename (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isValidFilename"));

  std::string directory, file_name;
  directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT (string_in.c_str ())));
  file_name =
    ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (string_in.c_str ())));

  // *TODO*: this isn't entirely accurate
  return (!Common_File_Tools::isDirectory (string_in) &&
          (Common_File_Tools::isDirectory (directory) &&
           !file_name.empty ()));
}

bool
Common_File_Tools::isValidPath (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isValidPath"));

  std::string directory, file_name;
  directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT (string_in.c_str ())));
  file_name =
    ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (string_in.c_str ())));

  // *TODO*: ACE::dirname() returns '.' on an empty argument; this isn't
  //         entirely accurate
  return ((directory != ACE_TEXT_ALWAYS_CHAR (".")) &&
          Common_File_Tools::isDirectory (directory));
}

bool
Common_File_Tools::create (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::create"));

  // sanity check(s)
  if (unlikely (Common_File_Tools::isReadable (path_in)))
    return true; // nothing to do
  if (unlikely (path_in.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was empty), aborting\n")));
    return false;
  } // end IF
  // *TODO*

  std::string file_name = path_in;
  int result = -1;
  if (!Common_File_Tools::isValidFilename (file_name))
  {
    ACE_TCHAR buffer[PATH_MAX];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    if (unlikely (!ACE_OS::getcwd (buffer, sizeof (buffer))))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));
      return false;
    } // end IF

    file_name = ACE_TEXT_ALWAYS_CHAR (buffer);
    file_name += ACE_DIRECTORY_SEPARATOR_STR_A;
    file_name += path_in;
  } // end IF

  FILE* file_p = NULL;
  file_p = ACE_OS::fopen (ACE_TEXT (file_name.c_str ()),
                          ACE_TEXT ("wb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (file_name.c_str ())));
    return false;
  } // end IF
  result = ACE_OS::fclose (file_p);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", aborting\n")));
    return false;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("created: \"%s\"\n"),
  //            ACE_TEXT (file_name.c_str ())));

  return true;
}
bool
Common_File_Tools::createDirectory (const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::createDirectory"));

  int result = -1;

  result = ACE_OS::mkdir (ACE_TEXT (directory_in.c_str ()),
                          ACE_DEFAULT_DIR_PERMS);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    switch (error)
    {
      case ENOENT:
      {
        // OK: some base sub-directory doesn't seem to exist...
        // --> try to recurse
        const ACE_TCHAR* directory_p =
          ACE::dirname (ACE_TEXT (directory_in.c_str ()),
                        ACE_DIRECTORY_SEPARATOR_CHAR);
        if (!directory_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE::dirname(\"%s\"): \"%m\", aborting\n"),
                      ACE_TEXT (directory_in.c_str ())));
          return false;
        } // end IF
        std::string base_directory = ACE_TEXT_ALWAYS_CHAR (directory_p);
        // sanity check: don't recurse for "." !
        if (base_directory != ACE_TEXT_ALWAYS_CHAR ("."))
        {
          if (createDirectory (base_directory))
            return createDirectory (directory_in);
          else
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"): \"%m\", aborting\n"),
                        ACE_TEXT (base_directory.c_str ())));
        } // end IF

        return false;
      }
      case EEXIST:
      {
        // entry already exists...
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("\"%s\" already exists, leaving\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return true;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::mkdir(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return false;
      }
    } // end SWITCH
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("created: \"%s\"\n"),
  //            ACE_TEXT (directory_in.c_str ())));

  return true;
}

bool
Common_File_Tools::copyFile (const std::string& path_in,
                             const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::createDirectory"));

  int result = -1;

  // connect to the file...
  ACE_FILE_Addr source_address, target_address;
  result = source_address.set (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  const ACE_TCHAR* filename_p =
    ACE::basename (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()),
                   ACE_DIRECTORY_SEPARATOR_CHAR);
  if (unlikely (!filename_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::basename(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  std::string target_filename = directory_in;
  target_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  target_filename += ACE_TEXT_ALWAYS_CHAR (filename_p);
  result =
    target_address.set (ACE_TEXT_CHAR_TO_TCHAR (target_filename.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (target_filename.c_str ())));
    return false;
  } // end IF

  // don't block on file opening (see ACE doc)...
  ACE_FILE_Connector connector, receptor;
  ACE_FILE_IO source_file, target_file;
  result =
    connector.connect (source_file,                                         // file
                       source_address,                                      // remote SAP
                       const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero), // timeout
                       ACE_Addr::sap_any,                                   // local SAP
                       0,                                                   // reuse address ?
                       (O_RDONLY | O_BINARY | O_EXCL),                      // flags
                       ACE_DEFAULT_FILE_PERMS);                             // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  result =
    receptor.connect (target_file,                                         // file
                      target_address,                                      // remote SAP
                      const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero), // timeout
                      ACE_Addr::sap_any,                                   // local SAP
                      0,                                                   // reuse address ?
                      (O_WRONLY | O_CREAT | O_BINARY | O_TRUNC),           // flags
                      ACE_DEFAULT_FILE_PERMS);                             // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (target_filename.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));

    return false;
  } // end IF

  // copy file
//  iovec io_vec;
//  ACE_OS::memset (&io_vec, 0, sizeof (io_vec));
//  if (source_file.recvv (&io_vec) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_FILE_IO::recvv() file \"%s\": \"%m\", aborting\n"),
//                ACE_TEXT (path_in.c_str())));

//    // clean up
//    if (source_file.close () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (path_in.c_str ())));
//    if (target_file.remove () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (target_filename.c_str ())));

//    return false;
//  } // end IF
//  ACE_ASSERT (io_vec.iov_base);
//  if (target_file.sendv_n (&io_vec, 1) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_FILE_IO::sendv_n() file \"%s\": \"%m\", aborting\n"),
//                ACE_TEXT (target_filename.c_str ())));

//    // clean up
//    delete[] io_vec.iov_base;
//    if (source_file.close () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (path_in.c_str ())));
//    if (target_file.remove () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (target_filename.c_str ())));

//    return false;
//  } // end IF
  ACE_FILE_Info file_info;
  result = source_file.get_info (file_info);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::get_info(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    result = target_file.remove ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (target_filename.c_str ())));

    return false;
  } // end IF
  ssize_t bytes_written =
    ACE_OS::sendfile (source_file.get_handle (),
                      target_file.get_handle (),
                      NULL,
                      static_cast<size_t> (file_info.size_));
  if (unlikely (bytes_written != file_info.size_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sendfile(\"%s\",\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (target_filename.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    result = target_file.remove ();
    if (result == -1)
      ACE_DEBUG  ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (target_filename.c_str ())));

    return false;
  } // end IF

  // clean up
//  delete[] io_vec.iov_base;
  result = source_file.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (path_in.c_str ())));
  result = target_file.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (target_filename.c_str ())));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("copied \"%s\" --> \"%s\"\n"),
              ACE_TEXT (path_in.c_str ()),
              ACE_TEXT (target_filename.c_str ())));

  return true;
}

bool
Common_File_Tools::deleteFile (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::deleteFile"));

  int result = -1;

  // connect to the file...
  ACE_FILE_Addr address;
  result = address.set (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // don't want to block on file opening (see ACE doc)...
  ACE_FILE_Connector connector;
  ACE_FILE_IO file;
  result =
    connector.connect (file,
                       address,
                       const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero),
                       ACE_Addr::sap_any,
                       0,
                       (O_WRONLY | O_BINARY),
                       ACE_DEFAULT_FILE_PERMS);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // delete file
  result = file.remove ();
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::remove() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("deleted \"%s\"\n"),
  //            ACE_TEXT (path_in.c_str ())));

  return true;
}

bool
Common_File_Tools::load (const std::string& path_in,
                         unsigned char*& file_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::load"));

  int result = -1;

  // initialize return value(s)
  file_out = NULL;

  FILE* file_p =
    ACE_OS::fopen (ACE_TEXT (path_in.c_str ()),
                   ACE_TEXT_ALWAYS_CHAR ("rb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // obtain file size
  result = ACE_OS::fseek (file_p, 0, SEEK_END);
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fseek(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));

    // clean up
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  long file_size = ACE_OS::ftell (file_p);
  if (unlikely (file_size == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::ftell(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));

    // clean up
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  ACE_OS::rewind (file_p);

  // *PORTABILITY* allocate array
//  file_out = new (std::nothrow) unsigned char[fsize];
  ACE_NEW_NORETURN (file_out,
                    unsigned char[static_cast<unsigned int> (file_size)]);
  if (unlikely (!file_out))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%d): \"%m\", aborting\n"),
                file_size));

    // clean up
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // read data
  result =
    static_cast<int> (ACE_OS::fread (static_cast<void*> (file_out),   // target buffer
                                     static_cast<size_t> (file_size), // read everything ...
                                     1,                               // ... at once
                                     file_p));                        // handle
  if (unlikely (result != 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fread(\"%s\",%d): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                file_size));

    // clean up
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    delete [] file_out;
    file_out = NULL;

    return false;
  } // end IF

  // clean up
  result = ACE_OS::fclose (file_p);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));

    // clean up
    delete [] file_out;
    file_out = NULL;

    return false;
  } // end IF

  return true;
}

bool
Common_File_Tools::open (const std::string& path_in,
                         int flags_in,
                         ACE_FILE_IO& stream_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::open"));

  int result = -1;

  ACE_FILE_Addr file_address;
  result = file_address.set (path_in.c_str ());
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  ACE_FILE_Connector file_connector;
  result =
    file_connector.connect (stream_out,              // stream
                            file_address,            // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            flags_in,                // flags
                            ACE_DEFAULT_FILE_PERMS); // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\",%d): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                flags_in));
    return false;
  } // end IF

  return true;
}

bool
Common_File_Tools::store (const std::string& path_in,
                          const unsigned char* buffer_in,
                          unsigned int size_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::store"));

  // initialize return value(s)
  bool result = false;

  size_t result_2 = 0;
  int result_3 = -1;
  FILE* file_p = NULL;

  file_p = ACE_OS::fopen (ACE_TEXT (path_in.c_str ()),
                          ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  result_2 = ACE_OS::fwrite (buffer_in, size_in, 1, file_p);
  if (unlikely (result_2 != 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                size_in,
                ACE_TEXT (path_in.c_str ())));
    goto clean;
  } // end IF

  result = true;

clean:
  // clean up
  if (likely (file_p))
  {
    result_3 = ACE_OS::fclose (file_p);
    if (unlikely (result_3 == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      result = false;
    } // end IF
  } // end IF

#if defined (_DEBUG)
  if (likely (result))
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("wrote file \"%s\"\n"),
                ACE_TEXT (path_in.c_str ())));
#endif

  return result;
}

unsigned int
Common_File_Tools::size (const ACE_FILE_Addr& address_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::size"));

  int result = -1;
  ACE_FILE_Connector file_connector;
  ACE_FILE_IO file_io;
  ACE_TCHAR buffer[PATH_MAX];
  ACE_FILE_Info file_info;

  result =
    file_connector.connect (file_io,                 // stream
                            address_in,              // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            (O_RDONLY |
                             O_BINARY),              // flags --> open
                            ACE_DEFAULT_FILE_PERMS); // permissions --> open
  if (unlikely (result == -1))
  {
    result = address_in.addr_to_string (buffer,
                                        sizeof (buffer));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                buffer));
    return 0;
  } // end IF

  result = file_io.get_info (file_info);
  if (unlikely (result == -1))
  {
    result = address_in.addr_to_string (buffer,
                                        sizeof (buffer));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                buffer));

    // clean up
    result = file_io.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_IO::close(): \"%m\", continuing\n")));

    return 0;
  } // end IF

  // clean up
  result = file_io.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::close(): \"%m\", continuing\n")));

  return static_cast<unsigned int> (file_info.size_);
}
unsigned int
Common_File_Tools::size (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::size"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (stat));
  result = ACE_OS::stat (path_in.c_str (), &stat);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (path_in.c_str ())));
    return 0;
  } // end IF

  return static_cast<unsigned int> (stat.st_size);
}

std::string
Common_File_Tools::realPath (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::realPath"));

  // initialize result(s)
  std::string result;

  char path[PATH_MAX];
  if (unlikely (!ACE_OS::realpath (path_in.c_str (), path)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::realpath(\"%s\"): %m, aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return result;
  } // end IF
  result = path;

  return result;
}

std::string
Common_File_Tools::getWorkingDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getWorkingDirectory"));

  std::string result;

  // retrieve working directory
  ACE_TCHAR buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  if (unlikely (!ACE_OS::getcwd (buffer, sizeof (buffer))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));
    return result;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer);

  return result;
}

std::string
Common_File_Tools::getConfigurationDataDirectory (const std::string& packageName_in,
                                                  bool isConfiguration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getConfigurationDataDirectory"));

  std::string result;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  TCHAR buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  HRESULT result_2 =
// *TODO*: this is apparently inconsistent (see also config.h for details)
#if defined (ACE_USES_WCHAR)
    SHGetFolderPathW (NULL,                                   // hwndOwner
#else
    SHGetFolderPathA (NULL,                                   // hwndOwner
#endif
                      CSIDL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                      NULL,                                   // hToken
                      SHGFP_TYPE_CURRENT,                     // dwFlags
                      buffer);                                // pszPath
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
                ACE_TEXT (Common_Tools::errorToString (static_cast<DWORD> (result_2)).c_str ())));
    return Common_File_Tools::getWorkingDirectory ();
  } // end IF

#if defined (ACE_USES_WCHAR)
  result = ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (buffer));
#else
  result = ACE_TEXT_ALWAYS_CHAR (buffer);
#endif
#else
#if defined (BASEDIR)
  result = BASEDIR;
#else
  result = ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_APPLICATION_STORAGE_DIRECTORY);
#endif
#endif
  result += ACE_DIRECTORY_SEPARATOR_STR;
  result += packageName_in;
  result += ACE_DIRECTORY_SEPARATOR_STR;
  result +=
      (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY)
                          : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_DIRECTORY));

  // sanity check(s)
  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("not a directory: \"%s\", falling back\n"),
                ACE_TEXT (result.c_str ())));

    // fallback
    return Common_File_Tools::getWorkingDirectory ();
  } // end IF

  return result;
}

std::string
Common_File_Tools::getHomeDirectory (const std::string& user_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getHomeDirectory"));

  // initialize result value(s)
  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE      token = 0;
  ACE_TCHAR   buffer[PATH_MAX];
  DWORD       buffer_size = sizeof (buffer);
#else
  int            result_2 = -1;
  struct passwd  pwd;
  struct passwd* result_p = NULL;
  char           buffer[BUFSIZ]; // _SC_GETPW_R_SIZE_MAX
#endif

  std::string user_name = user_in;
  if (unlikely (user_name.empty ()))
  {
    // fallback --> use current user
    std::string real_name;
    Common_Tools::getCurrentUserName (user_name, real_name);
    if (user_name.empty ())
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_Tools::getCurrentUserName(), falling back\n")));
      goto fallback;
    } // end IF
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (!OpenProcessToken (GetCurrentProcess (),
                                   TOKEN_QUERY,
                                   &token)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ::OpenProcessToken(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto fallback;
  } // end IF

  ACE_OS::memset (buffer, 0, sizeof (buffer));
  // *TODO*: use ACE_TEXT_ macro for this (see also: ace_wchar.h)
#if defined (ACE_USES_WCHAR)
  if (unlikely (!GetUserProfileDirectoryW (token, buffer, &buffer_size)))
#else
  if (unlikely (!GetUserProfileDirectoryA (token, buffer, &buffer_size)))
#endif
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to GetUserProfileDirectory(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError (), false).c_str ())));

    // clean up
    if (!CloseHandle (token))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::errorToString (::GetLastError (), false).c_str ())));

    goto fallback;
  } // end IF

  // clean up
  if (unlikely (!CloseHandle (token)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CloseHandle(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError (), false).c_str ())));

  result = ACE_TEXT_ALWAYS_CHAR (buffer);
#else
  ACE_OS::memset (buffer, 0, sizeof (BUFSIZ));
  result_2 = ACE_OS::getpwnam_r (user_name.c_str (), // user name
                                 &pwd,               // passwd entry
                                 buffer,             // buffer
                                 BUFSIZ,             // buffer size
                                 &result_p);         // result (handle)
  if (unlikely (!result_p))
  {
    if (result_2 == 0)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("user \"%s\" not found, falling back\n"),
                  ACE_TEXT (user_name.c_str ())));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to ACE_OS::getpwnam_r(\"%s\"): \"%m\", falling back\n"),
                  ACE_TEXT (user_name.c_str ())));
    goto fallback;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (pwd.pw_dir);
#endif

  return result;

fallback:
  ACE_TCHAR* string_p =
      ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE));
  if (unlikely (!string_p))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

  return result;
}

std::string
Common_File_Tools::getUserConfigurationDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getUserConfigurationDirectory"));

  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_TCHAR buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  HRESULT result_2 =
// *TODO*: this is apparently inconsistent (see also config.h for details)
#if defined (ACE_USES_WCHAR)
    SHGetFolderPathW (NULL,                                   // hwndOwner
#else
    SHGetFolderPathA (NULL,                                   // hwndOwner
#endif
                      CSIDL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                      NULL,                                   // hToken
                      SHGFP_TYPE_CURRENT,                     // dwFlags
                      buffer);                                // pszPath
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
                ACE_TEXT (Common_Tools::errorToString (static_cast<DWORD> (result_2), false).c_str ())));
    goto fallback;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (buffer);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  std::string user_name;
  std::string real_name;
  Common_Tools::getCurrentUserName (user_name, real_name);
  if (unlikely (user_name.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to Common_Tools::getCurrentUserName(), falling back\n")));
    goto fallback;
  } // end IF

  result = Common_File_Tools::getHomeDirectory (user_name);
  if (unlikely (result.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT("failed to Common_File_Tools::getHomeDirectory(\"%s\"), falling back\n"),
                ACE_TEXT (user_name.c_str ())));
    goto fallback;
  } // end IF

  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += '.';
#endif
  result += ACE_TEXT_ALWAYS_CHAR (LIBCOMMON_PACKAGE_NAME);

  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));
      goto fallback;
    } // end IF
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("created package directory \"%s\"\n"),
                  ACE_TEXT (result.c_str ())));
  } // end IF

  return result;

fallback:
  ACE_TCHAR* string_p =
      ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE));
  if (unlikely (!string_p))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

  return result;
}

std::string
Common_File_Tools::getTempDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getTempDirectory"));

  // initialize return value(s)
  std::string result;

  unsigned int fallback_level = 0;
  std::string environment_variable =
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD result_2 = 0;
  std::string::size_type position = std::string::npos;
use_environment:
#endif
  ACE_TCHAR* string_p =
      ACE_OS::getenv (ACE_TEXT (environment_variable.c_str ()));
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", falling back\n"),
                ACE_TEXT (environment_variable.c_str ())));
    goto fallback;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: %USERPROFILE% is not localized
  ACE_TCHAR buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  //HRESULT result_2 = S_OK;

  //result_2 =
  //  SHGetFolderPath (NULL,                                         // hwndOwner
  //                   CSIDL_LOCAL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
  //                   NULL,                                         // hToken
  //                   SHGFP_TYPE_CURRENT,                           // dwFlags
  //                   buffer);                                      // pszPath
  result_2 = ACE_TEXT_GetTempPath (sizeof (buffer), buffer);
  //if (FAILED (result_2))
  if (unlikely (result_2 == 0))
  {
    //ACE_DEBUG ((LM_WARNING,
    //            ACE_TEXT ("failed to SHGetFolderPath(CSIDL_LOCAL_APPDATA): \"%s\", falling back\n"),
    //            buffer));
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to GetTempPath(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto fallback;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (buffer);

  // strip trailing backslashes
  position =
    result.find_last_of ('\\',
                         std::string::npos); // begin searching at the end !
  if (likely (position != std::string::npos))
    result = result.substr (0, position);
#else
use_path:
#endif
  // sanity check(s): directory exists ?
  // No ? --> (try to) create it then
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
                ACE_TEXT ("created directory: \"%s\"\n"),
                ACE_TEXT (result.c_str ())));
  } // end IF

  return result;

fallback:
  ++fallback_level;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (fallback_level == 1)
  {
    environment_variable =
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE_2);
    goto use_environment;
  } // end IF
  ACE_ASSERT (false);
  // *TODO*: implement fallback levels dependent on host Windows (TM) version
  //         see: https://en.wikipedia.org/wiki/Environment_variable#Windows
#else
  switch (fallback_level)
  {
    case 1:
      result =
          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY);
      goto use_path;
    case 2:
      result =
          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY_2);
      goto use_path;
    default:
      break;
  } // end SWITCH
  ACE_ASSERT (false);
  // *TODO*: implement fallback levels dependent on host platform/version
  //         see e.g. https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
#endif

  return result;
}

std::string
Common_File_Tools::getTempFilename (const std::string& prefix_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getTempFilename"));

  std::string result = Common_File_Tools::getTempDirectory ();
  if (unlikely (result.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::getTempDirectory(), aborting\n")));
    return result;
  } // end IF
  result += ACE_DIRECTORY_SEPARATOR_STR;

  // sanity check(s)
  ACE_ASSERT (prefix_in.size () <= (BUFSIZ - 6 + 1));

  // *NOTE*: see also: man 3 mkstemp
  ACE_TCHAR buffer[BUFSIZ];
  if (unlikely (!prefix_in.empty ()))
    ACE_OS::strcpy (buffer, prefix_in.c_str ());
  ACE_OS::strcpy (buffer + prefix_in.size (), ACE_TEXT ("XXXXXX"));
  // *TODO*: consider using mktemp() instead (less hassle)
  ACE_HANDLE file_handle = ACE_OS::mkstemp (buffer);
  if (unlikely (file_handle == ACE_INVALID_HANDLE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::mkstemp(): \"%m\", aborting\n")));
    return std::string ();
  } // end IF
  result += buffer;

  // clean up
  int result_2 = ACE_OS::close (file_handle);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::close(): \"%m\", continuing\n")));
  result_2 = ACE_OS::unlink (buffer);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::unlink(\"%s\"): \"%m\", continuing\n"),
                buffer));

  return result;
}

std::string
Common_File_Tools::getLogFilename (const std::string& packageName_in,
                                   const std::string& programName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getLogFilename"));

  // sanity check(s)
  ACE_ASSERT (!programName_in.empty ());

  unsigned int fallback_level = 0;
  std::string result;
fallback:
  result = Common_File_Tools::getLogDirectory (packageName_in,
                                               fallback_level);

  if (unlikely (result.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to Common_File_Tools::getLogDirectory(\"%s\",%d), falling back\n"),
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
    return std::string ();
  } // end IF

  return result;
}

std::string
Common_File_Tools::getLogDirectory (const std::string& packageName_in,
                                    unsigned int fallbackLevel_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getLogDirectory"));

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
#endif
  goto use_path;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
use_environment:
#endif
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
#endif
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
#endif
    }
    case 3:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      result = Common_File_Tools::getWorkingDirectory ();
      goto use_path;
#else
#endif
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
#endif
      break;
    }
  } // end SWITCH

  return result;
}
