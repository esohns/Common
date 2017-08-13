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

// forward declarations
class ACE_Time_Value;

template <typename StateType>
class Common_IStateMachine_T
{
 public:
  virtual ~Common_IStateMachine_T () {}

  virtual bool change (StateType) = 0; // new state
  virtual StateType current () const = 0;
  virtual void initialize () = 0;
  virtual void reset () = 0;
  // *NOTE*: users need to provide absolute (!) values (i.e. deadline)
  virtual bool wait (StateType,
                     const ACE_Time_Value* = NULL) = 0; // timeout (absolute) ? : block

  // *NOTE*: unfortunately, this cannot be static
  virtual std::string stateToString (StateType) const = 0; // return value: state

 protected:
  virtual void onChange (StateType) = 0; // new state
};

#endif
