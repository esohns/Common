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

#ifndef COMMON_REFERENCECOUNTER_H
#define COMMON_REFERENCECOUNTER_H

#include "ace/Condition_Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Global_Macros.h"
#include "ace/Refcountable_T.h"

#include "common_ireferencecount.h"

template <ACE_SYNCH_DECL>
class Common_ReferenceCounter_T
 : public ACE_Refcountable_T<ACE_SYNCH_MUTEX_T>
 , virtual public Common_IReferenceCount
{
  typedef ACE_Refcountable_T<ACE_SYNCH_MUTEX_T> inherited;

 public:
  Common_ReferenceCounter_T (unsigned int = 1); // initial count (delete 'this' on 0)
  Common_ReferenceCounter_T (const Common_ReferenceCounter_T&);
  inline virtual ~Common_ReferenceCounter_T () {}

  // implement Common_IReferenceCount
  inline virtual unsigned int increase () { return static_cast<unsigned int> (inherited::increment ()); }
  virtual unsigned int decrease ();
  inline virtual unsigned int count () const { return static_cast<unsigned int> (inherited::refcount_.value ()); }
  virtual void wait (unsigned int = 0) const;

  // convenient types
  typedef Common_ReferenceCounter_T<ACE_SYNCH_USE> OWN_TYPE_T;
  OWN_TYPE_T& operator= (const OWN_TYPE_T&);

 protected:
  Common_ReferenceCounter_T ();
  // *TODO*: "delete on 0" may not work in a multi-threaded context
  Common_ReferenceCounter_T (unsigned int = 1, // initial reference count
                             bool = true);     // delete 'this' on 0 ?

  // *IMPORTANT NOTE*: cannot reuse the lock from ACE_Atomic_Op<> (it's private :()
  mutable ACE_SYNCH_MUTEX_T     lock_;

 private:
  mutable ACE_SYNCH_CONDITION_T condition_;
  bool                          deleteOnZero_;
};

// include template definition
#include "common_referencecounter.inl"

#endif
