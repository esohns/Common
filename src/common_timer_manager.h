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

#include "ace/Asynch_IO.h"
#include "ace/Event_Handler.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common.h"
#include "common_icontrol.h"
#include "common_idumpstate.h"
#include "common_iinitialize.h"
#include "common_itimer.h"
#include "common_timer_common.h"

template <typename TimerQueueAdapterType>
class Common_Timer_Manager_T
 : public TimerQueueAdapterType
 , public Common_ITimer
 , public Common_IControl
 , public Common_IDumpState
 , public Common_IInitialize_T<Common_TimerConfiguration>
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<Common_Timer_Manager_T<TimerQueueAdapterType>,
                             ACE_SYNCH_MUTEX>;

 public:
  // implement Common_ITimer
  // proactor version
  virtual long schedule_timer (ACE_Handler*,                                  // event handler
                               const void*,                                   // act
                               const ACE_Time_Value&,                         // delay
                               const ACE_Time_Value& = ACE_Time_Value::zero); // interval
  // *NOTE*: API adopted from ACE_Reactor_Timer_Interface
  virtual long schedule_timer (ACE_Event_Handler*,                            // event handler
                               const void*,                                   // act
                               const ACE_Time_Value&,                         // delay
                               const ACE_Time_Value& = ACE_Time_Value::zero); // interval
  virtual int reset_timer_interval (long,                   // timer id
                                    const ACE_Time_Value&); // interval
  virtual int cancel_timer (long,             // timer id
                            const void** = 0, // return value: act
                            int = 1);         // don't call handle_close()

  // implement (part of) Common_IControl
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

  // implement Common_IInitialize_T
  virtual bool initialize (const Common_TimerConfiguration&);

 private:
  typedef TimerQueueAdapterType inherited;

  // convenience types
  typedef typename TimerQueueAdapterType::TIMER_QUEUE TIMER_QUEUE_T;
  //typedef typename TimerQueueAdapterType::TIMER_QUEUE_T TIMER_QUEUE_T;
  typedef Common_Timer_Manager_T<TimerQueueAdapterType> OWN_TYPE_T;

  Common_Timer_Manager_T ();
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T (const Common_Timer_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T& operator= (const Common_Timer_Manager_T&))
  virtual ~Common_Timer_Manager_T ();

  // implement (part of) Common_IControl
  virtual void initialize ();

  // helper methods
  unsigned int flushTimers (bool = true); // locked access ?
  bool initializeTimerQueue ();

  Common_TimerConfiguration configuration_;

  // *NOTE*: this is only the functor, individual handlers are managed in the
  //        queue
  Common_TimeoutUpcall_t    timerHandler_;
  TIMER_QUEUE_T*            timerQueue_;
};

// include template implementation
#include "common_timer_manager.inl"

#endif
