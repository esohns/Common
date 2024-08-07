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

#ifndef COMMON_PARSER_BENCODING_PARSER_DRIVER_H
#define COMMON_PARSER_BENCODING_PARSER_DRIVER_H

#include <stack>
#include <string>

#include "ace/Global_Macros.h"

#include "location.hh"

#include "common_parser_base.h"
#include "common_parser_common.h"

#include "common_parser_bencoding_iparser.h"
#include "bencoding_parser.h"
#include "bencoding_scanner.h"

// forward declarations
typedef void* yyscan_t;
struct yy_buffer_state;

class Common_Parser_Bencoding_ParserDriver
 : public Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              Bencoding_IParser_t,
                              void>
{
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              Bencoding_IParser_t,
                              void> inherited;

 public:
  Common_Parser_Bencoding_ParserDriver ();
  inline virtual ~Common_Parser_Bencoding_ParserDriver () {}

  // convenient types
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::parser,
                              Bencoding_IParser_t,
                              void> PARSER_BASE_T;

  // implement (part of) Bencoding_IParser
  using PARSER_BASE_T::initialize;
  using PARSER_BASE_T::buffer;
//  using PARSER_BASE_T::debug;
  using PARSER_BASE_T::isBlocking;
  using PARSER_BASE_T::offset;
  using PARSER_BASE_T::parse;
  using PARSER_BASE_T::switchBuffer;
  using PARSER_BASE_T::waitBuffer;

  // implement (part of) Bencoding_IParser_t
  inline virtual void addLevel () { ++level_; }
  inline virtual void removeLevel () { --level_; }
  inline virtual unsigned int level () { return level_; }
  inline virtual bool toggleKeyValue () { bool is_key_b = isKey_; isKey_ = !isKey_; return is_key_b; }

  inline virtual Bencoding_Dictionary_t& current () { ACE_ASSERT (!dictionaries_.empty ()); return *dictionaries_.top (); }
  inline virtual Bencoding_List_t& current_2 () { ACE_ASSERT (!lists_.empty ()); return *lists_.top (); }
  inline virtual bool hasFinished () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }

  ////////////////////////////////////////
  // callbacks
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void record (Bencoding_Dictionary_t*&); // data record
  virtual void record_2 (Bencoding_List_t*&); // data record
  virtual void record_3 (std::string*&); // data record
  virtual void record_4 (ACE_INT64); // data record

  inline virtual std::string* popKey () { std::string* result_p = keys_.top (); keys_.pop (); return result_p; }
  inline virtual void popDictionary () { dictionaries_.pop (); }
  inline virtual void popList () { lists_.pop (); }
  inline virtual void pushDictionary (Bencoding_Dictionary_t* dictionary_in) { dictionaries_.push (dictionary_in); }
  inline virtual void pushKey (std::string* key_in) { keys_.push (key_in); }
  inline virtual void pushList (Bencoding_List_t* list_in) { lists_.push (list_in); }

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_Bencoding_ParserDriver (const Common_Parser_Bencoding_ParserDriver&))
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_Bencoding_ParserDriver& operator= (const Common_Parser_Bencoding_ParserDriver&))

  // implement Common_ILexScanner_T
  inline virtual const void* const getP_2 () const { return this; }
  //inline virtual void setP (Bencoding_IParser_t*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void setDebug (yyscan_t state_in, bool toggle_in) { Bencoding_set_debug ((toggle_in ? 1 : 0), state_in); }
  inline virtual void reset () { Bencoding_set_lineno (1, inherited::scannerState_.context); Bencoding_set_column (1, inherited::scannerState_.context); }
  inline virtual bool initialize (yyscan_t& state_inout, void* extra_in) { return (Bencoding_lex_init_extra (extra_in, &state_inout) == 0); }
  inline virtual void finalize (yyscan_t& state_inout) { int result = Bencoding_lex_destroy (state_inout); ACE_UNUSED_ARG (result); state_inout = NULL; }
  virtual struct yy_buffer_state* create (yyscan_t, // state handle
                                          char*,    // buffer handle
                                          size_t);  // buffer size
  inline virtual void destroy (yyscan_t state_in, struct yy_buffer_state*& buffer_inout) { Bencoding__delete_buffer (buffer_inout, state_in); buffer_inout = NULL; }
  inline virtual bool lex (yyscan_t state_in) { ACE_ASSERT (false); return Bencoding_lex (NULL, NULL, this, state_in); }

  unsigned int                        level_;
  bool                                isKey_;
  std::stack<Bencoding_Dictionary_t*> dictionaries_;
  std::stack<std::string*>            keys_;
  std::stack<Bencoding_List_t*>       lists_;
};

#endif
