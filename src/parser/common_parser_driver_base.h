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

#ifndef COMMON_PARSER_DRIVER_BASE_H
#define COMMON_PARSER_DRIVER_BASE_H

#include "ace/Global_Macros.h"

#include "common_parser_base.h"
#include "common_parser_common.h"

// forward declarations
namespace yy
{
  class parser;
};

template <typename ConfigurationType,
          typename ParserInterfaceType, // implements Common_IParser_T
          typename ExtraDataType>       // (f)lex->
class Common_Parser_Driver_Base_T
 : public Common_ParserBase_T<ConfigurationType,
                              yy::parser,
                              ParserInterfaceType,
                              ExtraDataType>
{
  typedef Common_ParserBase_T<ConfigurationType,
                              yy::parser,
                              ParserInterfaceType,
                              ExtraDataType> inherited;

 public:
  Common_Parser_Driver_Base_T ();
  inline virtual ~Common_Parser_Driver_Base_T () {}

  // convenient types
  typedef Common_ParserBase_T<ConfigurationType,
                              yy::parser,
                              ParserInterfaceType,
                              ExtraDataType> PARSER_BASE_T;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_Driver_Base_T (const Common_Parser_Driver_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Parser_Driver_Base_T& operator= (const Common_Parser_Driver_Base_T&))
};

// include template definition
#include "common_parser_driver_base.inl"

#endif
