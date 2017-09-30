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

#include "common_ilock.h"
#include "common_task_base.h"
#include "common_time_common.h"

//#include "common_ui_exports.h"
#include "common_ui_gtk_common.h"

// GLib debug/log handler callbacks
void glib_log_handler (const gchar*,   // domain
                       GLogLevelFlags, // priority
                       const gchar*,   // message
                       gpointer);      // user data
inline void glib_print_debug_handler (const gchar* message_in) { glib_log_handler (NULL, G_LOG_LEVEL_DEBUG, message_in, NULL); };
inline void glib_print_error_handler (const gchar* message_in) { glib_log_handler (NULL, G_LOG_LEVEL_ERROR, message_in, NULL); };

template <typename StateType>
class Common_UI_GTK_Manager_T
 : public Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t,
                            Common_IRecursiveLock_T<ACE_MT_SYNCH> >
{
  typedef Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t,
                            Common_IRecursiveLock_T<ACE_MT_SYNCH> > inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Common_UI_GTK_Manager_T<StateType>,
                             typename ACE_MT_SYNCH::RECURSIVE_MUTEX>;

 public:
  // convenient types
  typedef Common_UI_IGTK_T<StateType> UI_INTERFACE_T;
  typedef ACE_Singleton<Common_UI_GTK_Manager_T<StateType>,
                        typename ACE_MT_SYNCH::RECURSIVE_MUTEX> SINGLETON_T;

  bool initialize (int,              // argc
                   ACE_TCHAR** ,     // argv
                   StateType*,       // state handle
                   UI_INTERFACE_T*); // UI interface handle

  // override (part of) Common_ITask
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?

 private:
  Common_UI_GTK_Manager_T ();
  inline virtual ~Common_UI_GTK_Manager_T () {};
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

  int             argc_;
  ACE_TCHAR**     argv_;
  bool            GTKIsInitialized_;
  bool            isInitialized_;
//#if defined (GTKGL_SUPPORT)
//  // *TODO*: as a 'GTK-' OpenGL context is tied to a GdkWindow, it probably
//  //         makes sense to move this into the state_ (better: into a separate
//  //         presentation manager)
//  GdkGLContext*   openGLContext_;
//#endif
  StateType*      state_;
  UI_INTERFACE_T* UIInterfaceHandle_;
};

// include template definition
#include "common_ui_gtk_manager.inl"

#endif
