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

#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename StatisticContainerType>
Common_StatisticHandler_T<StatisticContainerType>::Common_StatisticHandler_T (enum Common_StatisticActionType action_in,
                                                                              ISTATISTIC_T* interfaceHandle_in,
                                                                              bool reportOnCollect_in)
 : inherited (this,  // dispatch interface
              false) // invoke only once ?
 , action_ (action_in)
 , interfaceHandle_ (interfaceHandle_in)
 , reportOnCollect_ (reportOnCollect_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StatisticHandler_T::Common_StatisticHandler_T"));

}

template <typename StatisticContainerType>
void
Common_StatisticHandler_T<StatisticContainerType>::handle (const void* arg_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_StatisticHandler_T::handle"));

  ACE_UNUSED_ARG (arg_in);

  switch (action_)
  {
    case COMMON_STATISTIC_ACTION_COLLECT:
    {
      StatisticContainerType result;
      ACE_OS::memset (&result, 0, sizeof (StatisticContainerType));
      try {
        if (!interfaceHandle_->collect (result))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Common_IStatistic::collect(), continuing\n")));
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught an exception in Common_IStatistic::collect(), continuing\n")));
      }
      if (reportOnCollect_)
        goto report;
      break;
    }
    case COMMON_STATISTIC_ACTION_UPDATE:
    {
      try {
        interfaceHandle_->update ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught an exception in Common_IStatistic::update(), continuing\n")));
      }
      break;
    }
    case COMMON_STATISTIC_ACTION_REPORT:
    {
report:
      try {
        interfaceHandle_->report ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught an exception in Common_IStatistic::report(), continuing\n")));
      }
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid action (was: %d), continuing\n"),
                  action_));
      break;
    }
  } // end SWITCH
}
