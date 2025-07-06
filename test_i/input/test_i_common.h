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

#ifndef TEST_I_COMMON_H
#define TEST_I_COMMON_H

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Thread_Mutex.h"

#include "common_ui_common.h"
#if defined (GTK_SUPPORT)
#include "common_ui_gtk_common.h"
#endif // GTK_SUPPORT

#include "common_input_common.h"
#include "common_input_manager.h"

#include "test_i_inputhandler.h"

typedef Test_I_InputHandler_T<
#if defined (GTK_USE)
                              Common_UI_GTK_State_t
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
#if defined (WXWIDGETS_USE)
                              ,Test_I_WxWidgetsInterface_t
#endif // WXWIDGETS_USE
#if defined (GTK_USE) || defined (QT_USE) || defined (WXWIDGETS_USE)
#else
                              void
#endif // GTK_USE || QT_USE || WXWIDGETS_USE
                             > Test_I_InputHandler_t;

typedef Common_Input_Manager_T<ACE_MT_SYNCH,
                               struct Common_Input_Manager_Configuration,
                               Test_I_InputHandler_t> Test_I_Input_Manager_t;
typedef ACE_Singleton<Test_I_Input_Manager_t,
                      ACE_Thread_Mutex> INPUT_MANAGER_SINGLETON;

#endif // TEST_I_COMMON_H
