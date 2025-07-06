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

#ifndef COMMON_TEST_U_COMMON_H
#define COMMON_TEST_U_COMMON_H

#include "common_event_common.h"

#include "common_signal_common.h"

#include "common_ui_common.h"
#if defined (GTK_SUPPORT)
#include "common_ui_gtk_common.h"
#endif // GTK_SUPPORT
#if defined (QT_SUPPORT)
#include "common_ui_qt_common.h"
#endif // QT_SUPPORT
#if defined (WXWIDGETS_SUPPORT)
#include "common_ui_wxwidgets_common.h"
#endif // WXWIDGETS_SUPPORT

struct Test_U_UserData
{
  Test_U_UserData ()
   : userData (NULL)
  {}

  void* userData;
};

struct Test_U_SignalHandlerConfiguration
 : Common_SignalHandlerConfiguration
{
  ACE_Thread_Condition<ACE_Thread_Mutex>* condition;
};

struct Test_U_Configuration
{
  Test_U_Configuration ()
   : dispatchConfiguration ()
   , signalHandlerConfiguration ()
   , userData (NULL)
  {}

  struct Common_EventDispatchConfiguration dispatchConfiguration;
  struct Common_SignalHandlerConfiguration signalHandlerConfiguration;

  struct Test_U_UserData*                  userData;
};

//////////////////////////////////////////

struct Test_U_UI_CBData;
struct Test_U_UI_ProgressData
{
  Test_U_UI_ProgressData ()
   : CBData (NULL)
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
   , state (NULL)
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
  {}

  struct Test_U_UI_CBData* CBData;
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
  struct Common_UI_State*  state;
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
};

struct Test_U_UI_CBData
{
  Test_U_UI_CBData ()
   : allowUserRuntimeStatistic (true)
   , progressData ()
   , UIState (NULL)
  {
    progressData.CBData = this;
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
    progressData.state = UIState;
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
  }

  bool                          allowUserRuntimeStatistic;
  struct Test_U_UI_ProgressData progressData;
#if defined (GTK_USE)
  Common_UI_GTK_State_t*        UIState;
#elif defined (QT_USE)
  struct Common_UI_Qt_State*    UIState;
#elif defined (WXWIDGETS_USE)
  Common_UI_wxWidgets_State*    UIState;
#else
  struct Common_UI_State*       UIState;
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
};

struct Test_U_ThreadData
{
  Test_U_ThreadData ()
   : CBData (NULL)
   , sessionId (0)
  {}

  struct Test_U_UI_CBData* CBData;
  size_t                   sessionId;
};

#endif
