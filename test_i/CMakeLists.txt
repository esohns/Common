# configure C/C++ exceptions
include (SetCppExceptions)

include (Threads)

include (ffmpeg)
include (ImageMagick)

include (tensorflow)

include (ACE)

option (BUILD_TEST_I "build test_i" OFF)

# user interface
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts")
include (PreUIFramework)
include (SetUIFramework)
if (GTK_SUPPORT)
 include (SetGTKVersion)
endif (GTK_SUPPORT)

if (UNIX)
 add_link_options ("LINKER:--as-needed")
endif (UNIX)

set (LIBRARY ${PROJECT_NAME}_Test_I)
add_library (${LIBRARY} STATIC "")

if (CURSES_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${CURSES_INCLUDE_DIRS}>")
endif (CURSES_SUPPORT)
if (GTK_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")

 if (GTKGLAREA_SUPPORT)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# *TODO*: verify automatically
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${GTKGLAREA_INCLUDES}>)
 endif (GTKGLAREA_SUPPORT)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 if (WIN32)
  cmake_policy (SET CMP0020 OLD)
 endif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${QT_INCLUDE_DIRS}>)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")
endif (WXWIDGETS_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/event>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/signal>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/log>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
if (CURSES_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/curses>)
endif (CURSES_SUPPORT)
if (GTK_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/qt>)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
endif (WXWIDGETS_SUPPORT)

if (GTK_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${GTK_LIBRARIES})
 if (GTKGL_SUPPORT)
  if (GTKGLAREA_SUPPORT)
   target_link_libraries (${LIBRARY} PUBLIC ${GTKGLAREA_LIBRARY})
  endif (GTKGLAREA_SUPPORT)
 endif (GTKGL_SUPPORT)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${QT_LIBRARIES})
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${wxWidgets_LIBRARIES})
endif (WXWIDGETS_SUPPORT)

if (UNIX)
 target_link_libraries (${LIBRARY} PUBLIC X11)
 if (GTK_SUPPORT)
  if (GTKGL_SUPPORT)
   target_link_libraries (${LIBRARY} PUBLIC GL)
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)
elseif (WIN32)
 if (GTK_SUPPORT)
  if (GTKGL_SUPPORT)
   target_link_libraries (${LIBRARY} PUBLIC OpenGL32.lib)
   if (GTKGLAREA_SUPPORT)
    target_link_libraries (${LIBRARY} PUBLIC $ENV{LIB_ROOT}/libepoxy/lib/epoxy.lib)
   endif (GTKGLAREA_SUPPORT)
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)
endif ()

set (source_files
     common_test_i_common.h
     common_test_i_defines.h
     common_test_i_tools.cpp
     common_test_i_tools.h
     common_test_i_tools.inl)

foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()

##########################################

target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

##########################################

install (FILES
         common_test_i_common.h
         common_test_i_defines.h
         common_test_i_tools.h
         common_test_i_tools.inl
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT lib)

##########################################

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

##########################################

add_custom_target (test_i ALL
                   COMMENT "building test_i...")
if (BUILD_TEST_I)
 if (UNIX)
  add_subdirectory (capabilities)
 endif (UNIX)
 if (FFMPEG_SUPPORT)
  add_subdirectory (ffmpeg)
 endif (FFMPEG_SUPPORT)
 if (IMAGEMAGICK_SUPPORT)
  add_subdirectory (imagemagick)
 endif (IMAGEMAGICK_SUPPORT)
 add_subdirectory (input)
 add_subdirectory (opengl)
 add_subdirectory (parser)
 if (UNIX)
  add_subdirectory (print)
 endif (UNIX)
 if (TENSORFLOW_SUPPORT)
  add_subdirectory (tensorflow)
 endif (TENSORFLOW_SUPPORT)
 add_subdirectory (ui)
 add_subdirectory (vulkan)

 if (UNIX)
  add_dependencies (test_i capability_wrapper)
 endif ()
 if (FFMPEG_SUPPORT)
  add_dependencies (test_i ffmpeg)
 endif (FFMPEG_SUPPORT)
 if (IMAGEMAGICK_SUPPORT)
  add_dependencies (test_i imagemagick)
 endif (IMAGEMAGICK_SUPPORT)
 add_dependencies (test_i input)
 add_dependencies (test_i opengl vulkan)
 add_dependencies (test_i parser parser_2 parser_3 parser_4)
 if (UNIX)
  add_dependencies (test_i print)
 endif ()
 if (TENSORFLOW_SUPPORT)
  add_dependencies (test_i tensorflow)
 endif (TENSORFLOW_SUPPORT)
 add_dependencies (test_i console)
 if (CURSES_SUPPORT)
  add_dependencies (test_i curses)
 endif (CURSES_SUPPORT)
 if (GTK_SUPPORT)
  add_dependencies (test_i gtk gtk_2 gtk_3)
 endif (GTK_SUPPORT)
 if (QT_SUPPORT)
  add_dependencies (test_i qt qt_2)
 endif (QT_SUPPORT)
 if (WXWIDGETS_SUPPORT)
  add_dependencies (test_i wxwidgets wxwidgets_2 wxwidgets_3)
 endif (WXWIDGETS_SUPPORT)
endif (BUILD_TEST_I)

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
