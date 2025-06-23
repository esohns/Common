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

//#if defined (_DEBUG)
//#undef _DEBUG // *NOTE*: do not (!) #define __WXDEBUG__
//#define REDEDINE_DEBUG 1
//#endif // _DEBUG
#undef DrawText
#undef SIZEOF_SIZE_T
#include "wx/wx.h"
//#if defined (REDEDINE_DEBUG)
//#undef REDEDINE_DEBUG
//#define _DEBUG
//#endif // REDEDINE_DEBUG

#include <string>

#include "ace/Global_Macros.h"

#include "common_ui_wxwidgets_defines.h"
#include "common_ui_wxwidgets_iapplication.h"
#include "common_ui_wxwidgets_itoplevel.h"

template <typename DefinitionType,    // implements Common_UI_IDefinition_T
          ////////////////////////////////
          typename StateType,
          typename CBDataType,
          ////////////////////////////////
          typename TopLevelClassType, // i.e. wxDialog, wxWindow, etc.
          typename TraitsType>        // *NOTE*: needs to inherit wxGUIAppTraits
class Comon_UI_WxWidgets_Application_T
 : public wxApp
 , public Common_UI_wxWidgets_IApplication_T<StateType,
                                             CBDataType>
{
  typedef wxApp inherited;

 public:
  // convenient types
  //typedef Comon_UI_WxWidgets_Application_T<DefinitionType
  //                                         StateType,
  //                                         ConfigurationType,
  //                                         TopLevelClassType,
  //                                         TraitsType> OWN_TYPE_T;
  typedef Common_UI_wxWidgets_IApplication_T<StateType,
                                             CBDataType> INTERFACE_T;
  typedef Common_IInitialize_T<CBDataType> IINITIALIZE_T;

   Comon_UI_WxWidgets_Application_T (const std::string&, // (top-level-) widget name
                                     int,                // argc
                                     wxChar**,           // argv
                                     bool = COMMON_UI_WXWIDGETS_APP_DEFAULT_CMDLINE_PARSE);
  inline virtual ~Comon_UI_WxWidgets_Application_T () {}

  // override (part of) wxAppConsole
  //inline virtual wxAppTraits* CreateTraits () { wxAppTraits* result_p = NULL; ACE_NEW_NORETURN (result_p, TraitsType ()); return result_p; }
  virtual bool OnInit ();
  virtual void OnEndSession (wxCloseEvent&);
  virtual void OnQueryEndSession (wxCloseEvent&);
  virtual int OnExit ();

  //wxDECLARE_APP(OWN_TYPE_T);

  virtual int FilterEvent (wxEvent&);

  // implement Common_UI_wxWidgets_IApplication_T
  inline virtual const StateType& getR () const { return state_; }
  virtual void dump_state () const;
  inline virtual bool initialize (const CBDataType& CBData_in) { CBData_ = &const_cast<CBDataType&> (CBData_in); CBData_->UIState = &state_; return true; }
  inline virtual const CBDataType& getR_2 () const { ACE_ASSERT (CBData_); return *CBData_; }
  virtual bool run ();
  virtual void stop ();
#if wxCHECK_VERSION(3,0,0)
  inline virtual void wait () { while (inherited::HasPendingEvents ()) inherited::ProcessPendingEvents(); }
#elif wxCHECK_VERSION(2,0,0)
  inline virtual void wait () { inherited::ProcessPendingEvents(); }
#endif // wxCHECK_VERSION

 protected:
  // convenient types
  typedef Common_UI_wxWidgets_ITopLevel_T<StateType,
                                          CBDataType> ITOPLEVEL_T;

  CBDataType* CBData_;
  StateType   state_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T ())
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T (const Comon_UI_WxWidgets_Application_T&))
  ACE_UNIMPLEMENTED_FUNC (Comon_UI_WxWidgets_Application_T& operator= (const Comon_UI_WxWidgets_Application_T&))

  // override (part of) wxAppConsole
  inline virtual void OnInitCmdLine (wxCmdLineParser& parser_in) {}
  inline virtual bool OnCmdLineError (wxCmdLineParser& parser_in) { return true; }

  DefinitionType     definition_;
  //wxInitializer      initializer_;
  bool               parseCommandLine_;
};

//wxDECLARE_APP (Comon_UI_WxWidgets_Application_T<DefinitionType, StateType, CallbackDataType>);
//wxIMPLEMENT_APP (Comon_UI_WxWidgets_Application_T<DefinitionType, StateType, CallbackDataType>);

// include template definition
#include "common_ui_wxwidgets_application.inl"

#endif
