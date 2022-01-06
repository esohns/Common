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

#include "gtk/gtk.h"

#if GTK_CHECK_VERSION(2,32,0)
#else
#define G_SOURCE_CONTINUE                                       TRUE
#define G_SOURCE_REMOVE                                         FALSE
#endif // GTK_CHECK_VERSION(2,32,0)

// refresh rates / timeouts
#define COMMON_UI_GTK_REFRESH_DEFAULT_CAIRO_MS                  COMMON_UI_REFRESH_DEFAULT_VIDEO_MS
#define COMMON_UI_GTK_REFRESH_DEFAULT_OPENGL_MS                 COMMON_UI_REFRESH_DEFAULT_VIDEO_MS

#define COMMON_UI_GTK_TIMEOUT_DEFAULT_MANAGER_INITIALIZATION_MS 100 // ms
#define COMMON_UI_GTK_TIMEOUT_DEFAULT_WIDGET_TOOLTIP_DELAY_MS   100 // ms

// application
#define COMMON_UI_GTK_APPLICATION_ID_DEFAULT                    "application_id"

// widgets
#define COMMON_UI_GTK_DEFINITION_WIDGET_MAIN                    "dialog_main"

#endif
