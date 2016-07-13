#cmake_minimum_required (VERSION 2.8)

# *NOTE*: on Win32 systems, the (binary) gtk3 applications have linkage issues
#         with the latest (2.6.4) libglade distribution (specifically, the
#         libxml2 dll cannot be easily shared between these two packages; also,
#         the 'embedded' library names do not match ('lib'-prefixes))
#         --> disable libglade support when targeting gtk3
# *TODO*: retrieve the available gtk version number(s) from the pkg-config
#         output and pre-set this option accordingly
option (LIBGLADE_SUPPORT "compile libglade support (early versions of gtk do not support GtkBuilder)"
        OFF)

#include (CheckLibraryExists)
#check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
#find_library (HAVE_LIB_ACE libACE)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace build/linux/ace
                DOC "searching for ${ACE_LIB_FILE}"
                NO_DEFAULT_PATH)
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()

include (FindPkgConfig)
#pkg_check_modules (PKG_ACE REQUIRED ACE)
if (UNIX)
 pkg_check_modules (PKG_GTK REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0)
 if (LIBGLADE_SUPPORT)
  pkg_check_modules (PKG_GLADE REQUIRED libglade-2.0)
 endif ()
endif ()
# *TODO*: repair WIN32 pkgconfig support

if (UNIX)
 include_directories (${PKG_GTK_INCLUDE_DIRS})
 if (LIBGLADE_SUPPORT)
  include_directories (${PKG_GLADE_INCLUDE_DIRS})
 endif ()
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
elseif (WIN32)
#*TODO*: use pkg-config instead
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../ATCD/ACE)
 include_directories ($ENV{ACE_ROOT})

# gtk
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/glib-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/glib-2.0/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/gtk-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gtk-2.0/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/gdk-pixbuf-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/atk-1.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/pango-1.0)

 if (LIBGLADE_SUPPORT)
# libglade
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/include/libglade-2.0)
 endif ()
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../)

############## next target ################
set (${PROJECT_NAME}UI_LIB_SRCS
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_gtk_builder_definition.cpp
     common_ui_gtk_builder_definition.h
     common_ui_gtk_manager.cpp
     common_ui_gtk_manager.h
     common_ui_igtk.h
     common_ui_tools.cpp
     common_ui_tools.h
     common_ui_types.h
     stdafx.cpp
     stdafx.h)
set (${PROJECT_NAME}UI_LIB_GLADE_SRCS
     common_ui_glade_definition.cpp
     common_ui_glade_definition.h)

if (LIBGLADE_SUPPORT)
 add_definitions (-DLIBGLADE_SUPPORT)
 add_library (${PROJECT_NAME}UI SHARED
              ${CommonUI_LIB_SRCS} ${CommonUI_LIB_GLADE_SRCS})
else ()
 add_library (${PROJECT_NAME}UI SHARED ${CommonUI_LIB_SRCS})
endif ()

#*TODO*: use pkg-config instead
if (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME}UI Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME}UI Msvcrt.lib)
 endif ()

 target_link_libraries (${PROJECT_NAME}UI ${ACE_LIBRARY})

# gtk
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/glib-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gobject-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gthread-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gdk-win32-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gtk-win32-2.0.lib)

 if (LIBGLADE_SUPPORT)
# libglade
  target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/lib/libglade-2.0.dll.a)
 endif ()

# target_link_libraries (${PROJECT_NAME}UI ${COMMON_TARGET_NAME})
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
 set_target_properties (${PROJECT_NAME}UI PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}UI PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${PROJECT_NAME}UI PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}UI PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}UI)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_LIB_HDRS
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_gtk_builder_definition.h
     common_ui_gtk_manager.h
     common_ui_igtk.h
     common_ui_tools.h
     common_ui_types.h)
set (${PROJECT_NAME}UI_LIB_GLADE_HDRS
     common_ui_glade_definition.h)
if (LIBGLADE_SUPPORT)
install (FILES
         ${CommonUI_LIB_HDRS} ${CommonUI_LIB_GLADE_HDRS}
         DESTINATION include)
else ()
install (FILES
         ${CommonUI_LIB_HDRS}
         DESTINATION include)
endif ()
install (TARGETS ${PROJECT_NAME}UI
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
