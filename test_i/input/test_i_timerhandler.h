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

#ifndef TEST_I_TIMER_HANDLER_H
#define TEST_I_TIMER_HANDLER_H

#include "ace/Global_Macros.h"

#include "common_icounter.h"

class Test_I_TimerHandler
 : public Common_ICounter
{
 public:
  Test_I_TimerHandler ();
  inline virtual ~Test_I_TimerHandler () {}

  // implement Common_ICounter
  virtual void reset ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_I_TimerHandler (const Test_I_TimerHandler&))
  ACE_UNIMPLEMENTED_FUNC (Test_I_TimerHandler& operator= (const Test_I_TimerHandler&))
};

#endif
