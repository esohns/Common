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

#ifndef COMMON_UI_GTK_BUILDER_DEFINITION_H
#define COMMON_UI_GTK_BUILDER_DEFINITION_H

#include <string>

#include "ace/Global_Macros.h"

#include "common_ui_common.h"
#include "common_ui_exports.h"
#include "common_ui_igtk.h"

class Common_UI_Export Common_UI_GtkBuilderDefinition
 : public Common_UI_IGTK_T<Common_UI_GTKState>
{
 public:
  Common_UI_GtkBuilderDefinition (int,          // argc
                                  ACE_TCHAR**); // argv
  virtual ~Common_UI_GtkBuilderDefinition ();

  // implement Common_UI_IGTK_T
  virtual bool initialize (const std::string&,   // definiton filename
                           Common_UI_GTKState&); // return value: GTK state
  virtual void finalize ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition ());
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition (const Common_UI_GtkBuilderDefinition&));
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GtkBuilderDefinition& operator= (const Common_UI_GtkBuilderDefinition&));

  int                 argc_;
  ACE_TCHAR**         argv_;
  Common_UI_GTKState* GTKState_;
};

#endif
