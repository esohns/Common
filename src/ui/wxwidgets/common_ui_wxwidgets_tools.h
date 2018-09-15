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

#ifndef COMMON_UI_WXWIDGETS_TOOLS_H
#define COMMON_UI_WXWIDGETS_TOOLS_H

#include "wx/wx.h"

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

#include "common_ui_common.h"

class Common_UI_WxWidgets_Tools
 : public Common_SInitializeFinalize_T<Common_UI_WxWidgets_Tools>
{
 public:
  static bool initialize ();
  static bool finalize ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // --- main ---
  // *IMPORTANT NOTE*: callers must delete[] the return value (if any) !
  static wxChar** convertArgV (ACE_TCHAR**); // argv
#endif // ACE_WIN32 || ACE_WIN64

  // --- logging ---
  static bool initializeLogging ();
  static void finalizeLogging ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_UI_WxWidgets_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Tools (const Common_UI_WxWidgets_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Tools& operator= (const Common_UI_WxWidgets_Tools&))
};

#endif
