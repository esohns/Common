#ifndef TEST_I_3_H
#define TEST_I_3_H

#include "wx/apptrait.h"

#include "common_ui_common.h"

#include "common_ui_wxwidgets_application.h"
#include "common_ui_wxwidgets_xrc_definition.h"

#include "test_i_3_ui.h"
#include "test_i_3_ui_base.h"

extern const char toplevel_widget_classname_string_[];

//////////////////////////////////////////

typedef Common_UI_WxWidgetsXRCDefinition_T<struct Common_UI_wxWidgets_State,
                                           toplevel_widget_classname_string_> Test_I_WxWidgetsXRCDefinition_t;
typedef Common_UI_wxWidgets_IApplication_T<struct Common_UI_wxWidgets_State,
                                           struct Common_UI_wxWidgets_CBData> Test_I_WxWidgetsIApplication_t;
typedef Comon_UI_WxWidgets_Application_T<Test_I_WxWidgetsXRCDefinition_t,
                                         struct Common_UI_wxWidgets_State,
                                         struct Common_UI_wxWidgets_CBData,
                                         Test_I_WxWidgetsDialog,
                                         wxGUIAppTraits> Test_I_WxWidgetsApplication_t;

#endif // TEST_I_3_H
