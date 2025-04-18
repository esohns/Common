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

#include "common_ui_wxwidgets_logger.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_ui_wxwidgets_tools.h"

Common_UI_WxWidgets_Logger::~Common_UI_WxWidgets_Logger ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Logger::~Common_UI_WxWidgets_Logger"));

  // *IMPORTANT NOTE*: currently, wxWidget applications behave strangely in that
  //                   wxEntryStart() delete's the active wxLog
  //                   (see: init.cpp:366)
  // --> if 'this' happens to be the logger singleton, reset
  if (this == Common_UI_WxWidgets_Tools::logger)
  {
    Common_UI_WxWidgets_Tools::logger = NULL;
    if (unlikely (!Common_UI_WxWidgets_Tools::initializeLogging ()))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_WxWidgets_Tools::initializeLogging(), continuing\n")));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("reset wxwidgets logger\n")));
  } // end IF
}

void
Common_UI_WxWidgets_Logger::DoLogTextAtLevel (wxLogLevel logLevel_in,
                                              const wxString& message_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Logger::DoLogTextAtLevel"));

  // translate loglevel
  ACE_Log_Priority log_priority = LM_ERROR;
  switch (logLevel_in)
  {
    case wxLOG_FatalError:
      log_priority = LM_CRITICAL; break;
    case wxLOG_Error:
      break;
    case wxLOG_Warning:
      log_priority = LM_WARNING; break;
    case wxLOG_Message:
    case wxLOG_Status:
      log_priority = LM_NOTICE; break;
    case wxLOG_Info:
      log_priority = LM_INFO; break;
    case wxLOG_Debug:
      log_priority = LM_DEBUG; break;
    case wxLOG_Trace:
    case wxLOG_Progress:
      log_priority = LM_TRACE; break;
    case wxLOG_User:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown wxwidgets loglevel (was: %d), returning\n"),
                  logLevel_in));
      return;
    }
  } // end SWITCH

  u_long priority_mask_i = ACE_LOG_MSG->priority_mask (ACE_Log_Msg::PROCESS);
  if (((log_priority == LM_DEBUG) &&
       !(priority_mask_i & LM_DEBUG)) ||
      ((log_priority == LM_TRACE) &&
       !(priority_mask_i & LM_TRACE)))
    return; // don't log

#if wxCHECK_VERSION(3,0,0)
  ACE_DEBUG ((log_priority,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (message_in.ToStdString ().c_str ())));
#elif wxCHECK_VERSION(2,0,0)
  ACE_DEBUG ((log_priority,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (message_in.c_str ())));
#endif // wxCHECK_VERSION
}
