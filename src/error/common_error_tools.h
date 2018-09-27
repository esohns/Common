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

#ifndef COMMON_ERROR_TOOLS_H
#define COMMON_ERROR_TOOLS_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <string>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

void
common_error_terminate_function ();

class Common_Error_Tools
 : public Common_SInitializeFinalize_T<Common_Error_Tools>
{
 public:
  static void initialize ();
  static void finalize ();

  // --- debug ---
  static bool inDebugSession ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::string errorToString (DWORD,         // error
                                    bool = false); // ? use AMGetErrorText() : use FormatMessage()
#endif // ACE_WIN32 || ACE_WIN64

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Error_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools (const Common_Error_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools& operator= (const Common_Error_Tools&))
};

#endif
