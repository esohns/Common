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
#include "ace/Synch_Traits.h"

class Common_ILock
{
 public:
  virtual ~Common_ILock () {}

  // exposed interface
  // *NOTE*: this returns whether unlock() needs to be called
  virtual bool lock (bool = true) = 0; // block ?
  // *NOTE*: this returns the new nesting level (or -1, if the lock is not held
  //         by the caller)
  virtual int unlock (bool = false) = 0; // unlock ?
};

template <ACE_SYNCH_DECL>
class Common_ILock_T
 : public Common_ILock
{
 public:
  virtual ~Common_ILock_T () {}

  // exposed interface
  virtual ACE_SYNCH_RECURSIVE_MUTEX& getLock () = 0;
};

#define COMMON_ILOCK_ACQUIRE_N(ilock, count) \
  do { \
    ACE_ASSERT (ilock); \
    ACE_ASSERT (count > 0); \
    for (int i = 0; i < count; ++i) \
      ilock->lock (true); \
  } while (0)

#endif
