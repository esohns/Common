include (Threads)

include (ffmpeg)
include (ImageMagick)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts")
include (PreUIFramework)
include (SetUIFramework)

include (ACE)

option (BUILD_TEST_I "build test_i" OFF)

add_library (${PROJECT_NAME}_Test_I STATIC "")

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
if (UNIX)
 if (GUI_SUPPORT)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${PKG_GTK_INCLUDE_DIRS}>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${PKG_WXWIDGETS_INCLUDE_DIRS}>)
  endif ()
 endif (GUI_SUPPORT)
 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
 if (GUI_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
  elseif (QT_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/qt>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
  endif ()
 endif (GUI_SUPPORT)
elseif (WIN32)
 if (GUI_SUPPORT)
  if (GTK_USE)
#   message (STATUS "GTK_INCLUDE_DIRS: \"${GTK_INCLUDE_DIRS}\"")
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")

# libglade
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/include/libglade-2.0)
  elseif (WXWIDGETS_USE)
#   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
#                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../wxWidgets_win32/wxWidgets-3.0.0/include>)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")
  endif ()
 endif (GUI_SUPPORT)

 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/log>)
 if (GUI_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
  endif ()
 endif (GUI_SUPPORT)
endif ()
target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

if (WIN32)
 if (GUI_SUPPORT)
  if (GTK_USE)
   target_link_libraries (${PROJECT_NAME}_Test_I ${GTK_LIBRARIES})

   if (GTKGL_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_I OpenGL32.lib)
    if (GTKGLAREA_SUPPORT)
     target_link_libraries (${PROJECT_NAME}_Test_I $ENV{LIB_ROOT}/libepoxy/lib/epoxy.lib)
     target_link_libraries (${PROJECT_NAME}_Test_I $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
    endif (GTKGLAREA_SUPPORT)
   endif (GTKGL_SUPPORT)
  elseif (WXWIDGETS_USE)
   target_link_libraries (${PROJECT_NAME}_Test_I ${wxWidgets_LIBRARIES})
  endif ()
 endif (GUI_SUPPORT)
endif (WIN32)

set (source_files
     common_test_i_common.h
#     test_i_defines.h
     common_test_i_tools.cpp
     common_test_i_tools.h
     common_test_i_tools.inl)
#if (GTK_SUPPORT)
# set (source_files
#      ${source_files}
#      test_u_gtk_common.h
#      test_u_gtk_eventhandler.cpp
#      test_u_gtk_eventhandler.h
#      test_u_gtk_eventhandler.inl)
#endif (GTK_SUPPORT)
#if (WXWIDGETS_SUPPORT)
# set (source_files
#      ${source_files}
#      test_u_wxwidgets_common.h)
#endif (WXWIDGETS_SUPPORT)

foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()

##########################################

# add PCH
set_target_properties (${PROJECT_NAME}_Test_I PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}_Test_I)
#add_msvc_precompiled_header (stdafx.h, stdafx.cpp, ${absolute_source_files})

##########################################

target_sources (${PROJECT_NAME}_Test_I
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#set_target_properties (${PROJECT_NAME}_Test_U PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

#get_target_property (debug ${PROJECT_NAME}_Test_U INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES=${INTERFACE_INCLUDE_DIRECTORIES}")

##########################################

install (FILES
         common_test_i_common.h
         common_test_i_tools.h
         common_test_i_tools.inl
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}_Test_I
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT lib)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}_Test_I
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_Test_I
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

##########################################

# ----------------------------------------

if (UNIX)
 add_subdirectory (capabilities)
endif (UNIX)
if (FFMPEG_SUPPORT)
 add_subdirectory (ffmpeg)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 add_subdirectory (imagemagick)
endif (IMAGEMAGICK_SUPPORT)
add_subdirectory (parser)
if (UNIX)
 add_subdirectory (print)
endif (UNIX)
add_subdirectory (ui)

# phonies
add_custom_target (test_i ALL
                   COMMENT "building test_i...")
if (UNIX)
 add_dependencies (test_i capability_wrapper)
endif ()
if (FFMPEG_SUPPORT)
 add_dependencies (test_i ffmpeg)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 add_dependencies (test_i imagemagick)
endif (IMAGEMAGICK_SUPPORT)
add_dependencies (test_i parser parser_2)
if (UNIX)
 add_dependencies (test_i print)
endif ()
if (GTK_SUPPORT)
 add_dependencies (test_i gtk_ui gtk_ui_2 gtk_ui_3)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 add_dependencies (test_i qt_ui qt_ui_2)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_dependencies (test_i wxwidgets wxwidgets_2 wxwidgets_3)
endif (WXWIDGETS_SUPPORT)

if (NOT BUILD_TEST_I)
 set_target_properties (test_i PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_i PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif ()
endif ()

# ----------------------------------------

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
