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

#ifndef COMMON_REFERENCECOUNTER_BASE_H
#define COMMON_REFERENCECOUNTER_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Condition_T.h"
#include "ace/Synch.h"

#include "common_exports.h"
#include "common_irefcount.h"

class Common_Export Common_ReferenceCounterBase
 : virtual public Common_IRefCount
{
 public:
  virtual ~Common_ReferenceCounterBase ();

  // implement Common_IRefCount
  virtual void increase ();
  virtual void decrease ();
  virtual unsigned int count ();
  virtual void wait_zero ();

 protected:
  // *WARNING*: "delete on 0" may not work predictably if there are
  // any waiters (or in ANY multithreaded context, for that matter)...
  Common_ReferenceCounterBase (unsigned int = 1, // initial reference count
                               bool = true);     // delete on 0 ?

  mutable ACE_Recursive_Thread_Mutex        lock_;
  unsigned int                              counter_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_ReferenceCounterBase ());
  ACE_UNIMPLEMENTED_FUNC (Common_ReferenceCounterBase (const Common_ReferenceCounterBase&););
  ACE_UNIMPLEMENTED_FUNC (Common_ReferenceCounterBase& operator= (const Common_ReferenceCounterBase&));

  bool                                      deleteOnZero_;
  ACE_Condition<ACE_Recursive_Thread_Mutex> condition_;
};

#endif
