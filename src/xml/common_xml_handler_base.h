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

#ifndef COMMON_XML_HANDLER_BASE_H
#define COMMON_XML_HANDLER_BASE_H

#include "ace/Global_Macros.h"

#include "ACEXML/common/ContentHandler.h"
//#include "ACEXML/common/DTDHandler.h"
//#include "ACEXML/common/EntityResolver.h"
#include "ACEXML/common/ErrorHandler.h"
#include "ACEXML/common/SAXExceptions.h"
#include "ACEXML/common/XML_Types.h"

// forward declarations
class ACEXML_Locator;
class ACEXML_Attributes;
//class ACEXML_InputSource;

template <typename ContextType>
class Common_XML_Handler_Base_T
 : public ACEXML_ContentHandler,
   //public ACEXML_DTDHandler,
   //public ACEXML_EntityResolver,
   public ACEXML_ErrorHandler
{
 public:
  inline virtual ~Common_XML_Handler_Base_T () {}

  // implement ACEXML_ContentHandler
  virtual void characters (const ACEXML_Char*, // chars
                           size_t,             // start
                           size_t);            // length
  virtual void endDocument (void);
  virtual void endElement (const ACEXML_Char*,  // namespace uri
                           const ACEXML_Char*,  // localname
                           const ACEXML_Char*); // qname
  virtual void endPrefixMapping (const ACEXML_Char*); // prefix
  virtual void ignorableWhitespace (const ACEXML_Char*,  // chars
                                    int,  // start
                                    int); // length
  virtual void processingInstruction (const ACEXML_Char*,  // target
                                      const ACEXML_Char*); // data
  inline virtual void setDocumentLocator (ACEXML_Locator* locator_in) { locator_ = locator_in; }
  virtual void skippedEntity (const ACEXML_Char*); // name
  virtual void startDocument (void);
  virtual void startElement (const ACEXML_Char*,  // namespace uri
                             const ACEXML_Char*,  // localname
                             const ACEXML_Char*,  // qname
                             ACEXML_Attributes*); // attrs
  virtual void startPrefixMapping (const ACEXML_Char*,  // prefix
                                   const ACEXML_Char*); // uri

  //// implement ACEXML_DTDHandler
  //virtual void notationDecl (const ACEXML_Char*,  // name
  //                           const ACEXML_Char*,  // publicId
  //                           const ACEXML_Char*); // systemId
  //virtual void unparsedEntityDecl (const ACEXML_Char*,  // name
  //                                 const ACEXML_Char*,  // publicId
  //                                 const ACEXML_Char*,  // systemId
  //                                 const ACEXML_Char*); // notationName

  //// implement ACEXML_EntityResolver
  //virtual ACEXML_InputSource* resolveEntity (const ACEXML_Char*,  // publicId
  //                                           const ACEXML_Char*); // systemId

  // implement ACEXML_ErrorHandler
  virtual void error (ACEXML_SAXParseException&); // exception
  virtual void fatalError (ACEXML_SAXParseException&); // exception
  virtual void warning (ACEXML_SAXParseException&); // exception

 protected:
  Common_XML_Handler_Base_T ();

  ACEXML_Locator* locator_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Handler_Base_T (const Common_XML_Handler_Base_T&));
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Handler_Base_T& operator= (const Common_XML_Handler_Base_T&));
};

// include template definition
#include "common_xml_handler_base.inl"

#endif
