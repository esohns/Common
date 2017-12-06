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

#ifndef COMMON_ITASKCONTROL_H
#define COMMON_ITASKCONTROL_H

#include "ace/Global_Macros.h"

#include "common_itask.h"

template <ACE_SYNCH_DECL,
          typename LockType> // implements Common_ILock_T/Common_IRecursiveLock_T
class Common_ITaskControl_T
 : virtual public Common_ITask_T<ACE_SYNCH_USE,
                                 LockType>
{
 public:
  // convenient types
  typedef Common_ITask_T<ACE_SYNCH_USE,
                         LockType> ITASK_T;

  // *NOTE*: signal asynchronous completion
  virtual void finished () = 0;
  virtual void wait (bool = true) const = 0; // wait for the message queue ? : worker thread(s) only
};

//////////////////////////////////////////

#endif
