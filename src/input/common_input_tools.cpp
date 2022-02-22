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
void
common_reset_input_mode (void)
{
  tcsetattr (ACE_STDIN,
             TCSANOW,
             &Common_Input_Tools::terminalSettings);
}

// initialize statics
struct termios Common_Input_Tools::terminalSettings;
#endif // ACE_WIN32 || ACE_WIN64

bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Common_Input_Tools::initialize ()
#else
Common_Input_Tools::initialize (bool lineMode_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initialize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (unlikely (!Common_Input_Tools::initializeInput (lineMode_in,
                                                      Common_Input_Tools::terminalSettings)))
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
Common_Input_Tools::initializeInput (bool lineMode_in,
                                     struct termios& terminalSettings_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initializeInput"));

  int result = -1;
  struct termios termios_s;

  // sanity check(s)
  if (unlikely (!isatty (ACE_STDIN)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%d is not associated to a terminal, aborting\n"),
                ACE_STDIN));
    return false;
  } // end IF
  if (unlikely (lineMode_in))
    return true; // nothing to do

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
  //  cfmakeraw (&termios_s);
//  termios_s.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  termios_s.c_lflag &= ~(ICANON); /* Clear ICANON */
  termios_s.c_cc[VMIN] = 1;
  termios_s.c_cc[VTIME] = 0;

  result = ::tcsetattr (ACE_STDIN,
                        TCSAFLUSH,
                        &termios_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to tcsetattr(%d,TCSANOW): \"%m\", aborting\n"),
                ACE_STDIN));
    return false;
  } // end IF

  // schedule reset at program end
  result = ACE_OS::atexit (common_reset_input_mode);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::atexit(): \"%m\", aborting\n")));
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
