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

#ifndef TEST_I_INPUTHANDLER_H
#define TEST_I_INPUTHANDLER_H

#include "ace/Global_Macros.h"

#include "common_input_common.h"
#include "common_input_handler_base.h"

template <
#if defined (GUI_SUPPORT)
          typename UIStateType
#if defined (WXWIDGETS_USE)
          ,typename InterfaceType // implements Common_UI_wxWidgets_IApplicationBase_T
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
         >
class Test_I_InputHandler_T
 : public Common_InputHandler_Base_T<struct Common_Input_Configuration>
{
  typedef Common_InputHandler_Base_T<struct Common_Input_Configuration> inherited;

 public:
  Test_I_InputHandler_T ();
  inline virtual ~Test_I_InputHandler_T () {}

  // override Common_InputHandler_Base_T
  virtual bool handle_input (ACE_Message_Block*); // message block containing input

 private:
//  ACE_UNIMPLEMENTED_FUNC (Test_I_InputHandler_T ())
  ACE_UNIMPLEMENTED_FUNC (Test_I_InputHandler_T (const Test_I_InputHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Test_I_InputHandler_T& operator= (const Test_I_InputHandler_T&))

#if defined (GUI_SUPPORT)
#if defined (WXWIDGETS_USE)
  InterfaceType* interface_;
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
};

// include template definition
#include "test_i_inputhandler.inl"

#endif
