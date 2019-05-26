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

#ifndef COMMON_TIMER_SECOND_PUBLISHER_H
#define COMMON_TIMER_SECOND_PUBLISHER_H

#include <list>

#include "ace/Condition_T.h"
#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Synch_Traits.h"
#include "ace/Thread_Mutex.h"

#include "common_icounter.h"
#include "common_idumpstate.h"
#include "common_ilock.h"
#include "common_isubscribe.h"
#include "common_itaskcontrol.h"

#include "common_timer_resetcounterhandler.h"
#include "common_timer_manager_common.h"

template <typename TimerManagerType>
class Common_Timer_SecondPublisher_T
 : public Common_ITaskControl_T<ACE_NULL_SYNCH,
                                Common_ILock_T<ACE_NULL_SYNCH> >
 , public Common_ISubscribe_T<Common_ICounter>
 , public Common_IDumpState
 , public Common_ICounter
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<Common_Timer_SecondPublisher_T<TimerManagerType>,
                             ACE_Thread_Mutex>;

 public:
  // convenient types
  typedef Common_ISubscribe_T<Common_ICounter> INTERFACE_T;
  typedef ACE_Singleton<Common_Timer_SecondPublisher_T<TimerManagerType>,
                        ACE_Thread_Mutex> SINGLETON_T;

  // implement (part of) Common_ITaskControl_T
  virtual void start (ACE_thread_t&); // return value: thread handle (if any)
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // N/A
  inline virtual bool isRunning () const { return (timerId_ != -1); }
  virtual void wait (bool = true) const; // N/A

  virtual void subscribe (Common_ICounter*);
  virtual void unsubscribe (Common_ICounter*);

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  // convenient types
  typedef Common_ITaskControl_T<ACE_NULL_SYNCH,
                                Common_ILock_T<ACE_NULL_SYNCH> > ITASKCONTROL_T;
  typedef std::list<Common_ICounter*> SUBSCRIBERS_T;
  typedef SUBSCRIBERS_T::iterator SUBSCRIBERS_ITERATOR_T;

  Common_Timer_SecondPublisher_T ();
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_SecondPublisher_T (const Common_Timer_SecondPublisher_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_SecondPublisher_T& operator= (const Common_Timer_SecondPublisher_T&))
  virtual ~Common_Timer_SecondPublisher_T ();

  // hide (part of) Common_ITaskControl_T
  inline virtual bool lock (bool = true) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual int unlock (bool = false) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  inline virtual const typename ITASKCONTROL_T::MUTEX_T& getR () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (typename ITASKCONTROL_T::MUTEX_T ()); ACE_NOTREACHED (return typename ITASKCONTROL_T::MUTEX_T ();) }
  inline virtual void idle () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void finished () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  // implement Common_ICounter
  virtual void reset ();

  // helper methods
  long toggleTimer ();

  ACE_Condition_Recursive_Thread_Mutex condition_;
  Common_Timer_ResetCounterHandler     handler_;
  mutable ACE_Recursive_Thread_Mutex   lock_;
  std::list<Common_ICounter*>          subscribers_;
  long                                 timerId_;
};

// include template definition
#include "common_timer_second_publisher.inl"

//////////////////////////////////////////

typedef Common_Timer_SecondPublisher_T<Common_Timer_Manager_t> Common_Timer_SecondPublisher_t;

typedef ACE_Singleton<Common_Timer_SecondPublisher_t,
                      ACE_SYNCH_MUTEX> COMMON_TIMER_SECONDPUBLISHER_SINGLETON;

#endif
