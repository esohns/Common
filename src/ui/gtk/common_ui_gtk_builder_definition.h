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

#ifndef COMMON_UI_GTK_BUILDER_DEFINITION_T_H
#define COMMON_UI_GTK_BUILDER_DEFINITION_T_H

#include "ace/Global_Macros.h"

#include "common_ui_idefinition.h"

template <typename StateType>
class Common_UI_GtkBuilderDefinition_T
 : public Common_UI_IDefinition_T<StateType>
{
 public:
  // convenient types
  typedef Common_UI_IDefinition_T<StateType> INTERFACE_T;

  Common_UI_GtkBuilderDefinition_T ();
  virtual ~Common_UI_GtkBuilderDefinition_T ();

  // implement Common_UI_IDefinition_T
  virtual bool initialize (StateType&); // return value: UI state
  virtual void finalize (bool = true);  // clear pending event source(s) ?

 private:
//  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition_T (const Common_UI_GtkBuilderDefinition_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition_T& operator= (const Common_UI_GtkBuilderDefinition_T&))

  StateType* state_;
};

// include template definition
#include "common_ui_gtk_builder_definition.inl"

#endif
