//#if defined (_DEBUG)
//#undef _DEBUG // *NOTE*: do not (!) #define __WXDEBUG__
//#define REDEDINE_DEBUG 1
//#endif // _DEBUG
#include "wx/wx.h"
//#if defined (REDEDINE_DEBUG)
//#undef REDEDINE_DEBUG
//#define _DEBUG
//#endif // REDEDINE_DEBUG

class Simple
 : public wxFrame
{
 public:
  Simple (const wxString& title)
   : wxFrame (NULL, wxID_ANY, title, wxDefaultPosition, wxSize (250, 250))
  {
    Centre ();
  }
};

class MyApp
 : public wxApp
{
 public:
  bool OnInit ()
  {
#if defined (_DEBUG)
    wxLog::SetLogLevel (wxLOG_Max);
#endif // _DEBUG
    Simple *simple = new Simple (wxT ("Simple"));
    simple->Show (true);
    return true;
  }
};

#if wxCHECK_VERSION(3,0,0)
wxIMPLEMENT_APP (MyApp);
#elif wxCHECK_VERSION(2,0,0)
IMPLEMENT_APP (MyApp);

void wxApp::OnAssertFailure (const wxChar *file,
                             int line,
                             const wxChar *func,
                             const wxChar *cond,
                             const wxChar *msg) {};
void wxAppConsole::OnAssert (const wxChar *file,
                             int line,
                             const wxChar *cond,
                             const wxChar *msg) {};
#endif // wxCHECK_VERSION
