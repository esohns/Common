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

#ifndef COMMON_UI_WXWIDGETS_IAPPLICATION_T_H
#define COMMON_UI_WXWIDGETS_IAPPLICATION_T_H

#include "common_iget.h"
#include "common_iinitialize.h"

template <typename StateType>
class Common_UI_wxWidgets_IApplicationBase_T
 : public Common_IGetR_T<StateType>
{
 public:
  // convenient types
  typedef StateType STATE_T;

  virtual bool run () = 0;
};

template <typename StateType,
          ////////////////////////////////
          typename ConfigurationType>
class Common_UI_wxWidgets_IApplication_T
 : public Common_UI_wxWidgets_IApplicationBase_T<StateType>
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IGetR_2_T<ConfigurationType>
{
 public:
  // convenient types
  typedef ConfigurationType CONFIGURATION_T;
};

#endif
