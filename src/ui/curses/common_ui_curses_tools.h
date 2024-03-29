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

#ifndef COMMON_UI_CURSES_TOOLS_H
#define COMMON_UI_CURSES_TOOLS_H

#include "ace/ace_wchar.h"
#include "ace/Basic_Types.h"
#include "ace/Global_Macros.h"

class Common_UI_Curses_Tools
// : public Common_SInitializeFinalize_T<Common_UI_Tools>
{
 public:
  static bool initialize (int,           // argc
                          ACE_TCHAR*[]); // argv
  static bool finalize ();

  static void init_colorpairs ();
  inline static short curs_color (ACE_UINT8 color) { return (7 & color); }
  static ACE_UINT8 colornum (ACE_UINT8,  // foreground
                             ACE_UINT8); // background
  static bool is_bold (ACE_UINT8); // color
  static void setcolor (ACE_UINT8,  // foreground
                        ACE_UINT8); // background
  static void unsetcolor (ACE_UINT8,  // foreground
                          ACE_UINT8); // background

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_Curses_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_Tools (const Common_UI_Curses_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_Tools& operator= (const Common_UI_Curses_Tools&))
};

#endif
