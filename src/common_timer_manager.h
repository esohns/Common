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

#ifndef COMMON_TIMER_MANAGER_H
#define COMMON_TIMER_MANAGER_H

#include "ace/Singleton.h"
#include "ace/Time_Value.h"

#include "common_exports.h"
#include "common.h"
#include "common_icontrol.h"
#include "common_idumpstate.h"

// *NOTE*: global time policy (supplies gettimeofday())
static Common_TimePolicy_t COMMON_TIME_POLICY;

class Common_Export Common_Timer_Manager
 : public Common_TimerQueue_t,
   public Common_IControl,
   public Common_IDumpState
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<Common_Timer_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  void resetInterval (long,                   // timer ID
                      const ACE_Time_Value&); // interval

  // implement Common_IControl
  virtual void stop (bool = true);
  virtual bool isRunning () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Common_TimerQueue_t inherited;

  virtual void start ();

  Common_Timer_Manager ();
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager (const Common_Timer_Manager&));
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager& operator= (const Common_Timer_Manager&));
  virtual ~Common_Timer_Manager ();

  // helper methods
  void fini_timers ();

  // *IMPORTANT NOTE*: this is only the functor, individual handlers are
  //                   managed in the queue
  Common_TimeoutUpcall_t   timerHandler_;
  Common_TimerQueueImpl_t* timerQueue_;
};

typedef ACE_Singleton<Common_Timer_Manager,
                      ACE_Recursive_Thread_Mutex> COMMON_TIMERMANAGER_SINGLETON;
COMMON_SINGLETON_DECLARE (ACE_Singleton,
                          Common_Timer_Manager,
                          ACE_Recursive_Thread_Mutex);

#endif
