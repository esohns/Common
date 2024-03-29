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

#ifndef COMMON_ISTATISTIC_H
#define COMMON_ISTATISTIC_H

#include "ace/Time_Value.h"

template <typename StatisticContainerType>
class Common_IStatistic_T
{
 public:
  // *NOTE*: how all methods implement a visitor pattern
   
  // *NOTE*: the argument may (!) serve both as input/output, this depends on
   //        the implementation
  virtual bool collect (StatisticContainerType&) = 0;
  virtual void update (const ACE_Time_Value&) = 0; // update interval

  virtual void report () const = 0;
};

#endif
