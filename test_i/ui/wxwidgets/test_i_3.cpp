// -*- C++ -*-
//
// generated by wxGlade
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include "stdafx.h"

#include "wx/wx.h"

#include "common_ui_wxwidgets_defines.h"
#include "common_ui_wxwidgets_tools.h"

#include "test_i_3.h"

extern const char toplevel_widget_classname_string_[] =
    ACE_TEXT_ALWAYS_CHAR ("wxID_DIALOG_MAIN");

int
main (int argc, char** argv)
{
  Test_I_WxWidgetsApplication_t* application_p = NULL;
  ACE_NEW_NORETURN (application_p,
                    Test_I_WxWidgetsApplication_t (toplevel_widget_classname_string_,
                                                   argc,
                                                   Common_UI_WxWidgets_Tools::convertArgV (argc,
                                                                                           argv),
                                                   COMMON_UI_WXWIDGETS_APP_DEFAULT_CMDLINE_PARSE));
  ACE_ASSERT (application_p);
  struct Common_UI_wxWidgets_CBData ui_cb_data;
  application_p->initialize (ui_cb_data);
  struct Common_UI_wxWidgets_State& state_r =
    const_cast<struct Common_UI_wxWidgets_State&> (application_p->getR ());
  state_r.resources[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (ACE_TEXT_ALWAYS_CHAR ("test_i_3.xrc"), static_cast<wxObject*> (NULL));

  application_p->run ();

  return 0;
}

#if wxCHECK_VERSION (3,0,0)
//wxIMPLEMENT_APP (Test_I_WxWidgetsApplication_t);
#elif wxCHECK_VERSION (2,0,0)
//IMPLEMENT_APP(Test_I_WxWidgetsApplication_t)

void wxApp::OnAssertFailure (const wxChar *file,
                             int line,
                             const wxChar *func,
                             const wxChar *cond,
                             const wxChar *msg) {}
void wxAppConsole::OnAssert (const wxChar *file,
                             int line,
                             const wxChar *cond,
                             const wxChar *msg) {}
#endif // wxCHECK_VERSION
