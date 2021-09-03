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

#ifndef COMMON_UI_MONITORS_SETUP_XML_HANDLER_H
#define COMMON_UI_MONITORS_SETUP_XML_HANDLER_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_xml_handler_base.h"
#include "common_xml_parser.h"

#include "common_ui_common.h"


enum Common_UI_MonitorSetup_ParserState
{
  COMMON_UI_MONITORSETUP_PARSERSTATE_LOGICALMONITOR = 0,
  ////////////////////////////////////////
  COMMON_UI_MONITORSETUP_PARSERSTATE_X,
  COMMON_UI_MONITORSETUP_PARSERSTATE_Y,
  COMMON_UI_MONITORSETUP_PARSERSTATE_PRIMARY,
  COMMON_UI_MONITORSETUP_PARSERSTATE_CONNECTOR,
  COMMON_UI_MONITORSETUP_PARSERSTATE_WIDTH,
  COMMON_UI_MONITORSETUP_PARSERSTATE_HEIGHT,
  ////////////////////////////////////////
  COMMON_UI_MONITORSETUP_PARSERSTATE_MAX,
  COMMON_UI_MONITORSETUP_PARSERSTATE_INVALID
};

struct Common_UI_MonitorSetup_ParserContext
{
  Common_UI_DisplayDevices_t              displays;
  struct Common_UI_DisplayDevice          current;
  enum Common_UI_MonitorSetup_ParserState state;
};

class Common_UI_MonitorSetup_XML_Handler
 : public Common_XML_Handler_Base_T<struct Common_UI_MonitorSetup_ParserContext>
{
  typedef Common_XML_Handler_Base_T<struct Common_UI_MonitorSetup_ParserContext> inherited;

 public:
  Common_UI_MonitorSetup_XML_Handler (struct Common_UI_MonitorSetup_ParserContext*); // parser context handle
  inline virtual ~Common_UI_MonitorSetup_XML_Handler () {}

  // implement (part of) ACEXML_ContentHandler
  virtual void characters (const ACEXML_Char*, // chars
                           size_t,             // start
                           size_t);            // length
  virtual void endElement (const ACEXML_Char*,  // namespace uri
                           const ACEXML_Char*,  // localname
                           const ACEXML_Char*); // qname
  virtual void startElement (const ACEXML_Char*,  // namespace uri
                             const ACEXML_Char*,  // localname
                             const ACEXML_Char*,  // qname
                             ACEXML_Attributes*); // attrs

  // implement ACEXML_ErrorHandler
  virtual void error (ACEXML_SAXParseException&); // exception
  virtual void fatalError (ACEXML_SAXParseException&); // exception
  virtual void warning (ACEXML_SAXParseException&); // exception

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_MonitorSetup_XML_Handler ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_MonitorSetup_XML_Handler (const Common_UI_MonitorSetup_XML_Handler&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_MonitorSetup_XML_Handler& operator= (const Common_UI_MonitorSetup_XML_Handler&))

  struct Common_UI_MonitorSetup_ParserContext* parserContextHandle_;
};

typedef Common_XML_Parser_T<ACE_MT_SYNCH,
                            struct Common_XML_ParserConfiguration,
                            struct Common_UI_MonitorSetup_ParserContext,
                            Common_UI_MonitorSetup_XML_Handler> Common_UI_MonitorSetup_Parser_t;

#endif
