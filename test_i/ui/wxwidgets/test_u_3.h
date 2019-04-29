#ifndef TEST_U_3_H
#define TEST_U_3_H

#include "ace/os_include/sys/os_types.h"

#include "wx/apptrait.h"

#include "common_ui_common.h"

#include "common_ui_wxwidgets_application.h"
#include "common_ui_wxwidgets_xrc_definition.h"

#include "test_u_3_ui.h"
#include "test_u_3_ui_base.h"

extern const char toplevel_widget_classname_string_[];

//////////////////////////////////////////

typedef Common_UI_WxWidgetsXRCDefinition_T<struct Common_UI_wxWidgets_State,
                                           toplevel_widget_classname_string_> Test_U_WxWidgetsXRCDefinition_t;
typedef Common_UI_wxWidgets_IApplication_T<struct Common_UI_wxWidgets_State,
                                           struct Common_UI_wxWidgets_CBData> Test_U_WxWidgetsIApplication_t;
typedef Test_U_WxWidgetsDialog_T<dialog_main_base,
                                 Test_U_WxWidgetsIApplication_t> Test_U_WxWidgetsDialog_t;
typedef Comon_UI_WxWidgets_Application_T<Test_U_WxWidgetsXRCDefinition_t,
                                         struct Common_UI_wxWidgets_State,
                                         struct Common_UI_wxWidgets_CBData,
                                         Test_U_WxWidgetsDialog_t,
                                         wxGUIAppTraits> Test_U_WxWidgetsApplication_t;

#endif // TEST_U_3_H
