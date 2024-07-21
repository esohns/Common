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

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Task.h"
#include "ace/Time_Value.h"
#include "ace/Timer_Queue_Adapters.h"

#include "common.h"
#include "common_idumpstate.h"
#include "common_itask.h"

#include "common_itimer.h"
#include "common_timer_common.h"

// forward declarations
class Common_Timer_Handler;

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
class Common_Timer_Manager_T
 : public TimerQueueAdapterType
 , public Common_IAsynchTask
 , public Common_ITimerCB_T<ConfigurationType>
 , private Common_IGetR_3_T<ACE_SYNCH_RECURSIVE_MUTEX>
 //, private Common_IGetR_4_T<typename TimerQueueAdapterType::TIMER_QUEUE>
 , private Common_IGetR_5_T<ACE_Task_Base>
 , public Common_IDumpState
{
  typedef TimerQueueAdapterType inherited;

  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<Common_Timer_Manager_T<ACE_SYNCH_USE,
                                                    ConfigurationType,
                                                    TimerQueueAdapterType>,
                             ACE_SYNCH_MUTEX>;

 public:
  // convenient types
  typedef Common_ITimerCB_T<ConfigurationType> INTERFACE_T;
  typedef ACE_Singleton<Common_Timer_Manager_T<ACE_SYNCH_USE,
                                               ConfigurationType,
                                               TimerQueueAdapterType>,
                        ACE_SYNCH_MUTEX> SINGLETON_T;

  // implement (part of) Common_IAsynchTask
  virtual bool isRunning () const;
  inline virtual bool isShuttingDown () const { return !isRunning (); }
  virtual bool start (ACE_Time_Value* = NULL); // N/A
  virtual void stop (bool = true,   // wait for completion ?
                     bool = false); // N/A
  virtual void wait (bool = true) const; // N/A

  // implement Common_ITimerCB_T
  virtual int cancel_timer (long,               // timer id
                            const void** = 0); // return value: asynchronous completion token
  virtual int reset_timer_interval (long,                   // timer id
                                    const ACE_Time_Value&); // interval
  // *NOTE*: when using reactor scheduling, the expiration time is relative (!)
  virtual long schedule_timer (Common_Timer_Handler*,                         // event handler handle
                               const void*,                                   // asynchronous completion token
                               const ACE_Time_Value&,                         // expiration time (relative)
                               const ACE_Time_Value& = ACE_Time_Value::zero); // set periodic interval ?
//  virtual long schedule_timer (ACE_Event_Handler*,                            // event handler handle
//                               const void*,                                   // asynchronous completion token
//                               const ACE_Time_Value&,                         // expiration time (absolute)
//                               const ACE_Time_Value& = ACE_Time_Value::zero); // set periodic interval ?
//  virtual long schedule_timer (ACE_Handler*,                                  // event handler handle
//                               const void*,                                   // asynchronous completion token
//                               const ACE_Time_Value&,                         // expiration time (absolute)
//                               const ACE_Time_Value& = ACE_Time_Value::zero); // set periodic interval ?
  virtual bool initialize (const ConfigurationType&);
  inline virtual const ConfigurationType& getR () const { ACE_ASSERT (configuration_); return *configuration_; }

  // implement Common_IDumpState
  virtual void dump_state () const;

  // override (part of) ACE_Task_Base
  virtual int svc ();

 private:
  // convenient types
  typedef typename TimerQueueAdapterType::TIMER_QUEUE TIMER_QUEUE_T;
  typedef ACE_Async_Timer_Queue_Adapter<TIMER_QUEUE_T, ACE_Event_Handler*> ASYNCH_TIMER_QUEUE_T;
  typedef ACE_Thread_Timer_Queue_Adapter<TIMER_QUEUE_T, ACE_Event_Handler*> THREAD_TIMER_QUEUE_T;
  typedef Common_Timer_Manager_T<ACE_SYNCH_USE,
                                 ConfigurationType,
                                 TimerQueueAdapterType> OWN_TYPE_T;

  Common_Timer_Manager_T ();
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T (const Common_Timer_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T& operator= (const Common_Timer_Manager_T&))
  virtual ~Common_Timer_Manager_T ();

  // hide (part of) Common_IAsynchTask
  // *TODO*: this could be implemented
  inline virtual void idle () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void pause () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void resume () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void finished () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  // helper methods
  unsigned int flushTimers (bool = true); // locked access ?
  virtual const ACE_SYNCH_RECURSIVE_MUTEX& getR_3 () const;
  //virtual const typename TimerQueueAdapterType::TIMER_QUEUE& getR_4 () const;
  virtual const ACE_Task_Base& getR_5 () const;
  bool initializeTimerQueue ();

  ConfigurationType*            configuration_;
  enum Common_TimerDispatchType dispatch_;
  bool                          isInitialized_;
  enum Common_TimerQueueType    queueType_;

  // *NOTE*: this is only the functor, individual handlers are managed in the
  //         queue
  Common_TimeoutUpcall_t        timerHandler_;
  TIMER_QUEUE_T*                timerQueue_;
};

// include template definition
#include "common_timer_manager.inl"

#endif
