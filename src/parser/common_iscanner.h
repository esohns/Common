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

#ifndef COMMON_ISCANNER_H
#define COMMON_ISCANNER_H

#include <string>

#include "common_iget.h"
#include "common_iinitialize.h"
#include "common_idumpstate.h"

// forward declarations
typedef void* yyscan_t;
struct yy_buffer_state;
class ACE_Message_Block;

class Common_IScannerBase
{
 public:
  virtual ACE_Message_Block* buffer () = 0;
#if defined (_DEBUG)
  virtual bool debug () const = 0;
#endif // _DEBUG
  virtual bool isBlocking () const = 0;
  virtual unsigned int offset () const = 0;

  virtual bool begin (const char*,       // buffer
                      unsigned int) = 0; // size
  virtual void end () = 0;

  // *NOTE*: appends a new buffer to the ACE_Message_Block chain
  virtual bool switchBuffer (bool = false) = 0; // unlink current buffer ?
  virtual void waitBuffer () = 0;

  ////////////////////////////////////////
  virtual void error (const std::string&) = 0;
  virtual void head (ACE_Message_Block*) = 0; // new data buffer handle
  virtual void offset (unsigned int) = 0; // offset (/increment)
};

template <typename ConfigurationType>
class Common_IScanner_T
 : public Common_IScannerBase
 , public Common_IGetR_T<ConfigurationType>
{};

template <typename StateType,     // implements struct Common_ScannerState
          typename ExtraDataType> // 'extra' data type
class Common_ILexScanner_T
 : public Common_IScanner_T<struct Common_FlexBisonParserConfiguration>
 , public Common_IGetR_2_T<StateType>
//, public Common_IGetSetP_T<ExtraDataType>
//, public Common_IGetP_2_T<ExtraDataType>
{
 public:
  virtual void setDebug (yyscan_t,  // state handle
                         bool) = 0; // toggle
  virtual void reset () = 0; // resets the offsets (line/column to 1,1)

  virtual bool initialize (yyscan_t&,                  // return value: state handle
                           ExtraDataType* = NULL) = 0; // 'extra' data handle
  virtual void finalize (yyscan_t&) = 0; // state handle

  virtual struct yy_buffer_state* create (yyscan_t,    // state handle
                                          char*,       // buffer handle
                                          size_t) = 0; // buffer size
  virtual void destroy (yyscan_t,                      // state handle
                        struct yy_buffer_state*&) = 0; // buffer handle

  virtual bool lex (yyscan_t) = 0; // state handle
};

#endif
