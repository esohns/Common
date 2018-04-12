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

#ifndef COMMON_TIMER_TOOLS_H
#define COMMON_TIMER_TOOLS_H

#include <string>

#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"

#include "common_timer_common.h"
 //#include "common_timer_exports.h"

// forward declaration(s)
//class Common_ITimer;

//class Common_Export Common_Timer_Tools
class Common_Timer_Tools
{
 public:
  //// --- singleton ---
  //static Common_ITimer* getTimerManager ();

  // --- strings ---
  // *NOTE*: uses ::snprintf(3): "HH:MM:SS.usec"
  static std::string periodToString (const ACE_Time_Value&); // period
  // *NOTE*: uses ::snprintf(3): "YYYY-MM-DD HH:MM:SS.usec"
  static std::string timestampToString (const ACE_Time_Value&, // timestamp
                                        bool);                 // UTC ? : localtime

  // --- timers ---
  static bool initializeTimers (const struct Common_TimerConfiguration&); // configuration
  static void finalizeTimers (enum Common_TimerDispatchType, // dispatch type
                              bool = true);                  // wait for completion ?

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Timer_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools (const Common_Timer_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools& operator= (const Common_Timer_Tools&))
};

#endif
