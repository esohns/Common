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

#ifndef COMMON_CONFIGURATION_H
#define COMMON_CONFIGURATION_H

#include <cstdio>

//#include "ace/config-lite.h"

#include "common_defines.h"

// forward declarations
class ACE_Message_Queue_Base;

struct Common_AllocatorConfiguration
{
  Common_AllocatorConfiguration ()
   : defaultBufferSize (BUFSIZ)
   , paddingBytes (0)
  {};

  unsigned int defaultBufferSize;
  // *NOTE*: add x bytes to each malloc(), override as needed
  //         (e.g. flex requires additional 2 YY_END_OF_BUFFER_CHARs). Note that
  //         this affects the ACE_Data_Block capacity, not its allotted size
  unsigned int paddingBytes;
};

struct Common_FlexParserAllocatorConfiguration
 : Common_AllocatorConfiguration
{
  Common_FlexParserAllocatorConfiguration ()
   : Common_AllocatorConfiguration ()
  {
    // *NOTE*: this facilitates (message block) data buffers to be scanned with
    //         'flex's yy_scan_buffer() method
    paddingBytes = COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE;
  };
};

struct Common_ParserConfiguration
{
  Common_ParserConfiguration ()
   : block (true)
   , messageQueue (NULL)
   , useYYScanBuffer (true)
   , debugParser (COMMON_PARSER_DEFAULT_YACC_TRACE)
   , debugScanner (COMMON_PARSER_DEFAULT_LEX_TRACE)
  {};

  bool                    block; // block in parse (i.e. wait for data in yywrap() ?)
  ACE_Message_Queue_Base* messageQueue; // queue (if any) to use for yywrap
  bool                    useYYScanBuffer; // yy_scan_buffer() ? : yy_scan_bytes() (C parsers only)

  // debug
  bool                    debugParser;
  bool                    debugScanner;
};

struct Common_SignalHandlerConfiguration
{
  Common_SignalHandlerConfiguration ()
   : hasUI (false)
   , useReactor (COMMON_EVENT_USE_REACTOR)
  {};

  bool hasUI;
  bool useReactor;
};

#endif
