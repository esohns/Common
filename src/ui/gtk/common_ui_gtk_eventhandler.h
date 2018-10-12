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

// forward declarations
struct Common_UI_GTK_EventHookConfiguration;

template <typename CallbackDataType>
class Comon_UI_GTK_EventHandler_T
{
 public:
  Comon_UI_GTK_EventHandler_T (struct Common_UI_GTK_EventHookConfiguration*,
                               CallbackDataType*); // callback data handle
  inline virtual ~Comon_UI_GTK_EventHandler_T () {}

 protected:
  struct Common_UI_GTK_EventConfiguration* hooks_;
  CallbackDataType*                        CBData_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_GTK_EventHandler_T ())
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_GTK_EventHandler_T (const Comon_UI_GTK_EventHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_GTK_EventHandler_T& operator= (const Comon_UI_GTK_EventHandler_T&))
};

// include template definition
#include "common_ui_gtk_eventhandler.inl"

#endif
