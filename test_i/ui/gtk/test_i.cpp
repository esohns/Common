#include "stdafx.h"

#include <iostream>
#include <string>

#include "gtk/gtk.h"

#include "ace/config-lite.h"
#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Init_ACE.h"
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
//#include "ace/Synch.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_tools.h"

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
void
togglebutton_record_toggled_cb (GtkToggleButton* toggleButton_in,
                                gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("togglebutton toggled\n")));
}

void
combobox_source_changed_cb (GtkWidget* widget_in,
                            gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("combobox changed\n")));
}

void
combobox_source_2_changed_cb (GtkWidget* widget_in,
                              gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("combobox changed\n")));
}

GdkGLContext*
glarea_create_context_cb (GtkGLArea* GLArea_in,
                          gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea create context\n")));
  return NULL;
}

gboolean
glarea_render_cb (GtkGLArea* area_in,
                  GdkGLContext* context_in,
                  gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea render\n")));
  return true;
}

void
glarea_resize_cb (GtkGLArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("glarea resize\n")));
}

gint
button_about_clicked_cb (GtkWidget* widget,
                         gpointer callback_data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button about clicked\n")));
  return 0;
}

gint
button_quit_clicked_cb (GtkWidget* widget,
                        gpointer callback_data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button quit clicked\n")));
  return 0;
}

gboolean
on_delete_cb (GtkWidget* widget,
              GdkEvent* event,
              gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("delete event\n")));
  return TRUE;
}
#if GTK_CHECK_VERSION(4,0,0)
void
on_activate_cb (GtkWidget* widget,
                gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("activate event\n")));

  GtkWidget* mainwin = (GtkWidget*)data;
  gtk_widget_show (mainwin);
}
#else
void
on_destroy_cb (GtkWidget* widget,
               gpointer data)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("destroy event\n")));

  gtk_main_quit ();
}
#endif // GTK_CHECK_VERSION (4,0,0)
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
  g_object_unref (G_OBJECT (gtkBuilder));

#if GTK_CHECK_VERSION(4,0,0)
  GtkApplication* app_p =
    gtk_application_new ("org.gnome.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app_p, "activate", G_CALLBACK (on_activate_cb), mainwin);
#else
  gtk_builder_connect_signals (gtkBuilder, NULL);
#endif // GTK_CHECK_VERSION(4,0,0)

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
