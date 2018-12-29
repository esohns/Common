﻿/***************************************************************************
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

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "X11/Xlib.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION(3,0,0)
#if GTK_CHECK_VERSION(3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#else
#include "gtk/gtkgl.h" // gtkglext
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(3,0,0)
#endif // GTKGL_SUPPORT

#if defined (LIBGLADE_SUPPORT)
#include "glade/glade.h"
#endif // LIBGLADE_SUPPORT

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Thread.h"

#include "common_macros.h"

#include "common_ui_defines.h"

#include "common_ui_gtk_defines.h"
#if defined (GTKGL_SUPPORT)
#include "common_ui_gtk_gl_common.h"
#endif // GTKGL_SUPPORT
#include "common_ui_gtk_tools.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::Common_UI_GTK_Manager_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_EVENT_THREAD_NAME), // thread name
              COMMON_UI_EVENT_THREAD_GROUP_ID,                    // group id
              1,                                                  // # threads
              false)                                              // do NOT auto-start !
 , configuration_ (NULL)
 , CBData_ (NULL)
 , GTKIsInitialized_ (false)
 , state_ ()
 , UIIsInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::Common_UI_GTK_Manager_T"));

}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
bool
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);
  CBData_ = configuration_in.CBData;

//  if (likely (!GTKIsInitialized_))
//  {
//    GTKIsInitialized_ = initializeGTK ();
//    if (unlikely (!GTKIsInitialized_))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Common_UI_GTK_Manager_T::initializeGTK(): \"%m\", aborting\n")));
//      return false;
//    } // end IF
//  } // end IF

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
ACE_thread_t
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::start"));

  int result = inherited::open (NULL);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_TaskBase_T::open(NULL): \"%m\", continuing\n")));

  { ACE_GUARD_RETURN (typename inherited::ITASKCONTROL_T::MUTEX_T, aGuard, inherited::lock_, 0);
    ACE_ASSERT (inherited::threads_.size () == 1);
    return inherited::threads_[0].id ();
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
void
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::stop (bool waitForCompletion_in,
                                                 bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  int result = close (1);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_GTK_Manager_T::close(1): \"%m\", continuing\n")));

  if (waitForCompletion_in)
    inherited::wait ();
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
int
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::close (u_long arg_in)
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
      if (likely (ACE_OS::thr_equal (ACE_Thread::self (),
                                     inherited::last_thread ())))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (unlikely (inherited::thr_count_ == 0))
        return 0; // nothing to do


      // sanity check(s)
      ACE_ASSERT (configuration_);

      // schedule UI finalization
      guint event_source_id = 0;
      if (configuration_->eventHooks.finiHook)
      { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, -1);
        event_source_id = g_idle_add (configuration_->eventHooks.finiHook,
                                      CBData_);
        if (unlikely (!event_source_id))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting")));
          return -1;
        } // end IF
        state_.eventSourceIds.insert (event_source_id);
      } // end lock scope

      if (likely (UIIsInitialized_))
      { ACE_ASSERT (configuration_->interface);
        try {
          configuration_->interface->finalize ();
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Common_UI_IGTK_T::finalize, continuing\n")));
        }
      } // end IF
      else
      {
#if GTK_CHECK_VERSION(3,6,0)
#else
        gdk_threads_enter ();
#endif // GTK_CHECK_VERSION(3,6,0)
        guint level = gtk_main_level ();
        if (level > 0)
          gtk_main_quit ();
#if GTK_CHECK_VERSION(3,6,0)
#else
        gdk_threads_leave ();
#endif // GTK_CHECK_VERSION(3,6,0)
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

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
int
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Error_Tools::setThreadName (inherited::threadName_,
                                     0);
#endif // ACE_WIN32 || ACE_WIN64
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t, group: %d) starting\n"),
              ACE_TEXT (inherited::threadName_.c_str ()),
              inherited::grp_id_));
#endif // _DEBUG

  // sanity check(s)
  ACE_ASSERT (configuration_);

  int result = 0;
  guint event_source_id = 0;
#if GTK_CHECK_VERSION(3,6,0)
#else
  bool leave_gdk_threads = false;
#endif // GTK_CHECK_VERSION(3,6,0)
#if defined (GTKGL_SUPPORT)
  Common_UI_GTK_BuildersIterator_t iterator;
  GtkWidget* widget_p = NULL;
  GError* error_p = NULL;
#if GTK_CHECK_VERSION(3,0,0)
#if GTK_CHECK_VERSION(3,16,0)
  GdkGLContext* context_p = NULL;
#endif // GTK_CHECK_VERSION(3,16,0)
#endif // GTK_CHECK_VERSION(3,0,0)
#if defined (_DEBUG)
  Common_UI_GTK_GLContextsIterator_t iterator_2;
#endif // _DEBUG
#endif // GTKGL_SUPPORT

  // step1: initialize GTK
  if (unlikely (!GTKIsInitialized_))
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

  // step2: initialize UI
  if (likely (!UIIsInitialized_))
  { ACE_ASSERT (configuration_->interface);
    UIIsInitialized_ = configuration_->interface->initialize (state_);
    if (unlikely (!UIIsInitialized_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_IGTK_t::initialize(): \"%m\", aborting\n")));
      result = -1;
      goto done;
    } // end IF
  } // end IF

#if defined (GTKGL_SUPPORT)
  // step3: initialize OpenGL
  // sanity check(s)
  ACE_ASSERT (!state_.builders.empty ());
  iterator =
    state_.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != state_.builders.end ());
  ACE_ASSERT (state_.OpenGLContexts.empty ());
  if (configuration_->widgetName.empty ())
    goto continue_;

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing OpenGL...\n")));
#endif // _DEBUG
  widget_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (configuration_->widgetName.c_str ())));
  ACE_ASSERT (widget_p);
#if GTK_CHECK_VERSION(3,0,0)
#if GTK_CHECK_VERSION(3,16,0)
  context_p =
      gdk_window_create_gl_context (gtk_widget_get_window (widget_p),
                                    &error_p);
  if (unlikely (!context_p || error_p))
  { // *NOTE*: on UNIX, try setting a 'GDK_BACKEND=x11' environment variable
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(%@): \"%s\", aborting\n"),
                gtk_widget_get_window (widget_p),
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto clean;
  } // end IF
  if (unlikely (!gdk_gl_context_realize (context_p,
                                         &error_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", aborting\n"),
                context_p,
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto clean;
  } // end IF
  state_.OpenGLContexts.insert (std::make_pair (static_cast<GtkGLArea*> (NULL),
                                                context_p));
  context_p = NULL;
#else
#if defined (GTKGLAREA_SUPPORT)
  GglaContext* context_p = NULL;
  /* Attribute list for gtkglarea widget. Specifies a list of Boolean attributes
     and enum/integer attribute/value pairs. The last attribute must be
     GGLA_NONE. See glXChooseVisual manpage for further explanation. */
  int attribute_list[] = {
    GGLA_USE_GL,
    GGLA_RGBA,
    GGLA_DOUBLEBUFFER,
    GGLA_RED_SIZE,   1,
    GGLA_GREEN_SIZE, 1,
    GGLA_BLUE_SIZE,  1,
    GGLA_NONE
  };
  GdkVisual* visual_p = ggla_choose_visual (attribute_list);
  if (unlikely (!visual_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ggla_choose_visual(), falling back\n")));

    GdkScreen* screen_p = gdk_screen_get_default ();
    ACE_ASSERT (screen_p);
    GList* list_p = gdk_screen_list_visuals (screen_p);
    GdkVisual* visual_p = NULL;
    for (GList* l = list_p;
         l != NULL;
         l = l->next)
    { ACE_ASSERT (l->data);
      visual_p = static_cast<GdkVisual*> (l->data);
      break;
    } // end FOR
    g_list_free (list_p); list_p = NULL;
    if (!visual_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("could not find visual, aborting\n")));

      g_object_unref (screen_p); screen_p = NULL;
      result = -1;
      goto clean_2;
    } // end IF
    g_object_unref (screen_p); screen_p = NULL;
  } // end IF
  ACE_ASSERT (visual_p);
  context_p = ggla_context_new (visual_p);
  if (unlikely (!context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ggla_context_new(), aborting\n")));
    result = -1;
    goto clean_2;
  } // end IF
  state_.OpenGLContexts.insert (std::make_pair (static_cast<GglaArea*> (NULL),
                                                context_p));
  context_p = NULL;

clean_2:
  if (visual_p)
  {
    g_object_unref (visual_p); visual_p = NULL;
  } // end IF
#else
  // sanity check(s)
  ACE_ASSERT (!state_.openGLContext);
  ACE_ASSERT (state_.openGLWindow);

  state_.openGLContext = gdk_window_create_gl_context (state_.openGLWindow,
                                                       &error_p);
  if (unlikely (!state_.openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(0x%@): \"%s\", aborting\n"),
                state_.openGLWindow,
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto done;
  } // end IF

  error_p = NULL;
  if (unlikely (gdk_gl_context_realize (state_.openGLContext,
                                        &error_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", aborting\n"),
                state_.openGLContext,
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto done;
  } // end IF
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION(3,16,0) */
#else
  // *TODO*: remove type inferences
  if (unlikely (!state_.OpenGLWindow))
    goto continue_;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing OpenGL (window handle: 0x%@)...\n"),
              state_.OpenGLWindow));

#if defined (GTKGLAREA_SUPPORT)
#else
  // sanity check(s)
  ACE_ASSERT (!state_.openGLContext);

  state_.openGLContext = gdk_window_create_gl_context (state_.openGLWindow,
                                                        &error_p);
  if (unlikely (!state_.openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(0x%@): \"%s\", aborting\n"),
                state_.openGLWindow,
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto done;
  } // end IF

  error_p = NULL;
  if (unlikely (gdk_gl_context_realize (state_.openGLContext,
                                        &error_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", aborting\n"),
                state_.openGLContext,
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    result = -1;
    goto done;
  } // end IF
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION(3,0,0) */
clean:
  if (context_p)
  {
    g_object_unref (context_p); context_p = NULL;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing OpenGL...DONE\n")));
#endif // _DEBUG
#endif // GTKGL_SUPPORT

#if defined (GTKGL_SUPPORT)
#if defined (_DEBUG)
  iterator_2 = state_.OpenGLContexts.find (NULL);
  ACE_ASSERT (iterator_2 != state_.OpenGLContexts.end ());
#if GTK_CHECK_VERSION(3,0,0)
  Common_UI_GTK_Tools::dumpGtkOpenGLInfo ((*iterator_2).second);
#else
#if defined (GTKGLAREA_SUPPORT)
  Common_UI_GTK_Tools::dumpGtkOpenGLInfo ((*iterator_2).second);
#else
  Common_UI_GTK_Tools::dumpGtkOpenGLInfo ();
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(3,0,0)
#endif // _DEBUG
#endif // GTKGL_SUPPORT

#if defined (GTKGL_SUPPORT)
continue_:
#endif // GTKGL_SUPPORT
  if (configuration_->eventHooks.initHook)
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, -1);
    event_source_id = g_idle_add (configuration_->eventHooks.initHook,
                                  CBData_);
    if (unlikely (!event_source_id))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));
      result = -1;
      goto done;
    } // end IF
    state_.eventSourceIds.insert (event_source_id);
  } // end IF

#if GTK_CHECK_VERSION(3,6,0)
#else
  if (likely (g_thread_get_initialized ()))
  {
    gdk_threads_enter ();
    leave_gdk_threads = true;
  } // end IF
#endif // GTK_CHECK_VERSION (3,6,0)
  gtk_main ();
#if GTK_CHECK_VERSION(3,6,0)
#else
  if (likely (leave_gdk_threads))
    gdk_threads_leave ();
#endif // GTK_CHECK_VERSION (3,6,0)

  // stop() (close() --> gtk_main_quit ()) was called...

done:
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (inherited::threadName_.c_str ())));
#endif // _DEBUG

  return result;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename StateType,
          typename CallBackDataType>
bool
Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                        ConfigurationType,
                        StateType,
                        CallBackDataType>::initializeGTK ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager_T::initializeGTK"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  u_long process_priority_mask =
    ACE_LOG_MSG->priority_mask (ACE_Log_Msg::PROCESS);

  char* locale_p = ::setlocale (LC_ALL, "");
  if (likely (locale_p))
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("set locale to \"%s\"\n"),
                ACE_TEXT (locale_p)));
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::setlocale(): \"%m\", continuing\n")));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // step0: initialize X
  Status result = XInitThreads ();
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XInitThreads(): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // step1: initialize GLib
#if GTK_CHECK_VERSION(2,36,0)
#else
#if defined (_DEBUG)
  GTypeDebugFlags debug_flags =
    static_cast <GTypeDebugFlags> (//G_TYPE_DEBUG_INSTANCE_COUNT |
                                   G_TYPE_DEBUG_OBJECTS        |
                                   G_TYPE_DEBUG_SIGNALS);
  if (!(process_priority_mask & LM_DEBUG))
    debug_flags = G_TYPE_DEBUG_NONE;
  g_type_init_with_debug_flags (debug_flags);
#else
  g_type_init ();
#endif // _DEBUG
#endif // GTK_CHECK_VERSION(2,36,0)

#if GTK_CHECK_VERSION(2,32,0)
#else
  if (likely (g_thread_supported ())) // *NOTE*: sad but true...
  {
#if defined (_DEBUG)
    g_thread_init_with_errorcheck_mutexes (NULL);
#else
    g_thread_init (NULL);
#endif // _DEBUG
  } // end IF
#endif // GTK_CHECK_VERSION(2,32,0)
#if GTK_CHECK_VERSION(3,6,0)
#else
  gdk_threads_init ();
#endif // GTK_CHECK_VERSION(3,6,0)

#if GTK_CHECK_VERSION(3,0,0)
  GError* error_p = NULL;
#endif // GTK_CHECK_VERSION (3,0,0)
  // step1a: set log handlers
  //g_set_print_handler (glib_print_debug_handler);
  g_set_printerr_handler (glib_print_error_handler);
  g_log_set_default_handler (glib_log_handler, NULL);
  GLogLevelFlags log_flags =
    static_cast <GLogLevelFlags> (G_LOG_FLAG_FATAL    |
                                  G_LOG_FLAG_RECURSION);
  GLogLevelFlags log_level =
    static_cast <GLogLevelFlags> (G_LOG_LEVEL_ERROR    |
                                  G_LOG_LEVEL_CRITICAL |
                                  G_LOG_LEVEL_WARNING  |
                                  G_LOG_LEVEL_MESSAGE  |
                                  G_LOG_LEVEL_INFO     |
                                  G_LOG_LEVEL_DEBUG);
#if defined (_DEBUG)
  if (!(process_priority_mask & LM_DEBUG))
    log_level = static_cast <GLogLevelFlags> (log_level & ~G_LOG_LEVEL_DEBUG);
#endif // _DEBUG
  TASK_T* user_data_p = this;
  g_log_set_handler (G_LOG_DOMAIN,
                     static_cast <GLogLevelFlags> (log_flags | log_level),
                     glib_log_handler, user_data_p);

#if GTK_CHECK_VERSION(3,6,0)
#else
  bool leave_gdk_threads = false;
  gdk_threads_enter ();
  leave_gdk_threads = true;
#endif // GTK_CHECK_VERSION(3,6,0)
  int i = 1;

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION(3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  if (unlikely (!gdk_gl_init_check (&argc_,
                                    &argv_)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_init_check(), aborting\n")));
    goto error;
  } // end IF
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // GTKGL_SUPPORT

//  GdkDisplay* display_p =
//      gdk_display_manager_open_display (gdk_display_manager_get (),
//                                        ACE_TEXT_ALWAYS_CHAR ("wayland-0"));
//  ACE_ASSERT (display_p);
//  gdk_display_manager_set_default_display (gdk_display_manager_get (),
//                                           display_p);

  if (unlikely (!gtk_init_check (&configuration_->argc,
                                 &configuration_->argv)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_init_check(), aborting\n")));
    goto error;
  } // end IF
//  gtk_init (&argc_,
//            &argv_);
//  GOptionEntry entries_a[] = { {NULL} };
//  if (unlikely (!gtk_init_with_args (&argc_,     // argc
//                                     &argv_,     // argv
//                                     NULL,       // parameter string
//                                     entries_a,  // entries
//                                     NULL,       // translation domain
//                                     &error_p))) // error
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_init_with_args(): \"%s\", aborting\n"),
//                ACE_TEXT (error_p->message)));
//    g_error_free (error_p); error_p = NULL;
//    goto error;
//  } // end IF

#if defined (_DEBUG)
  Common_UI_GTK_Tools::dumpGtkLibraryInfo ();
#endif // _DEBUG

#if defined (LIBGLADE_SUPPORT)
  // step2: initialize (lib)glade
  glade_init ();
#endif // LIBGLADE_SUPPORT

  // step3a: specify any .rc files
  for (Common_UI_GTK_RCFilesIterator_t iterator = configuration_->RCFiles.begin ();
       iterator != configuration_->RCFiles.end ();
       ++iterator, ++i)
  {
    gtk_rc_add_default_file ((*iterator).c_str ());
//      gtk_rc_add_default_file_utf8 ((*iterator).c_str ());
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("#%u: added GTK .rc style file \"%s\"\n"),
                i, ACE::basename ((*iterator).c_str (), ACE_DIRECTORY_SEPARATOR_CHAR)));
#endif // _DEBUG
  } // end FOR

#if GTK_CHECK_VERSION(3,0,0)
  // step3b: specify any .css files
  i = 1;
  for (Common_UI_GTK_CSSProvidersIterator_t iterator = configuration_->CSSProviders.begin ();
       iterator != configuration_->CSSProviders.end ();
       ++iterator, ++i)
  { ACE_ASSERT (!(*iterator).second);
    (*iterator).second = gtk_css_provider_new ();
    if (unlikely (!(*iterator).second))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to gtk_css_provider_new(), aborting\n")));
      goto error;
    } // end IF

    if (unlikely (!gtk_css_provider_load_from_path ((*iterator).second,
                                                    (*iterator).first.c_str (),
                                                    &error_p)))
    { ACE_ASSERT (error_p);
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_css_provider_load_from_path(\"%s\"): \"%s\", continuing\n"),
                  ACE_TEXT ((*iterator).first.c_str ()),
                  ACE_TEXT (error_p->message)));
      g_object_unref ((*iterator).second); (*iterator).second = NULL;
      g_error_free (error_p); error_p = NULL;
      continue;
    } // end IF
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("#%u: added GTK .css style file \"%s\"\n"),
                i, ACE::basename ((*iterator).first.c_str ())));
#endif // _DEBUG
  } // end FOR
#endif // GTK_CHECK_VERSION (3,0,0)

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

#if GTK_CHECK_VERSION(3,6,0)
#else
  gdk_threads_leave ();
#endif // GTK_CHECK_VERSION(3,6,0)

  return true;

error:
#if GTK_CHECK_VERSION(3,6,0)
#else
  if (leave_gdk_threads)
    gdk_threads_leave ();
#endif // GTK_CHECK_VERSION(3,6,0)

  return false;
}
