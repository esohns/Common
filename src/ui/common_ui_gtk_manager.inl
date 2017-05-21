/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Log_Priority.h>
#include <ace/OS.h>
#include <ace/Thread.h>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include <X11/Xlib.h>
#endif

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#include <gtkgl/gdkgl.h>
#endif /* gtk >= 3 && <= 3.16 */
#else
#if defined (GTKGLAREA_SUPPORT)
#include <gtkgl/gdkgl.h>
#else
#include <gtk/gtkgl.h> // gtkglext
#endif
#endif
#endif

#if defined (LIBGLADE_SUPPORT)
#include <glade/glade.h>
#endif

#include "common_macros.h"
#include "common_timer_manager.h"

#include "common_ui_defines.h"
#include "common_ui_igtk.h"
#include "common_ui_tools.h"

template <typename StateType>
Common_UI_GTK_Manager_T<StateType>::Common_UI_GTK_Manager_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_THREAD_NAME), // thread name
              COMMON_UI_GTK_THREAD_GROUP_ID,                    // group id
              1,                                                // # threads
              false)                                            // do NOT auto-start !
 , argc_ (0)
 , argv_ (NULL)
 , GTKIsInitialized_ (false)
 , isInitialized_ (false)
//#if defined (GTKGL_SUPPORT)
// , openGLContext_ (NULL)
//#endif
 , state_ (NULL)
 , UIInterfaceHandle_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::Common_UI_GTK_Manager_T"));

}

template <typename StateType>
Common_UI_GTK_Manager_T<StateType>::~Common_UI_GTK_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::~Common_UI_GTK_Manager_T"));

}

template <typename StateType>
bool
Common_UI_GTK_Manager_T<StateType>::initialize (int argc_in,
                                                ACE_TCHAR** argv_in,
                                                StateType* state_in,
                                                Common_UI_IGTK_T<StateType>* interfaceHandle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (state_in);

  argc_ = argc_in;
  argv_ = argv_in;
  state_ = state_in;
  UIInterfaceHandle_ = interfaceHandle_in;

  if (!GTKIsInitialized_)
  {
    GTKIsInitialized_ = initializeGTK ();
    if (!GTKIsInitialized_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_GTK_Manager_T::initializeGTK(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  return true;
}

template <typename StateType>
void
Common_UI_GTK_Manager_T<StateType>::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::start"));

  int result = inherited::open (NULL);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_TaskBase_T::open(NULL): \"%m\", continuing\n")));
}

template <typename StateType>
void
Common_UI_GTK_Manager_T<StateType>::stop (bool waitForCompletion_in,
                                          bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  int result = close (1);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_GTK_Manager_T::close(1): \"%m\", continuing\n")));

  if (waitForCompletion_in)
  {
    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
  } // end IF
}

template <typename StateType>
int
Common_UI_GTK_Manager_T<StateType>::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::close"));

  // *NOTE*: this method may be invoked
  //         - by an external thread closing down the active object
  //           (arg_in == 1 !)
  //         - by the worker thread which calls this after returning from svc()
  //           (arg_in == 0 !) --> in this case, this should be a NOP
  switch (arg_in)
  {
    case 0:
    { // check specifically for the second case
      if (ACE_OS::thr_equal (ACE_Thread::self (),
                             inherited::last_thread ()))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count_ == 0)
        return 0; // nothing to do

      if (UIInterfaceHandle_)
      {
        try {
          UIInterfaceHandle_->finalize ();
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Common_UI_IGTK_T::finalize, continuing\n")));
        }
      } // end IF
      else
      {
        gdk_threads_enter ();
        guint level = gtk_main_level ();
        if (level > 0)
          gtk_main_quit ();
        gdk_threads_leave ();
      } // end ELSE

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument: %u, aborting\n"),
                  arg_in));
      return -1;
    }
  } // end SWITCH

  return 0;
}

template <typename StateType>
int
Common_UI_GTK_Manager_T<StateType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::svc"));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) GTK event dispatch starting\n")));

  int result = 0;
  bool leave_gdk_threads = false;
#if defined (GTKGL_SUPPORT)
//  GError* error_p = NULL;
#endif

  // step0: initialize GTK
  if (!GTKIsInitialized_)
  {
    GTKIsInitialized_ = initializeGTK ();
    if (!GTKIsInitialized_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_GTK_Manager_T::initializeGTK(): \"%m\", aborting\n")));

      result = -1;

      goto done;
    } // end IF
  } // end IF

  // sanity check(s)
  ACE_ASSERT (state_);

  // step1: initialize UI
  if (!isInitialized_ && UIInterfaceHandle_)
  {
    isInitialized_ = UIInterfaceHandle_->initialize (*state_);
    if (!isInitialized_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_IGTK_t::initialize() (errno was: \"%m\"), aborting\n")));

      result = -1;

      goto done;
    } // end IF
  } // end IF

  // step2: initialize OpenGL
#if defined (GTKGL_SUPPORT)
  // *TODO*: remove type inferences
  if (!state_->OpenGLWindow)
    goto continue_;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing OpenGL (window: 0x%@)...\n"),
              state_->OpenGLWindow));

#if defined (GTKGLAREA_SUPPORT)
#else
  // sanity check(s)
  ACE_ASSERT (!state_->openGLContext);

  state_->openGLContext = gdk_window_create_gl_context (state_->openGLWindow,
                                                        &error_p);
  if (!state_->openGLContext)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(0x%@): \"%s\", aborting\n"),
                state_->openGLWindow,
                ACE_TEXT (error_p->message)));

    // clean up
    g_error_free (error_p);

    return false;
  } // end IF

  error_p = NULL;
  if (gdk_gl_context_realize (state_->openGLContext,
                              &error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", aborting\n"),
                state_->openGLContext,
                ACE_TEXT (error_p->message)));

    // clean up
    g_error_free (error_p);

    return false;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing OpenGL...DONE\n")));
#endif /* GTKGLAREA_SUPPORT */

#if defined (_DEBUG)
    // debug info
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#if defined (GTKGL_SUPPORT)
#if defined (GTKGLAREA_SUPPORT)
#else
  Common_UI_Tools::OpenGLInfo (state_->openGLContext);
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTKGL_SUPPORT */
#else
  Common_UI_Tools::OpenGLInfo ();
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  Common_UI_Tools::OpenGLInfo ();
#else
  Common_UI_Tools::OpenGLInfo (NULL);
#endif
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // _DEBUG
#endif

#if defined (GTKGL_SUPPORT)
continue_:
#endif
  if (g_thread_get_initialized ())
  {
    gdk_threads_enter ();
    leave_gdk_threads = true;
  } // end IF
  gtk_main ();
  if (leave_gdk_threads)
    gdk_threads_leave ();

  // stop() (close() --> gtk_main_quit ()) was called...

done:
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%t) leaving GTK event dispatch\n")));

  return result;
}

template <typename StateType>
bool
Common_UI_GTK_Manager_T<StateType>::initializeGTK ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::initializeGTK"));

  // sanity check(s)
  ACE_ASSERT (state_);

  u_long process_priority_mask =
    ACE_LOG_MSG->priority_mask (ACE_Log_Msg::PROCESS);

  char* locale_p = ::setlocale (LC_ALL, "");
  if (locale_p)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("set locale to \"%s\"...\n"),
                ACE_TEXT (locale_p)));
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::setlocale(): \"%m\", continuing\n")));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // step0: initialize X
  Status result = XInitThreads ();
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XInitThreads(): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif

  // step1: initialize GLib
  //GTypeDebugFlags debug_flags =
  //  static_cast <GTypeDebugFlags> (G_TYPE_DEBUG_OBJECTS       |
  //                                 G_TYPE_DEBUG_SIGNALS       |
  //                                 G_TYPE_DEBUG_INSTANCE_COUNT);
  //if (!(process_priority_mask & LM_DEBUG))
  //  debug_flags = G_TYPE_DEBUG_NONE;
  //g_type_init_with_debug_flags (debug_flags);
  g_type_init ();

  // step1a: set log handlers
  //g_set_print_handler (glib_print_debug_handler);
  g_set_printerr_handler (glib_print_error_handler);
  //g_log_set_default_handler (glib_log_handler, NULL);
  GLogLevelFlags log_level =
    static_cast <GLogLevelFlags> (G_LOG_FLAG_FATAL     |
                                  G_LOG_FLAG_RECURSION |
                                  G_LOG_LEVEL_ERROR    |
                                  G_LOG_LEVEL_CRITICAL |
                                  G_LOG_LEVEL_WARNING  |
                                  G_LOG_LEVEL_MESSAGE  |
                                  G_LOG_LEVEL_INFO     |
                                  G_LOG_LEVEL_DEBUG);
  if (!(process_priority_mask & LM_DEBUG))
    log_level = static_cast <GLogLevelFlags> (log_level & ~G_LOG_LEVEL_DEBUG);
  g_log_set_handler (G_LOG_DOMAIN,
                     log_level,
                     glib_log_handler, NULL);

#if GTK_CHECK_VERSION (3,0,0)
  GError* error_p = NULL;
#else
  if (!g_thread_supported ())
  {
    g_thread_init (NULL);
    //g_thread_init_with_errorcheck_mutexes (NULL);
  } // end IF
#endif
  gdk_threads_init ();

  bool leave_gdk_threads = false;
  gdk_threads_enter ();
  leave_gdk_threads = true;

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  if (!gdk_gl_init_check (&argc_,
                          &argv_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_init_check(), aborting\n")));
    goto error;
  } // end IF
#endif
#endif
#endif

  if (!gtk_init_check (&argc_,
                       &argv_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_init_check(), aborting\n")));
    goto error;
  } // end IF
//  gtk_init (&argc_,
//            &argv_);
//  GOptionEntry entries_a[] = { {NULL} };
//  if (!gtk_init_with_args (&argc_,    // argc
//                           &argv_,    // argv
//                           NULL,      // parameter string
//                           entries_a, // entries
//                           NULL,      // translation domain
//                           &error_p)) // error
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_init_with_args(): \"%s\", aborting\n"),
//                ACE_TEXT (error_p->message)));

//    // clean up
//    g_error_free (error_p);

//    goto error;
//  } // end IF

#if defined (_DEBUG)
  // debug info
  Common_UI_Tools::GtkInfo ();
#endif

#if defined (LIBGLADE_SUPPORT)
  // step2: initialize (lib)glade
  glade_init ();
#endif

  // step3a: specify any .rc files
  if (!state_->RCFiles.empty ())
  {
    int i = 1;
    for (Common_UI_GTKRCFilesIterator_t iterator = state_->RCFiles.begin ();
         iterator != state_->RCFiles.end ();
         ++iterator, ++i)
    {
      gtk_rc_add_default_file ((*iterator).c_str ());
//      gtk_rc_add_default_file_utf8 ((*iterator).c_str ());
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("#%u: added GTK .rc file \"%s\"...\n"),
                  i, ACE::basename ((*iterator).c_str (), '/')));
    } // end FOR
  } // end FOR

#if GTK_CHECK_VERSION (3,0,0)
  // step3b: specify any .css files
  if (!state_->CSSProviders.empty ())
  {
    int i = 1;
    for (Common_UI_GTKCSSProvidersIterator_t iterator = state_->CSSProviders.begin ();
         iterator != state_->CSSProviders.end ();
         ++iterator, ++i)
    {
      ACE_ASSERT (!(*iterator).second);
      (*iterator).second = gtk_css_provider_new ();
      if (!(*iterator).second)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to gtk_css_provider_new(), aborting\n")));
        goto error;
      } // end IF

      if (!gtk_css_provider_load_from_path ((*iterator).second,
                                            (*iterator).first.c_str (),
                                            &error_p))
      { ACE_ASSERT (error_p);
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_css_provider_load_from_path(\"%s\"): \"%s\", continuing\n"),
                    ACE_TEXT ((*iterator).first.c_str ()),
                    ACE_TEXT (error_p->message)));

        // clean up
        g_object_unref ((*iterator).second);
        (*iterator).second = NULL;
        g_error_free (error_p);

        continue;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("#%u: added GTK .css file \"%s\"...\n"),
                  i, ACE::basename ((*iterator).first.c_str ())));
    } // end FOR
  } // end FOR
#endif

  //// step5: initialize GNOME
  //   GnomeClient* gnomeSession = NULL;
  //   gnomeSession = gnome_client_new();
  //   ACE_ASSERT(gnomeSession);
  //   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
  //  GnomeProgram* gnomeProgram = NULL;
  //  gnomeProgram = gnome_program_init (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GNOME_APPLICATION_ID), // app ID
  //#ifdef HAVE_CONFIG_H
  ////                                     ACE_TEXT_ALWAYS_CHAR (VERSION),    // version
  //                                    ACE_TEXT_ALWAYS_CHAR (RPG_VERSION),   // version
  //#else
  //	                                NULL,
  //#endif
  //                                    LIBGNOMEUI_MODULE,                   // module info
  //                                    argc_in,                             // cmdline
  //                                    argv_in,                             // cmdline
  //                                    NULL);                               // property name(s)
  //  ACE_ASSERT(gnomeProgram);

  gdk_threads_leave ();

  return true;

error:
  if (leave_gdk_threads)
    gdk_threads_leave ();

  return false;
}
