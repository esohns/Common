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

#ifndef COMMON_INPUT_COMMON_H
#define COMMON_INPUT_COMMON_H

#include "ace/Asynch_IO.h"
#include "ace/Malloc_Base.h"
#include "ace/Message_Queue.h"

#include "common_configuration.h"

#include "common_input_defines.h"
#include "common_input_handler_base.h"
#include "common_input_manager.h"

// froward declarations
#if defined (GUI_SUPPORT)
struct Common_UI_CBData;
#endif // GUI_SUPPORT

struct Common_Input_Configuration
{
  Common_Input_Configuration ()
   : allocatorConfiguration (NULL)
#if defined (GUI_SUPPORT)
   , CBData (NULL)
#endif // GUI_SUPPORT
   , eventDispatchConfiguration (NULL)
   , manager (NULL)
   , messageAllocator (NULL)
   , queue (NULL)
   , stream (NULL)
  {}

  struct Common_AllocatorConfiguration*     allocatorConfiguration;
#if defined (GUI_SUPPORT)
  struct Common_UI_CBData*                  CBData;
#endif // GUI_SUPPORT
  struct Common_EventDispatchConfiguration* eventDispatchConfiguration;
  Common_IRegister*                         manager;
  ACE_Allocator*                            messageAllocator;
  ACE_Message_Queue_Base*                   queue; // drop input data into this queue
  ACE_Asynch_Read_Stream*                   stream;
};

struct Common_Input_Manager_Configuration
{
  Common_Input_Manager_Configuration ()
#if defined (GUI_SUPPORT)
   : CBData (NULL)
   , eventDispatchConfiguration (NULL)
#else
   : eventDispatchConfiguration (NULL)
#endif // GUI_SUPPORT
   , eventDispatchState (NULL)
   , handlerConfiguration (NULL)
   , manageEventDispatch (false)
  {}

#if defined (GUI_SUPPORT)
  struct Common_UI_CBData*                  CBData;
#endif // GUI_SUPPORT
  struct Common_EventDispatchConfiguration* eventDispatchConfiguration;
  struct Common_EventDispatchState*         eventDispatchState;
  struct Common_Input_Configuration*        handlerConfiguration;
  bool                                      manageEventDispatch;
};

//////////////////////////////////////////

typedef Common_InputHandler_Base_T<struct Common_Input_Configuration> Common_InputHandler_t;

#endif
