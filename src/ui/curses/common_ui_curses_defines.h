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
#define COMMON_UI_CURSES_BLOCK_SOLID           L'\u2588' // 9608
#define COMMON_UI_CURSES_BLOCK_THREE_QUARTERS  L'\u2593' // 9619
#define COMMON_UI_CURSES_BLOCK_HALF            L'\u2592' // 9618
#define COMMON_UI_CURSES_BLOCK_QUARTER         L'\u2591' // 9617
// #define COMMON_UI_CURSES_BLOCK_SOLID           L'█'
// #define COMMON_UI_CURSES_BLOCK_THREE_QUARTERS  L'▓'
// #define COMMON_UI_CURSES_BLOCK_HALF            L'▒'
// #define COMMON_UI_CURSES_BLOCK_QUARTER         L'░'

// colors (use common_ui_curses_tools::init_colors() first)
#define COMMON_UI_CURSES_BLACK                 0x00
#define COMMON_UI_CURSES_DARK_GREY             0x08
#define COMMON_UI_CURSES_GREY                  0x07
#define COMMON_UI_CURSES_WHITE                 0x0F

#define COMMON_UI_CURSES_BLUE                  0x01
#define COMMON_UI_CURSES_GREEN                 0x02
#define COMMON_UI_CURSES_CYAN                  0x03
#define COMMON_UI_CURSES_RED                   0x04
#define COMMON_UI_CURSES_MAGENTA               0x05
#define COMMON_UI_CURSES_YELLOW                0x06

#define COMMON_UI_CURSES_BRIGHT_BLUE           0x09
#define COMMON_UI_CURSES_BRIGHT_GREEN          0x0A
#define COMMON_UI_CURSES_BRIGHT_CYAN           0x0B
#define COMMON_UI_CURSES_BRIGHT_RED            0x0C
#define COMMON_UI_CURSES_BRIGHT_MAGENTA        0x0D
#define COMMON_UI_CURSES_BRIGHT_YELLOW         0x0E

#endif
