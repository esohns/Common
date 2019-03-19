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

#ifndef COMMON_TIMERHANDLER_H
#define COMMON_TIMERHANDLER_H

#include "ace/Asynch_IO.h"
#include "ace/Global_Macros.h"
#include "ace/Event_Handler.h"
#include "ace/Time_Value.h"

#include "common_iget.h"
#include "common_itimerhandler.h"

class Common_TimerHandler
 : public ACE_Event_Handler
 , public ACE_Handler
 , public Common_ITimerHandler
 , public Common_ISet_T<long>
{
  typedef ACE_Event_Handler inherited;
  typedef ACE_Handler inherited2;

 public:
  // *NOTE*: if there is no default ctor, this will not compile
  inline Common_TimerHandler () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  // *NOTE*: the second argument applies to reactor-based dispatch only
  Common_TimerHandler (Common_ITimerHandler*, // effective dispatch interface (NULL: 'this')
                       bool);                 // invoke only once ?
  inline virtual ~Common_TimerHandler () {}

  // implement Common_ITimerHandler
  inline virtual const long get () const { return id_; }
  inline virtual void handle (const void* act_in) { ACE_UNUSED_ARG (act_in); }

  // implement Common_ISet_T
  inline virtual void set (const long id_in) { id_ = id_in; }

  // override (part of) ACE_Event_Handler
  virtual int handle_close (ACE_HANDLE,        // handle
                            ACE_Reactor_Mask); // mask
  virtual int handle_timeout (const ACE_Time_Value&, // dispatch time
                              const void*);          // asynchronous completion token
  // override (part of) ACE_Handler
  virtual void handle_time_out (const ACE_Time_Value&, // requested time
                                const void* = NULL);   // asynchronous completion token

 protected:
  long                  id_;
  bool                  isOneShot_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_TimerHandler (const Common_TimerHandler&))
  ACE_UNIMPLEMENTED_FUNC (Common_TimerHandler& operator= (const Common_TimerHandler&))

  Common_ITimerHandler* handler_;
};

#endif
