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

#ifndef COMMON_UI_QT_COMMON_H
#define COMMON_UI_QT_COMMON_H

#include <map>
#include <set>

//#include <QMainWindow>

#include "common_ui_common.h"

#include "common_ui_qt_iapplication.h"

// forward declarations
class QMainWindow;

struct Common_UI_Qt_State
 : Common_UI_State
{
  Common_UI_Qt_State ()
   : Common_UI_State ()
   , instance (NULL)
  {}

  QMainWindow* instance;
};

//////////////////////////////////////////

typedef std::map<unsigned int, ACE_Thread_ID> Common_UI_Qt_PendingActions_t;
typedef Common_UI_Qt_PendingActions_t::iterator Common_UI_Qt_PendingActionsIterator_t;
typedef std::set<unsigned int> Common_UI_Qt_CompletedActions_t;
typedef Common_UI_Qt_CompletedActions_t::iterator Common_UI_Qt_CompletedActionsIterator_t;

struct Common_UI_Qt_ProgressData
{
  Common_UI_Qt_ProgressData ()
   : completedActions ()
   , pendingActions ()
   , state (NULL)
  {}

  Common_UI_Qt_CompletedActions_t completedActions;
  Common_UI_Qt_PendingActions_t   pendingActions;
  struct Common_UI_Qt_State*      state;
};

//////////////////////////////////////////

struct Common_UI_Qt_CBData
 : Common_UI_CBData
{
  Common_UI_Qt_CBData ()
   : Common_UI_CBData ()
   , progressData ()
   , UIState (NULL)
  {
    progressData.state = UIState;
  }

  struct Common_UI_Qt_ProgressData progressData;
  struct Common_UI_Qt_State*       UIState;
};

//////////////////////////////////////////

typedef Common_UI_Qt_IApplicationBase_T<struct Common_UI_Qt_State> Common_UI_Qt_IApplicationBase_t;
typedef Common_UI_Qt_IApplication_T<struct Common_UI_Qt_State,
                                    struct Common_UI_Qt_CBData> Common_UI_Qt_IApplication_t;

#endif
