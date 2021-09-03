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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_ui_qt_common.h"

template <typename StateType,
          typename CBDataType,
          typename TopLevelClassType>
Comon_UI_Qt_Application_T<StateType,
                          CBDataType,
                          TopLevelClassType>::Comon_UI_Qt_Application_T (int argc_in,
                                                                         char* argv_in[])
 : inherited (argc_in, argv_in)
 , CBData_ (NULL)
 , state_ ()
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_Qt_Application_T::Comon_UI_Qt_Application_T"));

}

template <typename StateType,
          typename CBDataType,
          typename TopLevelClassType>
void
Comon_UI_Qt_Application_T<StateType,
                          CBDataType,
                          TopLevelClassType>::dump_state () const
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_Qt_Application_T::dump_state"));

}

template <typename StateType,
          typename CBDataType,
          typename TopLevelClassType>
bool
Comon_UI_Qt_Application_T<StateType,
                          CBDataType,
                          TopLevelClassType>::run ()
{
  COMMON_TRACE (ACE_TEXT ("Comon_UI_Qt_Application_T::run"));

  int result = inherited::exec ();

  return (!result);
}
