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

#ifndef COMMON_ILOCK_H
#define COMMON_ILOCK_H

#include "ace/Global_Macros.h"
//#include "ace/Synch_Traits.h"

#include "common_iget.h"

class Common_ILock
{
 public:
  // *NOTE*: returns whether unlock() needs to be called
  virtual bool lock (bool = true) = 0; // block ?
  // *NOTE*: returns the new nesting level (or -1, if the lock was not held by
  //         the caller)
  virtual int unlock (bool = false) = 0; // unlock completely ?
};

template <ACE_SYNCH_DECL>
class Common_ILock_T
 : public Common_ILock
 , public Common_IGetR_2_T<ACE_SYNCH_MUTEX_T>
{
 public:
  // convenient types
  typedef ACE_SYNCH_MUTEX_T MUTEX_T;
};

template <ACE_SYNCH_DECL>
class Common_IRecursiveLock_T
 : public Common_ILock
 , public Common_IGetR_2_T<typename ACE_SYNCH_USE::RECURSIVE_MUTEX>
{
 public:
  // convenient types
  typedef typename ACE_SYNCH_USE::RECURSIVE_MUTEX MUTEX_T;
};

//////////////////////////////////////////

#define COMMON_ILOCK_ACQUIRE_N(ilock, count) \
  do { \
    ACE_ASSERT (ilock); \
    for (int i = 0; i < count; ++i) \
      ilock->lock (true); \
  } while (0)

#endif
