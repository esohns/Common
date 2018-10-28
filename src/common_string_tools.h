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

#ifndef COMMON_STRING_TOOLS_H
#define COMMON_STRING_TOOLS_H

#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <WTypes.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

class Common_String_Tools
{
 public:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   static std::string to (const BSTR,     // string
                          UINT = CP_ACP); // code page {default: active}
#endif // ACE_WIN32 || ACE_WIN64

   static std::string sanitizeURI (const std::string&); // URI
  // replace ' ' with '_'
  static std::string sanitize (const std::string&); // string
  // remove leading and trailing whitespace
  static std::string strip (const std::string&); // string
  static bool isspace (const std::string&); // string

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_String_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_String_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_String_Tools (const Common_String_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_String_Tools& operator= (const Common_String_Tools&))
};

#endif
