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

#ifndef COMMON_UI_GTK_MANAGER_COMMON_H
#define COMMON_UI_GTK_MANAGER_COMMON_H

#include "gtk/gtk.h"

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"
#include "common_task_base.h"
#include "common_time_common.h"

#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager.h"

//typedef Common_TaskBase_T<ACE_MT_SYNCH,
//                          Common_TimePolicy_t,
//                          Common_ILock_T<ACE_NULL_SYNCH> > Common_UI_IGTK_Manager_t;
typedef Common_TaskBase_T<ACE_MT_SYNCH,
                          Common_TimePolicy_t,
                          Common_ILock_T<ACE_MT_SYNCH>,
                          ACE_Message_Block,
                          ACE_Message_Queue<ACE_MT_SYNCH,
                                            Common_TimePolicy_t>,
                          ACE_Task<ACE_MT_SYNCH,
                                   Common_TimePolicy_t> > Common_UI_IGTK_Manager_t;

//////////////////////////////////////////

typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                Common_UI_GTK_Configuration_t,
                                Common_UI_GTK_State_t,
                                gpointer> Common_UI_GTK_Manager_t;
typedef ACE_Singleton<Common_UI_GTK_Manager_t,
                      ACE_MT_SYNCH::MUTEX> COMMON_UI_GTK_MANAGER_SINGLETON;

#endif
