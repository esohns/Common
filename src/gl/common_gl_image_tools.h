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

#ifndef COMMON_GL_IMAGE_TOOLS_H
#define COMMON_GL_IMAGE_TOOLS_H

#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

class Common_GL_Image_Tools
{
 public:
#if defined (LIBPNG_SUPPORT)
  // *IMPORTANT NOTE*: callers need to free() allocated memory (5th argument)
  static bool loadPNG (const std::string&, // path
                       unsigned int&,      // return value: width
                       unsigned int&,      // return value: height
                       bool&,              // return value: has alpha channel ?
                       GLubyte*&);         // return value: data
#endif // LIBPNG_SUPPORT

#if defined (IMAGEMAGICK_SUPPORT)
  // *IMPORTANT NOTE*: callers need to free() allocated memory (4th argument)
  static bool loadPNG (const std::string&, // path
                       unsigned int&,      // return value: width
                       unsigned int&,      // return value: height
                       GLubyte*&);         // return value: data
#endif // IMAGEMAGICK_SUPPORT

#if defined (STB_IMAGE_SUPPORT)
  // *IMPORTANT NOTE*: callers need to free() allocated memory (6th argument)
  static bool loadSTB (const std::string&, // path
                       bool,               // flip image on load ? (required for OpenGL)
                       unsigned int&,      // return value: width
                       unsigned int&,      // return value: height
                       unsigned int&,      // return value: #channels
                       GLubyte*&);         // return value: data
  static bool saveSTB (const std::string&, // path
                       unsigned int,       // width
                       unsigned int,       // height
                       unsigned int,       // #channels
                       const GLubyte*);    // data
#endif // STB_IMAGE_SUPPORT

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_GL_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Image_Tools (const Common_GL_Image_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Image_Tools& operator= (const Common_GL_Image_Tools&))
};

#endif
