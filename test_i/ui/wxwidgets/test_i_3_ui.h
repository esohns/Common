#ifndef TEST_I_3_UI_H
#define TEST_I_3_UI_H

#undef DrawText
#undef SIZEOF_SIZE_T
#include "wx/wx.h"

#include "common_ui_wxwidgets_itoplevel.h"

#include "test_i_3_ui_base.h"

class Test_I_WxWidgetsDialog
 : public dialog_main_base
 , public Common_UI_wxWidgets_ITopLevel_T<struct Common_UI_wxWidgets_State,
                                          struct Common_UI_wxWidgets_CBData>
{
  typedef dialog_main_base inherited;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_ITopLevel_T<struct Common_UI_wxWidgets_State,
                                          struct Common_UI_wxWidgets_CBData> ITOPLEVEL_T;
  typedef Common_UI_wxWidgets_IApplication_T<struct Common_UI_wxWidgets_State,
                                             struct Common_UI_wxWidgets_CBData> IAPPLICATION_T;

  wxDECLARE_DYNAMIC_CLASS	(Test_I_WxWidgetsDialog);

  Test_I_WxWidgetsDialog ();
  inline virtual ~Test_I_WxWidgetsDialog () {}

  // implement Common_UI_wxWidgets_ITopLevel
  inline virtual const IAPPLICATION_T* const getP () const { ACE_ASSERT (application_); return application_; }

//  virtual void GTKHandleRealized () { ACE_ASSERT (false); }

 private:
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
  void on_close_cb (wxCloseEvent&);

  IAPPLICATION_T* application_;
  bool            initializing_;
  bool            untoggling_;
};

#endif
