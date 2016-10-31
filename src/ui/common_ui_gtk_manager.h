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

#ifndef COMMON_UI_GTK_MANAGER_H
#define COMMON_UI_GTK_MANAGER_H

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

#include <gtk/gtk.h>

#include "common_icontrol.h"
#include "common_task_base.h"
#include "common_time_common.h"

#include "common_ui_exports.h"
#include "common_ui_gtk_common.h"

void glib_print_debug_handler (const gchar*); // message
void glib_print_error_handler (const gchar*); // message
void glib_log_handler (const gchar*,   // domain
                       GLogLevelFlags, // priority
                       const gchar*,   // message
                       gpointer);      // user data

// forward declarations
struct Common_UI_GTKState;

class Common_UI_Export Common_UI_GTK_Manager
 : public Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t>
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Common_UI_GTK_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  bool initialize (int,                 // argc
                   ACE_TCHAR** ,        // argv
                   Common_UI_GTKState*, // state handle
                   Common_UI_IGTK_t*);  // UI interface handle

  // implement (part of) Common_IControl
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;

 private:
  typedef Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t> inherited;

  Common_UI_GTK_Manager ();
  virtual ~Common_UI_GTK_Manager ();
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Manager (const Common_UI_GTK_Manager&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GTK_Manager& operator= (const Common_UI_GTK_Manager&))

  // override ACE_Task_Base member(s)
  virtual int close (u_long = 0);
  virtual int svc (void);

  // implement (part of) Common_IControl
  virtual void initialize ();

  bool initializeGTK ();

  int                 argc_;
  ACE_TCHAR**         argv_;
  bool                GTKIsInitialized_;
  bool                isInitialized_;
  Common_UI_GTKState* state_;
  Common_UI_IGTK_t*   UIInterfaceHandle_;
};

typedef ACE_Singleton<Common_UI_GTK_Manager,
                      ACE_Recursive_Thread_Mutex> COMMON_UI_GTK_MANAGER_SINGLETON;
COMMON_UI_SINGLETON_DECLARE (ACE_Singleton,
                             Common_UI_GTK_Manager,
                             ACE_Recursive_Thread_Mutex);
#endif
