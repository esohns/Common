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

#ifndef TEST_I_OPENGL_COMMON_H
#define TEST_I_OPENGL_COMMON_H

#if defined (GLEW_SUPPORT)
#include "gl/glew.h"
#endif // GLEW_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#include "gl/GLU.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_gl_common.h"
#include "common_gl_shader.h"
#include "common_gl_texture.h"

#include "test_i_opengl_defines.h"

struct Common_OpenGL_GLUT_CBData
{
  Common_OpenGL_GLUT_CBData ()
   : width (static_cast<float> (TEST_I_OPENGL_DEFAULT_WINDOW_WIDTH))
   , height (static_cast<float> (TEST_I_OPENGL_DEFAULT_WINDOW_HEIGHT))
   , mouseButton0IsDown (false)
   , camera ()
   , shader ()
   , texture ()
   , wireframe (false)
   , VBO (0)
   , VAO (0)
   , EBO (0)
  {}

  GLfloat                 width;
  GLfloat                 height;

  bool                    mouseButton0IsDown;

  struct Common_GL_Camera camera;
  Common_GL_Shader        shader;
  Common_GL_Texture       texture;

  bool                    wireframe;

  GLuint                  VBO;
  GLuint                  VAO;
  GLuint                  EBO;
};

#endif
