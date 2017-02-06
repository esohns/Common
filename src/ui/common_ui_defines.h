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

#ifndef COMMON_UI_DEFINES_H
#define COMMON_UI_DEFINES_H

#include <ace/config-lite.h>

// window
#define COMMON_UI_WINDOW_DEFAULT_WIDTH                            320
#define COMMON_UI_WINDOW_DEFAULT_HEIGHT                           240

// ************************************ SDL ************************************

// event dispatch
#define COMMON_UI_SDL_EVENT_TIMER                                 SDL_USEREVENT
// *NOTE*: this defines the input resolution or "interactivity"
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define COMMON_UI_SDL_EVENT_TIMEOUT                               100 // ms
//// *NOTE*: sensible values are multiples of COMMON_UI_SDL_EVENT_TIMEOUT, as
//// that is the resolution factor
//#define COMMON_UI_SDL_EVENT_RESOLUTION           100 // ms

// video
#define COMMON_UI_SDL_VIDEO_DEFAULT_BPP                           32
#define COMMON_UI_SDL_VIDEO_DEFAULT_DOUBLEBUFFER                  false
#define COMMON_UI_SDL_VIDEO_DEFAULT_OPENGL                        false
#define COMMON_UI_SDL_VIDEO_DEFAULT_FULLSCREEN                    false
#define COMMON_UI_SDL_VIDEO_DEFAULT_INIT                          false
//#define COMMON_UI_SDL_VIDEO_DEFAULT_FRAMERATE    30
#define COMMON_UI_SDL_VIDEO_SCREENSHOT_PREFIX                     "screenshot"
#define COMMON_UI_SDL_VIDEO_SCREENSHOT_EXT                        ".png"

// text
#define COMMON_UI_SDL_TEXT_FONT                                   FONT_MAIN_SMALL
#define COMMON_UI_SDL_TEXT_COLOR                                  COLOR_WHITE
#define COMMON_UI_SDL_TEXT_SHADECOLOR                             COLOR_BLACK

// ************************************ GTK ************************************

// *IMPORTANT NOTE*: for some reason, the glib 2 windows package does not define
//                   these
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define G_SOURCE_CONTINUE                                         TRUE
#define G_SOURCE_REMOVE                                           FALSE
#endif

// interface definition
#define COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN                  "main"

// event dispatch
#define COMMON_UI_GTK_THREAD_GROUP_ID                             200
#define COMMON_UI_GTK_THREAD_NAME                                 "UI dispatch"

// informative widget(s)/(log) text views, ...
#define COMMON_UI_GTK_WIDGET_UPDATE_INTERVAL                      200 // ms --> 5 fps
#define COMMON_UI_GTK_PROGRESSBAR_UPDATE_INTERVAL                 27 // ms (?)

#endif
