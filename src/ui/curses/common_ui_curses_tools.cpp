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

short
Common_UI_Curses_Tools::colornum (ACE_UINT8 fg,
                                  ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::colornum"));

  const int B = 1 << 7;

  int bbb, ffff;
  bbb = (7 & bg) << 4;
  ffff = 7 & fg;

  return (B | bbb | ffff);
}

void
Common_UI_Curses_Tools::init_colorpairs ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::init_colorpairs"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(# curses-) COLORS: %d; COLOR_PAIRS: %d\n"),
              COLORS, COLOR_PAIRS));

  int result;
  for (ACE_UINT8 bg = 0; bg <= 7; bg++)
    for (ACE_UINT8 fg = 0; fg <= 7; fg++)
    {
      short colorpair = Common_UI_Curses_Tools::colornum (fg, bg);
      result = init_pair (colorpair,
                          Common_UI_Curses_Tools::curs_color (fg),
                          Common_UI_Curses_Tools::curs_color (bg));
      ACE_ASSERT (result == 0/*OK*/);
      ACE_UNUSED_ARG (result);
    } // end FOR
}

short
Common_UI_Curses_Tools::curs_color (ACE_UINT8 color)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::curs_color"));

  switch (7 & color)            /* RGB */
  {
    case 0:                     /* 000 */
      return COLOR_BLACK;
    case 1:                     /* 001 */
      return COLOR_BLUE;
    case 2:                     /* 010 */
      return COLOR_GREEN;
    case 3:                     /* 011 */
      return COLOR_CYAN;
    case 4:                     /* 100 */
      return COLOR_RED;
    case 5:                     /* 101 */
      return COLOR_MAGENTA;
    case 6:                     /* 110 */
      return COLOR_YELLOW;
    case 7:                     /* 111 */
      return COLOR_WHITE;
    default:
      break;
  } // end SWITCH

  ACE_ASSERT (false);
#if defined (_MSC_VER) || defined (__ghs__) || defined(__BORLANDC__) || defined (__USLC__) || defined (__DCC__) || defined (__PGI) || defined (__IAR_SYSTEMS_ICC__)
  return 0;
#else /* __ghs__ || ..... */
  ACE_NOTREACHED (return 0;)
#endif /* __ghs__ || ..... */
}

bool
Common_UI_Curses_Tools::is_bold (ACE_UINT8 color)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::is_bold"));

  /* return the intensity bit */
  const int i = 1 << 3;

  return (i & color);
}

void
Common_UI_Curses_Tools::setcolor (ACE_UINT8 fg,
                                  ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::setcolor"));

  /* set the color pair (colornum) and bold/bright (A_BOLD) */

  attron (COLOR_PAIR (Common_UI_Curses_Tools::colornum (fg, bg)));
  if (Common_UI_Curses_Tools::is_bold (fg)/* || Common_UI_Curses_Tools::is_bold (bg)*/)
    attron (A_BOLD);
}

void
Common_UI_Curses_Tools::unsetcolor (ACE_UINT8 fg,
                                    ACE_UINT8 bg)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_Curses_Tools::unsetcolor"));

  /* unset the color pair (colornum) and bold/bright (A_BOLD) */

  attroff (COLOR_PAIR (Common_UI_Curses_Tools::colornum (fg, bg)));
  if (Common_UI_Curses_Tools::is_bold (fg)/* || Common_UI_Curses_Tools::is_bold (bg)*/)
    attroff (A_BOLD);
}
