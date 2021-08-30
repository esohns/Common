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

#ifndef COMMON_IPARSER_H
#define COMMON_IPARSER_H

#include <string>

#include "location.hh"

#include "common_idumpstate.h"
#include "common_iinitialize.h"

// forward declarations
class ACE_Message_Block;

template <typename ConfigurationType>
class Common_IParser_T
 : public Common_IInitialize_T<ConfigurationType>
 , public Common_IDumpState
{
 public:
  virtual bool parse (ACE_Message_Block*) = 0; // data buffer handle
};

//////////////////////////////////////////

// forward declarations
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

template <typename ConfigurationType>
class Common_IYaccParser_T
 : public Common_IParser_T<ConfigurationType>
{
 public:
  ////////////////////////////////////////
  virtual void error (const struct YYLTYPE&, // location
                      const std::string&) = 0;
  virtual void error (const yy::location&,
                      const std::string&) = 0;
};

template <typename ConfigurationType,
          typename RecordType>
class Common_IYaccStreamParser_T
 : public Common_IYaccParser_T<ConfigurationType>
{
 public:
  // convenient types
  typedef Common_IYaccStreamParser_T<ConfigurationType,
                                     RecordType> IPARSER_T;

  virtual RecordType& current () = 0;

  ////////////////////////////////////////
  // callbacks
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void record (RecordType*&) = 0; // data record
};

//////////////////////////////////////////

template <typename ConfigurationType,
          typename RecordType>
class Common_IYaccRecordParser_T
 : public Common_IYaccStreamParser_T<ConfigurationType,
                                     RecordType>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<ConfigurationType,
                                     RecordType> IPARSER_T;

  virtual bool hasFinished () const = 0;
};

#endif
