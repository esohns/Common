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
#include "stdafx.h"

#include "common_input_tools.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
// initialize statics
struct termios Common_Input_Tools::terminalSettings;
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_Input_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initialize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (unlikely (!Common_Input_Tools::initializeInput (Common_Input_Tools::terminalSettings)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Tools::initializeInput(), aborting\n")));
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return true;
}

bool
Common_Input_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::finalize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Common_Input_Tools::finalizeInput (Common_Input_Tools::terminalSettings);
#endif // ACE_WIN32 || ACE_WIN64

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Common_Input_Tools::initializeInput (struct termios& terminalSettings_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initializeInput"));

  int result = -1;
  struct termios termios_s;

  result = ::tcgetattr (ACE_STDIN,
                        &terminalSettings_out);
  if (unlikely (result == -1))
  { int error_i = ACE_OS::last_error ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to tcgetattr(%d): %d:\"%m\", aborting\n"),
                ACE_STDIN,
                error_i));
    return false;
  } // end IF

  ACE_OS::memcpy (&termios_s, &terminalSettings_out, sizeof (struct termios));
//  ACE_OS::atexit(reset_terminal_mode);
  cfmakeraw (&termios_s);
  result = ::tcsetattr (ACE_STDIN,
                        TCSANOW,
                        &termios_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to tcsetattr(%d,TCSANOW): \"%m\", aborting\n"),
                ACE_STDIN));
    return false;
  } // end IF

  return true;
}

void
Common_Input_Tools::finalizeInput (const struct termios& terminalSettings_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::finalizeInput"));

  int result = ::tcsetattr (ACE_STDIN,
                            TCSANOW,
                            &terminalSettings_in);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to tcsetattr(%d,TCSANOW): \"%m\", returning\n"),
                ACE_STDIN));
    return;
  } // end IF
}
#endif // ACE_WIN32 || ACE_WIN64
