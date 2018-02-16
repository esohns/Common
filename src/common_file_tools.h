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

#include <string>

#include "ace/Global_Macros.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_IO.h"

//#include "common_exports.h"

//class Common_Export Common_File_Tools
class Common_File_Tools
{
 public:
  static std::string addressToString (const ACE_FILE_Addr&); // file name
  static bool create (const std::string&); // (FQ) path
  static bool createDirectory (const std::string&); // directory
  static bool copyFile (const std::string&,  // (FQ) path
                        const std::string&); // directory
  static bool deleteFile (const std::string&); // (FQ) path
  // *NOTE*: this doesn't do any sanity checking --> use with care
  static std::string fileExtension (const std::string&, // (FQ) path
                                    bool = false);      // return leading '.' (if any) ?

  // *IMPORTANT NOTE*: this 'isxxx'-API is inherently 'racy' and should best be
  //                   avoided altogether

  // *NOTE*: these checks currently verify accessibility by the 'owner' (!) of
  //         the file; this may not be the current 'user'
  // *TODO*: support passing in a user- and/or group id
  static bool isExecutable (const std::string&); // (FQ) path
  // *NOTE*: this also checks whether the given path specifies a 'regular' file
  //         (or (sym-)link or directory), i.e. does not refer to fifo-,
  //         device-, or other node types with file system entries
  static bool isReadable (const std::string&); // (FQ) path

  static bool isEmpty (const std::string&); // (FQ) path
  static bool isDirectory (const std::string&); // directory
  static bool isEmptyDirectory (const std::string&); // directory
  static bool isValidFilename (const std::string&); // (FQ) path
  static bool isValidPath (const std::string&); // (FQ) path

  // *NOTE*: users need to free (delete[]) the returned buffer
  static bool load (const std::string&, // (FQ) path
                    unsigned char*&);   // return value: memory buffer (array)
  static bool open (const std::string&, // (FQ) path
                    int,                // flags
                    ACE_FILE_IO&);      // return value: file stream
  static bool store (const std::string&,   // (FQ) path
                     const unsigned char*, // buffer handle
                     unsigned int);        // buffer size
  static unsigned int size (const ACE_FILE_Addr&); // file name
  static unsigned int size (const std::string&); // (FQ) path

  static std::string realPath (const std::string&); // path

//  // *PORTABILITY*: - on UNIX, this is passed as macro BASEDIR at compile time
//                      (/usr/share/$PACKAGENAME/ as fallback)
//  //                - on WIN32, this defaults to $APPDATA/$PACKAGENAME
  static std::string getConfigurationDataDirectory (const std::string&, // package name
                                                    bool);              // configuration ? : data

  static std::string getHomeDirectory (const std::string&); // user name
  // *NOTE*: (try to) create the directory if it doesn't exist
  static std::string getUserConfigurationDirectory ();
  static std::string getWorkingDirectory ();

  static std::string getLogDirectory (const std::string&, // package name
                                      unsigned int = 0);  // fallback level {0: default}
  static std::string getLogFilename (const std::string&,    // package name
                                       const std::string&); // program name

  static std::string getTempDirectory ();
  static std::string getTempFilename (const std::string&); // prefix

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools ())
  ACE_UNIMPLEMENTED_FUNC (virtual ~Common_File_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools (const Common_File_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_File_Tools& operator= (const Common_File_Tools&))

  //// helper methods
//  static int dirent_selector (const dirent*);
//  static int dirent_comparator (const dirent**,
//                                const dirent**);
};

#endif
