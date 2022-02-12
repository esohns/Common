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

#ifndef COMMON_TEST_I_COMMON_H
#define COMMON_TEST_I_COMMON_H

#include "ace/config-lite.h"

#include "common_configuration.h"

#include "common_signal_common.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "common_ui_gtk_common.h"
#elif defined (QT_USE)
#include "common_ui_qt_common.h"
#elif defined (WXWIDGETS_USE)
#include "common_ui_wxwidgets_common.h"
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#endif // GUI_SUPPORT

// forward declarations
struct Common_UI_State;

struct Common_Test_I_UserData
{
  Common_Test_I_UserData ()
   : userData (NULL)
  {}

  void* userData;
};

struct Common_Test_I_Configuration
{
  Common_Test_I_Configuration ()
   : dispatchConfiguration ()
   , signalHandlerConfiguration ()
   , userData (NULL)
  {}

  struct Common_EventDispatchConfiguration dispatchConfiguration;
  struct Common_SignalHandlerConfiguration signalHandlerConfiguration;

  struct Common_Test_I_UserData*           userData;
};

//////////////////////////////////////////

#if defined (GUI_SUPPORT)
struct Common_Test_I_UI_CBData;
struct Common_Test_I_UI_ProgressData
{
  Common_Test_I_UI_ProgressData ()
   : CBData (NULL)
#if defined (GUI_SUPPORT)
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
   , state (NULL)
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#endif // GUI_SUPPORT
  {}

  struct Common_Test_I_UI_CBData* CBData;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
  struct Common_UI_State*         state;
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#endif // GUI_SUPPORT
};

struct Common_Test_I_UI_CBData
{
  Common_Test_I_UI_CBData ()
   : allowUserRuntimeStatistic (true)
   , progressData ()
#if defined (GUI_SUPPORT)
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
   , UIState (NULL)
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#endif // GUI_SUPPORT
  {
    progressData.CBData = this;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
    progressData.state = UIState;
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#endif // GUI_SUPPORT
  }

  bool                                  allowUserRuntimeStatistic;
  struct Common_Test_I_UI_ProgressData  progressData;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  Common_UI_GTK_State_t*                UIState;
#elif defined (QT_USE)
  struct Common_UI_Qt_State*            UIState;
#elif defined (WXWIDGETS_USE)
  Common_UI_wxWidgets_State*            UIState;
#endif
#endif // GUI_SUPPORT
};

struct Common_Test_I_ThreadData
{
  Common_Test_I_ThreadData ()
#if defined (GUI_SUPPORT)
    : CBData (NULL)
    , sessionId (0)
#else
    : sessionId (0)
#endif // GUI_SUPPORT
  {}

#if defined (GUI_SUPPORT)
  struct Common_Test_I_UI_CBData* CBData;
#endif // GUI_SUPPORT
  size_t                          sessionId;
};
#endif // GUI_SUPPORT

#endif // COMMON_TEST_I_COMMON_H
