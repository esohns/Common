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

#ifndef COMMON_UI_GTK_DEFINES_H
#define COMMON_UI_GTK_DEFINES_H

#include "ace/config-lite.h"

// *IMPORTANT NOTE*: for some reason, the glib 2 Win32 package does not define
//                   these
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define G_SOURCE_CONTINUE                            TRUE
#define G_SOURCE_REMOVE                              FALSE
#endif

// interface definition
#define COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN     "main"

// timeouts / update intervals
#define COMMON_UI_GTK_TIMEOUT_DEFAULT_INITIALIZATION 100 // ms
#define COMMON_UI_GTK_UPDATE_OPENGL_INTERVAL         33 // ms --> ~30 fps
#define COMMON_UI_GTK_UPDATE_PROGRESSBAR_INTERVAL    33 // ms --> ~30 fps
#define COMMON_UI_GTK_UPDATE_WIDGET_INTERVAL         200 // ms --> 5 fps

#endif
