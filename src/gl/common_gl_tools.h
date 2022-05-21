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

#ifndef COMMON_GL_TOOLS_H
#define COMMON_GL_TOOLS_H

#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#endif // GLM_SUPPORT

#include "ace/Global_Macros.h"

#include "common_gl_common.h"

class Common_GL_Tools
{
 public:
  // debug
  static std::string errorToString (GLenum);

  // color
  static Common_GL_Color_t lerpRGB (const Common_GL_Color_t&, // color 1
                                    const Common_GL_Color_t&, // color 2
                                    float);                   // ratio [0.0..1.0]
  // transitions between red --> green --> blue
  static Common_GL_Color_t toRGBColor (float); // [0.0..1.0]

  // model
  static GLuint loadModel (const std::string&,          // path
                           Common_GL_BoundingBox_t&,    // return value: bounding box
#if defined (GLM_SUPPORT)
                           glm::vec3&);                 // return value: center
#else
                           struct Common_GL_VectorF3&); // return value: center
#endif // GLM_SUPPORT

  // texture
  // *NOTE*: invoke glTexImage2D() with 'target' GL_TEXTURE_2D and 'internal
  //         format' GL_RGBA8
  // *TODO*: currently, only PNG files are supported
  static GLuint loadTexture (const std::string&); // path
  // *NOTE*: invoke glTexImage2D() with 'target' GL_TEXTURE_2D and 'internal
  //         format' GL_RGBA8, format GL_RGBA
  static void loadTexture (const uint8_t*, // data
                           unsigned int,   // width
                           unsigned int,   // height
                           GLuint);        // texture id

  // drawing
  // *NOTE*: includes glBegin()/glEnd() calls
  static void drawCube (bool = false); // set (2d-)texture coordinates ?

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools (const Common_GL_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools& operator= (const Common_GL_Tools&))

  // *IMPORTANT NOTE*: callers need to free() allocated memory (5th argument)
  static bool loadPNG (const std::string&, // path
                       unsigned int&,      // return value: width
                       unsigned int&,      // return value: height
                       bool&,              // return value: has alpha channel ?
                       GLubyte*&);         // return value: data
};

#endif
