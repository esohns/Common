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

#include "common_parser_m3u_parser_driver.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "m3u_scanner.h"
#include "common_parser_m3u_tools.h"

Common_Parser_M3U_ParserDriver::Common_Parser_M3U_ParserDriver ()
 : inherited ()
 , popState_ (false)
 , dateTimeValue_ ()
 , lastKey_ ()
 , lastKeyWasURI_ (false)
 , playlist_ (NULL)
 , extInfElement_ (NULL)
 , mediaElement_ (NULL)
 , streamInfElement_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_ParserDriver::Common_Parser_M3U_ParserDriver"));

//  inherited::parser_.set (this);
}

bool
Common_Parser_M3U_ParserDriver::hasFinished () const
{
  if (inherited::finished_)
    return true;

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (inherited::fragment_);

  return (!inherited::configuration_->messageQueue &&
          inherited::scannerState_.offset >= inherited::fragment_->length ());
}

void
Common_Parser_M3U_ParserDriver::record (struct M3U_Playlist*& record_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_ParserDriver::record"));

  // sanity check(s)
  ACE_ASSERT (record_inout);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (Common_Parser_M3U_Tools::PlaylistToString (*record_inout).c_str ())));

  delete record_inout; record_inout = NULL;
  ACE_ASSERT (!record_inout);

  inherited::fragment_ = NULL;
  inherited::finished_ = true;
}

yy_buffer_state*
Common_Parser_M3U_ParserDriver::create (yyscan_t state_in,
                                        char* buffer_in,
                                        size_t size_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_ParserDriver::create"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (state_in);

  struct yy_buffer_state* result_p = NULL;

  if (inherited::configuration_->useYYScanBuffer)
    result_p =
      M3U__scan_buffer (buffer_in,
                        size_in + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                        state_in);
  else
    result_p = M3U__scan_bytes (buffer_in,
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
  //M3U__switch_to_buffer (result_p,
  //                       state_in);

  return result_p;
}
