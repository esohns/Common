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

#ifndef TEST_I_M3U_PARSER_DRIVER_H
#define TEST_I_M3U_PARSER_DRIVER_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/OS.h"

//#include "location.hh"

#include "common_parser_base.h"
#include "common_parser_common.h"

#include "common_parser_m3u_iparser.h"
#include "m3u_parser.h"
#include "m3u_scanner.h"

// forward declarations
typedef void* yyscan_t;
struct yy_buffer_state;

class Common_Parser_M3U_ParserDriver
 : public Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              M3U_IParser_t,
                              M3U_IParser_t>
{
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              M3U_IParser_t,
                              M3U_IParser_t> inherited;

 public:
  Common_Parser_M3U_ParserDriver ();
  inline virtual ~Common_Parser_M3U_ParserDriver () {}

  // convenient types
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              M3U_IParser_t,
                              M3U_IParser_t> PARSER_BASE_T;

  // implement (part of) Common_ILexScanner_T
  using PARSER_BASE_T::initialize;
  using PARSER_BASE_T::buffer;
//  using PARSER_BASE_T::debug;
  using PARSER_BASE_T::isBlocking;
  using PARSER_BASE_T::offset;
  using PARSER_BASE_T::parse;
  using PARSER_BASE_T::switchBuffer;
  using PARSER_BASE_T::waitBuffer;

  // implement (part of) M3U_IParser_t
  inline virtual void pop_next () { popState_ = true; }
  inline virtual bool pop_state () { bool pop_state_b = popState_; popState_ = false; return pop_state_b; }

  inline virtual void setDateTimeValue (const std::string& value_in) { dateTimeValue_ = value_in; }
  inline virtual std::string dateTimeValue () { return dateTimeValue_; }

  inline virtual void setKey (const std::string& key_in) { lastKey_ = key_in; lastKeyWasURI_ = ACE_OS::strcmp (lastKey_.c_str (), ACE_TEXT_ALWAYS_CHAR ("URI")) == 0; }
  inline virtual std::string lastKey () { return lastKey_; }
  inline virtual bool lastKeyWasURI () { return lastKeyWasURI_; }

  inline virtual struct M3U_Playlist& current () { ACE_ASSERT (playlist_); return *playlist_; }
  inline virtual struct M3U_ExtInf_Element& current_2 () { ACE_ASSERT (extInfElement_); return *extInfElement_; }
  inline virtual struct M3U_Media_Element& current_3 () { ACE_ASSERT (mediaElement_); return *mediaElement_; }
  inline virtual struct M3U_StreamInf_Element& current_4 () { ACE_ASSERT (streamInfElement_); return *streamInfElement_; }
  virtual bool hasFinished () const;

  ////////////////////////////////////////
  // callbacks
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void record (struct M3U_Playlist*&); // data record

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_M3U_ParserDriver (const Common_Parser_M3U_ParserDriver&))
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_M3U_ParserDriver& operator= (const Common_Parser_M3U_ParserDriver&))

  inline virtual void setP (struct M3U_Playlist* playlist_in) { /*ACE_ASSERT (!playlist_);*/ playlist_ = playlist_in; }
  inline virtual void setP_2 (struct M3U_ExtInf_Element* element_in) { /*ACE_ASSERT (!element_);*/ extInfElement_ = element_in; }
  inline virtual void setP_3 (struct M3U_Media_Element* element_in) { /*ACE_ASSERT (!element_);*/ mediaElement_ = element_in; }
  inline virtual void setP_4 (struct M3U_StreamInf_Element* element_in) { /*ACE_ASSERT (!element_);*/ streamInfElement_ = element_in; }

  // implement Common_ILexScanner_T
  inline virtual const M3U_IParser_t* const getP_2 () const { return this; }
  inline virtual void setDebug (yyscan_t state_in, bool toggle_in) { M3U_set_debug ((toggle_in ? 1 : 0), state_in); }
  inline virtual void reset () { M3U_set_lineno (1, inherited::scannerState_.context); M3U_set_column (1, inherited::scannerState_.context); }
  inline virtual bool initialize (yyscan_t& state_inout, M3U_IParser_t* extra_in) { return (M3U_lex_init_extra (extra_in, &state_inout) == 0); }
  inline virtual void finalize (yyscan_t& state_inout) { int result = M3U_lex_destroy (state_inout); ACE_UNUSED_ARG (result); state_inout = NULL; }
  virtual struct yy_buffer_state* create (yyscan_t, // state handle
                                          char*,    // buffer handle
                                          size_t);  // buffer size
  inline virtual void destroy (yyscan_t state_in, struct yy_buffer_state*& buffer_inout) { M3U__delete_buffer (buffer_inout, state_in); buffer_inout = NULL; }
  inline virtual bool lex (yyscan_t state_in) { ACE_ASSERT (false); return M3U_lex (NULL, NULL, this, state_in); }

  bool                          popState_;
  std::string                   dateTimeValue_;
  std::string                   lastKey_;
  bool                          lastKeyWasURI_;
  struct M3U_Playlist*          playlist_;
  struct M3U_ExtInf_Element*    extInfElement_;
  struct M3U_Media_Element*     mediaElement_;
  struct M3U_StreamInf_Element* streamInfElement_;
};

#endif
