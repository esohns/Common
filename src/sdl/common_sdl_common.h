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

#ifndef COMMON_SDL_COMMON_H
#define COMMON_SDL_COMMON_H

#include "common_sdl_defines.h"

struct Common_SDL_VideoConfiguration
{
  Common_SDL_VideoConfiguration ()
   : screen_width (1024)
   , screen_height (768)
   , screen_colordepth (32)
   , double_buffer (true)
   , use_OpenGL (false)
   , full_screen (false)
   , video_driver (ACE_TEXT_ALWAYS_CHAR (COMMON_SDL_VIDEO_DEFAULT_DRIVER_NAME))
#if defined (_DEBUG)
   , debug (false)
#endif // _DEBUG
  {}

  int         screen_width;
  int         screen_height;
  int         screen_colordepth; // bits/pixel
  //Uint32      screen_flags;
  bool        double_buffer;
  bool        use_OpenGL;
  bool        full_screen;
  std::string video_driver; // [wayland|x11|directx|windib|...]
#if defined (_DEBUG)
  bool        debug;
#endif // _DEBUG
};

#endif
