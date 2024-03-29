#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_ui_wxwidgets_tools.h"

template <typename WidgetBaseClassType,
          typename InterfaceType>
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::Test_U_WxWidgetsDialog_T (wxWindow* parent_in)
 : inherited (parent_in, wxID_ANY, wxEmptyString)
 , application_ (NULL)
 , initializing_ (true)
 , untoggling_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::Test_U_WxWidgetsDialog_T"));

#if defined (ACE_WIN64) || defined (ACE_WIN32)
  inherited::MSWSetOldWndProc (NULL);
#endif // ACE_WIN64 || ACE_WIN32
  inherited::SetExtraStyle (wxWS_EX_PROCESS_IDLE);
}

template <typename WidgetBaseClassType,
          typename InterfaceType>
bool
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::OnInit_2 (IAPPLICATION_T* iapplication_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::OnInit_2"));

  // sanity check(s)
  ACE_ASSERT (!application_);

  application_ = static_cast<InterfaceType*> (iapplication_in);
  ACE_ASSERT (application_);

  WidgetBaseClassType* base_p = this;
  //int id_i = wxXmlResource::DoGetXRCID ("wxID_NEW");

  //wxWindow* window_2 = wxWindow::FindWindowById (id_i);
  //wxWindow* window_p = base_p->FindWindow (id_i);
  //inherited::button_1 =
  //  static_cast<wxButton*> (window_p);
  inherited::button_1 = XRCCTRL (*base_p, "wxID_NEW", wxButton);
  inherited::button_2 = XRCCTRL (*base_p, "wxID_COPY", wxButton);
  inherited::button_3 = XRCCTRL (*base_p, "wxID_CLEAR", wxButton);

  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &OWN_TYPE_T::button_1_clicked_cb, this, wxID_NEW);
//  Connect (XRCID("button_1"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&wxDialog_main::button_1_clicked_cb);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &OWN_TYPE_T::button_2_clicked_cb, this, wxID_COPY);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &OWN_TYPE_T::button_3_clicked_cb, this, wxID_CLEAR);
  this->Connect (wxEVT_CLOSE_WINDOW, wxCloseEventHandler (OWN_TYPE_T::on_close_cb), NULL, this);

  // populate controls

  return true;
}
template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::OnExit_2 ()
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::OnExit_2"));

  // sanity check(s)
  ACE_ASSERT (application_);
}

//////////////////////////////////////////

template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::dialog_main_idle_cb (wxIdleEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::dialog_main_idle_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  typename InterfaceType::CBDATA_T& CBData_r =
    const_cast<typename InterfaceType::CBDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);

//  inherited::gauge_progress->Pulse ();
}

template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::dialog_main_keydown_cb (wxKeyEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::dialog_main_keydown_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  typename InterfaceType::CBDATA_T& CBData_r =
    const_cast<typename InterfaceType::CBDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);
}

//////////////////////////////////////////

template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::button_1_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_1_clicked_cb"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_1, clicked\n")));
}
template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::button_2_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_2_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  typename InterfaceType::CBDATA_T& CBData_r =
    const_cast<typename InterfaceType::CBDATA_T&> (application_->getR_2 ());
  ACE_UNUSED_ARG (CBData_r);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_2, clicked\n")));
}
template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::button_3_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_3_clicked_cb"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("button_3, clicked\n")));

  this->button_quit_clicked_cb (event_in);
}

//template <typename WidgetBaseClassType,
//          typename InterfaceType>
//void
//Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
//                         InterfaceType>::button_report_clicked_cb (wxCommandEvent& event_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_report_clicked_cb"));

//}
//template <typename WidgetBaseClassType,
//          typename InterfaceType>
//void
//Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
//                         InterfaceType>::choice_source_changed_cb (wxCommandEvent& event_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::choice_source_changed_cb"));

//  // sanity check(s)
//  ACE_ASSERT (application_);

//  typename InterfaceType::CBDATA_T& CBData_r =
//    const_cast<typename InterfaceType::CBDATA_T&> (application_->getR_2 ());
//  ACE_ASSERT (CBData_r.configuration);
//}
template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::button_about_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_about_clicked_cb"));

  wxAboutDialogInfo about_dialog_info;
  about_dialog_info.SetName (_ ("My Program"));
  about_dialog_info.SetVersion (_ ("1.2.3 Beta"));
  about_dialog_info.SetDescription (_ ("This program does something great."));
  about_dialog_info.SetCopyright (wxT ("(C) 2007 Me <my@email.addre.ss>"));
  wxAboutBox (about_dialog_info);
}
template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::button_quit_clicked_cb (wxCommandEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::button_quit_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  // step1: make sure the stream has stopped
  // step2: close main window
  this->Close (true); // force ?

  // step3: initiate shutdown sequence
  int result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));
}

template <typename WidgetBaseClassType,
          typename InterfaceType>
void
Test_U_WxWidgetsDialog_T<WidgetBaseClassType,
                         InterfaceType>::on_close_cb (wxCloseEvent& event_in)
{
  COMMON_TRACE (ACE_TEXT ("Test_U_WxWidgetsDialog_T::on_close_cb"));

  event_in.Skip ();

  wxCommandEvent event_s (wxEVT_BUTTON, XRCID ("wxID_CLEAR"));

  // Add any data; sometimes the only information needed at the destination is the arrival of the event itself
  //event.SetString ("This is the data");

  // Then post the event
  wxPostEvent (this, event_s);
  //this->Destroy ();
}
