#include "stdafx.h"

#undef Bool
#undef Status
#include <QtGui>

//#include "ace/Synch.h"

#include "common_ui_qt_application.h"
#include "common_ui_qt_common.h"
#include "common_ui_qt_iapplication.h"

#include "mainwindow.h"
#include "mainwindow_2.h"

typedef Common_UI_Qt_IApplication_T<struct Common_UI_Qt_State,
                                    struct Common_UI_Qt_CBData> Test_U_QtIApplication_t;
typedef MainWindow_T<MainWindow,
                     Test_U_QtIApplication_t> Test_U_MainWindow_t;
typedef Comon_UI_Qt_Application_T<struct Common_UI_Qt_State,
                                  struct Common_UI_Qt_CBData,
                                  Test_U_MainWindow_t> Test_U_QtApplication_t;

#undef main
int main(int argc, char *argv[])
{
  Test_U_QtApplication_t a (argc, argv);
  Test_U_MainWindow_t w;
  w.initialize (&a);
  w.show ();

  return a.run ();
}
