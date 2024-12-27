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

#include "ace/Assert.h"
// #include "ace/Global_Macros.h"

#include "common_gl_common.h"
#include "common_gl_defines.h"

class Common_GL_Camera
{
 public:
  enum Direction
  {
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
  };

  Common_GL_Camera ();
  inline ~Common_GL_Camera () {}

  void reset ();

  // *TODO*: to be tested
  void updatePosition (enum Common_GL_Camera::Direction, // direction
                       float);                           // dt
  void updateDirection (float, float);
  void updateZoom (float); // dz

#if defined (GLM_SUPPORT)
  glm::vec3 position_;
  glm::vec3 looking_at_;
  glm::vec3 up_;
  glm::vec3 right_;
#endif // GLM_SUPPORT
  float yaw_; // degrees
  float pitch_; // degrees
  float zoom_;  // *TODO*: use for FOV [1.0f, 45.0f]

#if defined (GLM_SUPPORT)
  glm::vec2 old_mouse_position_;

  // *TODO*: is this an alternative to glm::lookAt() ?
  inline static glm::mat4 getViewMatrix (const glm::vec3& position_in,
                                         const glm::vec3& forward_in,
                                         const glm::vec3& up_in)
  {
    glm::vec3 right = glm::normalize (glm::cross (forward_in, up_in));
    glm::vec3 up = glm::normalize (glm::cross (right, forward_in));
    glm::mat4 result = {{right.x, up.x, -forward_in.x, 0.0f},
                        {right.y, up.y, -forward_in.y, 0.0f},
                        {right.z, up.z, -forward_in.z, 0.0f},
                        {-glm::dot (right, position_in), -glm::dot (up, position_in), glm::dot (forward_in, position_in), 1.0f}};
    return result;
  }

  inline glm::mat4 getViewMatrix () { return glm::lookAt (position_, position_ + looking_at_, up_); }
  inline static glm::mat4 getProjectionMatrix (int width_in, int height_in,
                                               float FOV_in, // degrees
                                               float zNear_in, float zFar_in)
  { ACE_ASSERT (height_in);
    return glm::perspective (glm::radians (FOV_in),
                             (float)width_in / height_in,
                             zNear_in, zFar_in);
  }
#endif // GLM_SUPPORT

#if defined (GLM_SUPPORT)
  inline void forward (float speed_in)
  {
    position_ += looking_at_ * (speed_in * COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR);
  }
  inline void backward (float speed_in)
  {
    position_ -= looking_at_ * (speed_in * COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR);
  }
  void left (float speed_in)
  {
    glm::vec3 right = glm::cross (looking_at_, up_);
    position_ +=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }
  void right (float speed_in)
  {
    glm::vec3 right = glm::cross (looking_at_, up_);
    position_ -=
      right * (speed_in * COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR);
  }
#endif // GLM_SUPPORT

  void mouseLook (int, int);

 private:
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Camera (const Common_GL_Camera&))

  void updateVectors ();
};

#endif
