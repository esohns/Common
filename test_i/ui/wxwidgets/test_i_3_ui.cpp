#include "stdafx.h"

#include "test_i_3_ui.h"

#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_ui_wxwidgets_tools.h"

wxIMPLEMENT_DYNAMIC_CLASS (Test_I_WxWidgetsDialog, dialog_main_base)

Test_I_WxWidgetsDialog::Test_I_WxWidgetsDialog ()
 : inherited ()
 , application_ (NULL)
 , initializing_ (true)
 , untoggling_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::Test_I_WxWidgetsDialog"));

#if defined (ACE_WIN64) || defined (ACE_WIN32)
  inherited::MSWSetOldWndProc (NULL);
#endif // ACE_WIN64 || ACE_WIN32
  inherited::SetExtraStyle (inherited::GetExtraStyle () | wxWS_EX_PROCESS_IDLE);
}

bool
Test_I_WxWidgetsDialog::OnInit_2 (IAPPLICATION_T* iapplication_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::OnInit_2"));

  // sanity check(s)
  ACE_ASSERT (!application_);

  application_ = iapplication_in;
  ACE_ASSERT (application_);

         // int id_i = wxXmlResource::DoGetXRCID (ACE_TEXT_ALWAYS_CHAR ("wxID_NEW"));
         // wxWindow* window_2 = wxWindow::FindWindowById (id_i);
         // // wxWindow* window_p = base_p->FindWindow (id_i);
         // inherited::button_1 = static_cast<wxButton*> (window_2);

         // id_i = wxXmlResource::DoGetXRCID (ACE_TEXT_ALWAYS_CHAR ("wxID_COPY"));
         // window_2 = wxWindow::FindWindowById (id_i);
         // inherited::button_2 = static_cast<wxButton*> (window_2);

         // id_i = wxXmlResource::DoGetXRCID (ACE_TEXT_ALWAYS_CHAR ("wxID_CLEAR"));
         // window_2 = wxWindow::FindWindowById (id_i);
         // inherited::button_3 = static_cast<wxButton*> (window_2);

  dialog_main_base* base_p = this;
  inherited::button_1 = XRCCTRL (*base_p, ACE_TEXT_ALWAYS_CHAR ("wxID_NEW"), wxButton);
  inherited::button_2 = XRCCTRL (*base_p, ACE_TEXT_ALWAYS_CHAR ("wxID_COPY"), wxButton);
  inherited::button_3 = XRCCTRL (*base_p, ACE_TEXT_ALWAYS_CHAR ("wxID_CLEAR"), wxButton);

  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Test_I_WxWidgetsDialog::button_1_clicked_cb, this, wxID_NEW);
  // this->Connect (XRCID(ACE_TEXT_ALWAYS_CHAR ("wxID_NEW")), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&Test_I_WxWidgetsDialog::button_1_clicked_cb);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Test_I_WxWidgetsDialog::button_2_clicked_cb, this, wxID_COPY);
  // this->Connect (XRCID(ACE_TEXT_ALWAYS_CHAR ("wxID_COPY")), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&Test_I_WxWidgetsDialog::button_2_clicked_cb);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Test_I_WxWidgetsDialog::button_3_clicked_cb, this, wxID_CLEAR);
  // this->Connect (XRCID(ACE_TEXT_ALWAYS_CHAR ("wxID_CLEAR")), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&Test_I_WxWidgetsDialog::button_3_clicked_cb);

  this->Connect (wxEVT_CLOSE_WINDOW, wxCloseEventHandler (Test_I_WxWidgetsDialog::on_close_cb), NULL, this);

  // populate controls

  return true;
}

void
Test_I_WxWidgetsDialog::OnExit_2 ()
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::OnExit_2"));

  Destroy ();
}

//////////////////////////////////////////

void
Test_I_WxWidgetsDialog::dialog_main_idle_cb (wxIdleEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::dialog_main_idle_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  typename IAPPLICATION_T::CALLBACKDATA_T& CBData_r =
    const_cast<typename IAPPLICATION_T::CALLBACKDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);

  //  inherited::gauge_progress->Pulse ();
}

void
Test_I_WxWidgetsDialog::dialog_main_keydown_cb (wxKeyEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::dialog_main_keydown_cb"));

         // sanity check(s)
  ACE_ASSERT (application_);

  typename IAPPLICATION_T::CALLBACKDATA_T& CBData_r =
    const_cast<typename IAPPLICATION_T::CALLBACKDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);
}

//////////////////////////////////////////

void
Test_I_WxWidgetsDialog::button_1_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::button_1_clicked_cb"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_1, clicked\n")));
}

void
Test_I_WxWidgetsDialog::button_2_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::button_2_clicked_cb"));

         // sanity check(s)
  ACE_ASSERT (application_);

  typename IAPPLICATION_T::CALLBACKDATA_T& CBData_r =
    const_cast<typename IAPPLICATION_T::CALLBACKDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_2, clicked\n")));
}

void
Test_I_WxWidgetsDialog::button_3_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::button_3_clicked_cb"));

         // sanity check(s)
  ACE_ASSERT (application_);

  typename IAPPLICATION_T::CALLBACKDATA_T& CBData_r =
    const_cast<typename IAPPLICATION_T::CALLBACKDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_3, clicked\n")));
}

void
Test_I_WxWidgetsDialog::button_about_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::button_about_clicked_cb"));

  wxAboutDialogInfo about_dialog_info;
  about_dialog_info.SetName (_ ("My Program"));
  about_dialog_info.SetVersion (_ ("1.2.3 Beta"));
  about_dialog_info.SetDescription (_ ("This program does something great."));
  about_dialog_info.SetCopyright (wxT ("(C) 2007 Me <my@email.addre.ss>"));
  wxAboutBox (about_dialog_info);
}

void
Test_I_WxWidgetsDialog::button_quit_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::button_quit_clicked_cb"));

  bool application_vetoed_b = !this->Close (true); // force ?
  ACE_ASSERT (!application_vetoed_b);
}

void
Test_I_WxWidgetsDialog::on_close_cb (wxCloseEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_I_WxWidgetsDialog::on_close_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  application_->stop ();
}
