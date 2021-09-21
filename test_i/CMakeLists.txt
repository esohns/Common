include (Threads)

include (ffmpeg)
include (ImageMagick)

include (ACE)

option (BUILD_TEST_I "build test_i" OFF)

# user interface
option (GUI_SUPPORT "enable graphical user interface support" ON)
if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts")
 include (PreUIFramework)
 include (SetUIFramework)
 if (GTK_USE)
  include (SetGTKVersion)
 endif (GTK_USE)
endif (GTK_SUPPORT)

add_library (${PROJECT_NAME}_Test_I STATIC "")

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

if (GUI_SUPPORT)
 if (GTK_USE)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")
  if (GTKGL_SUPPORT)
   target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                               $<BUILD_INTERFACE:${OPENGL_INCLUDE_DIR}>)
  endif (GTKGL_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
 elseif (QT_USE)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              "$<BUILD_INTERFACE:${Qt_INCLUDE_DIRS}>")
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/qt>)
 elseif (WXWIDGETS_USE)
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")
  target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
 endif ()
 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
endif (GUI_SUPPORT)

if (WIN32)
 target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
endif (WIN32)
target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)
target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
target_include_directories (${PROJECT_NAME}_Test_I PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/log>)

if (GUI_SUPPORT)
 if (GTK_USE)
  target_link_libraries (${PROJECT_NAME}_Test_I ${GTK_LIBRARIES})
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_I ${GTKGLAREA_LIBRARY})
   endif (GTKGLAREA_SUPPORT)
  endif (GTKGL_SUPPORT)
 elseif (QT_USE)
  target_link_libraries (${PROJECT_NAME}_Test_I ${Qt_LIBRARIES})
 elseif (WXWIDGETS_USE)
  target_link_libraries (${PROJECT_NAME}_Test_I ${wxWidgets_LIBRARIES})
 endif ()
endif (GUI_SUPPORT)

if (UNIX)
 if (GUI_SUPPORT)
  target_link_libraries (${PROJECT_NAME}_Test_I X11)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_I GL)
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)
elseif (WIN32)
 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_I OpenGL32.lib)
    if (GTKGLAREA_SUPPORT)
     target_link_libraries (${PROJECT_NAME}_Test_I $ENV{LIB_ROOT}/libepoxy/lib/epoxy.lib)
    endif (GTKGLAREA_SUPPORT)
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)
endif ()

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
if (GUI_SUPPORT)
 add_subdirectory (ui)
endif (GUI_SUPPORT)

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
if (GUI_SUPPORT)
 if (GTK_SUPPORT)
  add_dependencies (test_i gtk_ui gtk_ui_2 gtk_ui_3)
 endif (GTK_SUPPORT)
 if (QT_SUPPORT)
  add_dependencies (test_i qt_ui qt_ui_2)
 endif (QT_SUPPORT)
 if (WXWIDGETS_SUPPORT)
  add_dependencies (test_i wxwidgets wxwidgets_2 wxwidgets_3)
 endif (WXWIDGETS_SUPPORT)
endif (GUI_SUPPORT)

if (NOT BUILD_TEST_I)
 set_target_properties (test_i PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_i PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif (WIN32)
endif (NOT BUILD_TEST_I)

# ----------------------------------------

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
