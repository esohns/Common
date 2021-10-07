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

#ifndef COMMON_UI_GTK_EVENTHANDLER_H
#define COMMON_UI_GTK_EVENTHANDLER_H

#include "ace/Global_Macros.h"

template <typename CallbackDataType>
class Common_UI_Curses_EventHandler_T
{
 public:
  Common_UI_Curses_EventHandler_T (CallbackDataType*); // callback data handle
  inline virtual ~Common_UI_Curses_EventHandler_T () {}

 protected:
  CallbackDataType* CBData_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_EventHandler_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_EventHandler_T (const Common_UI_Curses_EventHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_Curses_EventHandler_T& operator= (const Common_UI_Curses_EventHandler_T&))
};

// include template definition
#include "common_ui_curses_eventhandler.inl"

#endif
