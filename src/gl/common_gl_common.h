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

#ifndef COMMON_GL_COMMON_H
#define COMMON_GL_COMMON_H

#include <utility>

#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#endif // GLM_SUPPORT

#include "common_gl_defines.h"

// forward declaration
class Common_GL_Camera;

#if defined (GLM_SUPPORT)
typedef std::pair<glm::vec3, glm::vec3> Common_GL_BoundingBox_t;
#else
struct Common_GL_VectorF2
{
  Common_GL_VectorF2 ()
   : x (0.0f)
   , y (0.0f)
  {}

  Common_GL_VectorF2 (float x_in, float y_in)
   : x (x_in)
   , y (y_in)
  {}

  float x;
  float y;
};

struct Common_GL_VectorF3
{
  Common_GL_VectorF3 ()
   : x (0.0f)
   , y (0.0f)
   , z (0.0f)
  {}

  Common_GL_VectorF3 (float x_in, float y_in, float z_in)
   : x (x_in)
   , y (y_in)
   , z (z_in)
  {}

  float x;
  float y;
  float z;
};
typedef std::pair<struct Common_GL_VectorF3, struct Common_GL_VectorF3> Common_GL_BoundingBox_t;
#endif // GLM_SUPPORT

#if defined (GLM_SUPPORT)
typedef glm::u8vec3 Common_GL_Color_t;
#else
struct Common_GL_Color
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
typedef struct Common_GL_Color Common_GL_Color_t;
#endif // GLM_SUPPORT

struct Common_GL_Scene
{
  Common_GL_Scene ()
   : camera ()
   , boundingBox ()
   , center ()
   , orientation ()
  {}

  Common_GL_Camera*         camera;

  // scene
  Common_GL_BoundingBox_t   boundingBox;
#if defined (GLM_SUPPORT)
  glm::vec3                 center;
  glm::vec3                 orientation; // model/scene-
#else
  struct Common_GL_VectorF3 center;
  struct Common_GL_VectorF3 orientation; // model/scene-
#endif // GLM_SUPPORT
};

#endif
