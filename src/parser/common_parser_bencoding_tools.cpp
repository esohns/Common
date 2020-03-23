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
#include "stdafx.h"

#include "common_parser_bencoding_tools.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_macros.h"

std::string
Common_Parser_Bencoding_Tools::DictionaryToString (const Bencoding_Dictionary_t& dictionary_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::DictionaryToString"));

  std::string result (ACE_TEXT_ALWAYS_CHAR ("dictionary:\n"));

  for (Bencoding_DictionaryIterator_t iterator = dictionary_in.begin ();
       iterator != dictionary_in.end ();
       ++iterator)
  {
    result += ACE_TEXT_ALWAYS_CHAR ("\"");
    result += *(*iterator).first;
    result += ACE_TEXT_ALWAYS_CHAR ("\": ");

    switch ((*iterator).second->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
      {
        std::ostringstream converter;
        converter << (*iterator).second->integer;
        result += converter.str ();
        result += ACE_TEXT_ALWAYS_CHAR ("\n");
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_STRING:
      {
        result += ACE_TEXT_ALWAYS_CHAR ("\"");
        result += *(*iterator).second->string;
        result += ACE_TEXT_ALWAYS_CHAR ("\"\n");
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        result += Common_Parser_Bencoding_Tools::ListToString (*(*iterator).second->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        result += Common_Parser_Bencoding_Tools::DictionaryToString (*(*iterator).second->dictionary);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown type (was: %d), continuing\n"),
                    ACE_TEXT ((*iterator).second->type)));
        break;
      }
    } // end SWITCH
  } // end FOR

  return result;
}

std::string
Common_Parser_Bencoding_Tools::ListToString (const Bencoding_List_t& list_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::ListToString"));

  std::string result (ACE_TEXT_ALWAYS_CHAR ("list:\n"));

  for (Bencoding_ListIterator_t iterator = list_in.begin ();
       iterator != list_in.end ();
       ++iterator)
  {
    switch ((*iterator)->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
      {
        std::ostringstream converter;
        converter << (*iterator)->integer;
        result += converter.str ();
        result += ACE_TEXT_ALWAYS_CHAR ("\n");
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_STRING:
      {
        result += ACE_TEXT_ALWAYS_CHAR ("\"");
        result += *(*iterator)->string;
        result += ACE_TEXT_ALWAYS_CHAR ("\"\n");
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        result += Common_Parser_Bencoding_Tools::ListToString (*(*iterator)->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        result += Common_Parser_Bencoding_Tools::DictionaryToString (*(*iterator)->dictionary);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown type (was: %d), continuing\n"),
                    ACE_TEXT ((*iterator)->type)));
        break;
      }
    } // end SWITCH
  } // end FOR

  return result;
}
