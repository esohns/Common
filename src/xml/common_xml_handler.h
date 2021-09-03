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

#ifndef COMMON_XML_HANDLER_H
#define COMMON_XML_HANDLER_H

#include <string>
 //#include <fstream>
 //#include <vector>
 //#include <stack>

#include "ace/Global_Macros.h"

#include "common_xml_common.h"
#include "common_xml_handler_base.h"

// forward declarations
//class Handle_XMLEnumeration;
//class Handle_XMLSequence;

class Common_XML_Handler
 : public Common_XML_Handler_Base_T<struct Common_XML_ParserContext>
{
  typedef Common_XML_Handler_Base_T<struct Common_XML_ParserContext> inherited;

 public:
  Common_XML_Handler ();
  inline virtual ~Common_XML_Handler () {}

  // implement (part of) ACEXML_ContentHandler
  virtual void endDocument (void);
  virtual void endElement (const ACEXML_Char*,  // namespace uri
                           const ACEXML_Char*,  // localname
                           const ACEXML_Char*); // qname
  virtual void startDocument (void);
  virtual void startElement (const ACEXML_Char*,  // namespace uri
                             const ACEXML_Char*,  // localname
                             const ACEXML_Char*,  // qname
                             ACEXML_Attributes*); // attrs

  // implement ACEXML_ErrorHandler
  virtual void error (ACEXML_SAXParseException&); // exception
  virtual void fatalError (ACEXML_SAXParseException&); // exception
  virtual void warning (ACEXML_SAXParseException&); // exception

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Handler (const Common_XML_Handler&));
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Handler& operator= (const Common_XML_Handler&));

  //enum XMLElementType
  //{
  //  XML_ANNOTATION = 0,
  //  XML_ATTRIBUTE,
  //  XML_CHOICE,
  //  XML_COMPLEXCONTENT,
  //  XML_COMPLEXTYPE,
  //  XML_DOCUMENTATION,
  //  XML_ELEMENT,
  //  XML_ENUMERATION,
  //  XML_EXTENSION,
  //  XML_INCLUDE,
  //  XML_RESTRICTION,
  //  XML_SCHEMA,
  //  XML_SEQUENCE,
  //  XML_SIMPLETYPE,
  //  XML_UNION,
  //  //
  //  XML_INVALID
  //};

//  // helper methods
//  XMLElementType stringToXMLElementType(const std::string&) const; // element
//  void insertPreamble(std::ofstream&); // file stream
//  void insertMultipleIncludeProtection(const bool&,        // use "#pragma once" directive ?
//                                       const std::string&, // (header) filename
//                                       std::ofstream&);    // file stream
//  void insertPostscript(std::ofstream&); // file stream
//  typedef std::vector<std::string> XML2CppCode_Headers_t;
//  void insertIncludeHeaders(const XML2CppCode_Headers_t&, // headers
//                            const bool&,                  // include <vector>
//                            std::ofstream&);              // file stream
//
//  // helper types
//  typedef XML2CppCode_Headers_t::const_iterator XML2CppCode_HeadersIterator_t;
//  typedef std::stack<IXML_Definition_Handler*> XML2CppCode_Handlers_t;
//
//  XML2CppCode_Handlers_t   myDefinitionHandlers;
//
//  std::ofstream            myIncludeHeaderFile;
//  std::ofstream            myCurrentOutputFile;
//  bool                     myCurrentHasBaseClass;
//  std::string              myCurrentElementName;
//  bool                     myIsFirstRelevantElement;
//  std::string              myTargetDirectory;
//  bool                     myFilePerDefinition;
//  std::string              mySchemaFilename;
//  std::string              myPreamble;
//  std::string              myEmitClassQualifiers;
//  bool                     myEmitStringConversionUtilities;
//  bool                     myEmitTaggedUnions;
//  bool                     myGenerateIncludeHeader;
//  std::string              myTypePrefix;
//  std::string              myTypePostfix;
//  const Locator*           myLocator;
////   bool                     myIgnoreCharacters;
//  XML2CppCode_Headers_t    myHeaders;
//  bool                     myHeadersUseStdVector;
//  unsigned int             myCurrentNestingLevel;
};

#endif
