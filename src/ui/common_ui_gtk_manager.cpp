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
#include "stdafx.h"

#include "common_ui_gtk_manager.h"

#include "ace/OS.h"
#include "ace/Thread.h"
#include "ace/Log_Msg.h"

#include "gtk/gtk.h"

#include "common_macros.h"
#include "common_timer_manager.h"

#include "common_ui_defines.h"
#include "common_ui_igtk.h"

Common_UI_GTK_Manager::Common_UI_GTK_Manager ()
 : inherited (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_THREAD_NAME), // thread name
              COMMON_UI_GTK_THREAD_GROUP_ID,                    // group id
              1,                                                // # threads
              false)                                            // do NOT auto-start !
 , GTKIsInitialized_ (false)
 , argc_ (0)
 , argv_ (NULL)
 , UIDefinitionFile_ ()
 , interfaceHandle_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::Common_UI_GTK_Manager"));

}

Common_UI_GTK_Manager::~Common_UI_GTK_Manager ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::~Common_UI_GTK_Manager"));

}

void
Common_UI_GTK_Manager::initialize (int argc_in,
                                   ACE_TCHAR** argv_in,
                                   const std::string& filename_in,
                                   Common_UI_IGTK* interfaceHandle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::initialize"));

  argc_ = argc_in;
  argv_ = argv_in;
  UIDefinitionFile_ = filename_in;
  interfaceHandle_ = interfaceHandle_in;
}

void
Common_UI_GTK_Manager::start ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::start"));

  inherited::open (NULL);
}

void
Common_UI_GTK_Manager::stop (bool lockedAccess_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  if (close (0) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_GTK_Manager::close(0): \"%m\", continuing\n")));

  if (inherited::wait () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

bool
Common_UI_GTK_Manager::isRunning () const
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::isRunning"));

  return (inherited::thr_count () > 0);
}

int
Common_UI_GTK_Manager::close (u_long arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::close"));

  // *NOTE*: this method may be invoked
  // - by an external thread closing down the active object
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    case 0:
    { // check specifically for the second case...
      if (ACE_OS::thr_equal (ACE_Thread::self (),
                             inherited::last_thread ()))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count () == 0)
        return 0; // nothing to do

      if (interfaceHandle_)
      {
        try
        {
          interfaceHandle_->finalize ();
        }
        catch (...)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Common_UI_IInitGTK::finalize, continuing\n")));
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

int
Common_UI_GTK_Manager::svc (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Manager::svc"));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) GTK event dispatch starting...\n")));

  int result = 0;

  if (!GTKIsInitialized_)
  {
    // step1: init GTK
    //#if defined (ACE_WIN32) || defined (ACE_WIN64)
    //		g_thread_init (NULL);
    //#endif
    //		gdk_threads_init ();
    //		gdk_threads_enter ();
    if (!gtk_init_check (&argc_,
                         &argv_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_init_check(): \"%m\", aborting\n")));

      //// clean up
      //gdk_threads_leave ();

      result = -1;

      goto done;
    } // end IF
    //// step2: init GNOME
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

    // step3: init client window
    if (interfaceHandle_)
    {
      bool result = false;
      try
      {
        result = interfaceHandle_->initialize (UIDefinitionFile_);
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_UI_IInitGTK::initialize, aborting\n")));

        //// clean up
        //gdk_threads_leave ();

        result = -1;

        goto done;
      }
      if (!result)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize GTK UI, aborting\n")));

        //// clean up
        //gdk_threads_leave ();

        result = -1;

        goto done;
      } // end IF
    } // end IF

    GTKIsInitialized_ = true;
  } // end IF

  gtk_main ();
  //gdk_threads_leave ();

  // gtk_main_quit () has been called...

done:
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%t) leaving GTK event dispatch...\n")));

  return result;
}
