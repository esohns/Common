#include "stdafx.h"

#include "gtk/gtk.h"
#if defined (GTKGL_SUPPORT)
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
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#endif // GLM_SUPPORT
#endif // GTKGL_SUPPORT

#if GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gtkglarea.h"
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Init_ACE.h"
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_tools.h"

#if defined (GTKGL_SUPPORT)
#include "common_gl_camera.h"
#include "common_gl_debug.h"
#include "common_gl_defines.h"
#include "common_gl_shader.h"
#include "common_gl_texture.h"
#include "common_gl_tools.h"
#endif // GTKGL_SUPPORT

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "common_test_i_defines.h"

#include "test_i_gtk_defines.h"

#if GTK_CHECK_VERSION (4,0,0)
GtkApplication* app_p = NULL;
GtkWindow* main_window_p = NULL;
#endif // GTK_CHECK_VERSION (4,0,0)
#if defined (GTKGL_SUPPORT)
Common_GL_Texture texture;
GLuint VBO = 0, VAO = 0, EBO = 0;
Common_GL_Shader shader;
Common_GL_Camera camera;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
static std::chrono::steady_clock::time_point tp1;
#elif defined (ACE_LINUX)
static std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp1;
#else
#error missing implementation, aborting
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
int width_i, height_i;
#endif // GTKGL_SUPPORT

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::string ui_definition_file_path = path_root;
  ui_definition_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_file_path += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  ui_definition_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g [FILEPATH]: ui definition file [")
            << ui_definition_file_path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t           : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      bool& traceInformation_out,
                      std::string& UIDefinitionFilePath_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  traceInformation_out = false;
  UIDefinitionFilePath_out = path_root;
  UIDefinitionFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIDefinitionFilePath_out += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  UIDefinitionFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIDefinitionFilePath_out += ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("g:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  //std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'g':
      {
        UIDefinitionFilePath_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argument_parser.last_option ()));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    argument_parser.long_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT void
togglebutton_record_toggled_cb (GtkToggleButton* toggleButton_in,
                                gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("togglebutton toggled\n")));
}

G_MODULE_EXPORT void
combobox_source_changed_cb (GtkWidget* widget_in,
                            gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("combobox changed\n")));
}

G_MODULE_EXPORT void
combobox_source_2_changed_cb (GtkWidget* widget_in,
                              gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("combobox changed\n")));
}

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
G_MODULE_EXPORT GdkGLContext*
glarea_create_context_cb (GtkGLArea* GLArea_in,
                          gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea create context\n")));

  GdkGLContext* context_p = NULL; 
  context_p = gtk_gl_area_get_context (GLArea_in);
  if (context_p)
    return context_p;

  GError* error_p = NULL;
#if GTK_CHECK_VERSION (4,0,0)
  GtkNative* native_p = gtk_widget_get_native (GTK_WIDGET (GLArea_in));
  ACE_ASSERT (native_p);
  //gtk_native_realize (native_p);
  GdkSurface* surface_p = gtk_native_get_surface (native_p);
  ACE_ASSERT (surface_p);
  context_p = gdk_surface_create_gl_context (surface_p,
                                             &error_p);
#else
  GdkWindow* window_p = gtk_widget_get_window (GTK_WIDGET (GLArea_in));
  ACE_ASSERT (window_p);
  context_p = gdk_window_create_gl_context (window_p,
                                            &error_p);
#endif // GTK_CHECK_VERSION (4,0,0)
  if (!context_p || error_p)
  { ACE_ASSERT (error_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_surface_create_gl_context(): \"%s\", aborting\n"),
                ACE_TEXT (error_p->message)));
    g_error_free (error_p);
    return NULL;
  } // end IF

#if defined (_DEBUG)
  //gdk_gl_context_set_debug_enabled (context_p, TRUE);
#endif // _DEBUG
  //gdk_gl_context_set_required_version (context_p, 3, 3);
  //gdk_gl_context_set_allowed_apis (context_p, GdkGLAPI::GDK_GL_API_GLES);

  gtk_gl_area_set_has_depth_buffer (GLArea_in, TRUE);

  return context_p;
}

G_MODULE_EXPORT void
glarea_realize_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{ // *NOTE*: only run this once (see below)
  static bool has_been_realized_b = false;
  if (has_been_realized_b)
    return;
  has_been_realized_b = true;

  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  // gtk_gl_area_make_current (gl_area_p);
  GdkGLContext* context_p = gtk_gl_area_get_context (gl_area_p);
  ACE_ASSERT (context_p);
  gdk_gl_context_make_current (context_p);

  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  COMMON_GL_ASSERT;

  //glEnable (GL_TEXTURE_2D); // Enable Texture Mapping
  //COMMON_GL_ASSERT;

  // load texture ?
  std::string filename;

#if defined (GLEW_SUPPORT)
  GLenum status_e = glewInit ();
  if (status_e != GLEW_OK)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glewInit(): \"%s\", returning\n"),
                ACE_TEXT (glewGetErrorString (status_e))));
    return;
  }    // end IF
#endif // GLEW_SUPPORT
#if defined (_DEBUG)
  //Common_GL_Debug::Install ();
#endif // _DEBUG

  filename = Common_File_Tools::getWorkingDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR;
  filename += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY);
  filename += ACE_DIRECTORY_SEPARATOR_CHAR;
  filename += ACE_TEXT_ALWAYS_CHAR ("opengl_logo.png");
  if (!texture.load (filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), returning\n"),
                ACE_TEXT (filename.c_str ())));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("OpenGL texture id: %u\n"),
              texture.id_));

  camera.position_ = glm::vec3 (0.0f, 0.0f, 0.1f);
  camera.looking_at_ = glm::vec3 (0.0f, 0.0f, 0.0f);
  camera.up_ = glm::vec3 (0.0f, 1.0f, 0.0f);

  //glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  //COMMON_GL_ASSERT;
  //glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  //COMMON_GL_ASSERT;

  glEnable (GL_DEPTH_TEST); // Enables Depth Testing
  COMMON_GL_ASSERT;
  glEnable (GL_BLEND); // Enable Semi-Transparency
  COMMON_GL_ASSERT;
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  COMMON_GL_ASSERT;
  //glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  //COMMON_GL_ASSERT;
  //glDepthMask (GL_TRUE);
  //COMMON_GL_ASSERT;
  //glEnable (GL_CULL_FACE);
  //COMMON_GL_ASSERT;
  //glCullFace (GL_BACK);
  //COMMON_GL_ASSERT;
  //glFrontFace (GL_CCW);
  //COMMON_GL_ASSERT;

  glGenVertexArrays (1, &VAO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (VAO);
  glGenBuffers (1, &VBO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (VBO);
  glGenBuffers (1, &EBO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (EBO);

  glBindVertexArray (VAO);
  COMMON_GL_ASSERT;

  glBindBuffer (GL_ARRAY_BUFFER, VBO);
  COMMON_GL_ASSERT;
  static GLfloat cube_strip_color_texcoords[] = {
   // x       y      z         r    g     b     a         u    v
    -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f,

     0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f,

     0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f, 0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f, 0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f, 0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 1.0f, 0.5f,    1.0f, 1.0f
  };
  glBufferData (GL_ARRAY_BUFFER, sizeof (cube_strip_color_texcoords), cube_strip_color_texcoords, GL_STATIC_DRAW);
  COMMON_GL_ASSERT;

  // position attribute
  glEnableVertexAttribArray (0);
  COMMON_GL_ASSERT;
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof (GLfloat), (void*)0);
  COMMON_GL_ASSERT;

  // color attribute
  glEnableVertexAttribArray (1);
  COMMON_GL_ASSERT;
  glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof (GLfloat)));
  COMMON_GL_ASSERT;

  // texture coord attribute
  glEnableVertexAttribArray (2);
  COMMON_GL_ASSERT;
  glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof (GLfloat), (void*)(7 * sizeof (GLfloat)));
  COMMON_GL_ASSERT;

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
  COMMON_GL_ASSERT;
  static GLubyte cube_indices[34] = {
    0,  1,  2,  3,  3,      // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
    4,  4,  5,  6,  7,  7,  // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
    8,  8,  9,  10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
    12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
    16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
    20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
  };
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (cube_indices), cube_indices, GL_STATIC_DRAW);
  COMMON_GL_ASSERT;

  std::string vertex_shader_filename = Common_File_Tools::getWorkingDirectory ();
  vertex_shader_filename += ACE_DIRECTORY_SEPARATOR_STR_A;
  vertex_shader_filename +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  vertex_shader_filename += ACE_DIRECTORY_SEPARATOR_STR_A;
  vertex_shader_filename += ACE_TEXT_ALWAYS_CHAR ("vertex_shader.glsl");
  std::string fragment_shader_filename = Common_File_Tools::getWorkingDirectory ();
  fragment_shader_filename += ACE_TEXT_ALWAYS_CHAR (ACE_DIRECTORY_SEPARATOR_STR_A);
  fragment_shader_filename +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  fragment_shader_filename += ACE_DIRECTORY_SEPARATOR_STR_A;
  fragment_shader_filename += ACE_TEXT_ALWAYS_CHAR ("fragment_shader.glsl");
  bool success_b = shader.loadFromFile (vertex_shader_filename,
                                        fragment_shader_filename);
  if (success_b)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loaded shader\n")));

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;
  glBindBuffer (GL_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;
  glBindVertexArray (0);
  COMMON_GL_ASSERT;

  tp1 = std::chrono::high_resolution_clock::now ();
} // glarea_realize_cb

G_MODULE_EXPORT gboolean
glarea_render_cb (GtkGLArea* area_in,
                  GdkGLContext* context_in,
                  gpointer userData_in)
{
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("glarea render\n")));

  // sanity check(s)
  ACE_ASSERT (VAO && texture.id_);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  COMMON_GL_ASSERT;

  glBindVertexArray (VAO);
  COMMON_GL_ASSERT;
  texture.bind ();

  //static GLfloat rot_x = 0.0f;
  //static GLfloat rot_y = 0.0f;
  //static GLfloat rot_z = 0.0f;
  static GLfloat rotation = 0.0f; // *NOTE*: degrees

#if defined (GLM_SUPPORT)
  glm::mat4 model_matrix = glm::mat4 (1.0f); // make sure to initialize matrix to identity matrix first
  model_matrix = glm::translate (model_matrix, glm::vec3 (0.0f, 0.0f, -3.0f));
  model_matrix =
    glm::rotate (model_matrix, glm::radians (rotation), glm::vec3 (1.0f, 1.0f, 1.0f));

  glm::mat4 view_matrix = camera.getViewMatrix ();

  glm::mat4 projection_matrix =
    glm::perspective (glm::radians (45.0f), width_i / (float)height_i, 0.1f, 100.0f);
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
  std::chrono::duration<float> elapsed_time = tp2 - tp1;

  shader.use ();
#if defined (GLM_SUPPORT)
  shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("model"), model_matrix);
  shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("view"), view_matrix);
  shader.setMat4 (ACE_TEXT_ALWAYS_CHAR ("projection"), projection_matrix);
#endif // GLM_SUPPORT
  shader.setInt (ACE_TEXT_ALWAYS_CHAR ("texture1"), 0); // *IMPORTANT NOTE*: <-- texture unit (!) not -id
  shader.setFloat (ACE_TEXT_ALWAYS_CHAR ("time"), elapsed_time.count ());

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
  COMMON_GL_ASSERT;

  glDisable (GL_DEPTH_TEST);
  glDrawElements (GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_BYTE, (void*)0);
  glEnable (GL_DEPTH_TEST);

  COMMON_GL_ASSERT; // *NOTE*: GL_QUADS is not supported as primitive mode
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;

  //switch (Common_Tools::getRandomNumber (0, 2))
  //{
  //  case 0:
  //    rot_x += 0.1f;
  //    break;
  //  case 1:
  //    rot_y += 0.1f;
  //    break;
  //  case 2:
  //    rot_z += 0.1f;
  //    break;
  //  default:
  //    ACE_ASSERT (false);
  //} // end SWITCH
  rotation += 0.3f; // change the rotation variable for the cube

  // gtk_gl_area_swap_buffers (area_in);
  glFlush ();
  //COMMON_GL_CLEAR_ERROR;

  glBindVertexArray (0);
  COMMON_GL_ASSERT;
  texture.unbind ();

  // auto-redraw
  //gtk_widget_queue_draw (GTK_WIDGET (area_in));
  g_idle_add ((GSourceFunc)gtk_widget_queue_draw, (void*)GTK_WIDGET (area_in));

  return TRUE;
}

G_MODULE_EXPORT void
glarea_resize_cb (GtkGLArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("glarea resize\n")));

  //gtk_gl_area_make_current (GLArea_in);

  glViewport (0, 0,
              width_in, height_in);
  COMMON_GL_ASSERT;

//  glMatrixMode (GL_PROJECTION);
//  //COMMON_GL_CLEAR_ERROR;
//  glLoadIdentity (); // Reset The Projection Matrix
//  //COMMON_GL_ASSERT;
//
//#if defined (GLU_SUPPORT)
//  gluPerspective (45.0,
//                  width_in / (GLdouble)height_in,
//                  0.1,
//                  100.0); // Calculate The Aspect Ratio Of The Window
//#else
//  GLdouble fW, fH;
//  //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
//  fH = std::tan (((45.0f / 2.0f) / 180.0f) * static_cast<float> (M_PI)) * 0.1f;
//  fW = fH * (width_in / (GLdouble)height_in);
//
//  glFrustum (-fW, fW, -fH, fH, 0.1, 100.0);
//#endif // GLU_SUPPORT
//  //COMMON_GL_ASSERT;
//
//  glMatrixMode (GL_MODELVIEW);
//  //COMMON_GL_ASSERT;

  width_i = width_in;
  height_i = height_in;
}

G_MODULE_EXPORT void
glarea_destroy_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{
  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  gtk_gl_area_make_current (gl_area_p);

  texture.reset ();

  glDeleteVertexArrays (1, &VAO);
  glDeleteBuffers (1, &VBO);
  glDeleteBuffers (1, &EBO);
} // glarea_destroy_cb
#else
#if defined (GTKGLAREA_SUPPORT)
void
glarea_configure_event_cb (GtkWidget* widget_in,
                           GdkEvent* event_in,
                           gpointer userData_in)
{
  // sanity check(s)
  GglaArea* gl_area_p = GGLA_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!ggla_area_make_current (gl_area_p))
    return;

  glViewport (0, 0,
              event_in->configure.width, event_in->configure.height);
  COMMON_GL_ASSERT;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset The Projection Matrix
  COMMON_GL_ASSERT;

#if defined (GLU_SUPPORT)
  gluPerspective (45.0,
                  event_in->configure.width / (GLdouble)event_in->configure.height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
#else
  GLdouble fW, fH;
  //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
  fH = std::tan (((45.0f / 2.0f) / 180.0f) * static_cast<float> (M_PI)) * 0.1f;
  fW = fH * (event_in->configure.width / (GLdouble)event_in->configure.height);

  glFrustum (-fW, fW, -fH, fH, 0.1, 100.0);
#endif // GLU_SUPPORT
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;
}

gboolean
glarea_expose_event_cb (GtkWidget* widget_in,
                        GdkEvent* event_in,
                        gpointer userData_in)
{
  // sanity check(s)
  GglaArea* gl_area_p = GGLA_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!ggla_area_make_current (gl_area_p))
    return FALSE;
  ACE_UNUSED_ARG (event_in);
  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset the transformation matrix.
  COMMON_GL_ASSERT;
  glTranslatef (0.0F, 0.0F, -5.0F); // Move back into the screen 5 units
  COMMON_GL_ASSERT;

  glBindTexture (GL_TEXTURE_2D, *texture_id_p);
  COMMON_GL_ASSERT;

  static GLfloat rot_x = 0.0f;
  static GLfloat rot_y = 0.0f;
  static GLfloat rot_z = 0.0f;
  glRotatef (rot_x, 1.0f, 0.0f, 0.0f); // Rotate Around The X Axis
  glRotatef (rot_y, 0.0f, 1.0f, 0.0f); // Rotate Around The Y Axis
  glRotatef (rot_z, 0.0f, 0.0f, 1.0f); // Rotate Around The Z Axis
  //static GLfloat rotation = 0.0f;
  //glRotatef (rotation, 1.0f, 1.0f, 1.0f); // Rotate Around The X,Y,Z Axis
  COMMON_GL_ASSERT;

  static GLfloat cube_a[] = {
    // x,    y,    z,   s,   t,
     1.0f,-1.0f,-1.0f, 0.0f,1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,0.0f,
     1.0f, 1.0f,-1.0f, 0.0f,0.0f,

    -1.0f, 1.0f,-1.0f, 0.0f,0.0f,
    -1.0f,-1.0f,-1.0f, 0.0f,1.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,0.0f,

    -1.0f,-1.0f, 1.0f, 0.0f,1.0f,
     1.0f,-1.0f, 1.0f, 1.0f,1.0f,
     1.0f, 1.0f, 1.0f, 1.0f,0.0f,
    -1.0f, 1.0f, 1.0f, 0.0f,0.0f,

     1.0f,-1.0f,-1.0f, 1.0f,1.0f,
     1.0f, 1.0f,-1.0f, 1.0f,0.0f,
     1.0f, 1.0f, 1.0f, 0.0f,0.0f,
     1.0f,-1.0f, 1.0f, 0.0f,1.0f,

     1.0f, 1.0f,-1.0f, 0.0f,1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,0.0f,
     1.0f, 1.0f, 1.0f, 0.0f,0.0f,

     1.0f,-1.0f, 1.0f, 0.0f,0.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,0.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,1.0f,
     1.0f,-1.0f,-1.0f, 0.0f,1.0f,
  };

  glBegin (GL_QUADS);
  for (int i = 0; i < sizeof (cube_a) / (sizeof (cube_a[0])); i += 5)
  {
    glTexCoord2fv (cube_a + i + 3);
    glVertex3fv (cube_a + i + 0);
  } // end FOR
  glEnd ();

  glBindTexture (GL_TEXTURE_2D, 0);
  COMMON_GL_ASSERT;

  switch (Common_Tools::getRandomNumber (0, 2))
  {
    case 0:
      rot_x += 0.1f;
      break;
    case 1:
      rot_y += 0.1f;
      break;
    case 2:
      rot_z += 0.1f;
      break;
    default:
      ACE_ASSERT (false);
      break;
  } // end SWITCH
  rot_x = std::fmod (rot_x, 360.0f);
  rot_y = std::fmod (rot_y, 360.0f);
  rot_z = std::fmod (rot_z, 360.0f);
  // rotation += 0.1f; // change the rotation variable for the cube

  ggla_area_swap_buffers (gl_area_p);

  // auto-redraw
  gtk_widget_queue_draw (widget_in);

  return TRUE;
}

void
glarea_realize_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{
  // sanity check(s)
  GglaArea* gl_area_p = GGLA_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!ggla_area_make_current (gl_area_p))
    return;

#if defined (GLEW_SUPPORT)
  GLenum status_e = glewInit ();
  if (status_e != GLEW_OK)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glewInit(): \"%s\", returning\n"),
                ACE_TEXT (glewGetErrorString (status_e))));
    return;
  } // end IF
#endif // GLEW_SUPPORT
#if defined(_DEBUG)
  // Common_GL_Debug::Install ();
#endif // _DEBUG

  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);
  GtkAllocation allocation;
  // set up light colors (ambient, diffuse, specular)
  //GLfloat light_ambient[] = {1.0F, 1.0F, 1.0F, 1.0F};
  //GLfloat light_diffuse[] = {0.3F, 0.3F, 0.3F, 1.0F};
  //GLfloat light_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
  // position the light in eye space
  //GLfloat light0_position[] = {0.0F,
  //                             5.0F * 2,
  //                             5.0F * 2,
  //                             0.0F}; // --> directional light

  // load texture
  if (*texture_id_p > 0)
  {
    glDeleteTextures (1, texture_id_p);
    COMMON_GL_ASSERT;
    *texture_id_p = 0;
  } // end IF
  if (!*texture_id_p)
  {
    glActiveTexture (GL_TEXTURE0);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::string module_name =
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY);
    module_name += ACE_DIRECTORY_SEPARATOR_STR;
    module_name += ACE_TEXT_ALWAYS_CHAR (COMMON_TEST_I_UI_SUBDIRECTORY);
    std::string filename =
      Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                        module_name,
                                                        false); // data
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR ("opengl_logo.png");
    *texture_id_p = Common_GL_Tools::loadTexture (filename, true);
    if (!*texture_id_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return;
    } // end IF
    glBindTexture (GL_TEXTURE_2D, *texture_id_p);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glGenerateMipmap (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, 0);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL texture id: %u\n"),
                *texture_id_p));
  } // end IF

  // initialize perspective
  gtk_widget_get_allocation (widget_in,
                             &allocation);
  glViewport (0, 0,
              static_cast<GLsizei> (allocation.width), static_cast<GLsizei> (allocation.height));
  COMMON_GL_ASSERT;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset The Projection Matrix
  COMMON_GL_ASSERT;

#if defined (GLU_SUPPORT)
  gluPerspective (45.0,
                  allocation.width / (GLdouble)allocation.height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
#else
  GLdouble fW, fH;

  //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
  fH = tan (45.0 / 360.0 * M_PI) * 0.1;
  fW = fH * (allocation.width / (GLdouble)allocation.height);

  glFrustum (-fW, fW, -fH, fH, 0.1, 100.0);
#endif // GLU_SUPPORT
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // reset the projection matrix
  COMMON_GL_ASSERT;

  /* light */
//  GLfloat light_positions[2][4]   = { 50.0, 50.0, 0.0, 0.0,
//                                     -50.0, 50.0, 0.0, 0.0 };
//  GLfloat light_colors[2][4] = { .6, .6,  .6, 1.0,   /* white light */
//                                 .4, .4, 1.0, 1.0 }; /* cold blue light */
//  glLightfv (GL_LIGHT0, GL_POSITION, light_positions[0]);
//  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_colors[0]);
//  glLightfv (GL_LIGHT1, GL_POSITION, light_positions[1]);
//  glLightfv (GL_LIGHT1, GL_DIFFUSE,  light_colors[1]);
//  glEnable (GL_LIGHT0);
//  glEnable (GL_LIGHT1);
//  glEnable (GL_LIGHTING);

  // set up light colors (ambient, diffuse, specular)
  //glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
  //COMMON_GL_ASSERT;
  //glEnable (GL_LIGHT0);
  //COMMON_GL_ASSERT;

  //glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
  //COMMON_GL_ASSERT;
  glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
  COMMON_GL_ASSERT;
  //glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  //COMMON_GL_ASSERT;
  //glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  //COMMON_GL_ASSERT;

  glEnable (GL_BLEND);                                // Enable Semi-Transparency
  COMMON_GL_ASSERT;
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  COMMON_GL_ASSERT;
  //glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
  //COMMON_GL_ASSERT;
  //glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  //COMMON_GL_ASSERT;
  //glDepthMask (GL_TRUE);
  //COMMON_GL_ASSERT;
} // glarea_realize_cb

void
glarea_unrealize_cb (GtkWidget* widget_in,
                     gpointer   userData_in)
{
  // sanity check(s)
  GglaArea* gl_area_p = GGLA_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  ggla_area_make_current (gl_area_p);
  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);

  glDeleteTextures (1, texture_id_p);
  COMMON_GL_ASSERT;
  *texture_id_p = 0;
} // glarea_unrealize_cb
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
void
glarea_configure_event_cb (GtkWidget* widget_in,
                           GdkEvent* event_in,
                           gpointer userData_in)
{
  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!gtk_gl_area_make_current (gl_area_p))
    return;

  glViewport (0, 0,
              event_in->configure.width, event_in->configure.height);
  COMMON_GL_ASSERT;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset The Projection Matrix
  COMMON_GL_ASSERT;

#if defined (GLU_SUPPORT)
  gluPerspective (45.0,
                  event_in->configure.width / (GLdouble)event_in->configure.height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
#else
  GLdouble fW, fH;

  //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
  fH = tan (45.0 / 360 * M_PI) * 0.1;
  fW = fH * (event_in->configure.width / (GLdouble)event_in->configure.height);

  glFrustum (-fW, fW, -fH, fH, 0.1, 100.0);
#endif // GLU_SUPPORT
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;
}

gboolean
glarea_expose_event_cb (GtkWidget* widget_in,
                        GdkEvent* event_in,
                        gpointer userData_in)
{
  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!gtk_gl_area_make_current (gl_area_p))
    return FALSE;
  ACE_UNUSED_ARG (event_in);
  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset the transformation matrix.
  COMMON_GL_ASSERT;
  glTranslatef (0.0F, 0.0F, -5.0F); // Move back into the screen 5 units
  COMMON_GL_ASSERT;

  glBindTexture (GL_TEXTURE_2D, *texture_id_p);
  COMMON_GL_ASSERT;

//  static GLfloat rot_x = 0.0f;
//  static GLfloat rot_y = 0.0f;
//  static GLfloat rot_z = 0.0f;
//  glRotatef (rot_x, 1.0f, 0.0f, 0.0f); // Rotate On The X Axis
//  glRotatef (rot_y, 0.0f, 1.0f, 0.0f); // Rotate On The Y Axis
//  glRotatef (rot_z, 0.0f, 0.0f, 1.0f); // Rotate On The Z Axis
  static GLfloat rotation = 0.0F;
  glRotatef (rotation, 1.0F, 1.0F, 1.0F); // Rotate On The X,Y,Z Axis
  COMMON_GL_ASSERT;

//  glBegin (GL_QUADS);

//  glTexCoord2i (0, 0); glVertex3f (  0.0f,   0.0f, 0.0f);
//  glTexCoord2i (0, 1); glVertex3f (  0.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 1); glVertex3f (100.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 0); glVertex3f (100.0f,   0.0f, 0.0f);

  // *TODO*: these are slightly off...
  static GLfloat vertices[] = {
    -0.5f, 0.0f, 0.5f,   0.5f, 0.0f, 0.5f,   0.5f, 1.0f, 0.5f,  -0.5f, 1.0f, 0.5f,
    -0.5f, 1.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, 0.5f,   0.5f, 0.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 1.0f, 0.5f,
    -0.5f, 0.0f, -0.5f,  -0.5f, 0.0f, 0.5f,  -0.5f, 1.0f, 0.5f, -0.5f, 1.0f, -0.5f};
  static GLfloat texture_coordinates[] = {
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };
  static GLubyte cube_indices[24] = {
    0,1,2,3, 4,5,6,7, 3,2,5,4, 7,6,1,0,
    8,9,10,11, 12,13,14,15};

  glTexCoordPointer (2, GL_FLOAT, 0, texture_coordinates);
  COMMON_GL_ASSERT;
  glVertexPointer (3, GL_FLOAT, 0, vertices);
  COMMON_GL_ASSERT;
  glDrawElements (GL_QUADS, 24, GL_UNSIGNED_BYTE, cube_indices);
  COMMON_GL_ASSERT;

//  rot_x += 0.3f;
//  rot_y += 0.20f;
//  rot_z += 0.4f;
  rotation -= 1.0f; // Decrease The Rotation Variable For The Cube

  //GLuint vertex_array_id = 0;
  //glGenVertexArrays (1, &vertex_array_id);
  //glBindVertexArray (vertex_array_id);

  //static const GLfloat vertex_buffer_data[] = {
  //  -1.0f, -1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  1.0f,  1.0f, 0.0f,
  //};

  //GLuint vertex_buffer;
  //glGenBuffers (1, &vertex_buffer);
  //glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
  //glBufferData (GL_ARRAY_BUFFER,
  //              sizeof (vertex_buffer_data), vertex_buffer_data,
  //              GL_STATIC_DRAW);

  ////GLuint program_id = LoadShaders ("Passthrough.vertexshader",
  ////                                 "SimpleTexture.fragmentshader");
  ////GLuint tex_id = glGetUniformLocation (program_id, "renderedTexture");
  ////GLuint time_id = glGetUniformLocation (program_id, "time");

  //glBindFramebuffer (GL_FRAMEBUFFER, 0);
  //glViewport (0, 0,
  //            data_p->area3D.width, data_p->area3D.height);

  gtk_gl_area_swap_buffers (gl_area_p);

  // auto-redraw
  gtk_widget_queue_draw (widget_in);

  return TRUE;
}

void
glarea_realize_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{
  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  if (!gtk_gl_area_make_current (gl_area_p))
    return;
  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);
  GtkAllocation allocation;
  // set up light colors (ambient, diffuse, specular)
  //GLfloat light_ambient[] = {1.0F, 1.0F, 1.0F, 1.0F};
  //GLfloat light_diffuse[] = {0.3F, 0.3F, 0.3F, 1.0F};
  //GLfloat light_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
  // position the light in eye space
  //GLfloat light0_position[] = {0.0F,
  //                             5.0F * 2,
  //                             5.0F * 2,
  //                             0.0F}; // --> directional light

  // load texture
  if (*texture_id_p > 0)
  {
    glDeleteTextures (1, texture_id_p);
    COMMON_GL_ASSERT;
    *texture_id_p = 0;
  } // end IF
  if (!*texture_id_p)
  {
    std::string filename = Common_File_Tools::getWorkingDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY);
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR ("opengl_logo.png");
    *texture_id_p = Common_GL_Tools::loadTexture (filename);
    if (!*texture_id_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL texture id: %u\n"),
                *texture_id_p));
  } // end IF

  // initialize perspective
  gtk_widget_get_allocation (widget_in,
                             &allocation);
  glViewport (0, 0,
              static_cast<GLsizei> (allocation.width), static_cast<GLsizei> (allocation.height));
  COMMON_GL_ASSERT;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // Reset The Projection Matrix
  COMMON_GL_ASSERT;

#if defined (GLU_SUPPORT)
  gluPerspective (45.0,
                  allocation.width / (GLdouble)allocation.height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
#else
  GLdouble fW, fH;

  //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
  fH = tan (45.0 / 360 * M_PI) * 0.1;
  fW = fH * (allocation.width / (GLdouble)allocation.height);

  glFrustum (-fW, fW, -fH, fH, 0.1, 100.0);
#endif // GLU_SUPPORT
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;
  glLoadIdentity (); // reset the projection matrix
  COMMON_GL_ASSERT;

  /* light */
//  GLfloat light_positions[2][4]   = { 50.0, 50.0, 0.0, 0.0,
//                                     -50.0, 50.0, 0.0, 0.0 };
//  GLfloat light_colors[2][4] = { .6, .6,  .6, 1.0,   /* white light */
//                                 .4, .4, 1.0, 1.0 }; /* cold blue light */
//  glLightfv (GL_LIGHT0, GL_POSITION, light_positions[0]);
//  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_colors[0]);
//  glLightfv (GL_LIGHT1, GL_POSITION, light_positions[1]);
//  glLightfv (GL_LIGHT1, GL_DIFFUSE,  light_colors[1]);
//  glEnable (GL_LIGHT0);
//  glEnable (GL_LIGHT1);
//  glEnable (GL_LIGHTING);

  // set up light colors (ambient, diffuse, specular)
  //glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  //COMMON_GL_ASSERT;
  //glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
  //COMMON_GL_ASSERT;
  //glEnable (GL_LIGHT0);
  //COMMON_GL_ASSERT;

  //glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
  //COMMON_GL_ASSERT;
  glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
  COMMON_GL_ASSERT;
  //glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  //COMMON_GL_ASSERT;
  //glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  //COMMON_GL_ASSERT;

  glEnable (GL_BLEND);                                // Enable Semi-Transparency
  COMMON_GL_ASSERT;
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  COMMON_GL_ASSERT;
  //glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
  //COMMON_GL_ASSERT;
  //glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  //COMMON_GL_ASSERT;
  //glDepthMask (GL_TRUE);
  //COMMON_GL_ASSERT;
} // glarea_realize_cb

void
glarea_destroy_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{
  // sanity check(s)
  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  gtk_gl_area_make_current (gl_area_p);
  GLuint* texture_id_p = static_cast<GLuint*> (userData_in);
  ACE_ASSERT (texture_id_p);

  glDeleteTextures (1, texture_id_p);
  COMMON_GL_ASSERT;
  *texture_id_p = 0;
} // glarea_destroy_cb
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(2,0,0)
#endif // GTKGL_SUPPORT

G_MODULE_EXPORT gint
button_about_clicked_cb (GtkWidget* widget,
                         gpointer callback_data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button about clicked\n")));

  return 1;
}

G_MODULE_EXPORT gint
button_quit_clicked_cb (GtkWidget* widget,
                        gpointer callback_data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button quit clicked\n")));

#if GTK_CHECK_VERSION (4,0,0)
  ACE_ASSERT (app_p);
  g_application_quit (G_APPLICATION (app_p));
#else
  gtk_main_quit ();
#endif // GTK_CHECK_VERSION (4,0,0)

  return 0;
}

//G_MODULE_EXPORT void
//on_close_cb (GtkDialog* widget,
//             gpointer data)
//{
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("close event\n")));
//}
//G_MODULE_EXPORT void
//on_response_cb (GtkDialog* widget,
//                gint response_id,
//                gpointer data)
//{
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("response event\n")));
//}
//
//G_MODULE_EXPORT gboolean
//on_delete_event_cb (GtkWidget* widget,
//                    GdkEvent* event,
//                    gpointer data)
//{
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("delete-event event\n")));
//  return FALSE; // emit 'destroy'
//}

#if GTK_CHECK_VERSION (4,0,0)
G_MODULE_EXPORT void
on_activate_cb (GtkApplication* application,
                gpointer data)
{
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("activate event\n")));

  GtkWindow* mainwin = (GtkWindow*)data;

  gtk_application_add_window (application, mainwin);

  //gtk_window_present (mainwin);
}

G_MODULE_EXPORT void
dialog_main_close_cb (GtkDialog* d, 
                      gpointer data)
{
  /* callback for "close" signal */
  g_print ("dialog_main_close_cb()\n");

#if GTK_CHECK_VERSION (4,0,0)
  ACE_ASSERT (app_p);
  g_application_quit (G_APPLICATION (app_p));
#else
  gtk_main_quit ();
#endif // GTK_CHECK_VERSION (4,0,0)
}

G_MODULE_EXPORT void
dialog_main_close_request_cb (GtkWindow* w, 
                              gpointer data)
{
  /* callback for "close-request" signal */
  g_print ("dialog_main_close_request_cb()\n");

#if GTK_CHECK_VERSION (4,0,0)
  ACE_ASSERT (app_p);
  g_application_quit (G_APPLICATION (app_p));
#else
  gtk_main_quit ();
#endif // GTK_CHECK_VERSION (4,0,0)
}

G_MODULE_EXPORT void
dialog_main_response_cb (GtkDialog* d,
                         gint response_id,
                         gpointer data)
{
  /* callback for "response" signal */
  g_print ("dialog_main_response_cb()\n");
}
#else
G_MODULE_EXPORT gboolean
on_destroy_event_cb (GtkWidget* widget,
                     GdkEvent* event,
                     gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("destroy-event event\n")));

  gtk_main_quit ();

  return TRUE;
}
#endif // GTK_CHECK_VERSION (4,0,0)
G_MODULE_EXPORT void
on_destroy_cb (GtkWidget* widget,
               gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("destroy event\n")));

#if GTK_CHECK_VERSION (4,0,0)
  ACE_ASSERT (app_p);
  g_application_quit (G_APPLICATION (app_p));
#else
  gtk_main_quit ();
#endif // GTK_CHECK_VERSION (4,0,0)
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         const std::string& UIDefinitionFilePath_in)
{
  gtk_disable_setlocale ();

#if GTK_CHECK_VERSION (4,0,0)
  // *WARNING*: if you pass any commandline arguments, the "activate" signal is
  //            not emitted, and g_application_run() will return immediately,
  //            because no windows will have been added to it (see above)
#undef gtk_init
  gtk_init ();
#else
  gtk_init (&argc_in,
            &argv_in);
#endif // GTK_CHECK_VERSION (4,0,0)

  GError* error_p = NULL;
  GtkBuilder* builder_p = gtk_builder_new ();
  if (!gtk_builder_add_from_file (builder_p,
                                  UIDefinitionFilePath_in.c_str (),
                                  &error_p))
  { ACE_ASSERT (error_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", returning\n"),
                ACE_TEXT (UIDefinitionFilePath_in.c_str ()),
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    return;
  } // end IF
  GtkDialog* dialog_p =
    GTK_DIALOG (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_WIDGET_MAIN)));
  ACE_ASSERT (dialog_p);

#if GTK_CHECK_VERSION (4,0,0)
  main_window_p = GTK_WINDOW (gtk_window_new ());
  ACE_ASSERT (main_window_p);
  gtk_window_set_transient_for (GTK_WINDOW (dialog_p),
                                main_window_p);
  app_p =
    gtk_application_new (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_APPLICATION_ID_DEFAULT),
                         G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app_p, ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (on_activate_cb), main_window_p);
#else
  //g_signal_handlers_block_matched (mainwin, G_SIGNAL_MATCH_DATA,
  //                                 g_signal_lookup ("delete-event", GTK_TYPE_WIDGET),
  //                                 0, NULL, NULL, NULL);
  gtk_builder_connect_signals (builder_p, NULL);
#endif // GTK_CHECK_VERSION (4,0,0)

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (4,0,0)
  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new ());
  ACE_ASSERT (gl_area_p);
  static GLuint texture_id = 0;
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("realize"),
                    G_CALLBACK (glarea_realize_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (glarea_destroy_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("create-context"),
                    G_CALLBACK (glarea_create_context_cb), NULL);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("render"),
                    G_CALLBACK (glarea_render_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("resize"),
                    G_CALLBACK (glarea_resize_cb), NULL);
  //gtk_widget_set_parent (GTK_WIDGET (gl_area_p), GTK_WIDGET (dialog_p));
  //gtk_widget_realize (GTK_WIDGET (gl_area_p));
  //GdkGLContext* context_p = gtk_gl_area_get_context (gl_area_p);
  //ACE_ASSERT (context_p);
  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p), 512, 512);
#elif GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new ());
  ACE_ASSERT (gl_area_p);
  gtk_widget_realize (GTK_WIDGET (gl_area_p));
  GdkGLContext* context_p = gtk_gl_area_get_context (gl_area_p);
  ACE_ASSERT (context_p);
#else
#if defined (GTKGLAREA_SUPPORT)
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GDK_GL_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int gl_attributes_a[] = {
    GGLA_USE_GL,
// GGLA_BUFFER_SIZE
// GGLA_LEVEL
    GGLA_RGBA,
    GGLA_DOUBLEBUFFER,
//    GGLA_STEREO
//    GGLA_AUX_BUFFERS
    GGLA_RED_SIZE,   1,
    GGLA_GREEN_SIZE, 1,
    GGLA_BLUE_SIZE,  1,
    GGLA_ALPHA_SIZE, 1,
//    GGLA_DEPTH_SIZE
//    GGLA_STENCIL_SIZE
//    GGLA_ACCUM_RED_SIZE
//    GGLA_ACCUM_GREEN_SIZE
//    GGLA_ACCUM_BLUE_SIZE
//    GGLA_ACCUM_ALPHA_SIZE
//
//    GGLA_X_VISUAL_TYPE_EXT
//    GGLA_TRANSPARENT_TYPE_EXT
//    GGLA_TRANSPARENT_INDEX_VALUE_EXT
//    GGLA_TRANSPARENT_RED_VALUE_EXT
//    GGLA_TRANSPARENT_GREEN_VALUE_EXT
//    GGLA_TRANSPARENT_BLUE_VALUE_EXT
//    GGLA_TRANSPARENT_ALPHA_VALUE_EXT
    GGLA_NONE
  };

  GglaArea* gl_area_p = GGLA_AREA (ggla_area_new (gl_attributes_a));
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to ggla_area_new(), returning\n")));
    g_object_unref (G_OBJECT (builder_p));
    return;
  } // end IF
  gtk_widget_set_events (GTK_WIDGET (gl_area_p),
                         GDK_EXPOSURE_MASK            |
                         GDK_BUTTON_PRESS_MASK        |
                         GDK_BUTTON_RELEASE_MASK      |
                         GDK_POINTER_MOTION_MASK      |
                         GDK_POINTER_MOTION_HINT_MASK);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                    G_CALLBACK (glarea_configure_event_cb), NULL);
  GLuint texture_id = 0;
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("draw"),
                    G_CALLBACK (glarea_expose_event_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("realize"),
                    G_CALLBACK (glarea_realize_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("unrealize"),
                    G_CALLBACK (glarea_unrealize_cb), &texture_id);
  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p), 256, 256);
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GDK_GL_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int gl_attributes_a[] = {
    GDK_GL_USE_GL,
// GDK_GL_BUFFER_SIZE
// GDK_GL_LEVEL
    GDK_GL_RGBA,
    GDK_GL_DOUBLEBUFFER,
//    GDK_GL_STEREO
//    GDK_GL_AUX_BUFFERS
    GDK_GL_RED_SIZE,   1,
    GDK_GL_GREEN_SIZE, 1,
    GDK_GL_BLUE_SIZE,  1,
    GDK_GL_ALPHA_SIZE, 1,
//    GDK_GL_DEPTH_SIZE
//    GDK_GL_STENCIL_SIZE
//    GDK_GL_ACCUM_RED_SIZE
//    GDK_GL_ACCUM_GREEN_SIZE
//    GDK_GL_ACCUM_BLUE_SIZE
//    GDK_GL_ACCUM_ALPHA_SIZE
//
//    GDK_GL_X_VISUAL_TYPE_EXT
//    GDK_GL_TRANSPARENT_TYPE_EXT
//    GDK_GL_TRANSPARENT_INDEX_VALUE_EXT
//    GDK_GL_TRANSPARENT_RED_VALUE_EXT
//    GDK_GL_TRANSPARENT_GREEN_VALUE_EXT
//    GDK_GL_TRANSPARENT_BLUE_VALUE_EXT
//    GDK_GL_TRANSPARENT_ALPHA_VALUE_EXT
    GDK_GL_NONE
  };

  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new (gl_attributes_a));
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_gl_area_new(), returning\n")));
    g_object_unref (G_OBJECT (gtkBuilder));
    return;
  } // end IF
  gtk_widget_set_events (GTK_WIDGET (gl_area_p),
                         GDK_EXPOSURE_MASK            |
                         GDK_BUTTON_PRESS_MASK        |
                         GDK_BUTTON_RELEASE_MASK      |
                         GDK_POINTER_MOTION_MASK      |
                         GDK_POINTER_MOTION_HINT_MASK);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                    G_CALLBACK (glarea_configure_event_cb), NULL);
  static GLuint texture_id = 0;
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                    G_CALLBACK (glarea_expose_event_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("realize"),
                    G_CALLBACK (glarea_realize_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (glarea_destroy_cb), &texture_id);
  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p), 256, 256);
#endif // GTKGLAREA_SUPPORT
#else
#endif // GTK_CHECK_VERSION (2,0,0)
#if GTK_CHECK_VERSION (4,0,0)
  GtkBox* box_p =
    GTK_BOX (gtk_builder_get_object (builder_p,
                                     ACE_TEXT_ALWAYS_CHAR ("vbox3")));
  ACE_ASSERT (box_p);
  GtkWidget* childs = gtk_widget_get_first_child (GTK_WIDGET (box_p));
  while (childs != NULL)
  {
    gtk_box_remove (box_p, childs);
    //gtk_widget_unparent (childs);
    childs = gtk_widget_get_first_child (GTK_WIDGET (box_p));
  } // end WHILE
  g_object_ref (GTK_WIDGET (gl_area_p));
  gtk_widget_unparent (GTK_WIDGET (gl_area_p));
  gtk_box_append (GTK_BOX (box_p), GTK_WIDGET (gl_area_p));
#else
  GtkBox* box_p =
    GTK_BOX (gtk_builder_get_object (builder_p, ACE_TEXT_ALWAYS_CHAR ("vbox3")));
  ACE_ASSERT (box_p);
  gtk_container_foreach (GTK_CONTAINER (box_p), (GtkCallback)gtk_widget_destroy, NULL);
  gtk_box_pack_start (box_p, GTK_WIDGET (gl_area_p), TRUE, TRUE, 0);
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // GTKGL_SUPPORT

#if GTK_CHECK_VERSION (3,0,0)
  GtkGrid* grid_p =
    GTK_GRID (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR ("grid1")));
  ACE_ASSERT (grid_p);
#else
  GtkTable* table_p =
    GTK_TABLE (gtk_builder_get_object (builder_p,
                                       ACE_TEXT_ALWAYS_CHAR ("table1")));
  ACE_ASSERT (table_p);
#endif // GTK_CHECK_VERSION (3,0,0)
  unsigned int number_of_pieces_i = 100;
  unsigned int number_of_colummns_and_rows_i =
      static_cast<unsigned int> (std::ceil (std::sqrt (static_cast<float> (number_of_pieces_i))));
#if GTK_CHECK_VERSION (3,0,0)
#else
  gtk_table_resize (table_p,
                    number_of_colummns_and_rows_i,
                    number_of_colummns_and_rows_i);
#endif // GTK_CHECK_VERSION (3,0,0)
  int x, y;
  GtkButton* button_p = NULL;

#if GTK_CHECK_VERSION (3,0,0)
  // GdkRGBA black = { 0.0, 0.0, 0.0, 1.0 };

  GtkCssProvider* css_provider_p = gtk_css_provider_new ();
  ACE_ASSERT (css_provider_p);
  // Load CSS into the object ("-1" says, that the css string is \0-terminated)

#if GTK_CHECK_VERSION (4,0,0)
  gtk_css_provider_load_from_data (css_provider_p,
                                   ACE_TEXT_ALWAYS_CHAR ("* { background-image:none; background-color:black; }"),
                                   -1);
#else
  gtk_css_provider_load_from_data (css_provider_p,
                                   ACE_TEXT_ALWAYS_CHAR ("* { background-image:none; background-color:black; }"), -1,
                                   NULL);
#endif // GTK_CHECK_VERSION (4,0,0)
#else
  GdkColor black = {0, 0x0000, 0x0000, 0x0000};
//  GdkColor red = {0, 0xffff, 0x0000, 0x0000};
#endif // GTK_CHECK_VERSION (3,0,0)

  for (unsigned int i = 0;
       i < number_of_pieces_i;
       ++i)
  {
    button_p = GTK_BUTTON (gtk_button_new ());
    ACE_ASSERT (button_p);

#if GTK_CHECK_VERSION (3,0,0)
    //gtk_widget_override_background_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_NORMAL, &black);
    //gtk_widget_override_background_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_PRELIGHT, &black);
    //gtk_widget_override_background_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_ACTIVE, &black);
    //gtk_widget_override_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_NORMAL, &black);
    //gtk_widget_override_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_PRELIGHT, &black);
    //gtk_widget_override_color (GTK_WIDGET (button_p), GTK_STATE_FLAG_ACTIVE, &black);

    gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (button_p)),
                                    GTK_STYLE_PROVIDER (css_provider_p),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
#else
    gtk_widget_modify_bg (GTK_WIDGET (button_p), GTK_STATE_NORMAL, &black);
    gtk_widget_modify_bg (GTK_WIDGET (button_p), GTK_STATE_PRELIGHT, &black);
    gtk_widget_modify_bg (GTK_WIDGET (button_p), GTK_STATE_ACTIVE, &black);
    //gtk_widget_modify_fg (GTK_WIDGET (button_p), GTK_STATE_NORMAL, &red);
    //gtk_widget_modify_fg (GTK_WIDGET (button_p), GTK_STATE_PRELIGHT, &red);
    //gtk_widget_modify_fg (GTK_WIDGET (button_p), GTK_STATE_ACTIVE, &red);
#endif // GTK_CHECK_VERSION (3,0,0)

    x = i % number_of_colummns_and_rows_i;
    y = i / number_of_colummns_and_rows_i;

#if GTK_CHECK_VERSION (3,0,0)
    gtk_grid_attach (grid_p,
                     GTK_WIDGET (button_p),
                     x, y,
                     1, 1);
#else
    gtk_table_attach_defaults (table_p,
                               GTK_WIDGET (button_p),
                               x, x + 1,
                               y, y + 1);
#endif // GTK_CHECK_VERSION (3,0,0)
  } // end FOR
#if GTK_CHECK_VERSION (3,0,0)
  g_object_unref (css_provider_p); css_provider_p = NULL;
#endif // GTK_CHECK_VERSION (3,0,0)

  //g_object_unref (G_OBJECT (builder_p));

#if GTK_CHECK_VERSION (4,0,0)
  g_application_run (G_APPLICATION (app_p), 0, NULL);
  g_object_unref (app_p);
#else
  gtk_widget_show_all (GTK_WIDGET (dialog_p));

  gtk_main ();
#endif // GTK_CHECK_VERSION (4,0,0)
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  int result = EXIT_FAILURE, result_2 = -1;

  // step0: initialize
#if defined (GTKGL_SUPPORT)
#if defined (GLUT_SUPPORT)
  glutInit (&argc_in, argv_in);
#endif // GLUT_SUPPORT
#endif // GTKGL_SUPPORT
  // *PORTABILITY*: on Windows, initialize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result_2 = ACE::init ();
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Profile_Timer process_profile;
  process_profile.start ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // COM ?
                            false); // RNG ?
#else
  Common_Tools::initialize (false); // RNG ?
#endif // ACE_WIN32 || ACE_WIN64
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  bool trace_information = false;
  std::string ui_definition_file_path = path_root;
  ui_definition_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_file_path += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  ui_definition_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             trace_information,
                             ui_definition_file_path))
  {
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (ui_definition_file_path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                     Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                                       Common_File_Tools::executable),             // log file name
                                     false,                                                                        // log to syslog ?
                                     false,                                                                        // trace messages ?
                                     trace_information,                                                            // debug messages ?
                                     NULL))                                                                        // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
           ui_definition_file_path);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"\n"),
              ACE_TEXT (Common_Timer_Tools::periodToString (working_time).c_str ())));

  // step3: shut down
  process_profile.stop ();

  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result_2 = process_profile.elapsed_time (elapsed_time);
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));
    goto clean;
  } // end IF
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ()),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#endif // ACE_WIN32 || ACE_WIN64

  result = EXIT_SUCCESS;

clean:
  Common_Log_Tools::finalize ();

  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
