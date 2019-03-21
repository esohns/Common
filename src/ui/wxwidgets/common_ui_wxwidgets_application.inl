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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

//#include "common_ui_defines.h"

#include "common_ui_wxwidgets_common.h"

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::Comon_UI_WxWidgets_Application_T (const std::string& topLevelWidgetName_in,
                                                                                int argc_in,
                                                                                wxChar** argv_in,
                                                                                bool parseCommandLine_in)
 : inherited ()
 , CBData_ ()
 , state_ ()
 , definition_ (topLevelWidgetName_in)
 , parseCommandLine_ (parseCommandLine_in)
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::Comon_UI_WxWidgets_Application_T"));

  state_.argc = argc_in;
  state_.argv = argv_in;

  //// *NOTE*: wxInitializer registers the global wxApp instance 'automagically'
  //// *TODO*: find a better way to do this
  //ACE_ASSERT (!wxAppConsole::IsMainLoopRunning ());
  //wxApp* app_p = dynamic_cast<wxApp*> (wxAppConsole::GetInstance ());
  //ACE_ASSERT (app_p == this);
}

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
int
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::FilterEvent (wxEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::FilterEvent"));

  wxEventType event_type_i = event_in.GetEventType ();
  wxObject* object_p = event_in.GetEventObject ();

  // Continue processing the event normally as well.
  return Event_Skip;
}

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
void
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::dump_state"));

//  wxClientData* client_data_p = NULL;

//  wxENTER_CRIT_SECT (inherited::m_handlersWithPendingEventsLocker);

//  for (inherited::m_handlersWithPendingEvents::const_iterator iterator = inherited::m_handlersWithPendingEvents.begin ();
//       iterator != inherited::m_handlersWithPendingEvents.end ();
//       ++iterator)
//  {
//    client_data_p = (*iterator)->GetClientObject ();

//  } // end FOR

//  wxLEAVE_CRIT_SECT (inherited::m_handlersWithPendingEventsLocker);
}

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
bool
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::run ()
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::run"));

  // sanity check(s)
  ACE_ASSERT (state_.argc && state_.argv);

  int result = wxEntry (state_.argc,
                        state_.argv);
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to wxEntry(), aborting\n")));
    return false;
  } // end IF

  return true;
}

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
bool
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::OnInit ()
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::OnInit"));

  // sanity check(s)
  ACE_ASSERT (!state_.instance);

  // parse the command-line arguments ?
  // *NOTE*: this works iff wxUSE_CMDLINE_PARSER was set
  if (parseCommandLine_)
    if (!inherited::OnInit ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to wxAppConsole::OnInit(), aborting\n")));
      return false;
    } // end IF

  Common_UI_wxWidgets_XmlResourcesConstIterator_t iterator;
  ITOPLEVEL_T* itop_level_p = NULL;
  TopLevelClassType* widget_p = NULL;
  wxWindow* window_p = NULL;

  ACE_NEW_NORETURN (state_.instance,
                    TopLevelClassType (NULL));
  if (unlikely (!state_.instance))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, aborting\n")));
    return false;
  } // end IF

  if (unlikely (!definition_.initialize (state_)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize interface definition, aborting\n")));
    delete state_.instance; state_.instance = NULL;
    return false;
  } // end IF
  ACE_ASSERT (!state_.resources.empty ());

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, false);
    iterator =
      state_.resources.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
    ACE_ASSERT (iterator != state_.resources.end ());
    window_p = dynamic_cast<wxWindow*> ((*iterator).second.second);
    ACE_ASSERT (window_p);

//    itop_level_p = dynamic_cast<ITOPLEVEL_T*> ((*iterator).second.second);
//    ACE_ASSERT (itop_level_p);
    itop_level_p = dynamic_cast<ITOPLEVEL_T*> (state_.instance);
    ACE_ASSERT (itop_level_p);
  } // end lock scope

  try {
    itop_level_p->OnInit_2 (this);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_UI_wxWidgets_ITopLevel_T::OnInit_2(), aborting\n")));
    return false;
  }

  widget_p = dynamic_cast<TopLevelClassType*> (itop_level_p);
  ACE_ASSERT (widget_p);
  inherited::SetTopWindow (widget_p);
//  widget_p->Show (true);
  window_p->Show (true);

  return true;
}

template <typename DefinitionType,
          typename ConfigurationType,
          typename StateType,
          typename TopLevelClassType,
          typename TraitsType>
int
Comon_UI_WxWidgets_Application_T<DefinitionType,
                                 ConfigurationType,
                                 StateType,
                                 TopLevelClassType,
                                 TraitsType>::OnExit ()
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_WxWidgets_Application_T::OnExit"));

  Common_UI_wxWidgets_XmlResourcesConstIterator_t iterator;
  ITOPLEVEL_T* itop_level_p = NULL;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, false);
    iterator = state_.resources.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
    ACE_ASSERT (iterator != state_.resources.end ());

    itop_level_p = dynamic_cast<ITOPLEVEL_T*> ((*iterator).second.second);
    ACE_ASSERT (itop_level_p);
    try {
      itop_level_p->OnExit_2 ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_UI_wxWidgets_ITopLevel_T::OnExit_2(), aborting\n")));
      return false;
    }
  } // end lock scope

  definition_.finalize ();

  return 0;
}
