#ifndef TEST_U_IMAGESCREEN_UI_H
#define TEST_U_IMAGESCREEN_UI_H

#include "wx/wx.h"

#include "ace/config-macros.h"

#include "common_ui_wxwidgets_itoplevel.h"

template <typename WidgetBaseClassType, // implements wxWindow (e.g. wxDialog)
          typename InterfaceType>       // implements Common_UI_wxWidgets_IApplication_T
class Test_U_WxWidgetsDialog_T
 : public WidgetBaseClassType
 , public Common_UI_wxWidgets_ITopLevel_T<typename InterfaceType::STATE_T,
                                          typename InterfaceType::CBDATA_T>
{
  typedef WidgetBaseClassType inherited;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<typename InterfaceType::STATE_T,
                                             typename InterfaceType::CBDATA_T> IAPPLICATION_T;

  Test_U_WxWidgetsDialog_T (wxWindow* = NULL); // parent window (if any)
  inline virtual ~Test_U_WxWidgetsDialog_T () {}

  // implement Common_UI_wxWidgets_ITopLevel
  inline virtual const IAPPLICATION_T* const getP () const { ACE_ASSERT (application_); return application_; }

 private:
  // convenient types
  typedef Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                                   InterfaceType> OWN_TYPE_T;

  // implement Common_UI_wxWidgets_ITopLevel
  virtual bool OnInit_2 (IAPPLICATION_T*);
  virtual void OnExit_2 ();

  // event handlers
  virtual void dialog_main_idle_cb (wxIdleEvent&);
  virtual void dialog_main_keydown_cb (wxKeyEvent&);

  // control handlers
  void button_1_clicked_cb(wxCommandEvent&);
  void button_2_clicked_cb(wxCommandEvent&);
  void button_3_clicked_cb(wxCommandEvent&);
  void button_about_clicked_cb (wxCommandEvent&);
  void button_quit_clicked_cb (wxCommandEvent&);

//  wxDECLARE_DYNAMIC_CLASS (OWN_TYPE_T);
//  wxDECLARE_EVENT_TABLE ();

  InterfaceType* application_;
  bool           initializing_;
  bool           untoggling_;
};

// include template definition
#include "test_u_3_ui.inl"

#endif // __test_u_imagescreen_ui__
