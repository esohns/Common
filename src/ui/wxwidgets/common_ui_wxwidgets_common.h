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

#ifndef COMMON_UI_WXWIDGETS_COMMON_H
#define COMMON_UI_WXWIDGETS_COMMON_H

#include <map>
#include <set>
#include <string>
#include <utility>

#include "ace/config-lite.h"
#include "ace/OS_NS_Thread.h"

#include "common_itask.h"

#include "common_ui_common.h"
#include "common_ui_idefinition.h"

#include "common_ui_wxwidgets_iapplication.h"

// forward declarations
#if (wxUSE_UNICODE == 1)
typedef wchar_t wxChar;
#else
typedef char wxChar;
#endif // wxUSE_UNICODE == 1
class wxObject;

typedef std::pair<std::string, wxObject*> Common_UI_wxWidgets_XmlResource_t;
typedef std::map<std::string, Common_UI_wxWidgets_XmlResource_t> Common_UI_wxWidgets_XmlResources_t;
typedef Common_UI_wxWidgets_XmlResources_t::iterator Common_UI_wxWidgets_XmlResourcesIterator_t;
typedef Common_UI_wxWidgets_XmlResources_t::const_iterator Common_UI_wxWidgets_XmlResourcesConstIterator_t;

struct Common_UI_wxWidgets_State
 : Common_UI_State
{
  Common_UI_wxWidgets_State ()
   : Common_UI_State ()
   , argc (0)
   , argv (NULL)
   , instance (NULL)
   , resources ()
  {}

  int                                argc;
  wxChar**                           argv;
  wxObject*                          instance;
  Common_UI_wxWidgets_XmlResources_t resources;
};

//////////////////////////////////////////

typedef std::map<unsigned int, ACE_Thread_ID> Common_UI_wxWidgets_PendingActions_t;
typedef Common_UI_wxWidgets_PendingActions_t::iterator Common_UI_wxWidgets_PendingActionsIterator_t;
typedef std::set<unsigned int> Common_UI_wxWidgets_CompletedActions_t;
typedef Common_UI_wxWidgets_CompletedActions_t::iterator Common_UI_wxWidgets_CompletedActionsIterator_t;

struct Common_UI_wxWidgets_ProgressData
{
  Common_UI_wxWidgets_ProgressData ()
   : completedActions ()
   , pendingActions ()
   , state (NULL)
  {}

  Common_UI_wxWidgets_CompletedActions_t completedActions;
  Common_UI_wxWidgets_PendingActions_t   pendingActions;
  struct Common_UI_wxWidgets_State*      state;
};

struct Common_UI_wxWidgets_CBData
 : Common_UI_CBData
{
  Common_UI_wxWidgets_CBData ()
   : Common_UI_CBData ()
   , progressData ()
   , UIState (NULL)
  {
    progressData.state = UIState;
  }

  struct Common_UI_wxWidgets_ProgressData progressData;
  struct Common_UI_wxWidgets_State*       UIState;
};

struct Common_UI_wxWidgets_ThreadData
 : Common_UI_ThreadData
{
  Common_UI_wxWidgets_ThreadData ()
   : Common_UI_ThreadData ()
   , CBData (NULL)
  {}

  struct Common_UI_wxWidgets_CBData* CBData;
};

//////////////////////////////////////////

typedef Common_UI_IDefinition_T<struct Common_UI_wxWidgets_State> Common_UI_wxWidgets_IDefinition_t;
typedef Common_UI_wxWidgets_IApplicationBase_T<struct Common_UI_wxWidgets_State> Common_UI_wxWidgets_IApplicationBase_t;
typedef Common_ITask Common_UI_wxWidgets_IManager_t;

#endif
