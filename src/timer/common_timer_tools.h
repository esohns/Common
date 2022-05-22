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

#include "ace/Date_Time.h"
#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"

#include "common_iinitialize.h"

#include "common_timer_common.h"

class Common_Timer_Tools
 : public Common_SInitializeFinalize_T<Common_Timer_Tools>
{
 public:
  static void initialize ();
  static void finalize ();
   
  //// --- singleton ---
  //static Common_ITimer* getTimerManager ();

  static ACE_Time_Value localToUTC (const ACE_Time_Value&, // local time
                                    int = 0,               // timezone (UTC offset)
                                    bool = false);         // apply timezone ?
  static ACE_Time_Value UTCToLocal (const ACE_Time_Value&); // UTC time
  static std::string dateTimeToString (const ACE_Date_Time&); // date

  // --- parsers ---
  static ACE_Time_Value ISO8601ToTimestamp (const std::string&,
                                            int&);              // return value: UTC offset
  // *NOTE*: parses "YYYY/MM/DD HH:MM:SS"
  // *NOTE*: the dhclient lease file date format depends on the 'db-time-format'
  //         configuration parameter; this parses the 'default' setting
  // *NOTE*: the input is assumed to be UTC
  static ACE_Time_Value stringToTimestamp (const std::string&);

  // --- strings ---
  // *NOTE*: uses ::snprintf(3): "HH:MM:SS.usec"
  static std::string periodToString (const ACE_Time_Value&); // period
  // *NOTE*: uses ::snprintf(3): "YYYY-MM-DD HH:MM:SS.usec{ TZ/UTC}"
  static std::string timestampToString (const ACE_Time_Value&, // timestamp
                                        bool,                  // UTC ? : localtime
                                        bool = false);         // append timezone ?
  // *NOTE*: uses ::snprintf(3): "YYYY-MM-DD HH.MM.SS"
  static std::string timestampToString2 (const ACE_Time_Value&); // timestamp

  static struct Common_TimerConfiguration configuration_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Timer_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools (const Common_Timer_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Timer_Tools& operator= (const Common_Timer_Tools&))

  // --- timers ---
  static bool initializeTimers (const struct Common_TimerConfiguration&); // configuration
  static void finalizeTimers (enum Common_TimerDispatchType, // dispatch type
                              bool = true);                  // wait for completion ?
};

#endif
