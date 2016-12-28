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

#ifndef COMMON_UI_GTK_MANAGER_COMMON_H
#define COMMON_UI_GTK_MANAGER_COMMON_H

#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

#include "common_ui_exports.h"
#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager.h"

// *IMPORTANT NOTE*: these type definitions are instantiated in
//                   common_ui_gtk_manager.cpp
//#if defined (GTKGL_SUPPORT)
//typedef ACE_Singleton<Common_UI_GTK_Manager_T<struct Common_UI_GTKGLState>,
//                      typename ACE_MT_SYNCH::RECURSIVE_MUTEX> COMMON_UI_GTKGL_MANAGER_SINGLETON;
//#endif
typedef ACE_Singleton<Common_UI_GTK_Manager_T<struct Common_UI_GTKState>,
                      typename ACE_MT_SYNCH::RECURSIVE_MUTEX> COMMON_UI_GTK_MANAGER_SINGLETON;

#endif
