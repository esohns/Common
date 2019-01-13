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

#ifndef TEST_U_2_H
#define TEST_U_2_H

//#if defined (_DEBUG)
//#undef _DEBUG // *NOTE*: do not (!) #define __WXDEBUG__
//#define REDEDINE_DEBUG 1
//#endif // _DEBUG
#include <wx/wx.h>
#include <wx/image.h>
//#if defined (REDEDINE_DEBUG)
//#undef REDEDINE_DEBUG
//#define _DEBUG
//#endif // REDEDINE_DEBUG

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class wxDialog_main: public wxDialog {
public:
  // begin wxGlade: wxDialog_main::ids
  // end wxGlade

  wxDialog_main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  // begin wxGlade: wxDialog_main::methods
  void set_properties();
  void do_layout();
  // end wxGlade

protected:
  // begin wxGlade: wxDialog_main::attributes
  wxButton* button_1;
  wxButton* button_2;
  wxButton* button_3;
  // end wxGlade

  DECLARE_EVENT_TABLE();

public:
  void button_1_clicked_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_2_clicked_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_3_clicked_cb(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // TEST_U_2_H
