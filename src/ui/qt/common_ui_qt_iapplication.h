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

#ifndef COMMON_UI_QT_IAPPLICATION_T_H
#define COMMON_UI_QT_IAPPLICATION_T_H

#include "common_idumpstate.h"
#include "common_iget.h"
#include "common_iinitialize.h"

class Common_UI_Qt_IApplicationBase
{
 public:
  virtual bool run () = 0;
//  virtual void wait () = 0;
};

template <typename StateType>
class Common_UI_Qt_IApplicationBase_T
 : public Common_UI_Qt_IApplicationBase
 , public Common_IGetR_T<StateType>
 , public Common_IDumpState
{
 public:
  // convenient types
  typedef StateType STATE_T;
};

template <typename StateType,
          ////////////////////////////////
          typename CBDataType>
class Common_UI_Qt_IApplication_T
 : public Common_UI_Qt_IApplicationBase_T<StateType>
 , public Common_IInitialize_T<CBDataType>
 , public Common_IGetR_2_T<CBDataType>
{
 public:
  // convenient types
  typedef CBDataType CBDATA_T;
};

#endif
