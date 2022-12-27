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

#ifndef COMMON_ITASKCONTROL_H
#define COMMON_ITASKCONTROL_H

#include "ace/Global_Macros.h"

#include "common_idumpstate.h"
#include "common_isubscribe.h"
#include "common_itask.h"

// forward declarations
class ACE_Time_Value;
class ACE_Task_Base;

class Common_ITaskControl
{
 public:
  virtual void execute (ACE_Task_Base*,              // task handle
                        ACE_Time_Value* = NULL) = 0; // duration ? (relative !) timeout : run until finished

  ////////////////////////////////////////
  // callbacks
  // *NOTE*: signal asynchronous completion
  virtual void finished (ACE_Task_Base*) = 0;
};

//////////////////////////////////////////

class Common_ITaskManager
 : public Common_ITask
 , public Common_IRegister_T<ACE_Task_Base>
 , public Common_ITaskControl
 , public Common_IDumpState
{
 public:
  virtual void abort (bool = false) = 0; // wait for completion ? (see wait())

  virtual unsigned int count () const = 0; // return value: # of tasks
};

#endif
