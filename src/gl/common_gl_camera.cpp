#include "stdafx.h"

#include "common_gl_camera.h"

#if defined (GLM_SUPPORT)
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#endif // GLM_SUPPORT

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_gl_defines.h"

Common_GL_Camera::Common_GL_Camera ()
 : position_ (0.0f, 0.0f, 0.0f)
 , looking_at_ (0.0f, 0.0f, -1.0f)
 , up_ (0.0f, 1.0f, 0.0f)
 , right_ (1.0f, 0.0f, 0.0f)
 , yaw_ (0.0f)
 , pitch_ (0.0f)
 , zoom_ (45.0f) // fov
 , old_mouse_position_ (0.0f, 0.0f)
{

}

void
Common_GL_Camera::reset ()
{
  position_ = {0.0f, 0.0f, 0.0f};
  looking_at_ = {0.0f, 0.0f, -1.0f};
  up_ = {0.0f, 1.0f, 0.0f};
  right_ = {1.0f, 0.0f, 0.0f};
  yaw_ = 0.0f;
  pitch_ = 0.0f;
  zoom_ = 0.0f;
}

void
Common_GL_Camera::updateDirection (float dx, float dy)
{
  yaw_ += dx;
  pitch_ += dy;

  if (pitch_ > 89.0f)
    pitch_ = 89.0f;
  else if (pitch_ < -89.0f)
    pitch_ = -89.0f;

  updateVectors ();
}

void
Common_GL_Camera::updatePosition (enum Common_GL_CameraDirection direction,
                                  float dt)
{
  float velocity = dt * COMMON_GL_CAMERA_DEFAULT_SPEED;

  switch (direction)
  {
    case Common_GL_CameraDirection::FORWARD:
      position_ += looking_at_ * velocity;
      break;
    case Common_GL_CameraDirection::BACKWARD:
      position_ -= looking_at_ * velocity;
      break;
    case Common_GL_CameraDirection::RIGHT:
      position_ += right_ * velocity;
      break;
    case Common_GL_CameraDirection::LEFT:
      position_ -= right_ * velocity;
      break;
    case Common_GL_CameraDirection::UP:
      position_ += up_ * velocity;
      break;
    case Common_GL_CameraDirection::DOWN:
      position_ -= up_ * velocity;
      break;
    default:
      ACE_ASSERT (false);
      break;
  } // end SWITCH
}

void
Common_GL_Camera::updateZoom (float dz)
{
  if (zoom_ >= 1.0f && zoom_ <= 45.0f)
    zoom_ -= dz * COMMON_GL_CAMERA_DEFAULT_SPEED;
  else if (zoom_ < 1.0f)
    zoom_ = 1.0f;
  else // > 45.0f
    zoom_ = 45.0f;
}

void
Common_GL_Camera::updateVectors ()
{
  glm::vec3 direction;
  direction.x = std::cos (glm::radians (yaw_)) * std::cos (glm::radians (pitch_));
  direction.y = std::sin (glm::radians (pitch_));
  direction.z = std::sin (glm::radians (yaw_)) * std::cos (glm::radians (pitch_));
  looking_at_ = glm::normalize (direction);

  right_ = glm::normalize (glm::cross (looking_at_, {0.0f, 1.0f, 0.0f}));
  up_ = glm::normalize (glm::cross (right_, looking_at_));
}

void
Common_GL_Camera::mouseLook (int mouseX_in, int mouseY_in)
{
  glm::vec2 current_mouse_position (mouseX_in, mouseY_in);

  static bool first_b = true;
  if (first_b)
  {
    old_mouse_position_ = current_mouse_position;
    first_b = false;
  } // end IF

  glm::vec2 mouse_delta = old_mouse_position_ - current_mouse_position;
  mouse_delta *= COMMON_GL_CAMERA_DEFAULT_MOUSE_LOOK_FACTOR;
  looking_at_ = glm::rotate (looking_at_, glm::radians (mouse_delta.x), up_);

  old_mouse_position_ = current_mouse_position;
}
