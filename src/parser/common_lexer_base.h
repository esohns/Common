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

#ifndef COMMON_LEXER_BASE_T_H
#define COMMON_LEXER_BASE_T_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Message_Queue.h"

#include "common_iscanner.h"
#include "common_parser_common.h"

// forward declaration(s)
struct yy_buffer_state;

template <typename ConfigurationType, // implements struct Common_ParserConfiguration
          typename ParserInterfaceType, // implements Common_IParser_T, ExtraDataType
          typename ExtraDataType> // (f)lex-
class Common_LexerBase_T
 : public ParserInterfaceType
{
 public:
  // convenient types
  typedef ParserInterfaceType IPARSER_T;
  typedef typename ParserInterfaceType::ISCANNER_T ISCANNER_T;

  Common_LexerBase_T ();
  virtual ~Common_LexerBase_T ();

  // implement (part of) ParserInterfaceType
  virtual bool initialize (const ConfigurationType&);
  virtual bool parse (ACE_Message_Block*); // data buffer handle

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; }

 protected:
  // implement Common_IScannerBase
  inline virtual ACE_Message_Block* buffer () { return fragment_; }
#if defined (_DEBUG)
  inline virtual bool debug () const { ACE_ASSERT (configuration_); return configuration_->debugScanner; }
#endif // _DEBUG
  inline virtual bool isBlocking () const { ACE_ASSERT (configuration_); return configuration_->block; }
  inline virtual unsigned int offset () const { return scannerState_.offset; }
  virtual bool begin (const char*,   // buffer
                      unsigned int); // size
  virtual void end ();
  virtual bool switchBuffer (bool = false); // unlink current fragment ?
  virtual void waitBuffer ();
  inline virtual void offset (unsigned int offset_in) { scannerState_.offset += offset_in; } // offset (increment)
  virtual void error (const std::string&); // message

  // implement (part of) Common_ILexScanner_T
  inline virtual const struct Common_FlexScannerState& getR () const { return scannerState_; }
  inline virtual const ExtraDataType* const getP_2 () const { return this; }

  // convenient types
  typedef ACE_Message_Queue_Base MESSAGE_QUEUE_T;

  struct yy_buffer_state*        buffer_;
  ConfigurationType*             configuration_;
  bool                           finished_;
  ACE_Message_Block*             fragment_;

  // scanner
  struct Common_FlexScannerState scannerState_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_LexerBase_T (const Common_LexerBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_LexerBase_T& operator= (const Common_LexerBase_T&))

  bool                           isInitialized_;
};

// include template definition
#include "common_lexer_base.inl"

#endif
