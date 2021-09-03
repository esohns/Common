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

#include "common_ui_monitor_setup_xml_handler.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

Common_UI_MonitorSetup_XML_Handler::Common_UI_MonitorSetup_XML_Handler (struct Common_UI_MonitorSetup_ParserContext* context_in)
 : inherited ()
 , parserContextHandle_ (context_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_MonitorSetup_XML_Handler::error"));

  // sanity check(s)
  ACE_ASSERT (parserContextHandle_);
}


void
Common_UI_MonitorSetup_XML_Handler::characters (const ACEXML_Char* chars_in,
                                                size_t start_in,
                                                size_t length_in)
{
  COMMON_TRACE(ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::characters"));

  ACE_UNUSED_ARG (start_in);
  ACE_UNUSED_ARG (length_in);

  switch (parserContextHandle_->state)
  {
    case COMMON_UI_MONITORSETUP_PARSERSTATE_X:
    {
      parserContextHandle_->current.clippingArea.x =
          static_cast<short>(ACE_OS::atoi (chars_in));
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    case COMMON_UI_MONITORSETUP_PARSERSTATE_Y:
    {
      parserContextHandle_->current.clippingArea.y =
          static_cast<short>(ACE_OS::atoi (chars_in));
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    case COMMON_UI_MONITORSETUP_PARSERSTATE_PRIMARY:
    {
      parserContextHandle_->current.primary =
          !ACE_OS::strcmp (chars_in, ACE_TEXT_ALWAYS_CHAR ("yes"));
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    case COMMON_UI_MONITORSETUP_PARSERSTATE_CONNECTOR:
    {
      parserContextHandle_->current.device = chars_in;
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    case COMMON_UI_MONITORSETUP_PARSERSTATE_WIDTH:
    {
      parserContextHandle_->current.clippingArea.width =
          static_cast<unsigned short>(ACE_OS::atoi (chars_in));
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    case COMMON_UI_MONITORSETUP_PARSERSTATE_HEIGHT:
    {
      parserContextHandle_->current.clippingArea.height =
          static_cast<unsigned short>(ACE_OS::atoi (chars_in));
      parserContextHandle_->state =
          COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
      break;
    }
    default:
    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unkown state (was: %d), returning\n"),
//                  parserContextHandle_->state));
      break;
    }
  } // end SWITCH
}

void
Common_UI_MonitorSetup_XML_Handler::startElement (const ACEXML_Char* namespaceURI_in,
                                                  const ACEXML_Char* localname_in,
                                                  const ACEXML_Char* qname_in,
                                                  ACEXML_Attributes* attrs_in)
{
  COMMON_TRACE(ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::startElement"));

  ACE_UNUSED_ARG (namespaceURI_in);
  ACE_UNUSED_ARG (qname_in);
  ACE_UNUSED_ARG (attrs_in);

  if (!ACE_OS::strcmp (localname_in,
                       ACE_TEXT_ALWAYS_CHAR ("logicalmonitor")))
  {
    parserContextHandle_->state =
        COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR;
    return;
  } // end IF
  if (parserContextHandle_->state !=
      COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR)
    return;

  if (!ACE_OS::strcmp (localname_in,
                       ACE_TEXT_ALWAYS_CHAR ("x")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_X;
    return;
  } // end IF
  else if (!ACE_OS::strcmp (localname_in,
                            ACE_TEXT_ALWAYS_CHAR ("y")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_Y;
    return;
  } // end IF
  else if (!ACE_OS::strcmp (localname_in,
                            ACE_TEXT_ALWAYS_CHAR ("primary")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_PRIMARY;
    return;
  } // end IF
  else if (!ACE_OS::strcmp (localname_in,
                            ACE_TEXT_ALWAYS_CHAR ("connector")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_CONNECTOR;
    return;
  } // end IF
  else if (!ACE_OS::strcmp (localname_in,
                            ACE_TEXT_ALWAYS_CHAR ("width")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_WIDTH;
    return;
  } // end IF
  else if (!ACE_OS::strcmp (localname_in,
                            ACE_TEXT_ALWAYS_CHAR ("height")))
  {
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_HEIGHT;
    return;
  } // end IF
}

void
Common_UI_MonitorSetup_XML_Handler::endElement (const ACEXML_Char* namespaceURI_in,
                                                const ACEXML_Char* localname_in,
                                                const ACEXML_Char* qname_in)
{
  COMMON_TRACE(ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::endElement"));

  ACE_UNUSED_ARG (namespaceURI_in);
  ACE_UNUSED_ARG (qname_in);

  if (!ACE_OS::strcmp (localname_in,
                       ACE_TEXT_ALWAYS_CHAR ("logicalmonitor")))
  {
    parserContextHandle_->displays.push_back (parserContextHandle_->current);
    parserContextHandle_->current = Common_UI_DisplayDevice ();
    parserContextHandle_->state = COMMON_UI_MONITORSETUP_PARSERSTATE_INVALID;
    return;
  } // end IF
}

//////////////////////////////////////////

void
Common_UI_MonitorSetup_XML_Handler::error (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_MonitorSetup_XML_Handler::error"));

  //char* message = XMLString::transcode(exception_in.getMessage());
  //ACE_ASSERT(message);

  //ACE_DEBUG((LM_ERROR,
  //           ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::error(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
  //           ACE_TEXT(exception_in.getSystemId()),
  //           exception_in.getLineNumber(),
  //           exception_in.getColumnNumber(),
  //           ACE_TEXT(message)));

  //// clean up
  //XMLString::release(&message);
}

void
Common_UI_MonitorSetup_XML_Handler::fatalError (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_MonitorSetup_XML_Handler::fatalError"));

  //char* message = XMLString::transcode(exception_in.getMessage());
  //ACE_ASSERT(message);

  //ACE_DEBUG((LM_CRITICAL,
  //           ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::fatalError(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
  //           ACE_TEXT(exception_in.getSystemId()),
  //           exception_in.getLineNumber(),
  //           exception_in.getColumnNumber(),
  //           ACE_TEXT(message)));

  //// clean up
  //XMLString::release(&message);
}

void
Common_UI_MonitorSetup_XML_Handler::warning (ACEXML_SAXParseException& exception_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_MonitorSetup_XML_Handler::warning"));

  //char* message = XMLString::transcode(exception_in.getMessage());
  //ACE_ASSERT(message);

  //ACE_DEBUG((LM_WARNING,
  //           ACE_TEXT("Common_UI_MonitorSetup_XML_Handler::warning(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
  //           ACE_TEXT(exception_in.getSystemId()),
  //           exception_in.getLineNumber(),
  //           exception_in.getColumnNumber(),
  //           ACE_TEXT(message)));

  //// clean up
  //XMLString::release(&message);
}
