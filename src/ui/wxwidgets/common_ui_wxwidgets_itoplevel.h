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

#ifndef COMMON_UI_WXWIDGETS_ITOPLEVEL_H
#define COMMON_UI_WXWIDGETS_ITOPLEVEL_H

#include "common_iget.h"

#include "common_ui_wxwidgets_iapplication.h"

template <typename StateType,
          typename ConfigurationType>
class Common_UI_wxWidgets_ITopLevel_T
 : public Common_IGetP_T<Common_UI_wxWidgets_IApplication_T<StateType,
                                                            ConfigurationType> >
{
 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<StateType,
                                             ConfigurationType> IAPPLICATION_T;

  ////////////////////////////////////////

  // callbacks
  // *NOTE*: implement in derived classes to initialize control event bindings
  virtual bool OnInit_2 (IAPPLICATION_T*) = 0;
  virtual void OnExit_2 () = 0;
};

#endif
