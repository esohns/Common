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

// event dispatch
#define COMMON_UI_EVENT_THREAD_GROUP_ID                           200
#define COMMON_UI_EVENT_THREAD_NAME                               "UI dispatch"

// refresh rates
#define COMMON_UI_REFRESH_DEFAULT_PROGRESS                        50  // ms --> 20 fps
#define COMMON_UI_REFRESH_DEFAULT_VIDEO                           33  // ms --> ~30 fps
#define COMMON_UI_REFRESH_DEFAULT_WIDGET                          200 // ms --> 5 fps

// window
#define COMMON_UI_WINDOW_DEFAULT_WIDTH                            320
#define COMMON_UI_WINDOW_DEFAULT_HEIGHT                           240

// interface definition
#define COMMON_UI_DEFINITION_DESCRIPTOR_MAIN                      "main"

// ************************************ SDL ************************************

// event dispatch
#define COMMON_UI_SDL_EVENT_TIMER                                 SDL_USEREVENT
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define COMMON_UI_SDL_EVENT_MAX_TIMER_RESOLUTION                  10 // ms
// *NOTE*: defines the input event resolution, or UI 'interactivity'
// *NOTE*: sensible values are multiples of
//         COMMON_UI_SDL_EVENT_MAX_TIMER_RESOLUTION, as that is the resolution
//         factor (see above)
#define COMMON_UI_SDL_EVENT_TIMEOUT                               100 // ms

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

#endif
