/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef COMMON_UI_CURSES_MANAGER_COMMON_H
#define COMMON_UI_CURSES_MANAGER_COMMON_H

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_itask.h"

#include "common_ui_curses_common.h"
#include "common_ui_curses_manager.h"

typedef Common_ITask Common_UI_ICurses_Manager_t;

//////////////////////////////////////////

typedef Common_UI_Curses_Manager_T<ACE_MT_SYNCH,
                                   struct Common_UI_Curses_Configuration,
                                   struct Common_UI_Curses_State> Common_UI_Curses_Manager_t;
typedef ACE_Singleton<Common_UI_Curses_Manager_t,
                      ACE_MT_SYNCH::MUTEX> COMMON_UI_CURSES_MANAGER_SINGLETON;

#endif
