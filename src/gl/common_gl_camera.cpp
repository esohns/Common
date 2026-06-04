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
 : homePosition_ (0.0f, 0.0f, 0.0f)
 , position_ (0.0f, 0.0f, 0.0f)
 , looking_at_ (0.0f, 0.0f, 0.0f)
 , up_ (0.0f, 1.0f, 0.0f)
 , right_ (1.0f, 0.0f, 0.0f)
 , old_mouse_position_ (0.0f, 0.0f)
 , yaw_ (0.0f)
 , pitch_ (0.0f)
 , zoom_ (COMMON_GL_CAMERA_DEFAULT_FOV_DEG_F) // FOV in degrees
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::Common_GL_Camera"));

}

void
Common_GL_Camera::reset ()
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::reset"));

  homePosition_ = {0.0f, 0.0f, 0.0f};
  position_ = {0.0f, 0.0f, 0.0f};
  looking_at_ = {0.0f, 0.0f, 0.0f};
  up_ = {0.0f, 1.0f, 0.0f};
  right_ = {1.0f, 0.0f, 0.0f};
  yaw_ = 0.0f;
  pitch_ = 0.0f;
  zoom_ = COMMON_GL_CAMERA_DEFAULT_FOV_DEG_F; // FOV in degrees
}

void
Common_GL_Camera::updateDirection (float dx, float dy)
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::updateDirection"));

  yaw_ += dx;
  pitch_ += dy;

  if (pitch_ > COMMON_GL_CAMERA_DEFAULT_MAX_PITCH_DEG_F)
    pitch_ = COMMON_GL_CAMERA_DEFAULT_MAX_PITCH_DEG_F;
  else if (pitch_ < -COMMON_GL_CAMERA_DEFAULT_MAX_PITCH_DEG_F)
    pitch_ = -COMMON_GL_CAMERA_DEFAULT_MAX_PITCH_DEG_F;

  updateVectors ();
}

void
Common_GL_Camera::updatePosition (enum Common_GL_Camera::Direction direction_in,
                                  float dt)
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::updatePosition"));

  float velocity = dt * COMMON_GL_CAMERA_DEFAULT_SPEED_F;
#if defined (GLM_SUPPORT)
  glm::vec3 eye_target = looking_at_ - position_;
#else
  struct Common_GL_VectorF3 eye_target = {looking_at_.x - position_.x,
                                          looking_at_.y - position_.y,
                                          looking_at_.z - position_.z};
  float length_f =
    std::sqrt (eye_target.x * eye_target.x + eye_target.y * eye_target.y + eye_target.z * eye_target.z);
#endif // GLM_SUPPORT

  switch (direction_in)
  {
    case Common_GL_Camera::Direction::FORWARD:
      position_ += eye_target * velocity;
      break;
    case Common_GL_Camera::Direction::BACKWARD:
      position_ -= eye_target * velocity;
      break;
    case Common_GL_Camera::Direction::RIGHT:
#if defined (GLM_SUPPORT)
      position_ += glm::length (eye_target) * right_ * velocity;
#else
      position_ += length_f * right_ * velocity;
#endif // GLM_SUPPORT
      break;
    case Common_GL_Camera::Direction::LEFT:
#if defined (GLM_SUPPORT)
      position_ -= glm::length (eye_target) * right_ * velocity;
#else
      position_ -= length_f * right_ * velocity;
#endif // GLM_SUPPORT
      break;
    case Common_GL_Camera::Direction::UP:
#if defined (GLM_SUPPORT)
      position_ += glm::length (eye_target) * up_ * velocity;
#else
      position_ += length_f * up_ * velocity;
#endif // GLM_SUPPORT
      break;
    case Common_GL_Camera::Direction::DOWN:
#if defined (GLM_SUPPORT)
      position_ -= glm::length (eye_target) * up_ * velocity;
#else
      position_ -= length_f * up_ * velocity;
#endif // GLM_SUPPORT
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown direction (was: %d), continuing\n"),
                  direction_in));
      ACE_ASSERT (false);
      break;
    }
  } // end SWITCH
}

void
Common_GL_Camera::updateZoom (float dz)
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::updateZoom"));

  if (zoom_ >= 1.0f && zoom_ <= 45.0f)
    zoom_ -= dz * COMMON_GL_CAMERA_DEFAULT_SPEED_F;
  else if (zoom_ < 1.0f)
    zoom_ = 1.0f;
  else // > 45.0f
    zoom_ = 45.0f;
}

void
Common_GL_Camera::updateVectors ()
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::updateVectors"));

#if defined (GLM_SUPPORT)
  glm::vec3 direction;
  direction.x = std::cos (glm::radians (yaw_)) * std::cos (glm::radians (pitch_));
  direction.y = std::sin (glm::radians (pitch_));
  direction.z = std::sin (glm::radians (yaw_)) * std::cos (glm::radians (pitch_));
  looking_at_ = glm::normalize (direction);

  right_ = glm::normalize (glm::cross (looking_at_, {0.0f, 1.0f, 0.0f}));
  up_ = glm::normalize (glm::cross (right_, looking_at_));
#else
  ACE_ASSERT (false); // *TODO*
#endif // GLM_SUPPORT
}

void
Common_GL_Camera::mouseLook (int mouseX_in, int mouseY_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_GL_Camera::mouseLook"));

  static bool first_b = true;
#if defined (GLM_SUPPORT)
  glm::vec2 current_mouse_position (mouseX_in, mouseY_in);

  if (first_b)
  { first_b = false;
    homePosition_ = position_;
    old_mouse_position_ = current_mouse_position;
  } // end IF

  glm::vec2 mouse_delta = old_mouse_position_ - current_mouse_position;
  mouse_delta *= COMMON_GL_CAMERA_DEFAULT_MOUSE_LOOK_FACTOR_F;
  position_ = glm::rotate (position_,
                           glm::radians (mouse_delta.x),
                           up_);
  position_ = glm::rotate (position_,
                           glm::radians (mouse_delta.y),
                           right_);

  old_mouse_position_ = current_mouse_position;
#else
  ACE_ASSERT (false); // *TODO*
#endif // GLM_SUPPORT
}
