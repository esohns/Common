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

#ifndef COMMON_UI_CURSES_DEFINES_H
#define COMMON_UI_CURSES_DEFINES_H

#define COMMON_UI_CURSES_CURSOR_MODE_INVISIBLE 0 // invisible

#define COMMON_UI_CURSES_WINDOW_COLUMNS        120
#define COMMON_UI_CURSES_WINDOW_ROWS           30

// *TODO*: move these somewhere else ASAP
// *NOTE*: these are merely UNICODE characters
#define COMMON_UI_CURSES_BLOCK_SOLID           0x2588
#define COMMON_UI_CURSES_BLOCK_THREE_QUARTERS  0x2593
#define COMMON_UI_CURSES_BLOCK_HALF            0x2592
#define COMMON_UI_CURSES_BLOCK_QUARTER         0x2591

// colors (use common_ui_curses_tools::init_colors() first)
#define COMMON_UI_CURSES_BLACK                 0x0000
#define COMMON_UI_CURSES_DARK_GREY             0x0010
#define COMMON_UI_CURSES_GREY                  0x0007
#define COMMON_UI_CURSES_WHITE                 0x000F

#define COMMON_UI_CURSES_BLUE                  0x0001
#define COMMON_UI_CURSES_GREEN                 0x0002
#define COMMON_UI_CURSES_CYAN                  0x0003
#define COMMON_UI_CURSES_RED                   0x0004
#define COMMON_UI_CURSES_MAGENTA               0x0005
#define COMMON_UI_CURSES_YELLOW                0x0006

#define COMMON_UI_CURSES_BRIGHT_BLUE           0x0011
#define COMMON_UI_CURSES_BRIGHT_GREEN          0x0012
#define COMMON_UI_CURSES_BRIGHT_CYAN           0x0013
#define COMMON_UI_CURSES_BRIGHT_RED            0x0014
#define COMMON_UI_CURSES_BRIGHT_MAGENTA        0x0015
#define COMMON_UI_CURSES_BRIGHT_YELLOW         0x0016

#endif
