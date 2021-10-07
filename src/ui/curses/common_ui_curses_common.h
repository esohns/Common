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

#include "common_ui_common.h"

struct Common_UI_Curses_State
 : Common_UI_State
{
  Common_UI_Curses_State ()
   : Common_UI_State ()
   ///////////////////////////////////////
   , userData (NULL)
  {}

  ////////////////////////////////////////

  void* userData; // cb user data
};

struct Common_UI_Curses_Configuration
{
  Common_UI_Curses_Configuration ()
  {}
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
