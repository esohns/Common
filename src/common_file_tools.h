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

#ifndef COMMON_FILE_TOOLS_H
#define COMMON_FILE_TOOLS_H

#include <cstdint>
#include <string>

#include "ace/ACE.h"
#include "ace/Basic_Types.h"
#include "ace/Global_Macros.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_IO.h"
#include "ace/OS_NS_dirent.h"

#include "common_file_common.h"

class Common_File_Tools
{
 public:
  static void initialize (const std::string&); // *NOTE*: pass argv[0] here

  static std::string addressToString (const ACE_FILE_Addr&); // file name
  static std::string basename (const std::string&, // (FQ) path
                               bool = false);      // strip trailing '.xxx'-suffix ?
  // *NOTE*: returns dirname == empty && basename != empty
  static bool isBasename (const std::string&); // (FQ) path
  inline static bool backup (const std::string& path_in) { return Common_File_Tools::copyFile (path_in, ACE_TEXT_ALWAYS_CHAR ("")); }
  static bool create (const std::string&); // (FQ) path
  static bool createDirectory (const std::string&, // directory
                               bool = true);       // create missing subdirectories ?
  static bool copyFile (const std::string&,  // (FQ) path
                        const std::string&); // directory {"": make a backup in the same directory}
  static bool deleteFile (const std::string&); // (FQ) path
  static bool deleteFiles (const Common_File_IdentifierList_t&); // file(s)
  inline static std::string directory (const std::string& path_in) { return ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT (path_in.c_str ()), ACE_DIRECTORY_SEPARATOR_CHAR)); }
  static std::string cropExtension (const std::string&); // filename
  // *NOTE*: this doesn't do any sanity checking --> use with care
  static std::string fileExtension (const std::string&, // (FQ) path
                                    bool = false);      // return leading '.' (if any) ?

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::string escape (const std::string&); // (FQ) path
#endif // ACE_WIN32 || ACE_WIN64
  // *IMPORTANT NOTE*: APIs in this section are inherently 'racy' and should
  //                   best be avoided altogether
  static bool exists (const std::string&); // (FQ) path

  static bool access (const std::string&, // (FQ) path
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                      DWORD,              // (combination of) generic (!) access right(s)
                      PSID = NULL);       // user SID {NULL: 'current' user}
#else
                      ACE_UINT32);        // mask to check against 'st_mode'
                                          // field of 'struct stat' (see: man
                                          // stat(2))
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static bool protection (const std::string&, // (FQ) path
                          ACE_UINT32);        // mask to check against 'st_mode'
                                              // field of 'struct stat' (see: man
                                              // stat(2))
#endif // ACE_WIN32 || ACE_WIN64
  static bool type (const std::string&, // (FQ) path
                    ACE_UINT32);        // mask to check against 'st_mode'
                                        // field of 'struct stat' (see: man
                                        // stat(2))

  // *NOTE*: these verify that:
  //         - the path is valid
  //         - the path refers to a file system type
  //         - and that the property referred is permitted to any entity at all
  static bool isReadable (const std::string&); // (FQ) path
  static bool isWriteable (const std::string&); // (FQ) path
  static bool isExecutable (const std::string&); // (FQ) path

  // *NOTE*: on Win32 systems, the uid_t passed in currently has:
  //         - SECURITY_LOCAL_SID_AUTHORITY (which in turn means that the
  //           verification (most probably) applies to 'local user's only)
  //         - SECURITY_INTERACTIVE_RID (which in turn means that the
  //           verification (most probably) applies to 'real person' users only)
  // *TODO*: (on point one) consider using SECURITY_WORLD_SID_AUTHORITY instead,
  //         or passing in the authority setting altogether
  // *TODO*: (on point two) consider using SECURITY_AUTHENTICATED_USER_RID
  //         instead
  static bool canRead (const std::string&,               // (FQ) path
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                       const std::string&);              // (FQ) account name {"": current user}
#else
                       uid_t = static_cast<uid_t> (-1)); // uid {-1: euid}
#endif // ACE_WIN32 || ACE_WIN64
  static bool canWrite (const std::string&,               // (FQ) path
                        uid_t = static_cast<uid_t> (-1)); // uid {-1: euid}
  static bool canExecute (const std::string&,               // (FQ) path
                          uid_t = static_cast<uid_t> (-1)); // uid {-1: euid}

  static bool isEmpty (const std::string&); // (FQ) path
  static bool isFile (const std::string&); // (FQ) path
  static bool isDirectory (const std::string&); // directory
  static bool isLink (const std::string&); // directory
  static bool isEmptyDirectory (const std::string&); // directory
  static bool isValidFilename (const std::string&); // (FQ) path
  static bool isValidPath (const std::string&); // (FQ) path

  // *NOTE*: users need to free (delete[]) the returned buffer
  static bool load (const std::string&, // (FQ) path
                    uint8_t*&,          // return value: file data
                    ACE_UINT64&,        // return value: file size
                    unsigned int = 0);  // padding byte(s)
  static bool open (const std::string&, // (FQ) path
                    int,                // flags
                    ACE_FILE_IO&);      // return value: file stream
  // *IMPORTANT NOTE*: creates any sub-directories as necessary
  static bool store (const std::string&, // (FQ) path
                     const uint8_t*,     // buffer handle
                     unsigned int,       // buffer size
                     bool = false);      // append if file exists ?
  static ACE_UINT64 size (const ACE_FILE_Addr&); // file name
  static ACE_UINT64 size (const std::string&); // (FQ) path

  static Common_File_IdentifierList_t files (const std::string&,    // directory
                                             ACE_SCANDIR_SELECTOR); // selector
  static ACE_UINT64 size (const Common_File_IdentifierList_t&); // file(s)

  static std::string linkTarget (const std::string&); // path
  static std::string realPath (const std::string&); // path

  // *PORTABILITY*: this can be influenced by #define BASEDIR and returns
  //                BASEDIR/$PACKAGENAME/src
  //                this defaults to $PROJECTS_ROOT/$PACKAGENAME/src
  static std::string getSourceDirectory (const std::string&,  // package name
                                         const std::string&); // module name
  // *NOTE*: returns APPDATA/packagename[/modulename] | /usr/local/etc|share/packagename
  static std::string getSystemConfigurationDataDirectory (const std::string&, // package name
                                                          const std::string&, // module name
                                                          bool);              // configuration ? : data
  // *NOTE*: depends on whether running in a debug session
  static std::string getConfigurationDataDirectory (const std::string&, // package name
                                                    const std::string&, // module name
                                                    bool);              // configuration ? : data
  inline static std::string getExecutableDirectory() { return Common_File_Tools::executableBase; };
  static std::string getHomeDirectory (const std::string&); // user name (empty ? current user)
  // *NOTE*: (try to) create the directory if it doesn't exist
  static std::string getUserConfigurationDirectory ();
  static std::string getUserDownloadDirectory (const std::string&); // user name (empty ? current user)

  static bool setWorkingDirectory (const std::string&); // path string
  static std::string getWorkingDirectory ();

  static std::string getTempDirectory ();
  static std::string getTempFilename (const std::string&, // prefix
                                      bool = true);       // return full path ?

  static std::string executable; // executable base name + extension
  static std::string executableBase; // executables' base directory

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools ())
  ACE_UNIMPLEMENTED_FUNC (virtual ~Common_File_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools (const Common_File_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools& operator= (const Common_File_Tools&))
};

#endif
