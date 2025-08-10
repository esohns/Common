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

#include "test_i_timerhandler.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_input_tools.h"
#include "common_tools.h"

Test_I_TimerHandler::Test_I_TimerHandler ()
{
  COMMON_TRACE (ACE_TEXT ("Test_I_TimerHandler::Test_I_TimerHandler"));

}

void
Test_I_TimerHandler::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Test_I_TimerHandler::reset"));

  char char_c = static_cast<char> (Common_Tools::getRandomNumber (33, 126));

  Common_Input_Tools::input (char_c);
}
