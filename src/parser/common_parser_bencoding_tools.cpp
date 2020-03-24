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

void
Common_Parser_Bencoding_Tools::free (Bencoding_Dictionary_t*& dictionary_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::free"));

  if (!dictionary_inout)
    return;

  for (Bencoding_DictionaryIterator_t iterator = dictionary_inout->begin ();
       iterator != dictionary_inout->end ();
       ++iterator)
  {
    switch ((*iterator).second->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
        break;
      case Bencoding_Element::BENCODING_TYPE_STRING:
        delete (*iterator).second->string; break;
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        Common_Parser_Bencoding_Tools::free ((*iterator).second->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        Common_Parser_Bencoding_Tools::free ((*iterator).second->dictionary);
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

  delete dictionary_inout; dictionary_inout = NULL;
}

void
Common_Parser_Bencoding_Tools::free (Bencoding_List_t*& list_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::free"));

  if (!list_inout)
    return;

  for (Bencoding_ListIterator_t iterator = list_inout->begin ();
       iterator != list_inout->end ();
       ++iterator)
  {
    switch ((*iterator)->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
        break;
      case Bencoding_Element::BENCODING_TYPE_STRING:
        delete (*iterator)->string; break;
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        Common_Parser_Bencoding_Tools::free ((*iterator)->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        Common_Parser_Bencoding_Tools::free ((*iterator)->dictionary);
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

  delete list_inout; list_inout = NULL;
}

std::string
Common_Parser_Bencoding_Tools::bencode (const Bencoding_Dictionary_t& dictionary_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::bencode"));

  // initialize return value
  std::string result = ACE_TEXT_ALWAYS_CHAR ("d");

  std::ostringstream converter;

  for (Bencoding_DictionaryIterator_t iterator = dictionary_in.begin ();
       iterator != dictionary_in.end ();
       ++iterator)
  {
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << (*iterator).first->size ();
    result += converter.str ();
    result += ':';
    result += *(*iterator).first;

    switch ((*iterator).second->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
      {
        result += 'i';
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << (*iterator).second->integer;
        result += converter.str ();
        result += 'e';
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_STRING:
      {
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << (*iterator).second->string->size ();
        result += converter.str ();
        result += ':';
        result += *(*iterator).second->string;
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        result += Common_Parser_Bencoding_Tools::bencode (*(*iterator).second->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        result += Common_Parser_Bencoding_Tools::bencode (*(*iterator).second->dictionary);
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
  result += 'e';

  return result;
}

std::string
Common_Parser_Bencoding_Tools::bencode (const Bencoding_List_t& list_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_Bencoding_Tools::bencode"));

  // initialize return value
  std::string result = ACE_TEXT_ALWAYS_CHAR ("l");

  std::ostringstream converter;

  for (Bencoding_ListIterator_t iterator = list_in.begin ();
       iterator != list_in.end ();
       ++iterator)
  {
    switch ((*iterator)->type)
    {
      case Bencoding_Element::BENCODING_TYPE_INTEGER:
      {
        result += 'i';
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << (*iterator)->integer;
        result += converter.str ();
        result += 'e';
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_STRING:
      {
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << (*iterator)->string->size ();
        result += converter.str ();
        result += ':';
        result += *(*iterator)->string;
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_LIST:
      {
        result += Common_Parser_Bencoding_Tools::bencode (*(*iterator)->list);
        break;
      }
      case Bencoding_Element::BENCODING_TYPE_DICTIONARY:
      {
        result += Common_Parser_Bencoding_Tools::bencode (*(*iterator)->dictionary);
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
  result += 'e';

  return result;
}
