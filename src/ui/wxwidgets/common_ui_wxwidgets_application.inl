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

#include "common_ui_defines.h"

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
 , instance_ (NULL)
 , configuration_ ()
 , state_ ()
 , definition_ (topLevelWidgetName_in,
                &instance_)
 //, initializer_ (argc_in, argv_in)
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

  // parse the command-line arguments ?
  // *NOTE*: this works iff wxUSE_CMDLINE_PARSER was set
  if (parseCommandLine_)
    if (!inherited::OnInit ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to wxAppConsole::OnInit(), aborting\n")));
      return false;
    } // end IF

  if (unlikely (!definition_.initialize (state_)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize interface definition, aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (!state_.resources.empty ());

  Common_UI_wxWidgets_XmlResourcesConstIterator_t iterator;
  ITOPLEVEL_T* itop_level_p = NULL;
  TopLevelClassType* widget_p = NULL;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_.lock, false);
    iterator =
      state_.resources.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
    ACE_ASSERT (iterator != state_.resources.end ());

    itop_level_p = dynamic_cast<ITOPLEVEL_T*> ((*iterator).second.second);
    ACE_ASSERT (itop_level_p);
    try {
      itop_level_p->OnInit_2 (this);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_UI_wxWidgets_ITopLevel_T::OnInit_2(), aborting\n")));
      return false;
    }

    widget_p = dynamic_cast<TopLevelClassType*> ((*iterator).second.second);
    ACE_ASSERT (widget_p);
    inherited::SetTopWindow (widget_p);
    widget_p->Show (true);
  } // end lock scope

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
