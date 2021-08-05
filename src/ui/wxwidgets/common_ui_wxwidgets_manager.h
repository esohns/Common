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

#ifndef COMMON_UI_WXWIDGETS_MANAGER_T_H
#define COMMON_UI_WXWIDGETS_MANAGER_T_H

#include <string>

//#if defined (_DEBUG)
//#undef _DEBUG // *NOTE*: do not (!) #define __WXDEBUG__
//#define REDEDINE_DEBUG 1
//#endif // _DEBUG
#include "wx/wx.h"
//#if defined (REDEDINE_DEBUG)
//#undef REDEDINE_DEBUG
//#define _DEBUG
//#endif // REDEDINE_DEBUG

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iget.h"
#include "common_ilock.h"
#include "common_itaskcontrol.h"

template <typename ApplicationType>
class Common_UI_WxWidgets_Manager_T
 : public wxThread
 , public Common_ITaskControl_T<ACE_NULL_SYNCH,
                                Common_ILock_T<ACE_NULL_SYNCH> >
 , public Common_IGetP_T<typename ApplicationType::INTERFACE_T>
{
  typedef wxThread inherited;

 public:
   // convenient types
  typedef typename ApplicationType::INTERFACE_T APPLICATION_T;

  Common_UI_WxWidgets_Manager_T (const std::string&, // toplevel widget name
                                 int,                // argc
                                 ACE_TCHAR**,        // argv
                                 bool = false);      // auto-start ?
  // *NOTE*: fire-and-forget API: this instance assumes responsibility for the
  //         argument provided
  Common_UI_WxWidgets_Manager_T (ApplicationType*, // application handle
                                 bool = false);    // auto-start ?
  virtual ~Common_UI_WxWidgets_Manager_T ();

  // implement Common_ITaskControl_T
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true,  // high priority ?
                     bool = true); // locked access ?
  inline virtual bool isRunning () const { return inherited::IsAlive (); }
  inline virtual void idle () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual bool lock (bool = true) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual int unlock (bool = false) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  inline virtual const ACE_NULL_SYNCH::MUTEX& getR () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (ACE_NULL_SYNCH::MUTEX ()); ACE_NOTREACHED (return ACE_NULL_SYNCH::MUTEX ();) }
  inline virtual void finished () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
#if wxCHECK_VERSION(3,0,0)
  inline virtual void wait (bool = true) const { OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);  wxThread::ExitCode exit_code = this_p->Wait (wxTHREAD_WAIT_DEFAULT); ACE_UNUSED_ARG (exit_code); }
#elif wxCHECK_VERSION(2,0,0)
  inline virtual void wait (bool = true) const { OWN_TYPE_T* this_p = const_cast<OWN_TYPE_T*> (this);  wxThread::ExitCode exit_code = this_p->Wait (); ACE_UNUSED_ARG (exit_code); }
#endif // wxCHECK_VERSION
  inline virtual const typename ApplicationType::INTERFACE_T* const getP () const { return application_; }

 protected:
  virtual void* Entry ();

  virtual void OnDelete () {}
  virtual void OnKill () {}
  virtual void OnExit () {}

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Manager_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Manager_T (const Common_UI_WxWidgets_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Manager_T& operator= (const Common_UI_WxWidgets_Manager_T&))

  // convenient types
  typedef Common_UI_WxWidgets_Manager_T<ApplicationType> OWN_TYPE_T;

  ApplicationType* application_;
};

// include template definition
#include "common_ui_wxwidgets_manager.inl"

#endif
