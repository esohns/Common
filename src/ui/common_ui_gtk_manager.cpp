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

#include "ace/Synch.h"
#include "common_ui_gtk_manager.h"

void
glib_log_handler (const gchar* logDomain_in,
                  GLogLevelFlags logLevel_in,
                  const gchar* message_in,
                  gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::glib_log_handler"));

  ACE_UNUSED_ARG (logDomain_in);
  ACE_UNUSED_ARG (userData_in);

  // translate loglevel
  ACE_Log_Priority log_priority = LM_ERROR;
  switch (logLevel_in & G_LOG_LEVEL_MASK)
  {
    case G_LOG_LEVEL_ERROR:
      break;
    case G_LOG_LEVEL_CRITICAL:
      log_priority = LM_CRITICAL;
      break;
    case G_LOG_LEVEL_WARNING:
      log_priority = LM_WARNING;
      break;
    case G_LOG_LEVEL_MESSAGE:
      log_priority = LM_NOTICE;
      break;
    case G_LOG_LEVEL_INFO:
      log_priority = LM_INFO;
      break;
    case G_LOG_LEVEL_DEBUG:
      log_priority = LM_DEBUG;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown GLib log level (was: %d), continuing"),
                  logLevel_in));
      break;
    }
  } // end SWITCH

  ACE_DEBUG ((log_priority,
              ACE_TEXT ("GLib: %s\n"),
              ACE_TEXT (message_in)));
}

//////////////////////////////////////////

//*TODO*: template instantiations should be defined in the corresponding
//        library/executable translation unit(s)
//#if defined (GTKGL_SUPPORT)
//COMMON_UI_SINGLETON_DECLARE (ACE_Singleton,
//                             Common_UI_GTK_Manager_T<struct Common_UI_GTKGLState>,
//                             typename ACE_MT_SYNCH::RECURSIVE_MUTEX);
//#endif
//COMMON_UI_SINGLETON_DECLARE (ACE_Singleton,
//                             Common_UI_GTK_Manager_T<struct Common_UI_GTKState>,
//                             typename ACE_MT_SYNCH::RECURSIVE_MUTEX);
