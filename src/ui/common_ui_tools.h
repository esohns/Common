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

#ifndef COMMON_UI_TOOLS_H
#define COMMON_UI_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"

#include "common_ui_common.h"

class Common_UI_Tools
{
 public:
  // *NOTE*: these refer to output (!) display devices (i.e. monitors, screens,
  //         projectors, headsets, etc.), NOT graphics adapters
  inline static struct Common_UI_DisplayDevice getDefaultDisplayDevice () { return Common_UI_Tools::getDisplayDevice (ACE_TEXT_ALWAYS_CHAR ("")); }
  static Common_UI_DisplayDevices_t getDisplayDevices (); // return value: connected devices
  static struct Common_UI_DisplayDevice getDisplayDevice (const std::string&); // device identifier

  static bool displaySupportsResolution (const std::string&,             // device identifier
                                         const Common_UI_Resolution_t&); // resolution

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools (const Common_UI_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Tools& operator= (const Common_UI_Tools&));
};

#endif
