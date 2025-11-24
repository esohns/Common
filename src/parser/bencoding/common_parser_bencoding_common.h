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

#ifndef COMMON_PARSER_BENCODING_COMMON_H
#define COMMON_PARSER_BENCODING_COMMON_H

#include <string>
#include <utility>
#include <vector>

#include "ace/Basic_Types.h"

// *NOTE*: the bencoding format is not really type-safe, so 'strict' languages
//         like C/C++ need to jump through a few hoops here
struct Bencoding_Element;
typedef std::vector<struct Bencoding_Element*> Bencoding_List_t;
typedef Bencoding_List_t::const_iterator Bencoding_ListIterator_t;
// *NOTE*: key order is important to generate correct hashes --> cannot use map
typedef std::vector<std::pair<std::string*, struct Bencoding_Element*> > Bencoding_Dictionary_t;
typedef Bencoding_Dictionary_t::const_iterator Bencoding_DictionaryIterator_t;

struct Bencoding_Element
{
  Bencoding_Element ()
   : type (BENCODING_TYPE_INVALID)
   , integer (0)
  {}

  enum Bencoding_ElementType
  {
    BENCODING_TYPE_INTEGER = 0,
    BENCODING_TYPE_STRING,
    BENCODING_TYPE_LIST,
    BENCODING_TYPE_DICTIONARY,
    //////////////////////////////////////
    BENCODING_TYPE_MAX,
    BENCODING_TYPE_INVALID
  };

  enum Bencoding_ElementType type;
  union
  {
    ACE_INT64               integer;
    std::string*            string;
    Bencoding_List_t*       list;
    Bencoding_Dictionary_t* dictionary;
  };
};

#endif
