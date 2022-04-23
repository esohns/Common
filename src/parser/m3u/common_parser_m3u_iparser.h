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

#ifndef COMMON_PARSER_M3U_IPARSER_H
#define COMMON_PARSER_M3U_IPARSER_H

#include <string>
#include <vector>

#include "ace/Basic_Types.h"

#include "common_configuration.h"

#include "common_iscanner.h"
#include "common_iparser.h"
#include "common_parser_common.h"

typedef std::vector<std::pair<std::string, std::string> > M3U_KeyValues_t;
typedef M3U_KeyValues_t::const_iterator M3U_KeyValuesIterator_t;

struct M3U_ExtInf_Element
{
  std::string Artist;
  std::string Album;
  ACE_INT32 Length;
  std::string Title;
  std::string URL;
};
typedef std::vector<struct M3U_ExtInf_Element> M3U_ExtInf_Elements_t;
typedef M3U_ExtInf_Elements_t::const_iterator M3U_ExtInf_ElementsIterator_t;

struct M3U_Media_Element
{
  std::string URL;

  std::string key; // temp
  M3U_KeyValues_t keyValues;
};
typedef std::vector<struct M3U_Media_Element> M3U_Media_Elements_t;
typedef M3U_Media_Elements_t::const_iterator M3U_Media_ElementsIterator_t;

struct M3U_StreamInf_Element
{
  ACE_INT32 Length;
  std::string URL;

  std::string key; // temp
  M3U_KeyValues_t keyValues;
};
typedef std::vector<struct M3U_StreamInf_Element> M3U_StreamInf_Elements_t;
typedef M3U_StreamInf_Elements_t::const_iterator M3U_StreamInf_ElementsIterator_t;

struct M3U_Playlist
{
  M3U_ExtInf_Elements_t ext_inf_elements;
  M3U_Media_Elements_t media_elements;
  M3U_StreamInf_Elements_t stream_inf_elements;

  std::string key; // temp
  M3U_KeyValues_t keyValues;
};

class Common_Parser_M3U_IParser
 : public Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct M3U_Playlist>
 , virtual public Common_ILexScanner_T<struct Common_FlexScannerState,
                                       Common_Parser_M3U_IParser>
 , public Common_ISetP_T<struct M3U_Playlist>
 , public Common_ISetP_2_T<struct M3U_ExtInf_Element>
 , public Common_ISetP_3_T<struct M3U_Media_Element>
 , public Common_ISetP_4_T<struct M3U_StreamInf_Element>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct M3U_Playlist> IPARSER_T;
  typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                               Common_Parser_M3U_IParser> ISCANNER_T;

  using IPARSER_T::error;
//  using Common_IScanner::error;

  virtual struct M3U_ExtInf_Element& current_2 () = 0; // element handle
  virtual struct M3U_Media_Element& current_3 () = 0; // element handle
  virtual struct M3U_StreamInf_Element& current_4 () = 0; // element handle

  virtual void record (struct M3U_Playlist*&) = 0; // data record
};

////////////////////////////////////////////

typedef Common_Parser_M3U_IParser M3U_IParser_t;
typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                             Common_Parser_M3U_IParser> M3U_IScanner_t;

#endif
