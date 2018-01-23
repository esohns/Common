include (CMakeDependentOption)

option (GTK3_SUPPORT "enable GTK 3 support" ON)
#option (GTK2_SUPPORT "enable GTK2 support" OFF)
CMAKE_DEPENDENT_OPTION (GTK2_SUPPORT "enable GTK 2 support" ON
                        "NOT GTK3_SUPPORT" OFF)

# *NOTE*: early versions of gtk do not support GtkBuilder
# *TODO*: --> find the version number(s) and auto-enable this feature for these
#             target systems
# *NOTE*: on Win32 systems, the (binary) gtk3 applications have linkage issues
#         with the latest (2.6.4) libglade distribution (specifically, the
#         libxml2 dll cannot be easily shared between these two packages; also,
#         the 'embedded' library names do not match ('lib'-prefixes))
#         --> disable libglade support when targeting gtk3
# *TODO*: retrieve the available gtk version number(s) from the pkg-config
#         output and pre-set this option accordingly
#option (LIBGLADE_SUPPORT "enable libglade support" OFF)
CMAKE_DEPENDENT_OPTION (LIBGLADE_SUPPORT "enable libglade support" OFF
                        "NOT GTK3_SUPPORT" OFF)

option (GTKGL_SUPPORT "enable GTK OpenGL support" ON)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project
#option (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF)
CMAKE_DEPENDENT_OPTION (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF
                        "GTKGL_SUPPORT" OFF)

#include (CheckLibraryExists)
#check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
#find_library (HAVE_LIB_ACE libACE)

if (UNIX)
 set (ACE_LIB_FILE libACE.so)
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
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()

include (FindPkgConfig)
#pkg_check_modules (PKG_ACE REQUIRED ACE)
if (UNIX)
 if (GTK3_SUPPORT)
  pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   pkg_check_modules (PKG_GTKGL3 REQUIRED gtkgl-3.0)
  endif ()
 else ()
  pkg_check_modules (PKG_GTK2 REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0)
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    pkg_check_modules (PKG_GTKGLAREA REQUIRED gtkgl-2.0)
   else ()
    pkg_check_modules (PKG_GTKGL2 REQUIRED gdkglext-1.0 gtkglext-1.0)
   endif ()
  endif ()
 endif ()
 if (LIBGLADE_SUPPORT)
  pkg_check_modules (PKG_GLADE REQUIRED libglade-2.0)
 endif ()
endif ()
# *TODO*: repair WIN32 pkgconfig support

if (GTKGL_SUPPORT)
 add_definitions (-DGTKGL_SUPPORT)
endif ()
if (GTKGLAREA_SUPPORT)
 add_definitions (-DGTKGLAREA_SUPPORT)
endif ()
if (LIBGLADE_SUPPORT)
 add_definitions (-DLIBGLADE_SUPPORT)
endif ()

if (UNIX)
 if (GTK3_SUPPORT)
  include_directories (${PKG_GTK3_INCLUDE_DIRS})
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   include_directories (${PKG_GTKGLAREA_INCLUDE_DIRS})
# *NOTE*: check out the 'master' branch for gtk3-based applications
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
  endif ()
 else ()
  include_directories (${PKG_GTK2_INCLUDE_DIRS})
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    include_directories (${PKG_GTKGLAREA_INCLUDE_DIRS})
   else ()
    include_directories (${PKG_GTKGL2_INCLUDE_DIRS})
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
   endif ()
  endif ()
 endif ()

 if (LIBGLADE_SUPPORT)
  include_directories (${PKG_GLADE_INCLUDE_DIRS})
 endif ()
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/ACE)

elseif (WIN32)
#*TODO*: use pkg-config instead
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../../ATCD/ACE)
 include_directories ($ENV{ACE_ROOT})

# gtk
 if (GTK3_SUPPORT)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/atk-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/gdk-pixbuf-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/cairo)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/pango-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/glib-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/glib-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/include/gtk-3.0)
 else ()
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/atk-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gdk-pixbuf-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/cairo)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/pango-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/glib-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/glib-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gtk-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gtk-2.0)
 endif ()

 if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
  #message (STATUS "directory \"${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglarea\"")
 endif ()

 if (LIBGLADE_SUPPORT)
# libglade
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../libglade-2.6.4/include/libglade-2.0)
 endif ()
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../../)

############## next target ################
set (${PROJECT_NAME}UI_GTK_LIB_SRCS
     stdafx.cpp
     stdafx.h
     common_ui_gtk_builder_definition.cpp
     common_ui_gtk_builder_definition.h
     common_ui_gtk_builder_definition.inl
     common_ui_gtk_common.h
     common_ui_gtk_defines.h
     common_ui_gtk_eventhandler.cpp
     common_ui_gtk_eventhandler.h
     common_ui_gtk_eventhandler.inl
     common_ui_gtk_exports.h
     common_ui_gtk_manager.cpp
     common_ui_gtk_manager.h
     common_ui_gtk_manager.inl
     common_ui_gtk_manager_common.h
     common_ui_gtk_tools.cpp
     common_ui_gtk_tools.h
#     common_ui_gtk_types.h
     common_ui_igtk.h)
if (LIBGLADE_SUPPORT)
 set (${PROJECT_NAME}UI_GTK_LIB_SRCS ${CommonUI_GTK_LIB_SRCS}
      common_ui_gtk_glade_definition.cpp
      common_ui_gtk_glade_definition.h)
endif (LIBGLADE_SUPPORT) # LIBGLADE_SUPPORT
if (GTKGL_SUPPORT)
 set (${PROJECT_NAME}UI_GTK_LIB_SRCS ${CommonUI_GTK_LIB_SRCS}
      common_ui_gtk_gl_common.h)
endif (GTKGL_SUPPORT) # GTKGL_SUPPORT

#if (UNIX)
# add_definitions (-D_GLIBCXX_INCLUDE_NEXT_C_HEADERS)
#endif ()

add_library (${PROJECT_NAME}UI_GTK STATIC
             ${CommonUI_GTK_LIB_SRCS})

if (UNIX)
# if (GTK3_SUPPORT)
#  target_link_libraries (${PROJECT_NAME}UI_GTK ${PKG_GTKGL3_LIBRARIES})
#  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
#   target_link_libraries (${PROJECT_NAME}UI_GTK ${PKG_GTKGLAREA_LIBRARIES})
#  endif ()
# else ()
#  target_link_libraries (${PROJECT_NAME}UI_GTK ${PKG_GTKGL2_LIBRARIES})
#  if (GTKGL_SUPPORT)
#   if (GTKGLAREA_SUPPORT)
#    target_link_libraries (${PROJECT_NAME}UI_GTK ${PKG_GTKGLAREA_LIBRARIES})
#   else ()
#    target_link_libraries (${PROJECT_NAME}UI_GTK ${PKG_GTKGL2_LIBRARIES})
#   endif ()
#  endif ()
# endif ()
elseif (WIN32)
#*TODO*: use pkg-config instead
 add_dependencies (${PROJECT_NAME}UI_GTK Common)

 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME}UI_GTK Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME}UI_GTK Msvcrt.lib)
 endif ()

 target_link_libraries (${PROJECT_NAME}UI_GTK ${ACE_LIBRARY})

# gtk
 if (GTK3_SUPPORT)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/glib-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/gobject-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/gthread-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/gdk-3.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk/gtk/Win32/debug/lib/gtk-3.0.lib)
 else ()
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/libglib-2.0.dll.a)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/libgobject-2.0.dll.a)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/libgthread-2.0.dll.a)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/libgdk-win32-2.0.dll.a)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/libgtk-win32-2.0.dll.a)
 endif ()

 if (LIBGLADE_SUPPORT)
# libglade
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/lib/libglade-2.0.dll.a)
 endif ()

 target_link_libraries (${PROJECT_NAME}UI_GTK Common)
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
 set_target_properties (${PROJECT_NAME}UI_GTK PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}UI_GTK PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${PROJECT_NAME}UI_GTK PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}UI_GTK PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}UI_GTK)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI_GTK
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_GTK_LIB_HDRS
     common_ui_gtk_builder_definition.h
     common_ui_gtk_builder_definition.inl
     common_ui_gtk_common.h
     common_ui_gtk_defines.h
     common_ui_gtk_eventhandler.h
     common_ui_gtk_eventhandler.inl
     common_ui_gtk_exports.h
     common_ui_gtk_manager.h
     common_ui_gtk_manager.inl
     common_ui_gtk_manager_common.h
     common_ui_gtk_tools.h
#     common_ui_gtk_types.h
     common_ui_igtk.h)
if (LIBGLADE_SUPPORT)
 set (${PROJECT_NAME}UI_GTK_LIB_GLADE_HDRS ${CommonUI_GTK_LIB_GLADE_HDRS}
      common_ui_glade_definition.h)
endif (LIBGLADE_SUPPORT)
if (GTKGL_SUPPORT)
 set (${PROJECT_NAME}UI_GTK_LIB_GLADE_HDRS ${CommonUI_GTK_LIB_GLADE_HDRS}
      common_ui_gtk_gl_common.h)
endif (GTKGL_SUPPORT)
install (FILES
         ${CommonUI_GTK_LIB_HDRS}
         DESTINATION include)

install (TARGETS ${PROJECT_NAME}UI_GTK
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
