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
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common.h"
#include "common_idumpstate.h"
#include "common_ilock.h"
#include "common_itaskcontrol.h"
#include "common_itimer.h"
#include "common_timer_common.h"

// forward declarations
class Common_TimerHandler;

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename TimerQueueAdapterType>
class Common_Timer_Manager_T
 : public TimerQueueAdapterType
 , public Common_ITaskControl_T<ACE_SYNCH_USE,
                                Common_ILock_T<ACE_SYNCH_USE> >
 , public Common_ITimer_T<ConfigurationType>
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
  typedef Common_ITimer_T<ConfigurationType> INTERFACE_T;
  typedef ACE_Singleton<Common_Timer_Manager_T<ACE_SYNCH_USE,
                                               ConfigurationType,
                                               TimerQueueAdapterType>,
                        ACE_SYNCH_MUTEX> SINGLETON_T;

  // implement Common_ITimer_T
  virtual int cancel_timer (long,             // timer id
                            const void** = 0, // return value: asynchronous completion token
                            int = 1);         // do not (!) call handle_close() ?
  virtual int reset_timer_interval (long,                   // timer id
                                    const ACE_Time_Value&); // interval
//  inline virtual bool useReactor () const { ACE_ASSERT (configuration_); return (configuration_->mode != COMMON_TIMER_MODE_PROACTOR); }; // ? : uses proactor
  virtual long schedule_timer (Common_TimerHandler*,                          // event handler handle
                               const void*,                                   // asynchronous completion token
                               const ACE_Time_Value&,                         // delay
                               const ACE_Time_Value& = ACE_Time_Value::zero); // interval
//  virtual long schedule_timer (ACE_Event_Handler*,                            // event handler handle
//                               const void*,                                   // asynchronous completion token
//                               const ACE_Time_Value&,                         // delay
//                               const ACE_Time_Value& = ACE_Time_Value::zero); // interval
//  virtual long schedule_timer (ACE_Handler*,                                  // event handler handle
//                               const void*,                                   // asynchronous completion token
//                               const ACE_Time_Value&,                         // delay
//                               const ACE_Time_Value& = ACE_Time_Value::zero); // interval
  virtual bool initialize (const ConfigurationType&);
  inline virtual const ConfigurationType& getR_2 () const { ACE_ASSERT (configuration_); return *configuration_; }

  // implement (part of) Common_ITaskControl_T
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;
  virtual void wait () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  // convenient types
  typedef typename TimerQueueAdapterType::TIMER_QUEUE TIMER_QUEUE_T;
  typedef Common_ITaskControl_T<ACE_SYNCH_USE,
                                Common_ILock_T<ACE_SYNCH_USE> > ITASKCONTROL_T;
  typedef Common_Timer_Manager_T<ACE_SYNCH_USE,
                                 ConfigurationType,
                                 TimerQueueAdapterType> OWN_TYPE_T;

  Common_Timer_Manager_T ();
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T (const Common_Timer_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Manager_T& operator= (const Common_Timer_Manager_T&))
  virtual ~Common_Timer_Manager_T ();

  // hide (part of) Common_ITaskControl_T
  inline virtual bool lock (bool = true) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual int unlock (bool = false) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  inline virtual const typename ITASKCONTROL_T::MUTEX_T& getR () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (typename ITASKCONTROL_T::MUTEX_T ()); ACE_NOTREACHED (return typename ITASKCONTROL_T::MUTEX_T ();) }
  inline virtual void idle () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void finished () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  // helper methods
  unsigned int flushTimers (bool = true); // locked access ?
  virtual const ACE_SYNCH_RECURSIVE_MUTEX& getR_3 () const;
  //virtual const typename TimerQueueAdapterType::TIMER_QUEUE& getR_4 () const;
  virtual const ACE_Task_Base& getR_5 () const;
  bool initializeTimerQueue ();

  ConfigurationType*         configuration_;
  bool                       isInitialized_;
  enum Common_TimerMode      mode_;
  enum Common_TimerQueueType queueType_;

  // *NOTE*: this is only the functor, individual handlers are managed in the
  //         queue
  Common_TimeoutUpcall_t     timerHandler_;
  TIMER_QUEUE_T*             timerQueue_;
};

// include template definition
#include "common_timer_manager.inl"

#endif
