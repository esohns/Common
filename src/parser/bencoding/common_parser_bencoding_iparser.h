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

#ifndef COMMON_PARSER_BENCODING_IPARSER_T_H
#define COMMON_PARSER_BENCODING_IPARSER_T_H

#include <string>

#include "common_configuration.h"

#include "common_iscanner.h"
#include "common_iparser.h"
#include "common_parser_common.h"

#include "common_parser_bencoding_common.h"

class Common_Parser_Bencoding_IParser
 : public Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     Bencoding_Dictionary_t>
 , virtual public Common_ILexScanner_T<struct Common_FlexScannerState,
                                       void>
// , public Common_IGet_T<Bencoding_Dictionary_t>
// , public Common_IGet_T<Bencoding_List_t>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     Bencoding_Dictionary_t> IPARSER_T;
  typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                               void> ISCANNER_T;

  using IPARSER_T::error;
//  using Common_IScanner::error;

  // nesting-
  virtual void addLevel () = 0;
  virtual void removeLevel () = 0;
  virtual unsigned int level () = 0;
  // key/value
  virtual bool toggleKeyValue () = 0;

  virtual std::string* popKey () = 0;
  virtual void popDictionary () = 0;
  virtual void popList () = 0;
  virtual void pushDictionary (Bencoding_Dictionary_t*) = 0; // dictionary handle
  virtual void pushKey (std::string*) = 0; // key handle
  virtual void pushList (Bencoding_List_t*) = 0; // list handle

  virtual void record_2 (Bencoding_List_t*&) = 0; // data record
  virtual Bencoding_List_t& current_2 () = 0; // data record
  virtual void record_3 (std::string*&) = 0; // data record
  virtual void record_4 (ACE_INT64) = 0; // data record
};

////////////////////////////////////////////

typedef Common_Parser_Bencoding_IParser Bencoding_IParser_t;
typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                             void> Bencoding_IScanner_t;

#endif
