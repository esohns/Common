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

#ifndef COMMON_UI_GTK_MANAGER_T_H
#define COMMON_UI_GTK_MANAGER_T_H

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "gtk/gtk.h"

#include "common_iget.h"
#include "common_iinitialize.h"
#include "common_ilock.h"
#include "common_task_base.h"
#include "common_time_common.h"

#include "common_ui_idefinition.h"

#include "common_ui_gtk_common.h"

// GLib debug/log handler callbacks
void glib_log_handler (const gchar*,   // domain
                       GLogLevelFlags, // priority
                       const gchar*,   // message
                       gpointer);      // user data
inline void glib_print_debug_handler (const gchar* message_in) { glib_log_handler (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, message_in, NULL); }
inline void glib_print_error_handler (const gchar* message_in) { glib_log_handler (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, message_in, NULL); }

template <ACE_SYNCH_DECL,
          typename ConfigurationType = Common_UI_GTK_Configuration_t,
          typename StateType = Common_UI_GTK_State_t,
          typename CallBackDataType = gpointer>
class Common_UI_GTK_Manager_T
 : public Common_TaskBase_T<ACE_SYNCH_USE,
                            Common_TimePolicy_t,
                            Common_ILock_T<ACE_SYNCH_USE> >
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IGetR_2_T<StateType>
{
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            Common_TimePolicy_t,
                            Common_ILock_T<ACE_SYNCH_USE> > inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                                                     ConfigurationType,
                                                     StateType,
                                                     CallBackDataType>,
                             ACE_SYNCH_MUTEX_T>;

 public:
  // convenient types
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            Common_TimePolicy_t,
                            Common_ILock_T<ACE_SYNCH_USE> > TASK_T;
  typedef ACE_Singleton<Common_UI_GTK_Manager_T<ACE_SYNCH_USE,
                                                ConfigurationType,
                                                StateType,
                                                CallBackDataType>,
                        ACE_SYNCH_MUTEX_T> SINGLETON_T;

  // override (part of) Common_ITask
  virtual void start (ACE_thread_t&); // return value: thread handle (if any)
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?

  // implement Common_IInitialize
  virtual bool initialize (const ConfigurationType&);

  inline virtual const StateType& getR_2 () const { return state_; }

 private:
  Common_UI_GTK_Manager_T ();
  inline virtual ~Common_UI_GTK_Manager_T () {}
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Manager_T (const Common_UI_GTK_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Manager_T& operator= (const Common_UI_GTK_Manager_T&))

  // override/hide some ACE_Task_Base member(s)
  virtual int close (u_long = 0);
  virtual int svc (void);

  // hide some Common_TaskBase_T member(s)
  using inherited::lock;
  using inherited::unlock;
  using inherited::getR;
  using inherited::finished;

  // helper methods
  bool initializeGTK ();

  ConfigurationType* configuration_;
  CallBackDataType   CBData_;
  bool               GTKIsInitialized_;
  StateType          state_;
  bool               UIIsInitialized_;
};

// include template definition
#include "common_ui_gtk_manager.inl"

#endif
