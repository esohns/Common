include (CMakeDependentOption)

CMAKE_DEPENDENT_OPTION (GTK3_SUPPORT "enable GTK 3 support" ON
                        "GTK_SUPPORT" OFF)
#option (GTK3_SUPPORT "enable GTK 3 support" ON)
CMAKE_DEPENDENT_OPTION (GTK2_SUPPORT "enable GTK 2 support" ON
                        "GTK_SUPPORT; NOT GTK3_SUPPORT" OFF)
#option (GTK2_SUPPORT "enable GTK2 support" OFF)

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
                        "GTK_SUPPORT; NOT GTK3_SUPPORT" OFF)

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

add_library (${PROJECT_NAME}UI_GTK STATIC "")

if (UNIX)
 if (GTK3_SUPPORT)
  # *NOTE*: a bug in cmake (3.10.2) requires the additional comment
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              "$<BUILD_INTERFACE:${PKG_GTK3_INCLUDE_DIRS}>")
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               $<BUILD_INTERFACE:${PKG_GTKGLAREA_INCLUDE_DIRS}>)
# *NOTE*: check out the 'master' branch for gtk3-based applications
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea>)
  endif ()
 else ()
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${PKG_GTK2_INCLUDE_DIRS}>)
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                                $<BUILD_INTERFACE:${PKG_GTKGLAREA_INCLUDE_DIRS}>)
   else ()
    target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                                $<BUILD_INTERFACE:${PKG_GTKGL2_INCLUDE_DIRS}>)
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
    target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea>)
   endif ()
  endif ()
 endif ()

 if (LIBGLADE_SUPPORT)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${PKG_GLADE_INCLUDE_DIRS}>)
 endif ()

 target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/ACE>)
elseif (WIN32)
#*TODO*: use pkg-config instead
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../../ATCD/ACE)
 include_directories ($ENV{ACE_ROOT})

# gtk
 if (GTK3_SUPPORT)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/atk-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/gdk-pixbuf-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/cairo)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/pango-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/glib-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/glib-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/gtk-3.0)
  if (GTKGL_SUPPORT)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
  endif (GTKGL_SUPPORT)
 else ()
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/atk-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gdk-pixbuf-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/cairo)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/pango-1.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/glib-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/glib-2.0)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gtk-2.0/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gtk-2.0)
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
  endif (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
 endif (GTK3_SUPPORT)

 if (LIBGLADE_SUPPORT)
# libglade
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../libglade-2.6.4/include/libglade-2.0)
 endif (LIBGLADE_SUPPORT)
endif ()
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>)

############## next target ################
set (source_files
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
 set (source_files
      ${source_files}
      common_ui_gtk_glade_definition.cpp
      common_ui_gtk_glade_definition.h)
endif (LIBGLADE_SUPPORT) # LIBGLADE_SUPPORT
if (GTKGL_SUPPORT)
 set (source_files
      ${source_files}
      common_ui_gtk_gl_common.h)
endif (GTKGL_SUPPORT) # GTKGL_SUPPORT

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}UI_GTK
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#get_target_property (debug ${PROJECT_NAME}UI_GTK INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")

#if (UNIX)
# add_definitions (-D_GLIBCXX_INCLUDE_NEXT_C_HEADERS)
#endif ()

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
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gio-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/glib-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gobject-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gthread-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gdk-win32-3.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gtk-win32-3.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/pango-1.0.lib)
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
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
