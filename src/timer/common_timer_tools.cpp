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
#include "stdafx.h"

#include <sstream>

#include "ace/Synch.h"
#include "common_timer_tools.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_timer_manager_common.h"

//Common_ITimer*
//Common_Timer_Tools::getTimerManager ()
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::getTimerManager"));
//
//  return COMMON_TIMERMANAGER_SINGLETON::instance ();
//}

ACE_Time_Value
Common_Timer_Tools::stringToTimestamp (const std::string& timestamp_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::stringToTimestamp"));

  // initialize return value(s)
  ACE_Time_Value return_value = ACE_Time_Value::zero;

  // sanity check(s)
  ACE_ASSERT (timestamp_in.size () == 19);

  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  // *TODO*: use ::sscanf()/strptime() here, it's more efficient
  std::istringstream converter (timestamp_in);
  char char_c = 0;
  converter >> tm_s.tm_year;
  tm_s.tm_year -= 1900;
  converter >> char_c;
  ACE_ASSERT (char_c == '/');
  converter >> tm_s.tm_mon;
  --tm_s.tm_mon;
  converter >> char_c;
  ACE_ASSERT (char_c == '/');
  converter >> tm_s.tm_mday;
  converter >> char_c;
  ACE_ASSERT (char_c == ' ');
  converter >> tm_s.tm_hour;
  converter >> char_c;
  ACE_ASSERT (char_c == ':');
  converter >> tm_s.tm_min;
  converter >> char_c;
  ACE_ASSERT (char_c == ':');
  converter >> tm_s.tm_sec;
  tm_s.tm_isdst = -1; // (try to) guess DST
  time_t time = ACE_OS::mktime (&tm_s);
  if (unlikely (time == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::mktime(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (timestamp_in.c_str ())));
    return return_value;
  } // end IF
  return_value.set (time, 0);

  return return_value;
}

std::string
Common_Timer_Tools::periodToString (const ACE_Time_Value& period_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::periodToString"));

  // initialize return value(s)
  std::string return_value;

  // extract hours and minutes
  ACE_Time_Value temp = period_in;
  int hours = static_cast<int> (temp.sec()) / (60 * 60);
  temp.sec (temp.sec () % (60 * 60));

  int minutes = static_cast<int> (temp.sec ()) / 60;
  temp.sec (temp.sec () % 60);

  char time_string[BUFSIZ];
  // *TODO*: rewrite this in C++...
  if (unlikely (ACE_OS::snprintf (time_string,
                                  sizeof (time_string),
                                  ACE_TEXT_ALWAYS_CHAR ("%d:%d:%d.%d"),
                                  hours,
                                  minutes,
                                  static_cast<int> (temp.sec ()),
                                  static_cast<int> (temp.usec ())) < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  return_value = time_string;

  return return_value;
}

std::string
Common_Timer_Tools::timestampToString (const ACE_Time_Value& timeStamp_in,
                                       bool UTC_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::timestampToString"));

  // initialize return value(s)
  std::string return_value;

  time_t timestamp = timeStamp_in.sec ();
  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  if (UTC_in)
  {
    if (unlikely (!ACE_OS::gmtime_r (&timestamp, &tm_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::gmtime_r(): \"%m\", aborting\n")));
      return return_value;
    } // end IF
  } // end IF
  else
    if (unlikely (!ACE_OS::localtime_r (&timestamp, &tm_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
      return return_value;
    } // end IF

  char time_string[BUFSIZ];
  ACE_OS::memset (time_string, 0, BUFSIZ);
  // *TODO*: rewrite this in C++...
  if (unlikely (ACE_OS::snprintf (time_string,
                                  sizeof (time_string),
                                  ACE_TEXT_ALWAYS_CHAR ("%u-%u-%u %u:%u:%u.%ld"),
                                  tm_s.tm_year + 1900,
                                  tm_s.tm_mon + 1,
                                  tm_s.tm_mday,
                                  tm_s.tm_hour,
                                  tm_s.tm_min,
                                  tm_s.tm_sec,
                                  timeStamp_in.usec ()) < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  return_value = time_string;

  return return_value;
}

bool
Common_Timer_Tools::initializeTimers (const struct Common_TimerConfiguration& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::initializeTimers"));

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  //Common_Timer_Manager_Asynch_t* timer_manager_2 =
  //  COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  //ACE_ASSERT (timer_manager_2);

  switch (configuration_in.dispatch)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();

      return true;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();
      if (unlikely (!timer_manager_p->isRunning ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to start timer manager, aborting\n")));
        return false;
      } // end IF

      return true;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      if (unlikely (!timer_manager_p->initialize (configuration_in)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF

      timer_manager_p->start ();

      return true;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (false);
      
      ACE_NOTREACHED (return false;)
      //if (!timer_manager_2->initialize (configuration_in))
      //{
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to initialize timer manager, aborting\n")));
      //  return false;
      //} // end IF

      //timer_manager_2->start ();

      //return true;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer dispatch (was: %d), returning\n"),
                  configuration_in.dispatch));
      break;
    }
  } // end SWITCH

  return false;
}

void
Common_Timer_Tools::finalizeTimers (enum Common_TimerDispatchType dispatchType_in,
                                    bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::finalizeTimers"));

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  //Common_Timer_Manager_Asynch_t* timer_manager_2 =
  //  COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  //ACE_ASSERT (timer_manager_2);

  switch (dispatchType_in)
  {
    case COMMON_TIMER_DISPATCH_PROACTOR:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_QUEUE:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_REACTOR:
    {
      timer_manager_p->stop (waitForCompletion_in,
                             true);
      break;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP;

      ACE_NOTREACHED (return;)
      //timer_manager_2->stop (waitForCompletion_in,
      //                       true);
      //break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer dispatch type (was: %d), returning\n"),
                  dispatchType_in));
      break;
    }
  } // end SWITCH
}
