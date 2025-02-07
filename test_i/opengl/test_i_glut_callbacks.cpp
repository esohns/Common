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
#include "stdafx.h"

#include "test_i_glut_callbacks.h"

#include <sstream>

#if defined (GLEW_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/glew.h"
#else
#include "GL/glew.h"
#endif // ACE_WIN32 || ACE_WIN64
#endif // GLEW_SUPPORT
#if defined (GLUT_SUPPORT)
#include "GL/freeglut.h"
#endif // GLUT_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#include "gl/GLU.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Synch_Traits.h"

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_gl_defines.h"
#include "common_gl_texture.h"
#include "common_gl_tools.h"

#include "common_timer_manager.h"

#include "test_i_opengl_common.h"

void
test_i_opengl_glut_draw ()
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  // sanity check(s)
  ACE_ASSERT (cb_data_p->VAO && cb_data_p->texture.id_);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  COMMON_GL_ASSERT;

  glBindVertexArray (cb_data_p->VAO);
  COMMON_GL_ASSERT;

  glActiveTexture (GL_TEXTURE0);
  cb_data_p->texture.bind (); // texture unit

  static float angle_f = 0.0f; // *NOTE*: degrees
  static glm::vec3 rotation_s (0.0f, 0.0f, 1.0f);

#if defined (GLM_SUPPORT)
  glm::mat4 model_matrix = glm::mat4 (1.0f); // make sure to initialize matrix to identity matrix first
  model_matrix = glm::translate (model_matrix,
                                 glm::vec3 (0.0f, 0.0f, -3.0f));
  model_matrix = glm::rotate (model_matrix,
                              glm::radians (angle_f),
                              rotation_s);

  glm::mat4 view_matrix = cb_data_p->camera.getViewMatrix ();

  glm::mat4 projection_matrix =
    glm::perspective (glm::radians (TEST_I_OPENGL_DEFAULT_FOV),
#if defined (ACE_WIN32) || defined (ACE_WIN32)
                      (float)cb_data_p->perspective.resolution.cx / cb_data_p->perspective.resolution.cy,
#else
                      (float)cb_data_p->perspective.resolution.width / cb_data_p->perspective.resolution.height,
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
                      TEST_I_OPENGL_DEFAULT_ZNEAR, TEST_I_OPENGL_DEFAULT_ZFAR);
#endif // GLM_SUPPORT

  // compute elapsed time
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::chrono::steady_clock::time_point tp2 =
    std::chrono::high_resolution_clock::now ();
#elif defined (ACE_LINUX)
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp2 =
    std::chrono::high_resolution_clock::now ();
#else
#error missing implementation, aborting
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
  std::chrono::duration<float> elapsed_time = tp2 - cb_data_p->tp1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::chrono::steady_clock::time_point tp_last = tp2;
#elif defined (ACE_LINUX)
  static std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp_last = tp2;
#else
#error missing implementation, aborting
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
  std::chrono::duration<float> elapsed_time_2 = tp2 - tp_last;
  static float duration_f = 0.0f;
  duration_f += elapsed_time_2.count ();
  tp_last = tp2;

  cb_data_p->shader.use ();
#if defined (GLM_SUPPORT)
  cb_data_p->shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("model"), model_matrix);
  cb_data_p->shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("view"), view_matrix);
  cb_data_p->shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("projection"), projection_matrix);
#endif // GLM_SUPPORT
  cb_data_p->shader.setInt (ACE_TEXT_ALWAYS_CHAR ("texture1"), 0); // *IMPORTANT NOTE*: <-- texture unit (!) not -id
  cb_data_p->shader.setFloat (ACE_TEXT_ALWAYS_CHAR ("time"), elapsed_time.count ());

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, cb_data_p->EBO);
  COMMON_GL_ASSERT;

  glDisable (GL_DEPTH_TEST);
  COMMON_GL_ASSERT;
  glDrawElements (GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_BYTE, (void*)0); // see: cube_indices
  COMMON_GL_ASSERT;
  glEnable (GL_DEPTH_TEST);
  COMMON_GL_ASSERT;

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;

  cb_data_p->texture.unbind ();

  glBindVertexArray (0);
  COMMON_GL_ASSERT;

  // "smooth" (random-) rotation
#define TRANSITION_DURATION_F 2.0f // second(s)
  ACE_ASSERT (TRANSITION_DURATION_F != 0.0f); // would divide by 0 (see below)
  static glm::vec3 rotation_from (0.0f, 0.0f, 1.0f);
  static glm::vec3 rotation_to (1.0f, 1.0f, 1.0f);
  if (duration_f >= TRANSITION_DURATION_F)
  {
    duration_f -= TRANSITION_DURATION_F;

    rotation_from = rotation_s;
    rotation_to.x = Common_Tools::getRandomNumber (0.0f, 1.0f);
    rotation_to.y = Common_Tools::getRandomNumber (0.0f, 1.0f);
    rotation_to.z = Common_Tools::getRandomNumber (0.0f, 1.0f);
    rotation_to = glm::normalize (rotation_to);
  } // end IF
  rotation_s = glm::mix (rotation_from, rotation_to, duration_f / TRANSITION_DURATION_F);
  angle_f += 1.5f; // degrees
  angle_f = std::fmod (angle_f, 360.0f);

  glutSwapBuffers ();
}

void
test_i_opengl_glut_reshape (int width_in, int height_in)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  glViewport (0, 0, width_in, height_in);
  COMMON_GL_ASSERT;

#if defined (ACE_WIN32) || defined (ACE_WIN32)
  cb_data_p->perspective.resolution.cx =
#else
  cb_data_p->perspective.resolution.width =
#endif // ACE_WIN32 || ACE_WIN64
    static_cast<unsigned int> (width_in);
#if defined (ACE_WIN32) || defined (ACE_WIN32)
  cb_data_p->perspective.resolution.cy =
#else
  cb_data_p->perspective.resolution.height =
#endif // ACE_WIN32 || ACE_WIN64
  static_cast<unsigned int> (height_in);
}

//void
//test_i_opengl_glut_idle ()
//{
//  //glutPostRedisplay ();
//}

void
test_i_opengl_glut_timer (int value_in)
{
  glutPostRedisplay ();
  glutTimerFunc (1000 / 60, test_i_opengl_glut_timer, value_in);
}

void
test_i_opengl_glut_visible (int visibility_in)
{
  if (visibility_in == GLUT_VISIBLE)
    /*glutIdleFunc (test_i_opengl_glut_idle)*/;
  else
    glutIdleFunc (NULL);
}

void
test_i_opengl_glut_key (unsigned char key_in,
                        int x,
                        int y)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  switch (key_in)
  {
    case 'w':
      cb_data_p->camera.forward (0.5f);
      break;
    case 's':
      cb_data_p->camera.backward (0.5f);
      break;
    case 'a':
      cb_data_p->camera.left (0.5f);
      break;
    case 'd':
      cb_data_p->camera.right (0.5f);
      break;
    case 'q':
    case 27: /* Escape */
      glutLeaveMainLoop ();
      break;
    default:
      break;
  } // end SWITCH
}

void
test_i_opengl_glut_key_special (int key_in,
                                int x,
                                int y)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  switch (key_in)
  {
    case GLUT_KEY_LEFT:
      cb_data_p->camera.left (0.5f);
      break;
    case GLUT_KEY_RIGHT:
      cb_data_p->camera.right (0.5f);
      break;
    case GLUT_KEY_UP:
      cb_data_p->camera.forward (0.5f);
      break;
    case GLUT_KEY_DOWN:
      cb_data_p->camera.backward (0.5f);
      break;
    case GLUT_KEY_HOME:
      cb_data_p->camera.reset ();
      break;
    default:
      break;
  } // end SWITCH
}

void
test_i_opengl_glut_menu (int entry_in)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  switch (entry_in)
  {
    case 0:
      cb_data_p->wireframe = !cb_data_p->wireframe;
      glPolygonMode (GL_FRONT_AND_BACK, cb_data_p->wireframe ? GL_LINE : GL_FILL);
      break;
    default:
      break;
  } // end SWITCH
}

void
test_i_opengl_glut_mouse_button (int button, int state, int x, int y)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      cb_data_p->mouseButton0IsDown = (state == GLUT_DOWN);
      break;
    case 3:
      cb_data_p->camera.forward (0.5f);
      break;
    case 4:
      cb_data_p->camera.backward (0.5f);
      break;
    default:
      break;
  } // end SWITCH
}

void
test_i_opengl_glut_mouse_move (int x, int y)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  if (cb_data_p->mouseButton0IsDown)
#if defined (ACE_WIN32) || defined (ACE_WIN32)
    cb_data_p->camera.mouseLook (static_cast<int> (cb_data_p->perspective.resolution.cx) - x, y);
#else
    cb_data_p->camera.mouseLook (static_cast<int> (cb_data_p->perspective.resolution.width) - x, y);
#endif // ACE_WIN32 || ACE_WIN64
}

void
test_i_opengl_glut_mouse_wheel (int button, int dir, int x, int y)
{
  struct Common_OpenGL_GLUT_CBData* cb_data_p =
    static_cast<struct Common_OpenGL_GLUT_CBData*> (glutGetWindowData ());
  ACE_ASSERT (cb_data_p);

  if (dir > 0)
    cb_data_p->camera.forward (0.5f);
  else
    cb_data_p->camera.backward (0.5f);
}
