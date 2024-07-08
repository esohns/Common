#include "stdafx.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

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

#include "ace/config-lite.h"
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

#include "common_gl_debug.h"
#include "common_gl_defines.h"
#include "common_gl_shader.h"
#include "common_gl_texture.h"
#include "common_gl_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "common_test_i_defines.h"

#include "test_i_opengl_common.h"
#include "test_i_opengl_defines.h"
#include "test_i_glut_callbacks.h"

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
  std::string texture_file_path = path_root;
  texture_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  texture_file_path += COMMON_LOCATION_DATA_SUBDIRECTORY;
  texture_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  texture_file_path +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_OPENGL_TEXTURE_FILENAME);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [FILEPATH]: texture file [")
            << texture_file_path
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
                      std::string& textureFilePath_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  textureFilePath_out = path_root;
  textureFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  textureFilePath_out += COMMON_LOCATION_DATA_SUBDIRECTORY;
  textureFilePath_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  textureFilePath_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_OPENGL_TEXTURE_FILENAME);

  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("f:t"),
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
        textureFilePath_out =
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

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         const std::string& textureFilePath_in)
{
  struct Common_OpenGL_GLUT_CBData cb_data_s;

#if defined (GLUT_SUPPORT)
  // initialize GLUT
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize (TEST_I_OPENGL_DEFAULT_WINDOW_WIDTH,
                      TEST_I_OPENGL_DEFAULT_WINDOW_HEIGHT);

  int window_i = glutCreateWindow ("OpenGL");
  //glutSetWindow (window_i);
  glutSetWindowData (&cb_data_s);
#endif // GLUT_SUPPORT

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using OpenGL version: %s\n"),
              ACE_TEXT ((const char*)glGetString (GL_VERSION))));

#if defined (GLUT_SUPPORT)
  std::ostringstream converter;
  converter << GLUT_API_VERSION;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using OpenGL GLUT API: %s\n"),
              ACE_TEXT (converter.str ().c_str ())));
#endif // GLUT_SUPPORT

  // initialize GLEW
#if defined (GLEW_SUPPORT)
  GLenum err = glewInit ();
  if (GLEW_OK != err)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glewInit(): \"%s\", returning\n"),
                ACE_TEXT (glewGetErrorString (err))));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using GLEW version: %s\n"),
              ACE_TEXT (glewGetString (GLEW_VERSION))));
#endif // GLEW_SUPPORT

  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

  glEnable (GL_BLEND); // Enable Semi-Transparency
  COMMON_GL_ASSERT;
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  COMMON_GL_ASSERT;

#if defined (GLUT_SUPPORT)
  glutDisplayFunc (test_i_opengl_glut_draw);
  glutReshapeFunc (test_i_opengl_glut_reshape);
  glutVisibilityFunc (test_i_opengl_glut_visible);
  glutKeyboardFunc (test_i_opengl_glut_key);
  glutSpecialFunc (test_i_opengl_glut_key_special);
  glutMouseFunc (test_i_opengl_glut_mouse_button);
  glutMotionFunc (test_i_opengl_glut_mouse_move);
  glutPassiveMotionFunc (test_i_opengl_glut_mouse_move);
  glutIdleFunc (test_i_opengl_glut_idle);
  //glutTimerFunc (1000 / 30, test_i_opengl_glut_timer, 0);
  glutMouseWheelFunc (test_i_opengl_glut_mouse_wheel);

  glutCreateMenu (test_i_opengl_glut_menu);
  glutAddMenuEntry (ACE_TEXT_ALWAYS_CHAR ("wireframe"), 0);
  glutAttachMenu (GLUT_RIGHT_BUTTON);
#endif // GLUT_SUPPORT

  if (!cb_data_s.texture.load (textureFilePath_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Texture::load(\"%s\"), returning\n"),
                ACE_TEXT (textureFilePath_in.c_str ())));
    return;
  } // end IF

  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  std::string vertex_shader_file_path = path_root;
  vertex_shader_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  vertex_shader_file_path += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  vertex_shader_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  vertex_shader_file_path +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_OPENGL_VERTEX_SHADER_FILENAME);
  std::string fragment_shader_file_path = path_root;
  fragment_shader_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  fragment_shader_file_path += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  fragment_shader_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  fragment_shader_file_path +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_OPENGL_FRAGMENT_SHADER_FILENAME);
  if (!cb_data_s.shader.loadFromFile (vertex_shader_file_path,
                                      fragment_shader_file_path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Shader::loadFromFile(\"%s\",\"%s\"), returning\n"),
                ACE_TEXT (vertex_shader_file_path.c_str ()),
                ACE_TEXT (fragment_shader_file_path.c_str ())));
    return;
  } // end IF
  cb_data_s.shader.use ();

  glGenVertexArrays (1, &cb_data_s.VAO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (cb_data_s.VAO);
  glGenBuffers (1, &cb_data_s.VBO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (cb_data_s.VBO);
  glGenBuffers (1, &cb_data_s.EBO);
  COMMON_GL_ASSERT;
  ACE_ASSERT (cb_data_s.EBO);

  glBindVertexArray (cb_data_s.VAO);
  COMMON_GL_ASSERT;

  glBindBuffer (GL_ARRAY_BUFFER, cb_data_s.VBO);
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

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, cb_data_s.EBO);
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

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;
  glBindBuffer (GL_ARRAY_BUFFER, 0);
  COMMON_GL_ASSERT;
  glBindVertexArray (0);
  COMMON_GL_ASSERT;

  cb_data_s.tp1 = std::chrono::high_resolution_clock::now ();

#if defined (GLUT_SUPPORT)
  glutMainLoop ();

  glutDestroyWindow (window_i); window_i = -1;
#endif // GLUT_SUPPORT
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  int result = EXIT_FAILURE, result_2 = -1;

  // step0: initialize
#if defined (GLUT_SUPPORT)
  glutInit (&argc_in, argv_in);
#endif // GLUT_SUPPORT
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
  Common_Tools::initialize (false, // COM ?
                            true); // RNG ?
#else
  Common_Tools::initialize (true); // RNG ?
#endif // ACE_WIN32 || ACE_WIN64
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  std::string texture_file_path = path_root;
  texture_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  texture_file_path += COMMON_LOCATION_DATA_SUBDIRECTORY;
  texture_file_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  texture_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_OPENGL_TEXTURE_FILENAME);
  bool trace_information = false;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             texture_file_path,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (texture_file_path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])),                            // program name
                                            Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                                                              Common_File_Tools::executable),             // log file name
                                            false,                                                                        // log to syslog ?
                                            false,                                                                        // trace messages ?
                                            trace_information,                                                            // debug messages ?
                                            NULL))                                                                        // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
           texture_file_path);
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
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
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
  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
