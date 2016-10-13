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

#ifndef COMMON_IMAGE_TOOLS_H
#define COMMON_IMAGE_TOOLS_H

#include <string>

#include <ace/config-lite.h>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif

#include <ace/Global_Macros.h>

#include "common_image_exports.h"

class Common_Image_Export Common_Image_Tools
{
 public:
  inline virtual ~Common_Image_Tools () {};

  // *IMPORTANT NOTE*: callers need to free() the allocated memory
  static bool loadPNG2OpenGL (const std::string&, // string
                              unsigned int&,      // return value: width
                              unsigned int&,      // return value: height
                              bool&,              // return value: has alpha channel ?
                              GLubyte*&);         // return value: data

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools ());
  //ACE_UNIMPLEMENTED_FUNC (~Common_Image_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools (const Common_Image_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools& operator= (const Common_Image_Tools&));
};

#endif
