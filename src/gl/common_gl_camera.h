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

#ifndef COMMON_GL_CAMERA_H
#define COMMON_GL_CAMERA_H

#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#endif // GLM_SUPPORT

#include "common_gl_common.h"
#include "common_gl_defines.h"

class Common_GL_Camera
{
 public:
  Common_GL_Camera ();
  inline ~Common_GL_Camera () {}

  void reset ();

  glm::vec3 position;
  glm::vec3 looking_at;
  glm::vec3 up;
  float zoom;
  glm::vec2 old_mouse_position;

  inline glm::mat4 getViewMatrix () { return glm::lookAt (position, position + looking_at, up); }

  inline void forward (float speed_in)
  {
    position += looking_at * (speed_in * COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR);
  }
  inline void backward (float speed_in)
  {
    position -= looking_at * (speed_in * COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR);
  }
  void left (float speed_in)
  {
    glm::vec3 right = glm::cross (looking_at, up);
    position +=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }
  void right (float speed_in)
  {
    glm::vec3 right = glm::cross (looking_at, up);
    position -=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }

  void mouseLook (int, int);
};

#endif
