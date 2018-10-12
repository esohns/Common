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

#ifndef COMMON_LOG_TOOLS_H
#define COMMON_LOG_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"

// forward declarations
class ACE_Log_Msg_Backend;

class Common_Log_Tools
{
 public:
  static bool initializeLogging (const std::string&,           // program name (i.e. argv[0])
                                 const std::string&,           // log file {"" --> disable}
                                 bool = false,                 // log to syslog ?
                                 bool = false,                 // enable tracing messages ?
#if defined (_DEBUG)
                                 bool = true,                  // enable debug messages ?
#else
                                 bool = false,                 // enable debug messages ?
#endif // _DEBUG
                                 ACE_Log_Msg_Backend* = NULL); // logger backend {NULL --> disable}
  static void finalizeLogging ();

  static std::string getLogDirectory (const std::string&, // package name
                                      unsigned int = 0);  // fallback level {0: default}
  static std::string getLogFilename (const std::string&,    // package name
                                     const std::string&); // program name

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Log_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~Common_Log_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_Log_Tools (const Common_Log_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_Log_Tools& operator= (const Common_Log_Tools&));
};

#endif
