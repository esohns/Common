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

#include "common_timer_tools.h"

#include <ostream>
#include <regex>
#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_timer_manager_common.h"
#include "common_timer_second_publisher.h"

// initialize statics
struct Common_TimerConfiguration Common_Timer_Tools::configuration_;

void
Common_Timer_Tools::initialize ()
{
  if (!Common_Timer_Tools::initializeTimers (Common_Timer_Tools::configuration_))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Timer_Tools::initializeTimers(), continuing\n")));

  if (Common_Timer_Tools::configuration_.publishSeconds)
  {
    Common_Timer_SecondPublisher_t* publisher_p =
      COMMON_TIMER_SECONDPUBLISHER_SINGLETON::instance ();
    publisher_p->start (NULL);
  } // end IF
}
void
Common_Timer_Tools::finalize ()
{
  Common_Timer_Tools::finalizeTimers (Common_Timer_Tools::configuration_.dispatch,
                                      true);

  if (Common_Timer_Tools::configuration_.publishSeconds)
    COMMON_TIMER_SECONDPUBLISHER_SINGLETON::instance ()->stop (true,  // N/A
                                                               true); // N/A
}

std::string
Common_Timer_Tools::timeStampToLocalString (const ACE_Time_Value& timeStamp_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::::timeStampToLocalString"));

  // initialize return value(s)
  std::string result;

  //ACE_Date_Time time_local(timestamp_in);
  tm time_local;
  // init structure
  time_local.tm_sec = -1;
  time_local.tm_min = -1;
  time_local.tm_hour = -1;
  time_local.tm_mday = -1;
  time_local.tm_mon = -1;
  time_local.tm_year = -1;
  time_local.tm_wday = -1;
  time_local.tm_yday = -1;
  time_local.tm_isdst = -1; // expect localtime !!!
  // *PORTABILITY*: this isn't entirely portable so do an ugly hack
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  time_local.tm_gmtoff = 0;
  time_local.tm_zone = NULL;
#endif // ACE_WIN32 || ACE_WIN64

  // step1: compute UTC representation
  time_t time_seconds = timeStamp_in.sec ();
  // *PORTABILITY*: man page says call this before...
  ACE_OS::tzset ();
  if (!ACE_OS::localtime_r (&time_seconds,
                            &time_local))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
    return result;
  } // end IF

  // step2: create string
  // *TODO*: rewrite this in C++
  char buffer_a[BUFSIZ];
  if (ACE_OS::strftime (buffer_a,
                        sizeof (char[BUFSIZ]),
                        ACE_TEXT_ALWAYS_CHAR (COMMON_TIMER_STRFTIME_FORMAT),
                        &time_local) != COMMON_TIMER_STRFTIME_SIZE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strftime(): \"%m\", aborting\n")));
    return result;
  } // end IF
  result = buffer_a;

  // OK: append any usecs
  if (timeStamp_in.usec ())
  {
    std::ostringstream converter;
    converter << timeStamp_in.usec ();
    result += ACE_TEXT_ALWAYS_CHAR (".");
    result += converter.str ();
  } // end IF

  return result;
}

std::string
Common_Timer_Tools::dateTimeToString (const ACE_Date_Time& date_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::dateTimeToString"));

  std::string result;

  std::ostringstream converter;
  converter << date_in.year ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("-");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.month ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("-");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.day ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (" ");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.hour ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (":");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.minute ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (":");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.second ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (".");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << date_in.microsec ();
  result += converter.str ();

  return result;
}

ACE_Time_Value
Common_Timer_Tools::localToUTC (const ACE_Time_Value& localTime_in,
                                int tm_gmtoff_in,
                                bool applyTimezone_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::localToUTC"));

  // initialize return value(s)
  ACE_Time_Value return_value = ACE_Time_Value::zero;

  time_t time = localTime_in.sec ();
  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));

  if (unlikely (applyTimezone_in))
  {
    if (!ACE_OS::localtime_r (&time, &tm_s))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
      return return_value;
    } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    tm_s.tm_hour += tm_gmtoff_in;
    if (tm_s.tm_hour < 0)
    {
      tm_s.tm_hour += 23;
      tm_s.tm_yday--;
      if (tm_s.tm_yday < 0)
      {
        tm_s.tm_yday += 365; // *TODO*: this doesn't work
        tm_s.tm_year--;
      } // end IF
    } // end IF
    else if (tm_s.tm_hour > 23)
    {
      tm_s.tm_hour -= 23;
      tm_s.tm_yday++;
      if (tm_s.tm_yday > 365) // *TODO*: this doesn't work
      {
        tm_s.tm_yday -= 365;
        tm_s.tm_year++;
      } // end IF
    } // end ELSE IF
#else
    tm_s.tm_gmtoff = tm_gmtoff_in;
#endif // ACE_WIN32 || ACE_WIN64
    time = ACE_OS::mktime (&tm_s);
    if (unlikely (time == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("failed to ACE_OS::mktime(): \"%m\", aborting\n")));
      return return_value;
    } // end IF
  } // end IF

  if (!ACE_OS::gmtime_r (&time, &tm_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::gmtime_r(): \"%m\", aborting\n")));
    return return_value;
  } // end IF

  time = ACE_OS::mktime (&tm_s);
  if (unlikely (time == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::mktime(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  return_value.set (time, localTime_in.usec ());

  return return_value;
}

ACE_Time_Value
Common_Timer_Tools::UTCToLocal (const ACE_Time_Value& UTCTime_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::UTCToLocal"));

  // initialize return value(s)
  ACE_Time_Value return_value = ACE_Time_Value::zero;

  time_t time = UTCTime_in.sec ();
  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  if (!ACE_OS::localtime_r (&time, &tm_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
    return return_value;
  } // end IF

  time = ACE_OS::mktime (&tm_s);
  if (unlikely (time == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::mktime(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  return_value.set (time, UTCTime_in.usec ());

  return return_value;
}

ACE_Time_Value
Common_Timer_Tools::ISO8601ToTimestamp (const std::string& timestamp_in,
                                        int& tm_gmtoff_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::ISO8601ToTimestamp"));

  // initialize return value(s)
  tm_gmtoff_out = 0;
  ACE_Time_Value return_value = ACE_Time_Value::zero;

  // sanity check(s)
  ACE_ASSERT (timestamp_in.size () >= 23);

  std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]{4})-([[:digit:]]{2})-([[:digit:]]{2})T([[:digit:]]{2}):([[:digit:]]{2}):([[:digit:]]{2}).([[:digit:]]{3})(Z|.+)$"));
  std::smatch match_results;
  if (!std::regex_match (timestamp_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to match string (was: \"%s\"), aborting\n"),
                ACE_TEXT (timestamp_in.c_str ())));
    return return_value;
  } // end IF
  ACE_ASSERT (match_results[1].matched);
  ACE_ASSERT (match_results[2].matched);
  ACE_ASSERT (match_results[3].matched);
  ACE_ASSERT (match_results[4].matched);
  ACE_ASSERT (match_results[5].matched);
  ACE_ASSERT (match_results[6].matched);
  ACE_ASSERT (match_results[7].matched);
  ACE_ASSERT (match_results[8].matched);

  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  std::istringstream converter (match_results[1].str ());
  converter >> tm_s.tm_year;
  tm_s.tm_year -= 1900;
  converter.clear ();
  converter.str (match_results[2].str ());
  converter >> tm_s.tm_mon;
  --tm_s.tm_mon;
  converter.clear ();
  converter.str (match_results[3].str ());
  converter >> tm_s.tm_mday;
  converter.clear ();
  converter.str (match_results[4].str ());
  converter >> tm_s.tm_hour;
  converter.clear ();
  converter.str (match_results[5].str ());
  converter >> tm_s.tm_min;
  converter.clear ();
  converter.str (match_results[6].str ());
  converter >> tm_s.tm_sec;

  // process timezone
  converter.clear ();
  converter.str (match_results[8].str ());
  char char_c = 0;
  converter >> char_c;
  if (char_c == 'Z')
  {
//    tm_s.tm_isdst = 0; // input is UTC
  } // end IF
  else
  {
    // *TODO*: parse time zone
    ACE_ASSERT (false);
  } // end ELSE

  time_t time = ACE_OS::mktime (&tm_s);
  if (unlikely (time == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_OS::mktime(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT (timestamp_in.c_str ())));
    return return_value;
  } // end IF
  converter.clear ();
  converter.str (match_results[7].str ());
  converter >> tm_s.tm_sec;
  return_value.set (time, static_cast<suseconds_t> (1000 * tm_s.tm_sec));

  return return_value;
}

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
//  converter >> char_c;
//  ACE_ASSERT (char_c == ' ');
  converter >> tm_s.tm_hour;
  converter >> char_c;
  ACE_ASSERT (char_c == ':');
  converter >> tm_s.tm_min;
  converter >> char_c;
  ACE_ASSERT (char_c == ':');
  converter >> tm_s.tm_sec;
  tm_s.tm_isdst = 0; // input is UTC
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
                                       bool UTC_in,
                                       bool appendTimezone_in)
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
  {
    if (unlikely (!ACE_OS::localtime_r (&timestamp, &tm_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
      return return_value;
    } // end IF
  } // end ELSE

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

  if (unlikely (appendTimezone_in))
  {
    std::string timezone_string;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // set up the required privilege
    HANDLE token_h = ACE_INVALID_HANDLE;
    if (!OpenProcessToken (GetCurrentProcess (),
                           TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,
                           &token_h))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to OpenProcessToken(0x%@): \"%s\", aborting\n"),
                  GetCurrentProcess (),
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
      return return_value;
    } // end IF
    ACE_ASSERT (token_h != ACE_INVALID_HANDLE);
    struct _TOKEN_PRIVILEGES token_privileges_s;
    ACE_OS::memset (&token_privileges_s, 0, sizeof (struct _TOKEN_PRIVILEGES));
    struct _TIME_ZONE_INFORMATION time_zone_s;
    if (!LookupPrivilegeValue (NULL, // lpSystemName
                               SE_TIME_ZONE_NAME,
                               &token_privileges_s.Privileges[0].Luid))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to LookupPrivilegeValue(%s): \"%s\", aborting\n"),
                  COMMON_TEXT (SE_TIME_ZONE_NAME),
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
      goto clean;
    } // end IF
    token_privileges_s.PrivilegeCount = 1;
    token_privileges_s.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges (token_h,
                                FALSE,                   // DisableAllPrivileges
                                &token_privileges_s,
                                0,                       // BufferLength
                                (PTOKEN_PRIVILEGES)NULL, // PreviousState
                                NULL))                   // ReturnLength
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to AdjustTokenPrivileges(): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
      goto clean;
    } // end IF

    // retrieve the current time zone information
    ACE_OS::memset (&time_zone_s, 0, sizeof (struct _TIME_ZONE_INFORMATION));
    switch (GetTimeZoneInformation (&time_zone_s))
    {
      case TIME_ZONE_ID_UNKNOWN:
      case TIME_ZONE_ID_STANDARD:
        timezone_string =
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (time_zone_s.StandardName));
        break;
      case TIME_ZONE_ID_DAYLIGHT:
        timezone_string =
          ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (time_zone_s.DaylightName));
        break;
      case TIME_ZONE_ID_INVALID:
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to GetTimeZoneInformation(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
        goto clean;
      }
    } // end SWITCH
clean:
    if (token_h != ACE_INVALID_HANDLE)
      if (!CloseHandle (token_h))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CloseHandle(): \"%s\", continuing\n"),
                    ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
#else
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (return_value);
    ACE_NOTREACHED (return return_value;)
#endif // ACE_WIN32 || ACE_WIN64

    return_value += ACE_TEXT_ALWAYS_CHAR (" ");
    return_value += timezone_string;
  } // end IF

  return return_value;
}

std::string
Common_Timer_Tools::timestampToString2 (const ACE_Time_Value& timeStamp_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Timer_Tools::timestampToString2"));

  // initialize return value(s)
  std::string return_value;

  time_t timestamp = timeStamp_in.sec ();
  struct tm tm_s;
  ACE_OS::memset (&tm_s, 0, sizeof (struct tm));
  if (unlikely (!ACE_OS::localtime_r (&timestamp, &tm_s)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::localtime_r(): \"%m\", aborting\n")));
    return return_value;
  } // end IF

  char time_string[BUFSIZ];
  ACE_OS::memset (time_string, 0, sizeof (char[BUFSIZ]));
  // *TODO*: rewrite this in C++...
  if (unlikely (ACE_OS::snprintf (time_string,
                                  sizeof (time_string),
                                  ACE_TEXT_ALWAYS_CHAR ("%u-%u-%u %u.%u.%u"),
                                  tm_s.tm_mday,
                                  tm_s.tm_mon + 1,
                                  tm_s.tm_year + 1900,
                                  tm_s.tm_hour,
                                  tm_s.tm_min,
                                  tm_s.tm_sec) < 0))
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Common_Timer_Manager_Asynch_t* timer_manager_2 =
    COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_2);
#endif // ACE_WIN32 || ACE_WIN64

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
      timer_manager_p->start (NULL);
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

      timer_manager_p->start (NULL);
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
      timer_manager_p->start (NULL);
      return true;
    }
    case COMMON_TIMER_DISPATCH_SIGNAL:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      if (!timer_manager_2->initialize (configuration_in))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize timer manager, aborting\n")));
        return false;
      } // end IF
      timer_manager_2->start (NULL);
#endif // ACE_WIN32 || ACE_WIN64
      return true;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown timer dispatch (was: %d), aborting\n"),
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Common_Timer_Manager_Asynch_t* timer_manager_2 =
    COMMON_ASYNCHTIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_2);
#endif // ACE_WIN32 || ACE_WIN64

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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      timer_manager_2->stop (waitForCompletion_in,
                             true);
#endif // ACE_WIN32 || ACE_WIN64
      break;
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
