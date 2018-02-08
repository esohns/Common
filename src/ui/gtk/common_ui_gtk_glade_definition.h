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

#ifndef COMMON_UI_GTK_GLADE_DEFINITION_H
#define COMMON_UI_GTK_GLADE_DEFINITION_H

#include "ace/Global_Macros.h"

//#include "common_ui_common.h"
//#include "common_ui_exports.h"

#include "common_ui_gtk_common.h"
#include "common_ui_igtk.h"

//class Common_UI_Export Common_UI_GladeDefinition
class Common_UI_GladeDefinition
 : public Common_UI_IGTK_T<struct Common_UI_GTK_State>
{
 public:
  Common_UI_GladeDefinition (int,          // argc
                             ACE_TCHAR**); // argv
  virtual ~Common_UI_GladeDefinition ();

  // implement Common_UI_IGTK_T
  virtual bool initialize (struct Common_UI_GTK_State&); // return value: GTK state
  virtual void finalize ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GladeDefinition ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GladeDefinition (const Common_UI_GladeDefinition&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_GladeDefinition& operator= (const Common_UI_GladeDefinition&))

  int                         argc_;
  ACE_TCHAR**                 argv_;
  struct Common_UI_GTK_State* state_;
};

#endif
