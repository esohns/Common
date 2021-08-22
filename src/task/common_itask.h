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

#include "ace/Message_Block.h"

// forward declarations
class ACE_Time_Value;

class Common_ITask
{
 public:
  virtual bool isShuttingDown () = 0; // stop() has been called ?
  virtual void start (ACE_Time_Value* = NULL) = 0; // duration ? relative timeout : run until finished
  virtual void stop () = 0;
};

//////////////////////////////////////////

class Common_IAsynchTask
{
 public:
  virtual void idle () = 0;
  virtual bool isRunning () const = 0;
  virtual bool isShuttingDown () = 0; // stop() has been called ?

  virtual void start (ACE_Time_Value* = NULL) = 0; // duration ? relative timeout : run until finished
  virtual void stop (bool = true,      // wait for completion ?
                     bool = true,      // high priority ? (i.e. do not wait for queued messages)
                     bool = true) = 0; // locked access ?
  virtual void wait (bool = true) const = 0; // wait for the message queue ? : worker thread(s) only

  ////////////////////////////////////////
  // callbacks
  // *NOTE*: signal asynchronous completion
  virtual void finished () = 0;
};

template <typename MessageType = ACE_Message_Block>
class Common_ITaskHandler_T
{
 public:
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void handle (MessageType*&) = 0; // message handle
};

#endif
