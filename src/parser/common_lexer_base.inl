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

#include "common_macros.h"

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::Common_LexerBase_T ()
 : buffer_ (NULL)
 , configuration_ (NULL)
 , finished_ (false)
 , fragment_ (NULL)
 , scannerState_ ()
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::Common_LexerBase_T"));
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::~Common_LexerBase_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::~Common_LexerBase_T"));

//  if (headFragment_)
//    headFragment_->release ();

  // finalize lex scanner
  if (buffer_)
  { ACE_ASSERT (scannerState_.context);
    try {
      this->destroy (scannerState_.context,
                     buffer_);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ILexScanner_T::destroy(): \"%m\", continuing\n")));
    }
  } // end IF

  if (scannerState_.context)
  {
    try {
      this->finalize (scannerState_.context);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ILexScanner_T::finalize(): \"%m\", continuing\n")));
    }
  } // end IF
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
bool
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  if (isInitialized_)
  {
    finished_ = false;
    fragment_ = NULL;

    if (buffer_)
    { ACE_ASSERT (scannerState_.context);
      try {
        this->destroy (scannerState_.context,
                       buffer_);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_ILexScanner_T::destroy(): \"%m\", continuing\n")));
      }
      buffer_ = NULL;
    } // end IF
    if (scannerState_.context)
    {
      try {
        this->finalize (scannerState_.context);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_ILexScanner_T::finalize(): \"%m\", continuing\n")));
      }
      scannerState_.context = NULL;
    } // end IF
    scannerState_.offset = 0;

    //isFirst_ = true;

    isInitialized_ = false;
  } // end IF

  ACE_ASSERT (!scannerState_.context);
  bool result = false;
  ExtraDataType* extra_p = this;
  try {
    result = this->initialize (scannerState_.context,
                               extra_p);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ILexScanner_T::initialize(): \"%m\", continuing\n")));
  }
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_ILexScanner_T::initialize(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (scannerState_.context);

#if defined (_DEBUG)
  // trace ?
  try {
    this->setDebug (scannerState_.context,
                    configuration_->debugScanner);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ILexScanner_T::setDebug(): \"%m\", continuing\n")));
  }
#endif // _DEBUG

  isInitialized_ = true;

  return true;
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
void
//Common_LexerBase_T<SessionMessageType>::error (const YYLTYPE& location_in,
//                                                      const std::string& message_in)
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::error (const std::string& message_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::error"));

//  std::ostringstream converter;
//  converter << location_in;

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
//  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("(@%d.%d-%d.%d): %s\n"),
//              location_in.begin.line, location_in.begin.column,
//              location_in.end.line, location_in.end.column,
//              ACE_TEXT (message_in.c_str ())));
  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("failed to parse \"%s\" (@%s): \"%s\"\n"),
              ACE_TEXT ("failed to Common_IScannerBase::parse(): \"%s\"\n"),
//              std::string (fragment_->rd_ptr (), fragment_->length ()).c_str (),
//              converter.str ().c_str (),
              message_in.c_str ()));

  // dump message
  ACE_Message_Block* message_block_p = fragment_;
  while (message_block_p->prev ()) message_block_p = message_block_p->prev ();
  ACE_ASSERT (message_block_p);
  Common_IDumpState* idump_state_p =
    dynamic_cast<Common_IDumpState*> (message_block_p);
  ACE_ASSERT (idump_state_p);
  try {
    idump_state_p->dump_state ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
  }

  //std::clog << location_in << ": " << message_in << std::endl;
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
bool
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::parse (ACE_Message_Block* data_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::parse"));

  // sanity check(s)
  ACE_ASSERT (isInitialized_);
  ACE_ASSERT (data_in);
  ACE_ASSERT (scannerState_.context);

  fragment_ = data_in;
  scannerState_.offset = 0;

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT ((fragment_->capacity () - fragment_->length ()) >= COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(fragment_->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(fragment_->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  int result = -1;
  bool do_scan_end = false;
  if (!begin (NULL, 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IScannerBase::begin(), aborting\n")));
    goto error;
  } // end IF
  do_scan_end = true;

  try {
    this->reset (); // *WARNING*: this bombs out if there is no YY_CURRENT_BUFFER...
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ILexScanner_T::reset(): \"%m\", continuing\n")));
  }

  // parse data fragment
  try {
    result = this->lex (scannerState_.context);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in yylex(), continuing\n")));
    result = 1;
  }
  switch (result)
  {
    case 0:
      break; // done/need more data
    case 1:
    default:
    { // *NOTE*: most probable reason: connection
      //         has been closed --> session end
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to yylex (result was: %d), aborting\n"),
                  result));
      goto error;
    }
  } // end SWITCH

  // finalize buffer/scanner
  end ();
  do_scan_end = false;

  goto continue_;

error:
  if (do_scan_end)
    end ();
//  headFragment_ = NULL;
  fragment_ = NULL;

continue_:
  return (result == 0);
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
bool
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::switchBuffer (bool unlink_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::switchBuffer"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  ACE_Message_Block* message_block_p = fragment_;
  if (!fragment_->cont ())
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("parsed %Q byte(s), getting next fragment\n"),
    //            fragment_->length ()));

    // sanity check(s)
    if (!configuration_->block)
      return false; // not enough data, cannot proceed

    waitBuffer (); // <-- wait for data
    if (!fragment_->cont ())
    { // *NOTE*: most probable reason: received session end
      if (!finished_)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("no data after Common_IScannerBase::waitBuffer(), aborting\n")));
      return false;
    } // end IF
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("parsed %Q byte(s), using next fragment\n"),
  //            fragment_->length ()));
  fragment_ = fragment_->cont ();
  scannerState_.offset = 0;

  // unlink ?
  if (unlink_in)
    message_block_p->cont (NULL);

  // switch to the next fragment

  // clean state
  end ();

  // initialize next buffer
  ACE_ASSERT (fragment_);

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT ((fragment_->capacity () - fragment_->length ()) >= COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(fragment_->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(fragment_->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  if (!begin (NULL, 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IScannerBase::begin(), aborting\n")));
    return false;
  } // end IF

  return true;
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
void
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::waitBuffer ()
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::waitBuffer"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  int error = 0;
  bool done = false;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  // sanity check(s)
  ACE_ASSERT (configuration_);
  ACE_ASSERT (configuration_->block);
  if (unlikely (!configuration_->messageQueue))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("message queue not set - cannot wait, returning\n")));
    return;
  } // end IF
  if (configuration_->messageQueue->is_empty () &&
      finished_)
    return; // already finished

  // 1. wait for data
retry:
  result = configuration_->messageQueue->dequeue_head (message_block_p,
                                                       NULL);
  if (result == -1)
  {
    error = ACE_OS::last_error ();
    if (error != ESHUTDOWN)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Queue::dequeue_head(): \"%m\", returning\n")));
    return;
  } // end IF
  ACE_ASSERT (message_block_p);
  switch (message_block_p->msg_type ())
  {
    case ACE_Message_Block::MB_STOP:
    {
      // keep retrying while there are other (potentially data-) messages
      if (configuration_->messageQueue->is_empty ())
        done = true;
      result = configuration_->messageQueue->enqueue_tail (message_block_p);
      if (result == -1)
      {
        error = ACE_OS::last_error ();
        if (error != ESHUTDOWN)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n")));
        message_block_p->release (); message_block_p = NULL;
        return;
      } // end IF
      if (!done)
        goto retry;
      break;
    }
    case ACE_Message_Block::MB_EVENT:
    {
      result = configuration_->messageQueue->enqueue_tail (message_block_p);
      if (result == -1)
      {
        error = ACE_OS::last_error ();
        if (error != ESHUTDOWN)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n")));
        message_block_p->release (); message_block_p = NULL;
        return;
      } // end IF
      goto retry;
    }
    default:
      break;
  } // end SWITCH

  // 2. append data ?
  if (!done)
  { ACE_ASSERT (fragment_);
    ACE_ASSERT (message_block_p);
    ACE_Message_Block* message_block_2 = fragment_;
    for (;
         message_block_2->cont ();
         message_block_2 = message_block_2->cont ());
    message_block_2->cont (message_block_p);
  } // end IF
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
bool
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::begin (const char* buffer_in,
                                           unsigned int bufferSize_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::begin"));

//  static int counter = 1;

  ACE_UNUSED_ARG (buffer_in);
  ACE_UNUSED_ARG (bufferSize_in);

  // sanity check(s)
  ACE_ASSERT (!buffer_);
  ACE_ASSERT (fragment_);
  ACE_ASSERT (scannerState_.context);

  // create/initialize a new buffer state
  try {
    buffer_ = this->create (scannerState_.context,
                            fragment_->rd_ptr (),
                            fragment_->length ());
  }
  catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ILexScanner_T::create(): \"%m\", continuing\n")));
  }
  if (!buffer_)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_ILexScanner_T::create(): \"%m\", aborting\n")));
//  else
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("parsing fragment #%d --> %d byte(s)\n"),
//                counter++,
//                fragment_->length ()));

  return (buffer_ != NULL);
}

template <typename ConfigurationType,
          typename ParserInterfaceType,
          typename ExtraDataType>
void
Common_LexerBase_T<ConfigurationType,
                    ParserInterfaceType,
                    ExtraDataType>::end ()
{
  COMMON_TRACE (ACE_TEXT ("Common_LexerBase_T::end"));

  // sanity check(s)
  ACE_ASSERT (scannerState_.context);
  if (!buffer_)
    return;

  // clean buffer
  try {
    this->destroy (scannerState_.context,
                   buffer_);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ILexScanner_T::destroy(): \"%m\", continuing\n")));
  }
  buffer_ = NULL;
}
