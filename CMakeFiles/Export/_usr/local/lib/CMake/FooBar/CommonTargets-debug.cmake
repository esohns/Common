#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Common" for configuration "Debug"
set_property(TARGET Common APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Common PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommon.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Common )
list(APPEND _IMPORT_CHECK_FILES_FOR_Common "/usr/local/lib/libCommon.a" )

# Import target "CommonImage" for configuration "Debug"
set_property(TARGET CommonImage APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonImage PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonImage.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonImage )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonImage "/usr/local/lib/libCommonImage.a" )

# Import target "CommonMath" for configuration "Debug"
set_property(TARGET CommonMath APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonMath PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonMath.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonMath )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonMath "/usr/local/lib/libCommonMath.a" )

# Import target "CommonUI" for configuration "Debug"
set_property(TARGET CommonUI APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CommonUI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "gtkgl-3.0;epoxy;gtk-3;gdk-3;pangocairo-1.0;pango-1.0;atk-1.0;cairo-gobject;cairo;gdk_pixbuf-2.0;gio-2.0;gobject-2.0;glib-2.0"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libCommonUI.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CommonUI )
list(APPEND _IMPORT_CHECK_FILES_FOR_CommonUI "/usr/local/lib/libCommonUI.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
