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

#include "common_ui_curses_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN32)
#undef MOUSE_MOVED
#include "curses.h"
#else
#include "ncurses.h"
 // *NOTE*: the ncurses "timeout" macros conflicts with
 //         ACE_Synch_Options::timeout. Since not currently used, it's safe to
 //         undefine
#undef timeout
#endif // ACE_WIN32 || ACE_WIN32
#include "panel.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

bool
Common_UI_Curses_Tools::initialize (int argc_in,
                                    ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::initialize"));

  //Common_UI_Curses_Tools::init_colorpairs ();

  return true;
}

bool
Common_UI_Curses_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::finalize"));

  return true;
}

ACE_UINT8
Common_UI_Curses_Tools::colornum (ACE_UINT8 fg,
                                  ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::colornum"));

  if (bg == COMMON_UI_CURSES_BLACK && fg == COMMON_UI_CURSES_WHITE)
    return 0; // --> color-pair 0
  ACE_UINT8 bbbb, ffff;
  bbbb = (0xF & bg) << 4;
  if (bg == COMMON_UI_CURSES_BLACK && fg == COMMON_UI_CURSES_BLACK)
    ffff = 0xF & COMMON_UI_CURSES_WHITE; // 'slot' of color-pair 0
  else
    ffff = 0xF & fg;
  return (bbbb | ffff);
}

void
Common_UI_Curses_Tools::init_colorpairs ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::init_colorpairs"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(# curses-) COLORS: %d\n"),
              COLORS));

  int result = ERR;
  for (ACE_UINT8 bg = 0; bg <= 15; bg++)
    for (ACE_UINT8 fg = 0; fg <= 15; fg++)
    {
      if (bg == COMMON_UI_CURSES_BLACK && fg == COMMON_UI_CURSES_WHITE)
        continue; // --> color-pair 0
      if (bg == COMMON_UI_CURSES_BLACK && fg == COMMON_UI_CURSES_BLACK)
        result =
            init_pair (Common_UI_Curses_Tools::colornum (COMMON_UI_CURSES_BLACK, COMMON_UI_CURSES_WHITE),
                       fg, bg); // --> use the 'slot' of color-pair 0
      else
        result = init_pair (Common_UI_Curses_Tools::colornum (fg, bg),
                            fg, bg);
      ACE_ASSERT (result == OK);
    } // end FOR
}

bool
Common_UI_Curses_Tools::is_bold (ACE_UINT8 color)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::is_bold"));

  /* return the intensity bit */

  int i = 1 << 3;
  return (i & color);
}

void
Common_UI_Curses_Tools::setcolor (ACE_UINT8 fg, ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::setcolor"));

  /* set the color pair (colornum) and bold/bright (A_BOLD) */

  attron (COLOR_PAIR (Common_UI_Curses_Tools::colornum (fg, bg)));
  if (Common_UI_Curses_Tools::is_bold (bg) || Common_UI_Curses_Tools::is_bold (fg))
    attron (A_BOLD);
}

void
Common_UI_Curses_Tools::unsetcolor (ACE_UINT8 fg, ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::unsetcolor"));

  /* unset the color pair (colornum) and bold/bright (A_BOLD) */

  attroff (COLOR_PAIR (Common_UI_Curses_Tools::colornum (fg, bg)));
  if (Common_UI_Curses_Tools::is_bold (fg) || Common_UI_Curses_Tools::is_bold (bg))
    attroff (A_BOLD);
}
