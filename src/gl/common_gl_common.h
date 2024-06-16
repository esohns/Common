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
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#endif // GLM_SUPPORT

#include "common_gl_defines.h"

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

struct Common_GL_Camera
{
  Common_GL_Camera ()
#if defined (GLUT_SUPPORT)
   : position (0.0f, 0.0f, 0.0f)
   , looking_at (0.0f, 0.0f, -1.0f)
   , up (0.0f, 1.0f, 0.0f)
   , old_mouse_position (0.0f, 0.0f)
#else
   : zoom (0.0f)
   , rotation (0.0f, 0.0f, 0.0f)
   , translation (0.0f, 0.0f, 0.0f)
#endif // GLUT_SUPPORT
  {}

#if defined (GLUT_SUPPORT)
  void reset ()
  {
    position = {0.0f, 0.0f, 0.0f};
    looking_at = {0.0f, 0.0f, -1.0f};
    up = {0.0f, 1.0f, 0.0f};

    //old_mouse_position = {0.0f, 0.0f};
  }

#if defined (GLM_SUPPORT)
  glm::vec3 position;
  glm::vec3 looking_at;
  glm::vec3 up;

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
    position -=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }
  void right (float speed_in)
  {
    glm::vec3 right = glm::cross (looking_at, up);
    position +=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }

  glm::vec2 old_mouse_position;
  void mouseLook (int mouseX_in, int mouseY_in)
  {
    glm::vec2 current_mouse_position (mouseX_in, mouseY_in);

    static bool first_b = true;
    if (first_b)
    {
      old_mouse_position = current_mouse_position;
      first_b = false;
    } // end IF

    glm::vec2 mouse_delta = old_mouse_position - current_mouse_position;
    mouse_delta *= COMMON_GL_CAMERA_DEFAULT_MOUSE_LOOK_FACTOR;
    looking_at = glm::rotate (looking_at, glm::radians (mouse_delta.x), up);

    old_mouse_position = current_mouse_position;
  }
#else
  struct Common_GL_VectorF3 position;
  struct Common_GL_VectorF3 looking_at;
  struct Common_GL_VectorF3 up;

  struct Common_GL_VectorF2 old_mouse_position;
#endif // GLM_SUPPORT
#endif // GLUT_SUPPORT

#if defined (GLUT_SUPPORT)
#else
  void reset ()
  {
    zoom = COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR;
    rotation = {0.0f, 0.0f, 0.0f};
    translation = {0.0f, 0.0f, 0.0f};
  }

  float zoom;
  struct Common_GL_VectorF3 rotation;
  struct Common_GL_VectorF3 translation;
#endif // GLM_SUPPORT
};

struct Common_GL_Scene
{
  Common_GL_Scene ()
   : camera ()
   , boundingBox ()
   , center ()
   , orientation ()
  {}

  inline void resetCamera () { camera.reset (); }

  struct Common_GL_Camera   camera;

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
