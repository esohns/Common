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

#ifndef COMMON_ITASK_H
#define COMMON_ITASK_H

#include "ace/Global_Macros.h"

// *TODO*: find out why this doesn't work
//#include "common_itaskcontrol.h"

template <ACE_SYNCH_DECL>
class Common_ITaskControl_T
{
 public:
  virtual void start () = 0;
  virtual void stop (bool = true,      // wait for completion ?
                     bool = true) = 0; // locked access ?
  virtual bool isRunning () const = 0;

  // *NOTE*: signal asynchronous completion
  virtual void finished () = 0;
};

template <ACE_SYNCH_DECL>
class Common_ITask_T
 : virtual public Common_ITaskControl_T<ACE_SYNCH_USE>
{
 public:
  virtual int wait (void) = 0;
};

#endif
