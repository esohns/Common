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

#ifndef COMMON_STATISTIC_HANDLER_H
#define COMMON_STATISTIC_HANDLER_H

#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"

#include "common.h"
#include "common_istatistic.h"

#include "common_timer_handler.h"

template <typename StatisticContainerType>
class Common_StatisticHandler_T
 : public Common_TimerHandler
{
  typedef Common_TimerHandler inherited;

 public:
  // convenient types
  typedef Common_IStatistic_T<StatisticContainerType> ISTATISTIC_T;

  // *NOTE*: if there is no default ctor, this will not compile
  inline Common_StatisticHandler_T () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  Common_StatisticHandler_T (enum Common_StatisticActionType, // handler action
                             ISTATISTIC_T*,                   // interface handle
                             bool = false);                   // report on collect ?
  inline virtual ~Common_StatisticHandler_T () {}

  // implement Common_ITimerHandler
  virtual void handle (const void*); // asynchronous completion token

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_StatisticHandler_T (const Common_StatisticHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_StatisticHandler_T& operator= (const Common_StatisticHandler_T&))

  enum Common_StatisticActionType action_;
  ISTATISTIC_T*                   interfaceHandle_;
  bool                            reportOnCollect_;
};

// include template definition
#include "common_statistic_handler.inl"

#endif
