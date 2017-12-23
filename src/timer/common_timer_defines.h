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

#ifndef COMMON_TIMER_DEFINES_H
#define COMMON_TIMER_DEFINES_H

#define COMMON_TIMER_DEFAULT_DISPATCH           COMMON_TIMER_DISPATCH_QUEUE
#define COMMON_TIMER_DEFAULT_QUEUE              COMMON_TIMER_QUEUE_HEAP

// *IMPORTANT NOTE*: make sure group ids are consistent across the entire (!)
//                   application
#define COMMON_TIMER_THREAD_GROUP_ID            100
#define COMMON_TIMER_THREAD_NAME                "timer dispatch"
// *IMPORTANT NOTE*: currently used for (initial !) slot pre-allocation only;
//                   ultimately, the total number of available concurrent slots
//                   depends on the actual implementation
//                   --> check the code, don't rely on ACE_DEFAULT_TIMERS
//#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS    ACE_DEFAULT_TIMERS
#define COMMON_TIMER_DEFAULT_NUM_TIMER_SLOTS    32768
#define COMMON_TIMER_PREALLOCATE_TIMER_SLOTS    true

#endif
