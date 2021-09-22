#include "stdafx.h"

#include <cmath>
#include <iostream>
#include <string>

#include "gtk/gtk.h"
#if GTK_CHECK_VERSION(3,16,0)
#else
#if defined (GTKGL_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#include "gl/GLU.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gtkglarea.h"
#endif // GTKGLAREA_SUPPORT
#endif // GTKGL_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)

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

#if defined (GTKGL_SUPPORT)
#include "common_gl_defines.h"
#include "common_gl_tools.h"
#endif // GTKGL_SUPPORT

#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "test_i_gtk_callbacks.h"
#include "test_i_gtk_defines.h"

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
  std::stringstream converter;
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
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
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
#if GTK_CHECK_VERSION(3,0,0)
G_MODULE_EXPORT GdkGLContext*
glarea_create_context_cb (GtkGLArea* GLArea_in,
                          gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea create context\n")));
  return NULL;
}

G_MODULE_EXPORT gboolean
glarea_render_cb (GtkGLArea* area_in,
                  GdkGLContext* context_in,
                  gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea render\n")));
  return true;
}

G_MODULE_EXPORT void
glarea_resize_cb (GtkGLArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea resize\n")));
}
#elif GTK_CHECK_VERSION(2,0,0)
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
  const GLdouble pi = 3.1415926535897932384626433832795;
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
    filename += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
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
    event_in->configure.width / (GLdouble)event_in->configure.height,
    0.1,
    100.0); // Calculate The Aspect Ratio Of The Window
#else
  const GLdouble pi = 3.1415926535897932384626433832795;
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
  return 0;
}

G_MODULE_EXPORT gint
button_quit_clicked_cb (GtkWidget* widget,
                        gpointer callback_data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button quit clicked\n")));
  return 0;
}

G_MODULE_EXPORT void
on_close_cb (GtkDialog* widget,
             gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("close event\n")));
}
G_MODULE_EXPORT void
on_response_cb (GtkDialog* widget,
                gint response_id,
                gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("response event\n")));
}

G_MODULE_EXPORT gboolean
on_delete_event_cb (GtkWidget* widget,
                    GdkEvent* event,
                    gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("delete-event event\n")));
  return FALSE; // emit 'detroy'
}
#if GTK_CHECK_VERSION(4,0,0)
G_MODULE_EXPORT void
on_activate_cb (GtkWidget* widget,
                gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("activate event\n")));

  GtkWidget* mainwin = (GtkWidget*)data;
  gtk_widget_show (mainwin);
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
  gtk_main_quit ();
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         const std::string& UIDefinitionFilePath_in)
{
#if GTK_CHECK_VERSION(4,0,0)
#undef gtk_init
  gtk_init ();
#else
  gtk_init (&argc_in, &argv_in);
#endif // GTK_CHECK_VERSION(4,0,0)

  GError* error_p = NULL;
  GtkBuilder* gtkBuilder= gtk_builder_new ();
  if (!gtk_builder_add_from_file (gtkBuilder,
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
  GtkWidget* mainwin = GTK_WIDGET (gtk_builder_get_object (gtkBuilder, "dialog_main"));

#if GTK_CHECK_VERSION(4,0,0)
  GtkApplication* app_p =
    gtk_application_new ("org.gnome.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app_p, "activate", G_CALLBACK (on_activate_cb), mainwin);
#else
  //g_signal_handlers_block_matched (mainwin, G_SIGNAL_MATCH_DATA,
  //                                 g_signal_lookup ("delete-event", GTK_TYPE_WIDGET),
  //                                 0, NULL, NULL, NULL);
  gtk_builder_connect_signals (gtkBuilder, NULL);
#endif // GTK_CHECK_VERSION(4,0,0)

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION(3,0,0)
#if GTK_CHECK_VERSION(3,16,0)
  GtkGLArea* gl_area_p =
    GTK_GL_AREA (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_GLAREA_NAME)));
  ACE_ASSERT (gl_area_p);
  gtk_widget_realize (GTK_WIDGET (gl_area_p));
  GdkGLContext* context_p = gtk_gl_area_get_context (gl_area_p);
  ACE_ASSERT (context_p);
#else
#if defined (GTKGLAREA_SUPPORT)
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(3,16,0)
#elif GTK_CHECK_VERSION(2,0,0)
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
  GLuint texture_id = 0;
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                    G_CALLBACK (glarea_expose_event_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("realize"),
                    G_CALLBACK (glarea_realize_cb), &texture_id);
  g_signal_connect (G_OBJECT (gl_area_p), ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (glarea_destroy_cb), &texture_id);
  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p), 256, 256);
#endif // GTKGLAREA_SUPPORT
#else
#endif // GTK_CHECK_VERSION(2,0,0)
  GtkVBox* box_p = GTK_VBOX (gtk_builder_get_object (gtkBuilder, "vbox3"));
  gtk_container_foreach (GTK_CONTAINER (box_p), (GtkCallback)gtk_widget_destroy, NULL);
  gtk_box_pack_start (GTK_BOX (box_p), GTK_WIDGET (gl_area_p), TRUE, TRUE, 0);
#endif // GTKGL_SUPPORT
  g_object_unref (G_OBJECT (gtkBuilder));

#if GTK_CHECK_VERSION(4,0,0)
#else
  gtk_widget_show_all (mainwin);
#endif // GTK_CHECK_VERSION(4,0,0)

#if GTK_CHECK_VERSION(4,0,0)
  g_application_run (G_APPLICATION (app_p), 0, NULL);
  g_object_unref (app_p);
#else
  gtk_main ();
#endif // GTK_CHECK_VERSION(4,0,0)
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  int result = EXIT_FAILURE, result_2 = -1;

  // step0: initialize
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

  Common_Tools::initialize ();

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
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (ui_definition_file_path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]),           // program name
                                            ACE_TEXT_ALWAYS_CHAR (""),            // log file name
                                            false,                                // log to syslog ?
                                            false,                                // trace messages ?
                                            trace_information,                    // debug messages ?
                                            NULL))                                // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
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
