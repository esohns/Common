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

#ifndef COMMON_PARSER_COMMON_H
#define COMMON_PARSER_COMMON_H

#include "common_configuration.h"

#include "common_parser_defines.h"

// forward declaration(s)
typedef void* yyscan_t;
class ACE_Message_Queue_Base;

//////////////////////////////////////////

struct Common_ScannerState
{
  Common_ScannerState ()
   : offset (0)
  {}

  unsigned int offset; // parsed (fragment) byte(s)
};

struct Common_FlexScannerState
 : Common_ScannerState
{
  Common_FlexScannerState ()
   : Common_ScannerState ()
   , context (NULL)
  {}

  yyscan_t context;
};

//////////////////////////////////////////

struct Common_Parser_FlexAllocatorConfiguration
 : Common_AllocatorConfiguration
{
  Common_Parser_FlexAllocatorConfiguration ()
   : Common_AllocatorConfiguration ()
  {
    // *NOTE*: this facilitates (message block) data buffers to be scanned with
    //         'flex's yy_scan_buffer() method
    paddingBytes = COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE;
  }
};

//////////////////////////////////////////

struct Common_ParserConfiguration
{
  Common_ParserConfiguration ()
   : block (true)
   , messageQueue (NULL)
#if defined (_DEBUG)
   , debugParser (COMMON_PARSER_DEFAULT_YACC_TRACE)
   , debugScanner (COMMON_PARSER_DEFAULT_LEX_TRACE)
#endif // _DEBUG
  {}

  bool                    block; // block in parse (i.e. wait for data in yywrap() ?)
  ACE_Message_Queue_Base* messageQueue; // queue (if any) to use for yywrap

#if defined (_DEBUG)
  bool                    debugParser;
  bool                    debugScanner;
#endif // _DEBUG
};

struct Common_FlexBisonParserConfiguration
 : Common_ParserConfiguration
{
  Common_FlexBisonParserConfiguration ()
   : Common_ParserConfiguration ()
   , useYYScanBuffer (COMMON_PARSER_DEFAULT_FLEX_USE_YY_SCAN_BUFFER)
  {}

  bool useYYScanBuffer; // yy_scan_buffer() ? : yy_scan_bytes() (C parsers only)
};

#endif
