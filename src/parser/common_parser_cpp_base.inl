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

#include "common_parser_defines.h"

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::Common_CppParserBase_T (bool traceScanning_in,
                                                              bool traceParsing_in)
 : fragment_ (NULL)
 , offset_ (0)
 , trace_ (traceParsing_in)
 , parser_ (dynamic_cast<ParserInterfaceType*> (this), // parser
            &scanner_)                                 // scanner
// , argument_ ()
 , scanner_ ()
 , scannerState_ ()
 , blockInParse_ (false)
 , isFirst_ (true)
 , buffer_ (NULL)
 , streamBuffer_ ()
 , stream_ (&streamBuffer_)
 , messageQueue_ (NULL)
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::Common_CppParserBase_T"));

  scanner_.setP (this);

  scanner_.set_debug (traceScanning_in ? 1 : 0);
  parser_.set_debug_level (traceParsing_in ? 1 : 0);
#if YYDEBUG
//  yydebug = (trace_ ? 1 : 0);
//  yysetdebug (trace_ ? 1 : 0);
#endif
}

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::~Common_CppParserBase_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::~Common_CppParserBase_T"));

  // finalize lex scanner
  if (buffer_)
    scanner_.yy_delete_buffer (buffer_);
}

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
bool
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::initialize"));

  if (isInitialized_)
  {
    configuration_ = NULL;
    fragment_ = NULL;
    offset_ = 0;
    trace_ = COMMON_PARSER_DEFAULT_YACC_TRACE;

    blockInParse_ = false;
    isFirst_ = true;

    if (buffer_)
    {
      scanner_.yy_delete_buffer (buffer_);
      buffer_ = NULL;
    } // end IF

    messageQueue_ = NULL;

    isInitialized_ = false;
  } // end IF

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);
  trace_ = configuration_->debugParser;

  blockInParse_ = configuration_->block;

  messageQueue_ = configuration_->messageQueue;

  scanner_.set_debug (configuration_->debugScanner ? 1 : 0);
#if defined (YYDEBUG)
  parser_.set_debug_level (configuration_->debugParser ? 1 : 0);
//  yydebug = (trace_ ? 1 : 0);
//  yysetdebug (trace_ ? 1 : 0);
#endif

  isInitialized_ = true;

  return true;
}

//template <typename ConfigurationType,
//          typename ScannerType,
//          typename ParserType,
//          typename ParserInterfaceType,
//          typename ArgumentType,
//          typename SessionMessageType>
//void
//Common_CppParserBase_T<ConfigurationType,
//                    ScannerType,
//                    ParserType,
//                    ParserInterfaceType,
//                    ArgumentType,
//                    SessionMessageType>::error (const yy::location& location_in,
//                                                const std::string& message_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::error"));
//
////  std::ostringstream converter;
////  converter << location_in;
//
//  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
////  ACE_DEBUG ((LM_ERROR,
////              ACE_TEXT ("(@%d.%d-%d.%d): %s\n"),
////              location_in.begin.line, location_in.begin.column,
////              location_in.end.line, location_in.end.column,
////              ACE_TEXT (message_in.c_str ())));
//  ACE_DEBUG ((LM_ERROR,
////              ACE_TEXT ("failed to parse \"%s\" (@%s): \"%s\"\n"),
//              ACE_TEXT ("failed to BitTorrent_Parser::parse(): \"%s\"\n"),
////              std::string (fragment_->rd_ptr (), fragment_->length ()).c_str (),
////              converter.str ().c_str (),
//              message_in.c_str ()));
//
//  // dump message
//  ACE_Message_Block* message_block_p = fragment_;
//  while (message_block_p->prev ()) message_block_p = message_block_p->prev ();
//  ACE_ASSERT (message_block_p);
//  Common_IDumpState* idump_state_p =
//    dynamic_cast<Common_IDumpState*> (message_block_p);
//  ACE_ASSERT (idump_state_p);
//  try {
//    idump_state_p->dump_state ();
//  } catch (...) {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
//  }
//
//  //std::clog << location_in << ": " << message_in << std::endl;
//}

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
bool
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::parse (ACE_Message_Block* data_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::parse"));

  // sanity check(s)
  ACE_ASSERT (isInitialized_);
  ACE_ASSERT (data_in);

  // retain a handle to the 'current' fragment
  fragment_ = data_in;
  offset_ = 0;

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

  // initialize scanner ?
  if (isFirst_)
  {
    isFirst_ = false;
    offset_ = 0;

//    bittorrent_set_column (1, state_);
//    bittorrent_set_lineno (1, state_);
  } // end IF

  // parse data fragment
  try {
    result = parser_.parse ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IParser_T::parse(), continuing\n")));
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
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to parse data (result was: %d), aborting\n"),
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
  fragment_ = NULL;

continue_:
  return (result == 0);
}

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
void
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::error (const std::string& message_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::error"));

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
              ACE_TEXT ("failed to Common_IParser_T::parse(): \"%s\"\n"),
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
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
bool
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::switchBuffer (bool unlink_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::switchBuffer"));

  // sanity check(s)
  ACE_ASSERT (fragment_);

  ACE_Message_Block* message_block_p = fragment_;
  if (!fragment_->cont ())
  {
    // sanity check(s)
    if (!blockInParse_)
      return false; // not enough data, cannot proceed

    waitBuffer (); // <-- wait for data
    if (!fragment_->cont ())
    {
      // *NOTE*: most probable reason: received session end
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("no data after Common_IScannerBase::waitBuffer(), aborting\n")));
      return false;
    } // end IF
  } // end IF
  fragment_ = fragment_->cont ();
  offset_ = 0;

  // unlink ?
  if (unlink_in)
    message_block_p->cont (NULL);

  // switch to the next fragment

  // clean state
  end ();

  // initialize next buffer

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (fragment_->capacity () - fragment_->length () >= COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
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
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
void
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::waitBuffer ()
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::waitBuffer"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  int error = 0;
  bool done = false;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  // sanity check(s)
  ACE_ASSERT (blockInParse_);
  if (!messageQueue_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("message queue not set - cannot wait, returning\n")));
      return;
  } // end IF

  // 1. wait for data
  do
  {
    result = messageQueue_->dequeue_head (message_block_p,
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
        result = messageQueue_->enqueue (message_block_p);
        if (result == -1)
        {
          error = ACE_OS::last_error ();
          if (error != ESHUTDOWN)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Message_Queue::enqueue(): \"%m\", returning\n")));
          message_block_p->release (); message_block_p = NULL;
          return;
        } // end IF
        message_block_p = NULL;
        done = true;
        break;
      }
      case ACE_Message_Block::MB_USER:
      {
        result = messageQueue_->enqueue_tail (message_block_p);
        if (result == -1)
        {
          error = ACE_OS::last_error ();
          if (error != ESHUTDOWN)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n")));
          message_block_p->release (); message_block_p = NULL;
          return;
        } // end IF
        message_block_p = NULL;
        break;
      }
      default:
        break;
    } // end SWITCH
    if (message_block_p)
      break;
  } while (!done);

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
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
bool
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::begin (const char* buffer_in,
                                             unsigned int bufferSize_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::begin"));

//  static int counter = 1;

  ACE_UNUSED_ARG (buffer_in);
  ACE_UNUSED_ARG (bufferSize_in);

  // sanity check(s)
  ACE_ASSERT (!buffer_);
  ACE_ASSERT (fragment_);

  // create/initialize a new buffer state
  streamBuffer_.set (fragment_->rd_ptr (),
                     fragment_->length () + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
//  stream_.rdbuf (&streamBuffer_);
  scanner_.switch_streams (&stream_, NULL);

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("parsing fragment #%d --> %d byte(s)\n"),
//              counter++,
//              fragment_->length ()));

  return true;
}

template <typename ConfigurationType,
          typename ScannerType,
          typename ScannerStateType,
          typename ParserType,
          typename ParserInterfaceType,
          typename ArgumentType>
void
Common_CppParserBase_T<ConfigurationType,
                       ScannerType,
                       ScannerStateType,
                       ParserType,
                       ParserInterfaceType,
                       ArgumentType>::end ()
{
  COMMON_TRACE (ACE_TEXT ("Common_CppParserBase_T::end"));

  // sanity check(s)
  if (!buffer_)
    return;

  // clean state
  scanner_.yy_delete_buffer (buffer_);
  buffer_ = NULL;
}
