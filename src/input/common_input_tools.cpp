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

#if defined (ACE_LINUX)
#include "sys/ioctl.h"
#endif // ACE_LINUX

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
void
common_reset_input_mode (void)
{
  SetConsoleMode (ACE_STDIN,
                  Common_Input_Tools::terminalSettings);
}

// initialize statics
DWORD Common_Input_Tools::terminalSettings;
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
Common_Input_Tools::initialize (bool lineMode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initialize"));

  if (unlikely (!Common_Input_Tools::initializeInput (lineMode_in,
                                                      Common_Input_Tools::terminalSettings)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Input_Tools::initializeInput(), aborting\n")));
    return false;
  } // end IF

  return true;
}

bool
Common_Input_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::finalize"));

  Common_Input_Tools::finalizeInput (Common_Input_Tools::terminalSettings);

  return true;
}

bool
Common_Input_Tools::initializeInput (bool lineMode_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                     DWORD& terminalSettings_out)
#else
                                     struct termios& terminalSettings_out)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::initializeInput"));

  int result = -1;

  // sanity check(s)
  if (unlikely (lineMode_in))
    return true; // nothing to do

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // disable 'line input'
  if (unlikely (!GetConsoleMode (ACE_STDIN,
                                 &terminalSettings_out)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetConsoleMode(): \"%m\", aborting\n")));
    return false;
  } // end IF
  DWORD console_mode_i = terminalSettings_out;
  console_mode_i &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
  if (unlikely (!SetConsoleMode (ACE_STDIN,
                                 console_mode_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetConsoleMode(%u): \"%m\", aborting\n"),
                console_mode_i));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("disabled ENABLE_LINE_INPUT\n")));
#else
  struct termios termios_s;

  // sanity check(s)
  if (unlikely (!isatty (ACE_STDIN)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%d is not associated to a terminal, aborting\n"),
                ACE_STDIN));
    return false;
  } // end IF

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
                ACE_TEXT ("failed to tcsetattr(%d,TCSAFLUSH): \"%m\", aborting\n"),
                ACE_STDIN));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%d: disabled canonical mode\n"),
              ACE_STDIN));
#endif // ACE_WIN32 || ACE_WIN64

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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Common_Input_Tools::finalizeInput (DWORD terminalSettings_in)
#else
Common_Input_Tools::finalizeInput (const struct termios& terminalSettings_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::finalizeInput"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (!SetConsoleMode (ACE_STDIN,
                                 terminalSettings_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetConsoleMode(%u): \"%m\", returning\n"),
                terminalSettings_in));
    return;
  } // end IF
#else
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
#endif // ACE_WIN32 || ACE_WIN64
}

void Common_Input_Tools::input (char character_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Input_Tools::input"));

#if defined(ACE_WIN32) || defined(ACE_WIN64)
  struct tagINPUT input_s[2];
  ACE_OS::memset (&input_s[0], 0, sizeof (struct tagINPUT));

  HKL keyboad_layout_h = GetKeyboardLayout (0);
  ACE_ASSERT (keyboad_layout_h);

  input_s[0].type = INPUT_KEYBOARD;
  //input_s[0].ki.wScan = 0;
    //MapVirtualKeyEx (VkKeyScanEx (character_in, keyboad_layout_h), MAPVK_VK_TO_VSC, keyboad_layout_h);
  //input_s[0].ki.time = 0;
  //input_s[0].ki.dwExtraInfo = 0;
  input_s[0].ki.wVk = VkKeyScanEx (character_in, keyboad_layout_h);
  //input_s[0].ki.dwFlags = 0 /*| KEYEVENTF_SCANCODE*/; // 0 for key press

  input_s[1] = input_s[0];
  input_s[1].ki.dwFlags |= KEYEVENTF_KEYUP /* | KEYEVENTF_SCANCODE*/;

  UINT result = SendInput (ARRAYSIZE (input_s), input_s, sizeof (INPUT));
  if (unlikely (result != ARRAYSIZE (input_s)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SendInput(%c): \"%s\", returning\n"),
                character_in,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false, false).c_str ())));
    return;
  } // end IF
#else
  // sanity check(s)
  if (unlikely (!isatty (ACE_STDIN)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%d is not associated to a terminal, returning\n"),
                ACE_STDIN));
    return;
  } // end IF

  int result = ::ioctl (ACE_STDIN,
                        TIOCSTI,
                        &character_in);
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ioctl(%d,TIOCSTI,%c): \"%m\", returning\n"),
                ACE_STDIN,
                character_in));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
}
