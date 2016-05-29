#cmake_minimum_required (VERSION 2.8)

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
 pkg_check_modules (PKG_GTK_GLADE REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0 libglade-2.0)
endif ()
# *TODO*: repair WIN32 pkgconfig support

if (UNIX)
 include_directories (${PKG_GTK_GLADE_INCLUDE_DIRS})
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
elseif (WIN32)
#*TODO*: use pkg-config instead
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../ATCD/ACE)
 include_directories ($ENV{ACE_ROOT})

# gtk
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/glib-2.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/lib/glib-2.0/include)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/gtk-2.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/lib/gtk-2.0/include)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/gdk-pixbuf-2.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/atk-1.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/cairo)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk2/include/pango-1.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/glib-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/glib-2.0/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/gtk-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gtk-2.0/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/gdk-pixbuf-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/atk-1.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/include/pango-1.0)
# libglade
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade/include/libglade-2.0)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../)

############## next target ################
set (${PROJECT_NAME}UI_LIB_SRCS
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_glade_definition.cpp
     common_ui_glade_definition.h
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

add_library (${PROJECT_NAME}UI SHARED ${CommonUI_LIB_SRCS})

#*TODO*: use pkg-config instead
if (WIN32)
 target_link_libraries (${PROJECT_NAME}UI ${ACE_LIBRARY})

# gtk
# target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/lib/glib-2.0.lib)
# target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/lib/gobject-2.0.lib)
# target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/lib/gthread-2.0.lib)
# target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/lib/gdk-win32-2.0.lib)
# target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/lib/gtk-win32-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/glib-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gobject-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gthread-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gdk-win32-2.0.lib)
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk3/lib/gtk-win32-2.0.lib)
# libglade
 target_link_libraries (${PROJECT_NAME}UI ${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/glade/.libs/libglade-2.0.dll.a)

 target_link_libraries (${PROJECT_NAME}UI ${COMMON_TARGET_NAME})
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
 set_target_properties (${PROJECT_NAME}UI PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}UI PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${PROJECT_NAME}UI PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_ui_common.h
         common_ui_defines.h
         common_ui_exports.h
         common_ui_glade_definition.h
         common_ui_gtk_builder_definition.h
         common_ui_gtk_manager.h
         common_ui_igtk.h
         common_ui_tools.h
         common_ui_types.h
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}UI
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
