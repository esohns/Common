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

#include "ace/Synch.h"
#include "common_ui_wxwidgets_tools.h"

#include "wx/artprov.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_ui_wxwidgets_logger.h"

// initialize static variables
Common_UI_WxWidgets_Logger* Common_UI_WxWidgets_Tools::logger = NULL;

bool
Common_UI_WxWidgets_Tools::initialize (int argc_in,
                                       ACE_TCHAR** argv_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::initialize"));

//  if (!wxInitialize (argc_in,
//#if defined (ACE_USES_WCHAR)
//                     Common_UI_WxWidgets_Tools::convertArgV (argc_in,
//                                                             argv_in)))
//#else
//                     argv_in))
//#endif // ACE_USES_WCHAR
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to wxInitialize(), aborting\n")));
//    return false;
//  } // end IF

  if (unlikely (!Common_UI_WxWidgets_Tools::initializeLogging ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_WxWidgets_Tools::initializeLogging(), aborting\n")));
    return false;
  } // end IF

  wxModule::RegisterModules ();
  if (unlikely (!wxModule::InitializeModules ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to wxModule::InitializeModules(), aborting\n")));
    return false;
  } // end IF

  //wxArtProvider::InitStdProvider ();

  wxIdleEvent::SetMode (wxIDLE_PROCESS_SPECIFIED);

  return true;
}
bool
Common_UI_WxWidgets_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::finalize"));

  Common_UI_WxWidgets_Tools::finalizeLogging ();

  wxModule::CleanUpModules ();

  wxUninitialize();

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
wxChar**
Common_UI_WxWidgets_Tools::convertArgV (int argc_in,
                                        ACE_TCHAR** argv_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::convertArgV"));

  // initialize return value(s)
  wxChar** result_p = NULL;

  // sanity check(s)
  ACE_ASSERT (argc_in && argv_in);

  // *NOTE*: the standard stipulates a trailing 0 --> add 1 to argc
  ACE_NEW_NORETURN (result_p,
                    wxChar*[argc_in + 1]);
  if (!result_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return NULL;
  } // end IF
  wxString wx_string;
  wxChar* string_p = NULL;
  for (int i = 0;
       i < argc_in;
       ++i)
  {
    wx_string = wxString::FromAscii (ACE_TEXT_ALWAYS_CHAR (argv_in[i]));
    string_p = NULL;
    ACE_NEW_NORETURN (string_p,
                      wxChar[ACE_OS::strlen (wx_string.wc_str ()) + 1]);
    if (!string_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      for (int j = i - 1; j >= 0; --j)
      { delete [] result_p[j]; result_p[j] = NULL; } // end FOR
      delete [] result_p; result_p = NULL;
      return NULL;
    } // end IF
    ACE_OS::strcpy (string_p, wx_string.wc_str ());
    result_p[i] = string_p;
  } // end FOR
  result_p[argc_in] = NULL;

  return result_p;
}
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_UI_WxWidgets_Tools::initializeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::initializeLogging"));

  wxLogLevel log_level_u = wxLOG_Info;
#if defined (_DEBUG)
  log_level_u = wxLOG_Debug;
#endif // _DEBUG
  wxLog::SetLogLevel (log_level_u);

  if (!Common_UI_WxWidgets_Tools::logger)
  {
    ACE_NEW_NORETURN (Common_UI_WxWidgets_Tools::logger,
                      Common_UI_WxWidgets_Logger ());
    if (!Common_UI_WxWidgets_Tools::logger)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF
  wxLog* log_p = wxLog::SetActiveTarget (Common_UI_WxWidgets_Tools::logger);
  ACE_UNUSED_ARG (log_p);

  return true;
}
void
Common_UI_WxWidgets_Tools::finalizeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::finalizeLogging"));

  // sanity check(s)
  if (!Common_UI_WxWidgets_Tools::logger)
    return;

  wxLog* log_p = wxLog::SetActiveTarget (NULL);
  ACE_ASSERT (log_p);
  delete log_p; log_p = NULL;
  Common_UI_WxWidgets_Tools::logger = NULL;
}

int
Common_UI_WxWidgets_Tools::clientDataToIndex (wxObject* object_in,
                                              const std::string& clientData_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::clientDataToIndex"));

  wxItemContainer* item_container_p =
    dynamic_cast<wxItemContainer*> (object_in);
  ACE_ASSERT (item_container_p);
  ACE_ASSERT (item_container_p->HasClientObjectData ());
  wxStringClientData* string_client_data_p = NULL;
  for (unsigned int i = 0;
       i < item_container_p->GetCount ();
       ++i)
  {
    string_client_data_p =
      dynamic_cast<wxStringClientData*> (item_container_p->GetClientObject (i));
    ACE_ASSERT (string_client_data_p);
    if (!ACE_OS::strcmp (string_client_data_p->GetData ().ToStdString ().c_str (),
                         clientData_in.c_str ()))
      return i;
  } // end FOR

  return wxNOT_FOUND;
}
