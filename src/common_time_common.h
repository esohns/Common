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

#ifndef COMMON_TIME_COMMON_H
#define COMMON_TIME_COMMON_H

#include "ace/Version.h"

#include "common_defines.h"

#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 1) || \
     (ACE_BETA_VERSION  > 6))
#include "ace/Time_Policy.h"
// *NOTE*: use the high resolution time for accuracy and low latency
typedef ACE_HR_Time_Policy Common_TimePolicy_t;
#else
#error "ACE version > 6.1.6 required"
typedef ACE_System_Time_Policy Common_TimePolicy_t;
#endif

#if (ACE_MAJOR_VERSION >= 6) && \
    ((ACE_MINOR_VERSION > 1) || \
     (ACE_BETA_VERSION  > 6))
// *NOTE*: global time policy (supplies gettimeofday())
#if defined __GNUC__
//#pragma GCC diagnostic ignored "-Wunused-variable"
COMMON_GCC_UNUSED_GUARD static Common_TimePolicy_t COMMON_TIME_POLICY;
//#pragma GCC diagnostic pop
#else
static Common_TimePolicy_t COMMON_TIME_POLICY;
#endif
#define COMMON_TIME_NOW COMMON_TIME_POLICY ()
#else
#define COMMON_TIME_NOW ACE_OS::gettimeofday ()
#endif

#endif
