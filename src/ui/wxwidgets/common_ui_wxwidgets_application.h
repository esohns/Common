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

#ifndef COMMON_UI_WXWIDGETS_APPLICATION_H
#define COMMON_UI_WXWIDGETS_APPLICATION_H

#include <string>

#include "wx/wx.h"

#include "ace/Global_Macros.h"

#include "common_ui_wxwidgets_iapplication.h"
#include "common_ui_wxwidgets_itoplevel.h"

template <typename DefinitionType,    // implements Common_UI_IDefinition_T
          ////////////////////////////////
          typename StateType,
          typename ConfigurationType,
          ////////////////////////////////
          typename TopLevelClassType> // i.e. wxDialog, wxWindow, etc.
class Comon_UI_WxWidgets_Application_T
 : public wxApp
 , public DefinitionType
 , public Common_UI_wxWidgets_IApplication_T<StateType,
                                             ConfigurationType>
{
  typedef wxApp inherited;
  typedef DefinitionType inherited2;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<StateType,
                                             ConfigurationType> INTERFACE_T;
  typedef Common_IInitialize_T<ConfigurationType> IINITIALIZE_T;

   Comon_UI_WxWidgets_Application_T (int,                 // argc
                                     wxChar**,            // argv
                                     const std::string&); // (top-level-) widget name
  inline virtual ~Comon_UI_WxWidgets_Application_T () {}

  virtual bool OnInit ();
  virtual int OnExit ();

  // implement Common_UI_wxWidgets_IApplication_T
  inline virtual const StateType& getR () const { return state_; }
  inline virtual bool initialize (const ConfigurationType& configuration_in) { configuration_ = &const_cast<ConfigurationType&> (configuration_in); return true; }
  inline virtual const ConfigurationType& getR_2 () const { ACE_ASSERT (configuration_); return *configuration_; }
  virtual bool run ();

 protected:
  // convenient types
  //typedef Comon_UI_WxWidgets_Application_T<DefinitionType,
  //                                         StateType,
  //                                         ConfigurationType,
  //                                         TopLevelClassType> OWN_TYPE_T;
  typedef Common_UI_wxWidgets_ITopLevel_T<StateType,
                                          ConfigurationType> ITOPLEVEL_T;

  TopLevelClassType  instance_;
  ConfigurationType* configuration_;
  StateType          state_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T ())
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T (const Comon_UI_WxWidgets_Application_T&))
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T& operator= (const Comon_UI_WxWidgets_Application_T&))

  wxInitializer      initializer_;
};

//wxDECLARE_APP (Comon_UI_WxWidgets_Application_T<DefinitionType, StateType, CallbackDataType>);
//wxIMPLEMENT_APP (Comon_UI_WxWidgets_Application_T<DefinitionType, StateType, CallbackDataType>);

// include template definition
#include "common_ui_wxwidgets_application.inl"

#endif
