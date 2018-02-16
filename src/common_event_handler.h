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

#ifndef COMMON_EVENTHANDLER_H
#define COMMON_EVENTHANDLER_H

#include "ace/Asynch_IO.h"
#include "ace/Event_Handler.h"
#include "ace/Global_Macros.h"

class Common_EventHandlerBase
 : virtual public ACE_Event_Handler
 , virtual public ACE_Handler
{
  typedef ACE_Event_Handler inherited;
  typedef ACE_Handler inherited2;

 public:
  Common_EventHandlerBase ();
  inline virtual ~Common_EventHandlerBase () {}

 private:
//  ACE_UNIMPLEMENTED_FUNC (Common_EventHandlerBase ())
  ACE_UNIMPLEMENTED_FUNC (Common_EventHandlerBase (const Common_EventHandlerBase&))
  ACE_UNIMPLEMENTED_FUNC (Common_EventHandlerBase& operator= (const Common_EventHandlerBase&))
};

#endif
