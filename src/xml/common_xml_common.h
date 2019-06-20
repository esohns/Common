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

#ifndef COMMON_XML_COMMON_H
#define COMMON_XML_COMMON_H

#include <string>
#include <utility>
#include <vector>

//// default tab width
//#define XML2CPPCODE_INDENT                    2
//
//// default type postfix
//#define XML2CPPCODE_DEFAULTTYPEPOSTFIX        "_Type"
//
//// default union postfix
//#define XML2CPPCODE_DEFAULTUNIONPOSTFIX       "Union"
//// #define XML2CPPCODE_DEFAULTTAGGEDUNIONINFIX "Tagged"
//
//// default choice postfix
//#define XML2CPPCODE_DEFAULTCHOICEPOSTFIX      "Element"
//
//// defaul minOccurs/maxOccurs
//#define XML2CPPCODE_DEFAULTMINOCCURS          "1"
//#define XML2CPPCODE_DEFAULTMAXOCCURS          "1"
//
//// default include header extension/appendix
//#define XML2CPPCODE_HEADER_EXTENSION          ".h"
//#define XML2CPPCODE_HEADER_APPENDIX           "_incl"
//
//#if defined(ACE_WIN32) || defined(ACE_WIN64)
//#define XML2CPPCODE_DLL_EXPORT_QUALIFIER      "__declspec(dllexport)"
//#else
//#define XML2CPPCODE_DLL_EXPORT_QUALIFIER      ""
//#endif
//#define XML2CPPCODE_DLL_EXPORT_INCLUDE_SUFFIX "exports"

// *NOTE*: the feature identifier is a URL from http://xml.org/sax/features/
typedef std::pair<std::string, bool> Common_XML_SAXParserFeature_t;
typedef std::vector<Common_XML_SAXParserFeature_t> Common_XML_SAXParserFeatures_t;
typedef Common_XML_SAXParserFeatures_t::const_iterator Common_XML_SAXParserFeaturesIterator_t;
struct Common_XML_ParserConfiguration
{
  Common_XML_SAXParserFeatures_t SAXFeatures;
};

struct Common_XML_ParserContext
{

};

//typedef Common_XML_Parser_T<ACE_MT_SYNCH,
//                            struct Common_XML_ParserConfiguration,
//                            struct Common_XML_ParserContext,
//                            Common_XML_Handler> Common_XML_Parser_t;
//typedef ACE_Singleton<Common_XML_Parser_t,
//                      ACE_SYNCH_MUTEX> COMMON_XML_PARSER_SINGLETON;

#endif
