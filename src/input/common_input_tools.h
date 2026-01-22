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

#ifndef COMMON_INPUT_TOOLS_H
#define COMMON_INPUT_TOOLS_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "termios.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
void
common_reset_input_mode (void);
#endif // ACE_WIN32 || ACE_WIN64

class Common_Input_Tools
// : public Common_SInitializeFinalize_T<Common_Input_Tools>
{
 public:
  static bool initialize (bool = false); // line mode ? : character-by-character
  static bool finalize ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static bool initializeInput (bool,    // line mode ? : character-by-character
                               DWORD&); // return value: previous terminal settings
  static void finalizeInput (DWORD); // previous terminal settings
#else
  static bool initializeInput (bool,             // line mode ? : character-by-character
                               struct termios&); // return value: previous terminal settings
  static void finalizeInput (const struct termios&); // previous terminal settings
#endif // ACE_WIN32 || ACE_WIN64

  // send a keystroke to the terminal
  // *NOTE*: "...Since Linux 6.2, this operation may require the CAP_SYS_ADMIN
  //          capability (if the dev.tty.legacy_tiocsti sysctl variable is set
  //          to false) ..."
  static void input (char); // key to send

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static HWND mouseCursorToWindow ();
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static DWORD terminalSettings;
#else
  static struct termios terminalSettings;
#endif // ACE_WIN32 || ACE_WIN64

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Input_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Input_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Input_Tools (const Common_Input_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Input_Tools& operator= (const Common_Input_Tools&))
};

#endif
