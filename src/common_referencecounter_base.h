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

#include "ace/Refcountable_T.h"
#include "ace/Synch_Traits.h"

#include "common_exports.h"
#include "common_ireferencecount.h"

class Common_ReferenceCounterBase
 : public ACE_Refcountable_T<ACE_SYNCH_MUTEX>
 , virtual public Common_IReferenceCount
{
  typedef ACE_Refcountable_T<ACE_SYNCH_MUTEX> inherited;

 public:
  Common_ReferenceCounterBase (long); // initial count (no delete on 0)
  Common_ReferenceCounterBase (const Common_ReferenceCounterBase&);
  inline virtual ~Common_ReferenceCounterBase () {}

  // implement Common_IReferenceCount
  inline virtual unsigned int increase () { return static_cast<unsigned int> (inherited::increment ()); }
  virtual unsigned int decrease ();
  inline virtual unsigned int count () const { return static_cast<unsigned int> (inherited::refcount_.value ()); }
  virtual void wait (unsigned int = 0) const;

  Common_ReferenceCounterBase& operator= (const Common_ReferenceCounterBase&);

 protected:
  Common_ReferenceCounterBase ();
  // *TODO*: "delete on 0" may not work in a multi-threaded context
  Common_ReferenceCounterBase (long,  // initial reference count
                               bool); // delete on 0 ?

  mutable ACE_SYNCH_MUTEX     lock_;

 private:
  mutable ACE_SYNCH_CONDITION condition_;
  bool                        deleteOnZero_;
};

#endif
