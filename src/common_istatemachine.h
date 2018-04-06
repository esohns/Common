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

#ifndef COMMON_ISTATEMACHINE_T_H
#define COMMON_ISTATEMACHINE_T_H

#include <string>

#include "common_iinitialize.h"
//#include "common_itaskcontrol.h"

// forward declarations
class ACE_Time_Value;

class Common_IStateMachineBase
 : public Common_IInitialize
 , public Common_IReset
{};

//////////////////////////////////////////

template <typename StateType>
class Common_IStateMachine_T
 : public Common_IStateMachineBase
{
 public:
  virtual bool change (StateType) = 0; // new state
  virtual StateType current () const = 0;
  // *NOTE*: users need to provide absolute (!) values (i.e. deadline)
  virtual bool wait (StateType,
                     const ACE_Time_Value* = NULL) const = 0; // timeout (absolute) ? : block

  // *TODO*: this ought to be 'static'
  virtual std::string stateToString (StateType) const = 0; // return value: state

 protected:
  ////////////////////////////////////////
  virtual void onChange (StateType) = 0; // new state
};

template <typename StateType>
class Common_IStateMachine_2
 : public Common_IStateMachine_T<StateType>
{
  public:
  // *NOTE*: signal asynchronous completion
  virtual void finished () = 0;
};

//////////////////////////////////////////

//template <typename StateType,
//          ACE_SYNCH_DECL,
//          typename LockType> // implements Common_ILock_T/Common_IRecursiveLock_T>
//class Common_IStateMachineAsynch_T
// : public Common_IStateMachine_T<StateType>
// , virtual public Common_ITaskControl_T<ACE_SYNCH_USE,
//                                        LockType>
//{};

#endif
