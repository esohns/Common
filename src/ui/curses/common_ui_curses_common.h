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

#ifndef COMMON_UI_CURSES_COMMON_H
#define COMMON_UI_CURSES_COMMON_H

#include <map>
#include <string>

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

#include "common_ui_common.h"

#include "common_ui_curses_defines.h"

typedef std::map<std::string, struct panel*> Common_UI_Curses_Panels_t;
typedef Common_UI_Curses_Panels_t::iterator Common_UI_Curses_PanelsIterator_t;

struct Common_UI_Curses_State
 : Common_UI_State
{
  Common_UI_Curses_State ()
   : Common_UI_State ()
   , dispatchState (NULL)
   , finished (false)
   , lock ()
   , screen (NULL)
   , std_window (NULL)
   ///////////////////////////////////////
   , userData (NULL)
  {}

  // dispatch loop
  struct Common_EventDispatchState* dispatchState;
  bool                              finished;
  ACE_SYNCH_MUTEX                   lock;
  SCREEN*                           screen;
  WINDOW*                           std_window;

  ////////////////////////////////////////

  void*                             userData; // cb user data
};

typedef bool (*CursesFunc) (struct Common_UI_Curses_State*); // state
typedef bool (*CursesDataFunc) (struct Common_UI_Curses_State*, // state
                                int);                           // input character

struct Common_UI_Curses_EventHookConfiguration
{
  Common_UI_Curses_EventHookConfiguration ()
   : initHook (NULL)
   , finiHook (NULL)
   , inputHook (NULL)
   , mainHook (NULL)
  {}

  CursesFunc     initHook;
  CursesFunc     finiHook;
  CursesDataFunc inputHook;
  CursesFunc     mainHook;
};

struct Common_UI_Curses_Configuration
{
  Common_UI_Curses_Configuration ()
   : hooks ()
   , height (COMMON_UI_CURSES_WINDOW_ROWS)
   , width (COMMON_UI_CURSES_WINDOW_COLUMNS)
  {}

  struct Common_UI_Curses_EventHookConfiguration hooks;

  int                                            height;
  int                                            width;
};

//////////////////////////////////////////

struct Common_UI_Curses_ProgressData
{
  Common_UI_Curses_ProgressData ()
   : state (NULL)
  {}

  struct Common_UI_Curses_State* state;
};

struct Common_UI_Curses_CBData
 : Common_UI_CBData
{
  Common_UI_Curses_CBData ()
   : Common_UI_CBData ()
   , progressData ()
   , UIState (NULL)
  {
    progressData.state = UIState;
  }

  struct Common_UI_Curses_ProgressData progressData;
  struct Common_UI_Curses_State*       UIState;
};

#endif
