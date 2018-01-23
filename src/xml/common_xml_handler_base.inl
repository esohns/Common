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

#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ContextType>
Common_XML_Handler_Base_T<ContextType>::Common_XML_Handler_Base_T ()
 : locator_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::Common_XML_Handler_Base_T"));

}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::characters (const ACEXML_Char* characters_in,
                                                    size_t start_in,
                                                    size_t length_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::characters"));

  ACE_UNUSED_ARG (characters_in);
  ACE_UNUSED_ARG (start_in);
  ACE_UNUSED_ARG (length_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::endDocument (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::endDocument"));

}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::endElement (const ACEXML_Char* namespaceURI_in,
                                                    const ACEXML_Char* localName_in,
                                                    const ACEXML_Char* qName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::endElement"));

  ACE_UNUSED_ARG (namespaceURI_in);
  ACE_UNUSED_ARG (localName_in);
  ACE_UNUSED_ARG (qName_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::endPrefixMapping (const ACEXML_Char* prefix_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::endPrefixMapping"));

  ACE_UNUSED_ARG (prefix_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::ignorableWhitespace (const ACEXML_Char* characters_in,
                                                             int start_in,
                                                             int length_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::ignorableWhitespace"));

  ACE_UNUSED_ARG (characters_in);
  ACE_UNUSED_ARG (start_in);
  ACE_UNUSED_ARG (length_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::processingInstruction (const ACEXML_Char* target_in,
                                                               const ACEXML_Char* data_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::processingInstruction"));

  ACE_UNUSED_ARG (target_in);
  ACE_UNUSED_ARG (data_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::skippedEntity (const ACEXML_Char* name_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::skippedEntity"));

  ACE_UNUSED_ARG (name_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::startDocument (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::startDocument"));

}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::startElement (const ACEXML_Char* namespaceURI_in,
                                                      const ACEXML_Char* localName_in,
                                                      const ACEXML_Char* qName_in,
                                                      ACEXML_Attributes* attributes_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::startElement"));

  ACE_UNUSED_ARG (namespaceURI_in);
  ACE_UNUSED_ARG (localName_in);
  ACE_UNUSED_ARG (qName_in);
  ACE_UNUSED_ARG (attributes_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::startPrefixMapping (const ACEXML_Char* prefix_in,
                                                            const ACEXML_Char* URI_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::startPrefixMapping"));

  ACE_UNUSED_ARG (prefix_in);
  ACE_UNUSED_ARG (URI_in);
}

//////////////////////////////////////////

//void
//Common_XML_Handler_Base_T::notationDecl (const ACEXML_Char* name_in,
//                                       const ACEXML_Char* publicId_in,
//                                       const ACEXML_Char* systemId_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::notationDecl"));
//
//  ACE_UNUSED_ARG (name_in);
//  ACE_UNUSED_ARG (publicId_in);
//  ACE_UNUSED_ARG (systemId_in);
//}
//
//void
//Common_XML_Handler_Base_T::unparsedEntityDecl (const ACEXML_Char* name_in,
//                                             const ACEXML_Char* publicId_in,
//                                             const ACEXML_Char* systemId_in,
//                                             const ACEXML_Char* notationName_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::unparsedEntityDecl"));
//
//  ACE_UNUSED_ARG (name_in);
//  ACE_UNUSED_ARG (publicId_in);
//  ACE_UNUSED_ARG (systemId_in);
//  ACE_UNUSED_ARG (notationName_in);
//}

//////////////////////////////////////////

//ACEXML_InputSource*
//Common_XML_Handler_Base_T::resolveEntity (const ACEXML_Char* publicId_in,
//                                        const ACEXML_Char* systemId_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::resolveEntity"));
//
//  ACE_UNUSED_ARG (publicId_in);
//  ACE_UNUSED_ARG (systemId_in);
//
//  return NULL;
//}

//////////////////////////////////////////

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::error (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::error"));

  ACE_UNUSED_ARG (exception_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::fatalError (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::fatalError"));

  ACE_UNUSED_ARG (exception_in);
}

template <typename ContextType>
void
Common_XML_Handler_Base_T<ContextType>::warning (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Handler_Base_T::warning"));

  ACE_UNUSED_ARG (exception_in);
}
