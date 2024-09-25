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

#include "ace/Log_Msg.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_ui_defines.h"

#include "common_ui_wxwidgets_defines.h"
#include "common_ui_wxwidgets_tools.h"

template <typename ApplicationType>
Common_UI_WxWidgets_Manager_T<ApplicationType>::Common_UI_WxWidgets_Manager_T (const std::string& topLevelWidgetName_in,
                                                                               int argc_in,
                                                                               ACE_TCHAR** argv_in,
                                                                               bool autoStart_in)
 : inherited (wxTHREAD_JOINABLE)
 , application_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::Common_UI_WxWidgets_Manager_T"));

  ACE_NEW_NORETURN (application_,
                    ApplicationType (topLevelWidgetName_in,
                                     argc_in,
                                     Common_UI_WxWidgets_Tools::convertArgV (argc_in,
                                                                             argv_in),
                                     COMMON_UI_WXWIDGETS_APP_DEFAULT_CMDLINE_PARSE));
  if (unlikely (!application_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, returning\n")));
    return;
  } // end IF

  if (unlikely (autoStart_in))
    start ();
}

template <typename ApplicationType>
Common_UI_WxWidgets_Manager_T<ApplicationType>::Common_UI_WxWidgets_Manager_T (ApplicationType* application_in,
                                                                               bool autoStart_in)
 : inherited (wxTHREAD_JOINABLE)
 , application_ (application_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::Common_UI_WxWidgets_Manager_T"));

  // sanity check(s)
  ACE_ASSERT (application_);

  if (unlikely (autoStart_in))
    start ();
}

template <typename ApplicationType>
Common_UI_WxWidgets_Manager_T<ApplicationType>::~Common_UI_WxWidgets_Manager_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::~Common_UI_WxWidgets_Manager_T"));

  wxThread::ExitCode exit_code = NULL;
  wxThreadError result = wxTHREAD_NO_ERROR;
  if (inherited::IsAlive ())
  {
    result = inherited::Delete (&exit_code
#if wxCHECK_VERSION (3,0,0)
                                ,wxTHREAD_WAIT_DEFAULT
#endif // wxCHECK_VERSION
                               );
    if (unlikely (result != wxTHREAD_NO_ERROR))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to wxThread::Delete(): %d, continuing\n"),
                  result));
  } // end IF
  if (inherited::IsPaused ())
    inherited::Resume ();
  exit_code = inherited::Wait (
#if wxCHECK_VERSION (3,0,0)
                               wxTHREAD_WAIT_DEFAULT
#endif // wxCHECK_VERSION
                              );
  ACE_UNUSED_ARG (exit_code);

  delete application_;
}

template <typename ApplicationType>
bool
Common_UI_WxWidgets_Manager_T<ApplicationType>::start (ACE_Time_Value* timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::start"));

  ACE_UNUSED_ARG (timeout_in);

  wxThreadError result = inherited::Run ();
  if (unlikely (result != wxTHREAD_NO_ERROR))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to wxThread::Run(): %d, aborting\n"),
                result));
    return false;
  } // end IF

  return true;
}

template <typename ApplicationType>
void
Common_UI_WxWidgets_Manager_T<ApplicationType>::stop (bool waitForCompletion_in,
                                                      bool highPriority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::stop"));

  ACE_UNUSED_ARG (highPriority_in);

  wxThread::ExitCode exit_code = NULL;
  wxThreadError result = wxTHREAD_NO_ERROR;
  if (inherited::IsAlive ())
  {
    result = inherited::Delete (&exit_code
#if wxCHECK_VERSION (3,0,0)
                                ,wxTHREAD_WAIT_DEFAULT
#endif // wxCHECK_VERSION
                               );
    if (unlikely (result != wxTHREAD_NO_ERROR))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to wxThread::Delete(): %d, continuing\n"),
                  result));
  } // end IF

  if (likely (waitForCompletion_in))
  {
    exit_code = inherited::Wait (
#if wxCHECK_VERSION (3,0,0)
                                 wxTHREAD_WAIT_DEFAULT
#endif // wxCHECK_VERSION
                                );
    ACE_UNUSED_ARG (exit_code);
  } // end IF
}

template <typename ApplicationType>
void*
Common_UI_WxWidgets_Manager_T<ApplicationType>::Entry ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Manager_T::Entry"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_EVENT_THREAD_NAME),
                                     0);
#endif // ACE_WIN32 || ACE_WIN64
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) starting\n"),
              ACE_TEXT (COMMON_UI_EVENT_THREAD_NAME)));

  // sanity check(s)
  ACE_ASSERT (application_);

  COMMON_TRY {
    application_->run ();
  } COMMON_CATCH (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in wxThread::Entry(), returning\n")));
  }

//done:
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s): worker thread (id: %t) leaving\n"),
              ACE_TEXT (COMMON_UI_EVENT_THREAD_NAME)));

  return NULL;
}
