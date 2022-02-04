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

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"
#include "ace/Thread_Manager.h"

#include "common_defines.h"
#include "common_macros.h"

template <typename ConfigurationType>
Common_InputHandler_Base_T<ConfigurationType>::Common_InputHandler_Base_T ()
 : inherited (ACE_Reactor::instance (),       // reactor
              ACE_Event_Handler::LO_PRIORITY) // priority
 , inherited2 (ACE_Proactor::instance ()) // proactor
 , buffer_ (NULL)
 , configuration_ (NULL)
 , registered_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::Common_InputHandler_Base_T"));

}

template <typename ConfigurationType>
Common_InputHandler_Base_T<ConfigurationType>::~Common_InputHandler_Base_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::~Common_InputHandler_Base_T"));

  if (configuration_ &&
      configuration_->manager)
    configuration_->manager->deregister ();

  if (unlikely (registered_))
    deregister ();

  if (unlikely (buffer_))
    buffer_->release ();
}

template <typename ConfigurationType>
int
Common_InputHandler_Base_T<ConfigurationType>::handle_input (ACE_HANDLE handle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::handle_input"));

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (configuration_);
  ACE_ASSERT (configuration_->allocatorConfiguration);
  ACE_ASSERT (configuration_->queue);

  if (likely (!buffer_))
  { // allocate a message buffer
    buffer_ = allocateMessage (configuration_->allocatorConfiguration->defaultBufferSize);
    if (unlikely (!buffer_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_InputHandler_Base_T::allocateMessage(%u), aborting\n"),
                  configuration_->allocatorConfiguration->defaultBufferSize));
      return -1; // *NOTE*: will deregister/delete this
    } // end IF
  } // end IF
  ACE_ASSERT (buffer_);

  // read some data from STDIN
  ssize_t bytes_received = ACE_OS::read (handle_in,
                                         buffer_->wr_ptr (),
                                         buffer_->space () - 1); // \0
  switch (bytes_received)
  {
    case -1:
    {
      // *IMPORTANT NOTE*: a number of scenarios lead here:
      // - STDIN closed (application shutdown)
      int error_i = ACE_OS::last_error ();
      ACE_UNUSED_ARG (error_i);
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::read(%d): \"%m\", aborting\n"),
                  ACE_STDIN));
      // *WARNING*: falls through
    }
    // *** GOOD CASES ***
    case 0:
    {
      buffer_->release (); buffer_ = NULL;
      return -1; // *NOTE*: will deregister/delete this
    }
    default:
    {
      // adjust write pointer
      buffer_->wr_ptr (bytes_received);
      break;
    }
  } // end SWITCH
  *buffer_->wr_ptr () = 0; // 0-terminate string

  result = configuration_->queue->enqueue (buffer_, NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue_Base::enqueue(): \"%m\", aborting\n")));
    buffer_->release (); buffer_ = NULL;
    return -1;
  } // end IF

  return registered_ ? 0 : -1; // handle WIN32
}

template <typename ConfigurationType>
void
Common_InputHandler_Base_T<ConfigurationType>::handle_read_stream (const ACE_Asynch_Read_Stream::Result& result_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::handle_read_stream"));

  int result = -1;
  unsigned long error_i = 0;

  // sanity check
  ACE_ASSERT (configuration_);
  ACE_ASSERT (configuration_->queue);
  result = result_in.success ();
  if (unlikely (result == 0))
  {
    error_i = result_in.error ();
    goto error;
  } // end IF
  switch (static_cast<int> (result_in.bytes_transferred ()))
  {
    case -1:
    {
      error_i = result_in.error ();
      goto error;
    }
    // *** GOOD CASES ***
    case 0:
    {
      result_in.message_block ().release ();
      goto deregister;
    }
    default:
    {
      if (unlikely (!initiate_read_stream ()))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_InputHandler_Base_T::initiate_read_stream(), continuing\n")));
      break;
    }
  } // end SWITCH

  result = configuration_->queue->enqueue (&result_in.message_block (), NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_Message_Queue_Base::enqueue(): \"%m\", returning\n")));
    result_in.message_block ().release ();
    return;
  } // end IF

  return;

error:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("0x%@: failed to read from input stream: \"%s\", returning\n"),
              result_in.handle (),
              ACE_OS::strerror (static_cast<int> (error_i))));
#else
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("%d: failed to read from input stream: \"%s\", returning\n"),
              result_in.handle (),
              ACE_OS::strerror (static_cast<int> (error_i))));
#endif // ACE_WIN32 || ACE_WIN64

  result_in.message_block ().release ();

deregister:
  deregister ();
  if (configuration_ &&
      configuration_->manager)
    configuration_->manager->deregister ();
  delete this;
}

template <typename ConfigurationType>
bool
Common_InputHandler_Base_T<ConfigurationType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::initialize"));

  int result = -1;

  if (unlikely (registered_))
    deregister ();

  if (unlikely (buffer_))
  {
    buffer_->release (); buffer_ = NULL;
  } // end IF

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  return true;
}

template <typename ConfigurationType>
bool
Common_InputHandler_Base_T<ConfigurationType>::register_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::register_"));

  // sanity check(s)
  if (unlikely (!configuration_))
    return false;
  if (unlikely (registered_))
    return true;

  // sanity check(s)
  ACE_ASSERT (configuration_->eventDispatchConfiguration);

  int result = -1;

  switch (configuration_->eventDispatchConfiguration->dispatch)
  {
    case COMMON_EVENT_DISPATCH_REACTOR:
    {
      result =
          ACE_Event_Handler::register_stdin_handler (this,
                                                     ACE_Reactor::instance (),
                                                     ACE_Thread_Manager::instance (),
                                                     THR_DETACHED);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to ACE_Event_Handler::register_stdin_handler(): \"%m\", aborting\n")));
        return false;
      } // end IF
      break;
    }
    default: // *TODO*
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid/unknown dispatch type (was: %d), aborting\n"),
                 configuration_->eventDispatchConfiguration->dispatch));
      return false;
    }
  } // end SWITCH
  registered_ = true;
  ACE_DEBUG ((LM_DEBUG,
             ACE_TEXT ("registered input handler...\n")));

  return true;
}

template <typename ConfigurationType>
void
Common_InputHandler_Base_T<ConfigurationType>::deregister ()
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::deregister"));

  // sanity check(s)
  if (unlikely (!registered_))
    return; // nothing to do

  // sanity check(s)
  ACE_ASSERT (configuration_);
  ACE_ASSERT (configuration_->eventDispatchConfiguration);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  int result = -1;
#endif // ACE_WIN32 || ACE_WIN64

  registered_ = false;

  switch (configuration_->eventDispatchConfiguration->dispatch)
  {
    case COMMON_EVENT_DISPATCH_REACTOR:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      result =
          ACE_Event_Handler::remove_stdin_handler (ACE_Reactor::instance (),
                                                   ACE_Thread_Manager::instance ());
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                   ACE_TEXT ("failed to ACE_Event_Handler::remove_stdin_handler(): \"%m\", returning\n")));
        return;
      } // end IF
#endif // ACE_WIN32 || ACE_WIN64
      break;
    }
    default: // *TODO*
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid/unknown dispatch type (was: %d), returning\n"),
                 configuration_->eventDispatchConfiguration->dispatch));
      return;
    }
  } // end SWITCH
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("deregistered input handler...\n")));
}

template <typename ConfigurationType>
ACE_Message_Block*
Common_InputHandler_Base_T<ConfigurationType>::allocateMessage (unsigned int requestedSize_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::allocateMessage"));

  // initialize return value(s)
  ACE_Message_Block* message_block_p = NULL;

  // sanity check(s)
  ACE_ASSERT (configuration_);

  if (likely (configuration_->messageAllocator))
  {
//retry:
    try {
      message_block_p =
          static_cast<ACE_Message_Block*> (configuration_->messageAllocator->malloc (requestedSize_in));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ACE_Allocator::malloc(%u), aborting\n"),
                  requestedSize_in));
      return NULL;
    }
//    // keep retrying ?
//    if (unlikely (!message_block_p &&
//                  !configuration_->messageAllocator->block ()))
//      goto retry;
  } // end IF
  else
    ACE_NEW_NORETURN (message_block_p,
                      ACE_Message_Block (requestedSize_in,
                                         ACE_Message_Block::MB_DATA,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                         ACE_Time_Value::zero,
                                         ACE_Time_Value::max_time,
                                         NULL,
                                         NULL));
  if (unlikely (!message_block_p))
  {
//    if (configuration_->messageAllocator)
//    {
//      if (configuration_->messageAllocator->block ())
//        ACE_DEBUG ((LM_CRITICAL,
//                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
//    } // end IF
//    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  } // end IF

  return message_block_p;
}

template <typename ConfigurationType>
bool
Common_InputHandler_Base_T<ConfigurationType>::initiate_read_stream ()
{
  COMMON_TRACE (ACE_TEXT ("Common_InputHandler_Base_T::initiate_read_stream"));

  int result = -1;
  int error_i = 0;

  // sanity check(s)
  ACE_ASSERT (configuration_);
  ACE_ASSERT (configuration_->allocatorConfiguration);
  ACE_ASSERT (configuration_->stream);

  ACE_Message_Block* message_block_p =
      allocateMessage (configuration_->allocatorConfiguration->defaultBufferSize);
  if (unlikely (!message_block_p))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_InputHandler_Base_T::allocateMessage(%u), aborting\n"),
               configuration_->allocatorConfiguration->defaultBufferSize));
    return false;
  } // end IF

  // start (asynchronous) read...
retry:
  result =
    configuration_->stream->read (*message_block_p,                                          // buffer
                                  configuration_->allocatorConfiguration->defaultBufferSize, // bytes to read
                                  NULL,                                                      // ACT
                                  0,                                                         // priority
                                  COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);                      // signal
  if (unlikely (result == -1))
  {
    error_i = ACE_OS::last_error ();
    // *WARNING*: this could fail on multi-threaded proactors
    if (unlikely (error_i == EAGAIN))
      goto retry; // 11: happens on Linux
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Stream::read(): \"%m\", aborting\n")));
    message_block_p->release ();
    return false;
  } // end IF

  return true;
}
