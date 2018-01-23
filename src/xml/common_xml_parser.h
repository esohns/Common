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

#ifndef COMMON_XML_PARSER_H
#define COMMON_XML_PARSER_H

#include <string>

#include "ace/Global_Macros.h"
//#include "ace/Singleton.h"
//#include "ace/Synch_Traits.h"

#include "ACEXML/parser/parser/Parser.h"

#include "common_idumpstate.h"
#include "common_iinitialize.h"

// forward declarations
class ACEXML_InputSource;

template <ACE_SYNCH_DECL,
          typename ConfigurationType,
          typename ContextType,
          typename HandlerType>
class Common_XML_Parser_T
 : public ACEXML_Parser
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IDumpState
{
  //// singleton needs access to the ctor/dtors
  //friend class ACE_Singleton<Common_XML_Parser_T<ACE_SYNCH_USE,
  //                                               ConfigurationType>,
  //                           ACE_SYNCH_MUTEX>;
  typedef ACEXML_Parser inherited;

 public:
  //// convenient types
  //typedef ACE_Singleton<Common_XML_Parser_T<ACE_SYNCH_USE,
  //                                          ConfigurationType>,
  //                      ACE_SYNCH_MUTEX> SINGLETON_T;
  Common_XML_Parser_T (ContextType*); // context handle
  inline virtual ~Common_XML_Parser_T () {}

  virtual void parseFile (const std::string&); // FQ path
  virtual void parseString (const std::string&); // XML content

  // implement Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&);

  // implement Common_IDumpState
  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Parser_T ());
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Parser_T (const Common_XML_Parser_T&));
  ACE_UNIMPLEMENTED_FUNC (Common_XML_Parser_T& operator= (const Common_XML_Parser_T&));

  // helper methods
  void parse (ACEXML_InputSource*); // input source handle

  HandlerType handler_;
};

// include template definition
#include "common_xml_parser.inl"

#endif
