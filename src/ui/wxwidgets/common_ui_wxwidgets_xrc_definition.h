/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef COMMON_UI_WXWIDGETS_XRC_DEFINITION_T_H
#define COMMON_UI_WXWIDGETS_XRC_DEFINITION_T_H

#include <string>

//#if defined (_DEBUG)
//#undef _DEBUG // *NOTE*: do not (!) #define __WXDEBUG__
//#define REDEDINE_DEBUG 1
//#endif // _DEBUG
#include "wx/wx.h"
//#if defined (REDEDINE_DEBUG)
//#undef REDEDINE_DEBUG
//#define _DEBUG
//#endif // REDEDINE_DEBUG

#include "ace/Global_Macros.h"

#include "common_ui_idefinition.h"

template <typename StateType,
          const char* TopLevelClassName> // *TODO*: use a variadic character array
class Common_UI_WxWidgetsXRCDefinition_T
 : public Common_UI_IDefinition_T<StateType>
{
 public:
  Common_UI_WxWidgetsXRCDefinition_T (const std::string&); // (top-level-) widget name
  inline virtual ~Common_UI_WxWidgetsXRCDefinition_T () {}

  // implement Common_UI_IDefinition_T
  virtual bool initialize (StateType&); // return value: wxwidgets XRC state
  virtual void finalize ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgetsXRCDefinition_T ());
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgetsXRCDefinition_T (const Common_UI_WxWidgetsXRCDefinition_T&));
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgetsXRCDefinition_T& operator= (const Common_UI_WxWidgetsXRCDefinition_T&));

  std::string name_; // 'top-level' widget-
  StateType*  state_;
};

// include template definition
#include "common_ui_wxwidgets_xrc_definition.inl"

#endif
