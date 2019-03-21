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

#ifndef COMMON_UI_QT_APPLICATION_H
#define COMMON_UI_QT_APPLICATION_H

#include <QApplication>

#include "ace/Global_Macros.h"

#include "common_ui_qt_defines.h"
#include "common_ui_qt_iapplication.h"

template <typename StateType,
          typename CBDataType,
          ////////////////////////////////
          typename TopLevelClassType> // i.e. QMainWindow
class Comon_UI_Qt_Application_T
 : public QApplication
 , public Common_UI_Qt_IApplication_T<StateType,
                                      CBDataType>
{
  typedef QApplication inherited;

 public:
  // convenient types
  typedef Common_UI_Qt_IApplication_T<StateType,
                                      CBDataType> INTERFACE_T;
  typedef Common_IInitialize_T<CBDataType> IINITIALIZE_T;

   Comon_UI_Qt_Application_T (int,      // argc
                              char*[]); // argv
  inline virtual ~Comon_UI_Qt_Application_T () {}

  // implement Common_UI_Qt_IApplication_T
  inline virtual const StateType& getR () const { return state_; }
  virtual void dump_state () const;
  inline virtual bool initialize (const CBDataType& CBData_in) { CBData_ = &const_cast<CBDataType&> (CBData_in); CBData_->UIState = &state_; return true; }
  inline virtual const CBDataType& getR_2 () const { ACE_ASSERT (CBData_); return *CBData_; }
  virtual bool run ();

 protected:
  // convenient types
  //typedef Comon_UI_Qt_Application_T<StateType,
  //                                  CBDataType,
  //                                  TopLevelClassType> OWN_TYPE_T;

  CBDataType* CBData_;
  StateType   state_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_Qt_Application_T ())
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_Qt_Application_T (const Comon_UI_Qt_Application_T&))
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_Qt_Application_T& operator= (const Comon_UI_Qt_Application_T&))
};

// include template definition
#include "common_ui_qt_application.inl"

#endif
