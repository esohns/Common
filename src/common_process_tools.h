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

#ifndef COMMON_PROCESS_TOOLS_H
#define COMMON_PROCESS_TOOLS_H

#include <string>

#include "ace/config-lite.h"
#include "ace/ace_wchar.h"

class Common_Process_Tools
{
 public:
  static std::string toString (int,           // argc
                               ACE_TCHAR*[]); // argv

  // *NOTE*: this uses system(3), piping stdout into a temporary file
  //         --> the command line must not have piped stdout already
  // *TODO*: while this should work on most platforms, there are more efficient
  //         alternatives (e.g. see also: man popen() for Linux)
  // *TODO*: enhance the API to return the exit status
  static bool command (const std::string&, // command line
                       int&,               // return value: exit status
                       std::string&,       // return value: stdout
                       bool = true);       // return stdout

  // *NOTE*: the Windows implementation relies on the 'tasklist.exe' command
  // *NOTE*: the Linux implementation relies on the 'pidofproc' command, which
  //         may not be available on all versions of all distributions
  // *TODO*: parse /proc manually, or find a better alternative (e.g. 'libproc',
  //         testing for 'PID-files' in /var/run, ...)
  // *NOTE*: returns 0 on failure
  static pid_t id (const std::string&); // executable (base-)name
  // *NOTE*: the Windows implementation relies on the 'taskkill.exe' command
  static bool kill (pid_t);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *NOTE*: as used by polkit (i.e. queries /proc/self/stat)
  static uint64_t getStartTime ();
#endif // ACE_WIN32 || ACE_WIN64

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Process_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Process_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Process_Tools (const Common_Process_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Process_Tools& operator= (const Common_Process_Tools&))
};

#endif
