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

#include <iostream>

#if defined (GTK_SUPPORT)
#include "gtk/gtk.h"
#endif // GTK_SUPPORT

#include "ace/Guard_T.h"
#include "ace/Synch_Traits.h"

#include "common_macros.h"
#include "common_ui_common.h"

template <
          typename UIStateType
#if defined (WXWIDGETS_USE)
          ,typename InterfaceType
#endif // WXWIDGETS_USE
         >
Test_I_InputHandler_T<
                      UIStateType
#if defined (WXWIDGETS_USE)
                      ,InterfaceType
#endif // WXWIDGETS_USE
                     >::Test_I_InputHandler_T ()
 : inherited ()
#if defined (WXWIDGETS_USE)
 , interface_ (interface_in)
#endif // WXWIDGETS_USE
{
  COMMON_TRACE (ACE_TEXT ("Test_I_InputHandler_T::Test_I_InputHandler_T"));

}

template <
          typename UIStateType
#if defined (WXWIDGETS_USE)
          ,typename InterfaceType
#endif // WXWIDGETS_USE
         >
bool
Test_I_InputHandler_T<
                      UIStateType
#if defined (WXWIDGETS_USE)
                      ,InterfaceType
#endif // WXWIDGETS_USE
                     >::handle_input (ACE_Message_Block* messageBlock_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_InputHandler_T::handle_input"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (messageBlock_in);

  if (inherited::configuration_->lineMode)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("read input: \"%s\"\n"),
                ACE_TEXT (messageBlock_in->rd_ptr ())));
  else
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    struct _KEY_EVENT_RECORD* key_event_record_p =
      reinterpret_cast<struct _KEY_EVENT_RECORD*> (messageBlock_in->rd_ptr ());
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("read input: \"%c\"\n"),
                key_event_record_p->uChar.AsciiChar));
#else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("read input: \"%c\"\n"),
                *messageBlock_in->rd_ptr ()));
#endif // ACE_WIN32 || ACE_WIN64
  } // end ELSE
  messageBlock_in->release ();

  return true;
}
