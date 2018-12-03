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
include (CMakeDependentOption)
CMAKE_DEPENDENT_OPTION (LIBGLADE_SUPPORT "enable libglade support" OFF
                        "GTK_SUPPORT; NOT GTK3_SUPPORT" OFF)

if (UNIX)
 if (LIBGLADE_SUPPORT)
  pkg_check_modules (PKG_GLADE REQUIRED libglade-2.0)
 endif (LIBGLADE_SUPPORT)
endif (UNIX)
# *TODO*: repair WIN32 pkgconfig support

if (GTKGL_SUPPORT)
 include (FindOpenGL)
 if (NOT OPENGL_FOUND)
  if (UNIX)
   set (OPENGL_LIB_FILE libGL.so)
# find_package (OPENGL REQUIRED)
   find_library (OPENGL_LIBRARY
                 NAMES ${OPENGL_LIB_FILE}
                 PATHS /usr/lib/x86_64-linux-gnu
                 DOC "searching for ${OPENGL_LIB_FILE}")
  elseif (WIN32)
   set (OPENGL_LIB_FILE OpenGL32.lib)
# find_package (OPENGL REQUIRED)
   find_library (OPENGL_LIBRARY
                 NAMES ${OPENGL_LIB_FILE}
                 PATHS /usr/lib/x86_64-linux-gnu
                 DOC "searching for ${OPENGL_LIB_FILE}")
  endif ()
  if (NOT OPENGL_FOUND AND NOT OPENGL_LIBRARY)
   message (FATAL_ERROR "could not find ${OPENGL_LIB_FILE} (was: \"${OPENGL_LIBRARY}\"), aborting")
  else ()
   message (STATUS "Found OpenGL library \"${OPENGL_LIBRARY}\"")
  endif () # NOT OPENGL_FOUND AND NOT OPENGL_LIBRARY
 endif (NOT OPENGL_FOUND)
endif (GTKGL_SUPPORT)

if (LIBGLADE_SUPPORT)
 add_definitions (-DLIBGLADE_SUPPORT)
endif (LIBGLADE_SUPPORT)

add_library (${PROJECT_NAME}UI_GTK STATIC "")

if (UNIX)
 if (GTK3_FOUND)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${GTK3_INCLUDE_DIRS}>)
 elseif (GTK2_FOUND)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${GTK2_INCLUDE_DIRS}>)
 elseif (GTK_FOUND)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${GTK_INCLUDE_DIR}>)
 else ()
  if (GTK2_SUPPORT)
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               $<BUILD_INTERFACE:${PKG_GTK2_INCLUDE_DIRS}>)
  elseif (GTK3_SUPPORT)
# *NOTE*: a bug in cmake (3.10.2) requires the additional comment
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               "$<BUILD_INTERFACE:${PKG_GTK3_INCLUDE_DIRS}>")
  endif ()
 endif ()

 if (GTKGLAREA_SUPPORT)
#  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
#                              $<BUILD_INTERFACE:${PKG_GTKGLAREA_INCLUDE_DIRS}>)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# 'TODO*: verify automatically
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea>)
 endif ()

 if (LIBGLADE_SUPPORT)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${PKG_GLADE_INCLUDE_DIRS}>)
 endif (LIBGLADE_SUPPORT)

 target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/ACE>)
elseif (WIN32)
#*TODO*: use pkg-config instead
# gtk
 if (GTK3_SUPPORT)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/atk-1.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/gdk-pixbuf-2.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/cairo>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/pango-1.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/glib-2.0/include>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/glib-2.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/include/gtk-3.0>)
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea>)
  endif (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
 elseif (GTK2_SUPPORT)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/atk-1.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gdk-pixbuf-2.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/cairo>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/pango-1.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/glib-2.0/include>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/glib-2.0>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gtk-2.0/include>)
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/include/gtk-2.0>)
  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
   target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea>)
  endif (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
 endif (GTK3_SUPPORT)

 if (LIBGLADE_SUPPORT)
# libglade
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../libglade-2.6.4/include/libglade-2.0>)
 endif (LIBGLADE_SUPPORT)

 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../log>)
target_include_directories (${PROJECT_NAME}UI_GTK PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../timer>)

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
     common_ui_gtk_tools.h)
#     common_ui_gtk_types.h
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

# gtk
 if (GTK3_SUPPORT)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/glib-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gthread-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gobject-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gio-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gdk-win32-3.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/gtk-win32-3.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk3/lib/pango-1.0.lib)
 elseif (GTK2_SUPPORT)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/glib-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gthread-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gobject-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gio-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gdk_pixbuf-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gdk-win32-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/gtk-win32-2.0.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/cairo.lib)
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtk2/lib/pango-1.0.lib)
 endif ()

 if (GTKGL_SUPPORT)
  if (GTKGLAREA_SUPPORT)
   target_link_libraries (${PROJECT_NAME}UI_GTK "C:/Documents and Settings/Erik/My Documents/Visual Studio 2010/Projects/gtkglarea/Debug/gtkglarea.lib")
#   target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea/.lib)
  else (GTKGLAREA_SUPPORT)
 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk/.libs/libgdkglext-win32-1.0.dll.a)
 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gtk/.libs/libgtkglext-win32-1.0.dll.a)
  endif (GTKGLAREA_SUPPORT)
 endif (GTKGL_SUPPORT)

 if (LIBGLADE_SUPPORT)
# libglade
  target_link_libraries (${PROJECT_NAME}UI_GTK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/lib/libglade-2.0.dll.a)
 endif (LIBGLADE_SUPPORT)

# target_link_libraries (${PROJECT_NAME}UI_GTK ${ACE_LIBRARY})

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
     common_ui_gtk_tools.h)
#     common_ui_gtk_types.h
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
