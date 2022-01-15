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

#ifndef COMMON_TASK_TOOLS_H
#define COMMON_TASK_TOOLS_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
//#include "sched.h"
#include "sys/types.h"

typedef __pid_t pid_t;
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

class Common_Task_Tools
{
 public:
  static bool setThreadPriority (pid_t, // process/thread id (0: current process, current thread)
                                 int);  // priority (19: lowest to -20: highest [-INTMAX: highest possible: i.e. RLIMIT_NICE->rlim_max])

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Task_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Task_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Task_Tools (const Common_Task_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Task_Tools& operator= (const Common_Task_Tools&))
};

#endif
