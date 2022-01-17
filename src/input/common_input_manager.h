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

#ifndef COMMON_INPUT_MANAGER_T_H
#define COMMON_INPUT_MANAGER_T_H

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Message_Queue.h"
#include "ace/Task_T.h"
#include "ace/Time_Value.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_iinitialize.h"

#include "common_time_common.h"

#include "common_task_base.h"

#include "common_input_common.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType, // implements Common_Input_Manager_Configuration
          typename HandlerType> // implements Common_InputHandler_Base_T
class Common_Input_Manager_T
 : public Common_TaskBase_T<ACE_SYNCH_USE,
                            Common_TimePolicy_t,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_SYNCH_USE,
                                              Common_TimePolicy_t>,
                            ACE_Task<ACE_SYNCH_USE,
                                     Common_TimePolicy_t> >
 , public Common_IInitialize_T<ConfigurationType>
{
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            Common_TimePolicy_t,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_SYNCH_USE,
                                              Common_TimePolicy_t>,
                            ACE_Task<ACE_SYNCH_USE,
                                     Common_TimePolicy_t> > inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Common_Input_Manager_T<ACE_SYNCH_USE,
                                                    ConfigurationType,
                                                    HandlerType>,
                             ACE_SYNCH_MUTEX_T>;

 public:
  // convenient types
  typedef ACE_Singleton<Common_Input_Manager_T<ACE_SYNCH_USE,
                                               ConfigurationType,
                                               HandlerType>,
                        ACE_SYNCH_MUTEX_T> SINGLETON_T;

  // override (part of) Common_IAsynchTask
  virtual bool start (ACE_Time_Value* = NULL); // N/A
  virtual void stop (bool = true,   // wait for completion ?
                     bool = false); // N/A

  // implement Common_IInitialize
  virtual bool initialize (const ConfigurationType&);

 protected:
  Common_Input_Manager_T ();
  virtual ~Common_Input_Manager_T ();

  ConfigurationType* configuration_;
  HandlerType*       handler_;

 private:
  // convenient types
//  typedef Common_TaskBase_T<ACE_SYNCH_USE,
//                            Common_TimePolicy_t,
//                            ACE_Message_Block,
//                            ACE_Message_Queue<ACE_SYNCH_USE,
//                                              Common_TimePolicy_t>,
//                            ACE_Task<ACE_SYNCH_USE,
//                                     Common_TimePolicy_t> > TASKBASE_T;

  ACE_UNIMPLEMENTED_FUNC (Common_Input_Manager_T (const Common_Input_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Input_Manager_T& operator= (const Common_Input_Manager_T&))

  // override/hide some ACE_Task_Base member(s)
  virtual int close (u_long = 0);
  virtual int svc (void);

  // hide some Common_IAsynchTask member(s)
  inline virtual void idle () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual bool isShuttingDown () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
};

// include template definition
#include "common_input_manager.inl"

#endif
