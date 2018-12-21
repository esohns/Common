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

#include "common_xml_handler.h"

#include "ace/Log_Msg.h"

#include "ACEXML/common/FileCharStream.h"
#include "ACEXML/common/InputSource.h"
#include "ACEXML/common/StrCharStream.h"
#include "ACEXML/parser/parser/Parser.h"

#include "common_macros.h"

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
Common_XML_Parser_T<ACE_SYNCH_USE,
                    ConfigurationType,
                    ContextType,
                    HandlerType>::Common_XML_Parser_T (ContextType* contextHandle_in)
 : handler_ (contextHandle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Parser_T::Common_XML_Parser_T"));

}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
void
Common_XML_Parser_T<ACE_SYNCH_USE,
                    ConfigurationType,
                    ContextType,
                    HandlerType>::parseFile (const std::string& filename_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Parser_T::parseFile"));

  int result = -1;
  ACEXML_FileCharStream* stream_p = NULL;
  ACE_NEW_NORETURN (stream_p,
                    ACEXML_FileCharStream ());
  ACE_ASSERT (stream_p);
  ACEXML_InputSource input_source (stream_p);

  result = stream_p->open (ACE_TEXT (filename_in.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACEXML_FileCharStream::open (\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (filename_in.c_str ())));
    return;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("parsing XML file \"%s\"\n"),
              ACE_TEXT (filename_in.c_str ())));
#endif // _DEBUG
  parse (&input_source);
}
template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
void
Common_XML_Parser_T<ACE_SYNCH_USE,
                    ConfigurationType,
                    ContextType,
                    HandlerType>::parseString (const std::string& XML_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Parser_T::parseString"));

  int result = -1;
  ACEXML_StrCharStream* stream_p = NULL;
  ACE_NEW_NORETURN (stream_p,
                    ACEXML_StrCharStream ());
  ACE_ASSERT (stream_p);
  ACEXML_InputSource input_source (stream_p);

  result = stream_p->open (ACE_TEXT (XML_in.c_str ()),
                           ACE_TEXT (""));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACEXML_StrCharStream::open (\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (XML_in.c_str ())));
    return;
  } // end IF

  parse (&input_source);
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
bool
Common_XML_Parser_T<ACE_SYNCH_USE,
                    ConfigurationType,
                    ContextType,
                    HandlerType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Parser_T::initialize"));

  // set parser features
  for (Common_XML_SAXParserFeaturesIterator_t iterator = configuration_in.SAXFeatures.begin ();
       iterator != configuration_in.SAXFeatures.end ();
       ++iterator)
    inherited::setFeature (ACE_TEXT ((*iterator).first.c_str ()),
                           (*iterator).second);

  return true;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
void
Common_XML_Parser_T<ACE_SYNCH_USE,
                    ConfigurationType,
                    ContextType,
                    HandlerType>::parse (ACEXML_InputSource* inputSource_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_XML_Parser_T::parse"));

  // sanity check(s)
  ACE_ASSERT (inputSource_in);

  inherited::setContentHandler (&handler_);
  //inherited::setDTDHandler (&handler_);
  inherited::setErrorHandler (&handler_);
  //inherited::setEntityResolver (&handler_);

  try {
    inherited::parse (inputSource_in);
  } catch (ACEXML_SAXParseException& exception_in) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught SAX XML exception in ACEXML_Parser::parse(): \"%s\", returning\n"),
                ACE_TEXT (exception_in.message ())));
    return;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ACEXML_Parser::parse(), returning\n")));
    return;
  }
}
