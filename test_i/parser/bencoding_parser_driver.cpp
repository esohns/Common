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

#include "bencoding_parser_driver.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_parser_bencoding_tools.h"

#include "bencoding_scanner.h"

Bencoding_ParserDriver::Bencoding_ParserDriver ()
 : inherited ()
 , dictionaries_ ()
 , key_ (NULL)
 , lists_ ()
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver::Bencoding_ParserDriver_T"));

//  inherited::parser_.set (this);
}

void
Bencoding_ParserDriver::error (const yy::location& location_in,
                               const std::string& message_in)
//Bencoding_ParserDriver_T<MessageType,
//                          SessionMessageType>::error (const std::string& message_in)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver::error"));

  std::ostringstream converter;
  converter << location_in;

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("(@%d.%d-%d.%d): \"%s\"\n"),
              location_in.begin.line, location_in.begin.column,
              location_in.end.line, location_in.end.column,
              ACE_TEXT (message_in.c_str ())));
//  ACE_DEBUG ((LM_ERROR,
////              ACE_TEXT ("failed to parse \"%s\" (@%s): \"%s\"\n"),
//              ACE_TEXT ("failed to BitTorrent_Parser::parse(): \"%s\"\n"),
////              std::string (fragment_->rd_ptr (), fragment_->length ()).c_str (),
////              converter.str ().c_str (),
//              message_in.c_str ()));

  // dump message
  ACE_Message_Block* message_block_p = inherited::fragment_;
  while (message_block_p->prev ()) message_block_p = message_block_p->prev ();
  ACE_ASSERT (message_block_p);
  Common_IDumpState* idump_state_p =
    dynamic_cast<Common_IDumpState*> (message_block_p);
  if (idump_state_p)
    try {
      idump_state_p->dump_state ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
    }

  //std::clog << location_in << ": " << message_in << std::endl;
}

//void
//Bencoding_ParserDriver::error (const std::string& message_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver::error"));

//  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
//  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("\": \"%s\"...\n"),
//              ACE_TEXT (message_in.c_str ())));
////   ACE_DEBUG((LM_ERROR,
////              ACE_TEXT("failed to parse \"%s\": \"%s\"...\n"),
////              std::string(fragment_->rd_ptr(), fragment_->length()).c_str(),
////              message_in.c_str()));

////   std::clog << message_in << std::endl;
//}

void
Bencoding_ParserDriver::record (Bencoding_Dictionary_t*& record_inout)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver_T::record"));

  // sanity check(s)
  ACE_ASSERT (record_inout);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%s\n"),
             ACE_TEXT (Common_Parser_Bencoding_Tools::DictionaryToString (*record_inout).c_str ())));

  Common_Parser_Bencoding_Tools::free (record_inout);
  ACE_ASSERT (!record_inout);
}

void
Bencoding_ParserDriver::record_2 (Bencoding_List_t*& record_inout)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver_T::record_2"));

  // sanity check(s)
  ACE_ASSERT (record_inout);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%s\n"),
             ACE_TEXT (Common_Parser_Bencoding_Tools::ListToString (*record_inout).c_str ())));

  Common_Parser_Bencoding_Tools::free (record_inout);
  ACE_ASSERT (!record_inout);
}

void
Bencoding_ParserDriver::record_3 (std::string*& record_inout)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver_T::record_3"));

  // sanity check(s)
  ACE_ASSERT (record_inout);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("\"%s\"\n"),
             ACE_TEXT (record_inout->c_str ())));

  delete record_inout; record_inout = NULL;
}

void
Bencoding_ParserDriver::record_4 (unsigned int record_inout)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver_T::record_4"));

  // sanity check(s)
  ACE_ASSERT (record_inout);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%d\n"),
             record_inout));
}

yy_buffer_state*
Bencoding_ParserDriver::create (yyscan_t state_in,
                                char* buffer_in,
                                size_t size_in)
{
  COMMON_TRACE (ACE_TEXT ("Bencoding_ParserDriver_T::create"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (state_in);

  struct yy_buffer_state* result_p = NULL;

  if (inherited::configuration_->useYYScanBuffer)
    result_p =
      Bencoding__scan_buffer (buffer_in,
                              size_in + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                              state_in);
  else
    result_p = Bencoding__scan_bytes (buffer_in,
                                      static_cast<int> (size_in) + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                                      state_in);
  if (!result_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yy_scan_buffer/bytes(0x%@, %d), aborting\n"),
                buffer_in,
                size_in));
    return NULL;
  } // end IF

  //// *WARNING*: contrary (!) to the documentation, still need to switch_buffers()...
  //Bencoding__switch_to_buffer (result_p,
  //                             state_in);

  return result_p;
}
