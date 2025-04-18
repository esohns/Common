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

#include "common_task_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "sys/resource.h"
#if defined (ACE_LINUX)
#include "bits/sched.h"
//#include "linux/sched/types.h"
// struct sched_attr {
//   __u32 size;

//   __u32 sched_policy;
//   __u64 sched_flags;

//   /* SCHED_NORMAL, SCHED_BATCH */
//   __s32 sched_nice;

//   /* SCHED_FIFO, SCHED_RR */
//   __u32 sched_priority;

//   /* SCHED_DEADLINE */
//   __u64 sched_runtime;
//   __u64 sched_deadline;
//   __u64 sched_period;

//   /* Utilization hints */
//   __u32 sched_util_min;
//   __u32 sched_util_max;
// };
#include "sys/syscall.h"
#endif // ACE_LINUX
#endif // ACE_WIN32 || ACE_WIN64

#include <limits>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_defines.h"
#include "common_macros.h"

bool
Common_Task_Tools::setThreadPriority (pid_t threadId_in,
                                      int priority_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Task_Tools::setThreadPriority"));

  int priority_i = priority_in;
  int result = -1;
  if (priority_in == std::numeric_limits<int>::min ())
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_ASSERT (false); // *TODO*
#else
    struct rlimit rlimit_s;
    ACE_OS::memset (&rlimit_s, 0, sizeof (struct rlimit));
    result = ACE_OS::getrlimit (RLIMIT_NICE,
                                &rlimit_s);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NICE): \"%m\", aborting\n")));
      return false;
    } // end IF
    if (rlimit_s.rlim_cur < rlimit_s.rlim_max)
    {
      rlimit_s.rlim_cur = rlimit_s.rlim_max;
      result = ACE_OS::setrlimit (RLIMIT_NICE,
                                  &rlimit_s);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_NICE): \"%m\", aborting\n")));
        return false;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("raised RLIMIT_NICE soft limit to (%u): \"%m\"\n"),
                  rlimit_s.rlim_cur));
    } // end IF
    // *NOTE*: see also 'man 2 getrlimit'
    // *NOTE*: the man pages state that valid RLIMIT_NICE rlimit values range
    //         between 1-40 (low to high), which maps to priorities -20 to 19.
    //         But (on Fedora 35), the limits do not seem to be set (check with
    //         ulimit -e or /etc/security/[limits.conf|limits.d]), returning 0
    //         instead, so 20-rlim_max results in an (invalid) value of 20
    priority_i = (rlimit_s.rlim_max ? 20 - rlimit_s.rlim_max : 0);
#endif // ACE_WIN32 || ACE_WIN64
  } // end IF

  ACE_hthread_t thread_h = 0;
  if (!threadId_in)
    ACE_OS::thr_self (thread_h);
  else
  {
#if defined (ACE_LINUX)
//  thread_h = __find_thread_by_id (threadId_in); // *TODO*
//    pthread_getthreadid_np ()
#endif // ACE_LINUX
    ACE_ASSERT (false); // *TODO*
  } // end IF
  // sanity check(s)
  if (!thread_h)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to resolve pid_t (was: %u), aborting\n"),
               threadId_in));
    return false;
  } // end IF

#if defined (ACE_LINUX)
  struct sched_attr attributes_s;
  ACE_OS::memset (&attributes_s, 0, sizeof (struct sched_attr));
  result = ::syscall (SYS_sched_getattr,
                      threadId_in,
                      &attributes_s,
                      sizeof (struct sched_attr),
                      0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to syscall (SYS_sched_getattr): \"%m\", aborting\n")));
    return false;
  } // end IF
  if ((attributes_s.sched_policy == SCHED_OTHER) &&
      (attributes_s.sched_nice == priority_i))
    return true; // nothing to do
  ACE_OS::memset (&attributes_s, 0, sizeof (struct sched_attr));
  attributes_s.size = sizeof (struct sched_attr);
  attributes_s.sched_policy = SCHED_OTHER;
  attributes_s.sched_flags = SCHED_RESET_ON_FORK;
  attributes_s.sched_nice = priority_i;
  result = ::syscall (SYS_sched_setattr,
                      threadId_in,
                      &attributes_s,
                      0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to syscall (SYS_sched_setattr)(%d): \"%m\", aborting\n"),
               priority_i));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%u: set thread priority to %d\n"),
              threadId_in,
              priority_i));
#else
// *NOTE*: uses pthread_setschedparam()
  result = ACE_OS::thr_setprio (thread_h, priority_i, -1);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_OS::thr_setprio(%d): \"%m\", aborting\n"),
               priority_i));
    return false;
  } // end IF
#endif // ACE_LINUX

  return true;
}
