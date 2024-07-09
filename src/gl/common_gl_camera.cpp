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
 : position (0.0f, 0.0f, 0.0f)
 , looking_at (0.0f, 0.0f, -1.0f)
 , up (0.0f, 1.0f, 0.0f)
 , zoom (0.0f) // fov
 , old_mouse_position (0.0f, 0.0f)
{

}

void
Common_GL_Camera::reset ()
{
  position = {0.0f, 0.0f, 0.0f};
  looking_at = {0.0f, 0.0f, -1.0f};
  up = {0.0f, 1.0f, 0.0f};
  zoom = 0.0f;
}

void
Common_GL_Camera::mouseLook (int mouseX_in, int mouseY_in)
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
