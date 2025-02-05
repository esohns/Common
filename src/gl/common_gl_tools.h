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

#if defined (GLEW_SUPPORT)
#include "GL/glew.h"
#endif // GLEW_SUPPORT
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

  // lerp
  template <typename ValueType>
  static std::enable_if_t<std::is_integral<ValueType>::value, ValueType> map (ValueType,  // value
                                                                              ValueType,  // from start (inclusive)
                                                                              ValueType,  // from end   (inclusive)
                                                                              ValueType,  // to start (inclusive)
                                                                              ValueType); // to end   (inclusive)
  template <typename ValueType>
  static std::enable_if_t<!std::is_integral<ValueType>::value, ValueType> map (ValueType,  // value
                                                                               ValueType,  // from start (inclusive)
                                                                               ValueType,  // from end   (inclusive)
                                                                               ValueType,  // to start (inclusive)
                                                                               ValueType); // to end   (inclusive)

  // color
  static float brightness (const Common_GL_Color_t&); // returns [0.0f..1.0f]
  static Common_GL_Color_t lerpRGB (const Common_GL_Color_t&, // color 1
                                    const Common_GL_Color_t&, // color 2
                                    float);                   // ratio [0.0f..1.0f]
  // transitions between red --> green --> blue
  static Common_GL_Color_t toRGBColor (float); // [0.0..1.0]

  static Common_GL_Color_t randomColor ();

  // model
  // *MOTE*: returns a list id (OpenGL 2.x only)
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
  // *TODO*: currently, only PNG/JPG files are supported
  static GLuint loadTexture (const std::string&, // path
                             bool = true);       // flip image on load ? (required for OpenGL)
  // *NOTE*: invoke glTexImage2D() with 'target' GL_TEXTURE_2D and 'internal
  //         format' GL_RGBA8, format GL_RGBA (!)
  static void loadTexture (const uint8_t*, // data
                           unsigned int,   // width
                           unsigned int,   // height
                           unsigned int,   // depth (3||4)
                           GLuint,         // texture id
                           bool);          // initial ? : update

  // shader
  static bool loadAndCompileShaderFile (const std::string&, // path
                                        GLenum,             // shader type (i.e. GL_VERTEX_SHADER || GL_FRAGMENT_SHADER)
                                        GLuint&);           // return value: shader id
  static bool loadAndCompileShaderString (const std::string&, // shader code
                                          GLenum,             // shader type (i.e. GL_VERTEX_SHADER || GL_FRAGMENT_SHADER)
                                          GLuint&);           // return value: shader id

  // drawing (OpenGL 2.x only)
  // *NOTE*: includes glBegin()/glEnd() calls
  static void drawCube (bool = false, // set (2d-)texture coordinates ?
                        bool = true); // set color ?

  static void screenShot (const std::string&); // FQ path

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools (const Common_GL_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools& operator= (const Common_GL_Tools&))
};

// include template definition
#include "common_gl_tools.inl"

#endif
