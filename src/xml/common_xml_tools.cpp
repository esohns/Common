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

#include "common_xml_tools.h"

#if defined (LIBXML2_SUPPORT)
#include "libxml/parser.h"
#endif // LIBXML2_SUPPORT

#include "ace/Log_Msg.h"

#include "common_macros.h"

void
Common_XML_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Tools::initialize"));

#if defined (LIBXML2_SUPPORT)
  xmlInitParser ();
#endif // LIBXML2_SUPPORT
}
void
Common_XML_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Tools::finalize"));

#if defined (LIBXML2_SUPPORT)
  xmlCleanupParser ();
#endif // LIBXML2_SUPPORT
}

//bool
//Common_XML_Tools::XMLintegratedtypeToString (const std::string& typeSpecifier_in,
//                                             std::string& result_out)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_XML_Tools::XMLintegratedtypeToString"));
//
//  if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("boolean"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("bool");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("byte"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("signed char");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedByte"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned char");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("short"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("short int");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedShort"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned short int");
//  else if ((typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("integer")) ||
//           (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("int")))
//    result_out = ACE_TEXT_ALWAYS_CHAR("int");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedInt"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned int");
//  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("string"))
//    result_out = ACE_TEXT_ALWAYS_CHAR("std::string");
//  else
//  {
////    ACE_DEBUG((LM_DEBUG,
////               ACE_TEXT("unknown type: \"%s\", returning as-is\n"),
////               ACE_TEXT(typeSpecifier_in.c_str())));
//
//    result_out = typeSpecifier_in;
//
//    return false;
//  } // end ELSE
//
//  return true;
//}

std::string
Common_XML_Tools::applyNsPrefixToXPathQuery (const std::string& query_in,
                                             const std::string& prefix_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Tools::applyNsPrefixToXPathQuery"));

  // initialize return value(s)
  std::string return_value = query_in;

  // sanity check(s)
  ACE_ASSERT (!prefix_in.empty ());

  std::string::size_type position = 0;
  while ((position = return_value.find ('/', position)) != std::string::npos)
  {
    return_value.insert (position + 1, prefix_in);
    return_value.insert (position + 1 + prefix_in.size (),
                         ACE_TEXT_ALWAYS_CHAR (":"));
    position += prefix_in.size () + 1;
  } // end WHILE

  return return_value;
}
