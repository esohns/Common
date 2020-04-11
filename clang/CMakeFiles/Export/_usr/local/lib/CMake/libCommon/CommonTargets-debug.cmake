#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Common" for configuration "Debug"
set_property(TARGET Common APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Common PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "pthread;rt;cap;CommonDBus;CommonSignal;CommonError"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommon.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Common )
list(APPEND _IMPORT_CHECK_FILES_FOR_Common "/usr/local/lib/libCommon.a" )

# Import target "CommonDBus" for configuration "Debug"
set_property(TARGET CommonDBus APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonDBus PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "systemd"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonDBus.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonDBus )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonDBus "/usr/local/lib/libCommonDBus.a" )

# Import target "CommonError" for configuration "Debug"
set_property(TARGET CommonError APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonError PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonError.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonError )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonError "/usr/local/lib/libCommonError.a" )

# Import target "CommonLog" for configuration "Debug"
set_property(TARGET CommonLog APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonLog PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonLog.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonLog )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonLog "/usr/local/lib/libCommonLog.a" )

# Import target "CommonMath" for configuration "Debug"
set_property(TARGET CommonMath APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonMath PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonMath.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonMath )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonMath "/usr/local/lib/libCommonMath.a" )

# Import target "CommonParser" for configuration "Debug"
set_property(TARGET CommonParser APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonParser PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonParser.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonParser "/usr/local/lib/libCommonParser.a" )

# Import target "CommonSignal" for configuration "Debug"
set_property(TARGET CommonSignal APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonSignal PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonSignal.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonSignal )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonSignal "/usr/local/lib/libCommonSignal.a" )

# Import target "CommonTimer" for configuration "Debug"
set_property(TARGET CommonTimer APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonTimer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonTimer.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonTimer )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonTimer "/usr/local/lib/libCommonTimer.a" )

# Import target "CommonUI" for configuration "Debug"
set_property(TARGET CommonUI APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonUI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "X11;Xrandr"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonUI.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonUI )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonUI "/usr/local/lib/libCommonUI.a" )

# Import target "CommonUI_GTK" for configuration "Debug"
set_property(TARGET CommonUI_GTK APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonUI_GTK PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "gmodule-2.0;gthread-2.0;gtk-x11-2.0;gdk-x11-2.0;pangocairo-1.0;atk-1.0;cairo;gdk_pixbuf-2.0;gio-2.0;pangoft2-1.0;pango-1.0;gobject-2.0;glib-2.0;fontconfig;freetype;/media/erik/USB_BLACK/lib/gtkglarea/gtkgl/.libs/libgtkgl-2.0.a;X11;GL"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonUI_GTK.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonUI_GTK )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonUI_GTK "/usr/local/lib/libCommonUI_GTK.a" )

# Import target "CommonXML" for configuration "Debug"
set_property(TARGET CommonXML APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonXML PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/media/erik/USB_BLACK/lib/ACE_TAO/ACE/lib/libACEXML.so;/media/erik/USB_BLACK/lib/ACE_TAO/ACE/lib/libACEXML_Parser.so"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonXML.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonXML )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonXML "/usr/local/lib/libCommonXML.a" )

# Import target "Common_Test_U" for configuration "Debug"
set_property(TARGET Common_Test_U APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Common_Test_U PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommon_Test_U.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Common_Test_U )
list(APPEND _IMPORT_CHECK_FILES_FOR_Common_Test_U "/usr/local/lib/libCommon_Test_U.a" )

# Import target "capabilities" for configuration "Debug"
set_property(TARGET capabilities APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(capabilities PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/capabilities"
  )

list(APPEND _IMPORT_CHECK_TARGETS capabilities )
list(APPEND _IMPORT_CHECK_FILES_FOR_capabilities "/usr/local/bin/capabilities" )

# Import target "capabilities_2" for configuration "Debug"
set_property(TARGET capabilities_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(capabilities_2 PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/capabilities_2"
  )

list(APPEND _IMPORT_CHECK_TARGETS capabilities_2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_capabilities_2 "/usr/local/bin/capabilities_2" )

# Import target "debug_wrapper" for configuration "Debug"
set_property(TARGET debug_wrapper APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(debug_wrapper PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/debug_wrapper"
  )

list(APPEND _IMPORT_CHECK_TARGETS debug_wrapper )
list(APPEND _IMPORT_CHECK_FILES_FOR_debug_wrapper "/usr/local/bin/debug_wrapper" )

# Import target "features" for configuration "Debug"
set_property(TARGET features APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(features PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/features"
  )

list(APPEND _IMPORT_CHECK_TARGETS features )
list(APPEND _IMPORT_CHECK_FILES_FOR_features "/usr/local/bin/features" )

# Import target "file" for configuration "Debug"
set_property(TARGET file APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(file PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/file"
  )

list(APPEND _IMPORT_CHECK_TARGETS file )
list(APPEND _IMPORT_CHECK_FILES_FOR_file "/usr/local/bin/file" )

# Import target "regex" for configuration "Debug"
set_property(TARGET regex APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(regex PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/regex"
  )

list(APPEND _IMPORT_CHECK_TARGETS regex )
list(APPEND _IMPORT_CHECK_FILES_FOR_regex "/usr/local/bin/regex" )

# Import target "capability_wrapper" for configuration "Debug"
set_property(TARGET capability_wrapper APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(capability_wrapper PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/capability_wrapper"
  )

list(APPEND _IMPORT_CHECK_TARGETS capability_wrapper )
list(APPEND _IMPORT_CHECK_FILES_FOR_capability_wrapper "/usr/local/bin/capability_wrapper" )

# Import target "ffmpeg" for configuration "Debug"
set_property(TARGET ffmpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ffmpeg PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/ffmpeg"
  )

list(APPEND _IMPORT_CHECK_TARGETS ffmpeg )
list(APPEND _IMPORT_CHECK_FILES_FOR_ffmpeg "/usr/local/bin/ffmpeg" )

# Import target "imagemagick" for configuration "Debug"
set_property(TARGET imagemagick APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(imagemagick PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/imagemagick"
  )

list(APPEND _IMPORT_CHECK_TARGETS imagemagick )
list(APPEND _IMPORT_CHECK_FILES_FOR_imagemagick "/usr/local/bin/imagemagick" )

# Import target "parser" for configuration "Debug"
set_property(TARGET parser APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(parser PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/parser"
  )

list(APPEND _IMPORT_CHECK_TARGETS parser )
list(APPEND _IMPORT_CHECK_FILES_FOR_parser "/usr/local/bin/parser" )

# Import target "parser_2" for configuration "Debug"
set_property(TARGET parser_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(parser_2 PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/parser_2"
  )

list(APPEND _IMPORT_CHECK_TARGETS parser_2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_parser_2 "/usr/local/bin/parser_2" )

# Import target "print" for configuration "Debug"
set_property(TARGET print APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(print PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/print"
  )

list(APPEND _IMPORT_CHECK_TARGETS print )
list(APPEND _IMPORT_CHECK_FILES_FOR_print "/usr/local/bin/print" )

# Import target "gtk_ui" for configuration "Debug"
set_property(TARGET gtk_ui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gtk_ui PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/gtk_ui"
  )

list(APPEND _IMPORT_CHECK_TARGETS gtk_ui )
list(APPEND _IMPORT_CHECK_FILES_FOR_gtk_ui "/usr/local/bin/gtk_ui" )

# Import target "gtk_ui_2" for configuration "Debug"
set_property(TARGET gtk_ui_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gtk_ui_2 PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/gtk_ui_2"
  )

list(APPEND _IMPORT_CHECK_TARGETS gtk_ui_2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gtk_ui_2 "/usr/local/bin/gtk_ui_2" )

# Import target "gtk_ui_3" for configuration "Debug"
set_property(TARGET gtk_ui_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gtk_ui_3 PROPERTIES
  IMPORTED_LOCATION_DEBUG "/usr/local/bin/gtk_ui_3"
  )

list(APPEND _IMPORT_CHECK_TARGETS gtk_ui_3 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gtk_ui_3 "/usr/local/bin/gtk_ui_3" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
