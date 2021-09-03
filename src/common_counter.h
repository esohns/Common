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

#ifndef COMMON_COUNTER_H
#define COMMON_COUNTER_H

#include "ace/Global_Macros.h"

#include "common_icounter.h"
#include "common_referencecounter.h"

template <ACE_SYNCH_DECL>
class Common_Counter_T
 : public Common_ReferenceCounter_T<ACE_SYNCH_USE>
 , virtual Common_ICounter
{
  typedef Common_ReferenceCounter_T<ACE_SYNCH_USE> inherited;
 
 public:
  Common_Counter_T (unsigned int = 0); // initial count
  inline virtual ~Common_Counter_T () {}

  // implement Common_ICounter
  virtual void reset ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Counter_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Counter_T (const Common_Counter_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Counter_T& operator= (const Common_Counter_T&))

  unsigned int initialCount_;
};

// include template definition
#include "common_counter.inl"

#endif
