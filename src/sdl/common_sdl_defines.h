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

#ifndef COMMON_SDL_DEFINES_H
#define COMMON_SDL_DEFINES_H

// event dispatch
#define COMMON_SDL_EVENT_TIMER                SDL_USEREVENT
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define COMMON_SDL_EVENT_MAX_TIMER_RESOLUTION 10 // ms
// *NOTE*: defines the input event resolution, or UI 'interactivity'
// *NOTE*: sensible values are multiples of
//         COMMON_SDL_EVENT_MAX_TIMER_RESOLUTION, as that is the resolution
//         factor (see above)
#define COMMON_SDL_EVENT_TIMEOUT              100 // ms

// video
#if defined(SDL_USE)
#define COMMON_SDL_VIDEO_DRIVER_ENV_VAR       "SDL_VIDEODRIVER"
#elif defined (SDL2_USE) || defined (SDL3_USE)
#define COMMON_SDL_VIDEO_DRIVER_ENV_VAR       "SDL_VIDEO_DRIVER"
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (SDL_USE)
#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME  "windib"
#elif defined (SDL2_USE)
#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME  "windows"
#endif // SDL_USE || SDL2_USE
#elif defined (ACE_LINUX)
#if defined (SDL_USE)
#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME  "x11"
#elif defined (SDL2_USE) || defined (SDL3_USE)
#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME  "x11"
//#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME              "wayland"
#endif // SDL_USE || SDL2_USE || SDL3_USE
#else
#define COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME  ""
#endif // ACE_WINXX || ACE_LINUX

#define COMMON_SDL_VIDEO_DEFAULT_BPP          32
#define COMMON_SDL_VIDEO_DEFAULT_DOUBLEBUFFER false
#define COMMON_SDL_VIDEO_DEFAULT_OPENGL       false
#define COMMON_SDL_VIDEO_DEFAULT_FULLSCREEN   false
#define COMMON_SDL_VIDEO_DEFAULT_INIT         false
//#define COMMON_SDL_VIDEO_DEFAULT_FRAMERATE    30
#define COMMON_SDL_VIDEO_SCREENSHOT_PREFIX    "screenshot"
#define COMMON_SDL_VIDEO_SCREENSHOT_EXT       ".png"

// text
#define COMMON_SDL_TEXT_FONT                  FONT_MAIN_SMALL
#define COMMON_SDL_TEXT_COLOR                 COLOR_WHITE
#define COMMON_SDL_TEXT_SHADECOLOR            COLOR_BLACK
// buffer for printing keyboard events (debug info)
#define COMMON_SDL_KEYSYM_BUFFER_SIZE         32

#endif
