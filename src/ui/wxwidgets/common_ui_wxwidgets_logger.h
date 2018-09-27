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

#ifndef COMMON_UI_WXWIDGETS_LOGGER_T_H
#define COMMON_UI_WXWIDGETS_LOGGER_T_H

#include "wx/log.h"

#include "ace/Global_Macros.h"

class Common_UI_WxWidgets_Logger
 : public wxLog
{
 public:
  inline Common_UI_WxWidgets_Logger () {}
  virtual ~Common_UI_WxWidgets_Logger ();

  // override wxLog interface
  virtual void DoLogTextAtLevel (wxLogLevel,
                                 const wxString&);

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Logger (const Common_UI_WxWidgets_Logger&))
  ACE_UNIMPLEMENTED_FUNC (Common_UI_WxWidgets_Logger& operator= (const Common_UI_WxWidgets_Logger&))
};

#endif
