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

#ifndef COMMON_INPUT_HANDLER_BASE_T_H
#define COMMON_INPUT_HANDLER_BASE_T_H

#include "ace/Asynch_IO.h"
#include "ace/Global_Macros.h"
#include "ace/Event_Handler.h"
#include "ace/Message_Block.h"
#include "ace/Singleton.h"

#include "common_iinitialize.h"
#include "common_isubscribe.h"

template <typename ConfigurationType> // implements Common_Input_Configuration
class Common_InputHandler_Base_T
 : public ACE_Event_Handler
 , public ACE_Handler
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IRegister
{
  typedef ACE_Event_Handler inherited;
  typedef ACE_Handler inherited2;

 public:
  // convenient types
  typedef ConfigurationType CONFIGURATION_T;

  Common_InputHandler_Base_T ();
  virtual ~Common_InputHandler_Base_T ();

  // *NOTE*: the default action is to drop the message block into the queue (if any)
  // *IMPORTANT NOTE*: fire-and-forget first argument
  virtual bool handle_input (ACE_Message_Block*); // message block containing input

  // implement Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&);

  // implement Common_IRegister
  virtual bool register_ ();
  // *IMPORTANT NOTE*: should cause delete 'this'
  virtual void deregister ();

 protected:
  // implement (part of) ACE_Event_Handler
  virtual int handle_input (ACE_HANDLE = ACE_INVALID_HANDLE); // handle
  inline virtual int handle_close (ACE_HANDLE, ACE_Reactor_Mask) { delete this;  return 0; }

  // implement (part of) ACE_Handler
  virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result&);

  // helper methods
  ACE_Message_Block* allocateMessage (unsigned int); // #bytes
  bool initiate_read_stream (); // start (next) asynch I/O

  ACE_Message_Block* buffer_;
  ConfigurationType* configuration_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_InputHandler_Base_T (const Common_InputHandler_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_InputHandler_Base_T& operator= (const Common_InputHandler_Base_T&))

  bool               registered_;
};

// include template definition
#include "common_input_handler_base.inl"

#endif
