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

#ifndef COMMON_FILE_COMMON_H
#define COMMON_FILE_COMMON_H

#include <string>
//#include <utility>
#include <vector>

#include "ace/Assert.h"
#include "ace/OS_NS_dirent.h"

struct Common_File_Entry
{
  Common_File_Entry ()
   : type (Common_File_Entry::INVALID)
   , group ()
   , owner ()
   , size (0)
   , name ()
  {}

  enum Type
  {
    FILE = 0,
    DIRECTORY,
    LINK,
    INVALID
  };

  enum Type   type;
  std::string group;
  std::string owner;
  ACE_UINT64  size;
  std::string name;
};
typedef std::vector<struct Common_File_Entry> Common_File_Entries_t;
typedef Common_File_Entries_t::const_iterator Common_File_EntriesIterator_t;

struct Common_File_Identifier
{
  Common_File_Identifier ()
   : comparator (NULL)
   , identifier ()
   , identifierDiscriminator (Common_File_Identifier::FILE)
   , selector (NULL) // *NOTE*: NULL will select all (!) entries by default
  {}

  inline void clear () { identifier.clear (); identifierDiscriminator = Common_File_Identifier::FILE; selector = NULL; }
  inline bool empty () { return identifier.empty (); }

  enum discriminatorType
  {
    FILE = 0,
    DIRECTORY,
    INVALID
  };

  ACE_SCANDIR_COMPARATOR comparator;
  std::string            identifier;
  enum discriminatorType identifierDiscriminator;
  ACE_SCANDIR_SELECTOR   selector;
};
typedef std::vector<struct Common_File_Identifier> Common_File_IdentifierList_t;
typedef Common_File_IdentifierList_t::const_iterator Common_File_IdentifierListIterator_t;
struct common_file_identifier_less
{
  inline bool operator() (const struct Common_File_Identifier& lhs_in, const struct Common_File_Identifier& rhs_in) const { ACE_ASSERT (lhs_in.identifierDiscriminator == Common_File_Identifier::FILE); ACE_ASSERT (rhs_in.identifierDiscriminator == Common_File_Identifier::FILE); std::less<std::string> comparator; return comparator (lhs_in.identifier, rhs_in.identifier); }
};
//struct common_file_identifier_lexi_less
//{
//  bool operator() (struct Common_File_Identifier& lhs_in, struct Common_File_Identifier& rhs_in)
//  {
//    ACE_ASSERT (lhs_in.identifierDiscriminator == Common_File_Identifier::FILE);
//    ACE_ASSERT (rhs_in.identifierDiscriminator == Common_File_Identifier::FILE);
//    return std::lexicographical_compare (lhs_in.identifier.begin (),
//                                         lhs_in.identifier.end (),
//                                         rhs_in.identifier.begin (),
//                                         rhs_in.identifier.end ());
//  };
//};

#endif
